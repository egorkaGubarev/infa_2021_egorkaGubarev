#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <fstream>
#include <list>
#include <random>
#include <vector>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

template <typename t_data, typename t_key>
struct my_pair
{
    const t_key key_;
    t_data data_;

    my_pair(const t_key key, const t_data data): key_{key}, data_{data}
    {

    }
};

class Generator
{
    private:
        std::mt19937_64 engine_;
        std::uniform_int_distribution<uchar> distr_;
    public:
        Generator(): engine_(std::random_device{}()), distr_{0, 1}
        {

        }

        bool get_random()
        {
            const uchar random = distr_(engine_);
            if(random == 0){
                return false;
            }
            else{
                return true;
            }
        }
};

template <typename t_data>
struct Node
{
    const t_data value_;
    const bool is_head_;
    Node* up_ptr_;
    Node* down_ptr_;
    Node* left_ptr_;
    Node* right_ptr_;

    Node(const t_data value): value_{value}, up_ptr_{nullptr}, down_ptr_{nullptr}, left_ptr_{nullptr}, right_ptr_{nullptr}, is_head_{false}
    {

    }

    Node(): value_{}, up_ptr_{nullptr}, down_ptr_{nullptr}, left_ptr_{nullptr}, right_ptr_{nullptr}, is_head_{true}
    {

    }
};

template <typename t_data>
class SkipIterator: public std::iterator<std::bidirectional_iterator_tag, t_data>
{
    private:
        Node<t_data>* ptr_;

    public:
        SkipIterator(Node<t_data>* ptr): ptr_{ptr}
        {

        }

        bool operator!=(const SkipIterator& other) const
        {
            if(ptr_ == other.ptr_){
                return false;
            }
            else{
                return true;
            }
        }

        bool operator==(const SkipIterator& other) const
        {
            if(ptr_ == other.ptr_){
                return true;
            }
            else{
                return false;
            }
        }

        t_data operator*() const
        {
            const t_data value = ptr_->value_;
            return value;
        }

        SkipIterator& operator++()
        {
            ptr_ = ptr_->right_ptr_;
            return *this;
        }

        SkipIterator& operator--()
        {
            ptr_ = ptr_->left_ptr_;
            return *this;
        }
};

template<typename t_data>
class SkipList
{
    private:
        Node<t_data>* head_ptr_;
        size_t levels_;
        std::vector<size_t> length_;
        Generator generator_;

        bool get_random_value()
        {
            const bool result = generator_.get_random();
            return result;
        }

        Node<t_data>* get_down_head()
        {
            Node<t_data>* current_ptr = head_ptr_;
            while(current_ptr->down_ptr_ != nullptr) {
                current_ptr = current_ptr->down_ptr_;
            }
            return current_ptr;
        }

        const Node<t_data>* get_down_head() const
        {
            const Node<t_data>* current_ptr = head_ptr_;
            while(current_ptr->down_ptr_ != nullptr) {
                current_ptr = current_ptr->down_ptr_;
            }
            return current_ptr;
        }

        void erase_level(const size_t level)
        {
            const size_t down = levels_ - level - 1;
            Node<t_data>* current_ptr = head_ptr_;
            for(size_t i = 0; i < down; ++ i){
                current_ptr = current_ptr->down_ptr_;
            }
            Node<t_data>* up_head_ptr = current_ptr->up_ptr_;
            Node<t_data>* down_head_ptr = current_ptr->down_ptr_;
            if(up_head_ptr != nullptr){
                up_head_ptr->down_ptr_ = down_head_ptr;
            }
            if(down_head_ptr != nullptr){
                down_head_ptr->up_ptr_ = up_head_ptr;
            }
            while(current_ptr != nullptr){
                Node<t_data>* right_ptr = current_ptr->right_ptr_;
                delete current_ptr;
                current_ptr = nullptr;
                current_ptr = right_ptr;
            }
        }

        void erase()
        {
            const size_t max_level = levels_ - 1;
            for(size_t i = max_level; i > 0; -- i){
                erase_level(i);
            }
            erase_level(0);
        }

    public:
        typedef SkipIterator<t_data> iterator;
        typedef SkipIterator<t_data> reverse_iterator;

        SkipList(): levels_{1} {
            head_ptr_ = new Node<t_data>;
            length_.resize(2);
            length_[1] = 0;
        }

       template <typename t_it>
        SkipList(const t_it begin, const t_it end): SkipList()
        {
            insert(begin, end);
        }

        SkipList(const SkipList& source): SkipList()
        {
            const std::list<t_data> list = source.get_list();
            const typename std::list<t_data>::const_iterator begin_it = std::cbegin(list);
            const typename std::list<t_data>::const_iterator end_it = std::cend(list);
            insert(begin_it, end_it);
        }

        SkipList(SkipList&& source): SkipList()
        {
            head_ptr_ = source.head_ptr_;
            levels_ = std::move(source.levels_);
            length_ = std::move(source.length_);
            source.head_ptr_ = new Node<t_data>;
            source.levels_ = 1;
            source.length_.resize(2);
            source.length_[1] = 0;
        }

        bool empty() const
        {
            if(length_[1] == 0){
                return true;
            }
            else{
                return false;
            }
        }

        size_t size() const
        {
            const size_t size = length_[1];
            return size;
        }

        void insert(const t_data value)
        {
        Node<t_data>* current_ptr = get_down_head();
        Node<t_data>* new_ptr = nullptr;
        Node<t_data>* current_head_ptr = current_ptr;
        while(current_ptr->right_ptr_ != nullptr){
            if(value < current_ptr->right_ptr_->value_ && new_ptr == nullptr) {
                new_ptr = new Node<t_data>(value);
                new_ptr->right_ptr_ = current_ptr->right_ptr_;
                new_ptr->left_ptr_ = current_ptr;
                current_ptr->right_ptr_ = new_ptr;
                Node<t_data>* right_ptr = new_ptr->right_ptr_;
                if(right_ptr != nullptr){
                    right_ptr->left_ptr_ = new_ptr;
                }
                ++ length_[1];
            }
            current_ptr = current_ptr->right_ptr_;
        }
        if (new_ptr == nullptr) {
            new_ptr = new Node<t_data>(value);
            current_ptr->right_ptr_ = new_ptr;
            new_ptr->left_ptr_ = current_ptr;
            ++ length_[1];
        }
        size_t current_level = 1;
        bool up = get_random_value();
        while(up){
            ++ current_level;
            if(levels_ < current_level){
                ++ levels_;
                length_.resize(levels_ + 1);
                Node<t_data>* new_head_ptr = new Node<t_data>();
                new_head_ptr->down_ptr_ = head_ptr_;
                head_ptr_->up_ptr_ = new_head_ptr;
                head_ptr_ = new_head_ptr;
            }
            current_head_ptr = current_head_ptr->up_ptr_;
            current_ptr = current_head_ptr;
            Node<t_data>* skip_node_ptr = nullptr;
            while(current_ptr->right_ptr_ != nullptr){
                if(value < current_ptr->right_ptr_->value_ && skip_node_ptr == nullptr) {
                    skip_node_ptr = new Node<t_data>(value);
                    skip_node_ptr->right_ptr_ = current_ptr->right_ptr_;
                    current_ptr->right_ptr_ = skip_node_ptr;
                    skip_node_ptr->left_ptr_ = current_ptr;
                    Node<t_data>* right_ptr = skip_node_ptr->right_ptr_;
                    if(right_ptr != nullptr){
                        right_ptr->left_ptr_ = skip_node_ptr;
                    }
                    ++ length_[current_level];
                }
                current_ptr = current_ptr->right_ptr_;
            }
            if(skip_node_ptr == nullptr) {
                skip_node_ptr = new Node<t_data>(value);
                current_ptr->right_ptr_ = skip_node_ptr;
                skip_node_ptr->left_ptr_ = current_ptr;
                ++ length_[current_level];
            }
            while(new_ptr->up_ptr_ != nullptr) {
                new_ptr = new_ptr->up_ptr_;
            }
            skip_node_ptr->down_ptr_ = new_ptr;
            new_ptr->up_ptr_ = skip_node_ptr;
            up = get_random_value();
        }
    }

        template <typename t_it>
        void insert(const t_it begin, const t_it end)
        {
            t_it it = begin;
            for(it; it != end; it = std::next(it)){
                const t_data value = *it;
                insert(value);
            }
        }

        iterator search(const t_data value)
        {
            iterator end_it = end();
            Node<t_data>* current_ptr = nullptr;
            if(head_ptr_ == nullptr){
                return end_it;
            }
            current_ptr = head_ptr_;
            while(current_ptr->down_ptr_ != nullptr){
                while(current_ptr->right_ptr_ != nullptr){
                    if(value <= current_ptr->right_ptr_->value_) {
                        break;
                    }
                    current_ptr = current_ptr->right_ptr_;
                }
                current_ptr = current_ptr->down_ptr_;
            }
            while(current_ptr->right_ptr_ != nullptr){
                if(value > current_ptr->right_ptr_->value_){
                    current_ptr = current_ptr->right_ptr_;
                }
                else{
                    if(value == current_ptr->right_ptr_->value_){
                        Node<t_data>* ptr = current_ptr->right_ptr_;
                        iterator it(ptr);
                        return it;
                    }
                    else{
                        if (value < current_ptr->right_ptr_->value_) {
                            return end_it;
                        }
                    }
                }
            }
            return end_it;
        }

        void remove(const t_data value)
        {
            Node<t_data>* current_ptr = head_ptr_;
            Node<t_data>* upper_head_ptr = nullptr;
            size_t level = levels_;
            while(true){
                Node<t_data>* current_head_ptr = current_ptr;
                if(upper_head_ptr != nullptr){
                    current_head_ptr->up_ptr_ = nullptr;
                    upper_head_ptr->down_ptr_ = nullptr;
                    delete upper_head_ptr;
                    upper_head_ptr = nullptr;
                    std::vector<size_t>::iterator begin_it = std::begin(length_);
                    std::vector<size_t>::iterator first_it = std::next(begin_it);
                    std::vector<size_t>::iterator empty_it = first_it;
                    std::advance(empty_it, level);
                    std::vector<size_t>::iterator non_empty_it = std::next(empty_it);
                    std::vector<size_t>::iterator end_it = std::end(length_);
                    std::copy(non_empty_it, end_it, empty_it);
                    -- levels_;
                    head_ptr_ = current_head_ptr;
                }
                while(current_ptr != nullptr && current_ptr->right_ptr_ != nullptr){
                    if(value == current_ptr->right_ptr_->value_){
                        Node<t_data>* delete_ptr = current_ptr->right_ptr_;
                        current_ptr->right_ptr_ = current_ptr->right_ptr_->right_ptr_;
                        Node<t_data>* right_ptr = delete_ptr->right_ptr_;
                        if(right_ptr != nullptr){
                            right_ptr->left_ptr_ = current_ptr;
                        }
                        delete delete_ptr;
                        -- length_[level];
                    }
                    current_ptr = current_ptr->right_ptr_;
                }
                if(current_head_ptr->right_ptr_ == nullptr){
                    upper_head_ptr = current_head_ptr;
                }
                if (current_head_ptr->down_ptr_ == nullptr){
                    break;
                }
                else{
                    current_ptr = current_head_ptr->down_ptr_;
                    -- level;
                }
            }
        }

        iterator begin()
        {
            Node<t_data>* down_ptr = get_down_head();
            if(down_ptr != nullptr){
                Node<t_data>* begin_ptr = down_ptr->right_ptr_;
                iterator it(begin_ptr);
                return it;
            }
            else{
                iterator it(nullptr);
                return it;
            }
        }

        iterator end()
        {
            Node<t_data>* down_ptr = get_down_head();
            while(down_ptr != nullptr){
                down_ptr = down_ptr->right_ptr_;
            }
            iterator it(down_ptr);
            return it;
        }

        size_t count(const t_data value) const
        {
            const Node<t_data>* ptr = get_down_head();
            size_t count = 0;
            while(ptr != nullptr){
                ptr = ptr->right_ptr_;
                if(ptr != nullptr){
                    const Node<t_data>& node = *ptr;
                    const t_data data = node.value_;
                    if(data == value){
                        ++ count;
                    }
                }
            }
            return count;
        }

        SkipList& operator=(const SkipList& source)
        {
            if(this != &source){
                erase();
                head_ptr_ = new Node<t_data>;
                const std::list<t_data> list = source.get_list();
                const typename std::list<t_data>::const_iterator begin_it = std::cbegin(list);
                const typename std::list<t_data>::const_iterator end_it = std::cend(list);
                insert(begin_it, end_it);
            }
            return *this;
        }

        SkipList& operator=(SkipList&& source)
        {
            if(this == &source){
                return *this;
            }
            else{
                erase();
                head_ptr_ = source.head_ptr_;
                levels_ = std::move(source.levels_);
                length_ = std::move(source.length_);
                source.head_ptr_ = new Node<t_data>;
                source.levels_ = 1;
                source.length_.resize(2);
                source.length_[1] = 0;
                return *this;
            }
        }

        ~SkipList()
        {
            if(head_ptr_ != nullptr){
                erase();
            }
        }

        void get_statistics() const
        {
            std::cout << "Statistics: " << '\n';
            for(size_t i = 1; i <= levels_; ++ i){
                const size_t length = length_[i];
                std::cout << "Level " << i << ": " << length << '\n';
            }
        }

        std::list<t_data> get_list() const
        {
            std::list<t_data> result;
            if(head_ptr_ == nullptr){
                return result;
            }
            const Node<t_data>* current_ptr = get_down_head();
            if(current_ptr->right_ptr_ == nullptr){
                return result;
            }
            current_ptr = current_ptr->right_ptr_;
            while(current_ptr->right_ptr_ != nullptr)
            {
                const Node<t_data>& current_node = *current_ptr;
                const t_data value = current_node.value_;
                result.push_back(value);
                current_ptr = current_ptr->right_ptr_;
            }
            const Node<t_data>& current_node = *current_ptr;
            const t_data value = current_node.value_;
            result.push_back(value);
            return result;
        }
};

class TimeMeasure
{
    public:
        void start()
        {
            start_time_ = std::chrono::high_resolution_clock::now();
        }
        void stop()
        {
            stop_time_ = std::chrono::high_resolution_clock::now();
        }
        size_t elapsed() const
        {
            auto elapsed_time = stop_time_ - start_time_;
            return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time).count();
        }
    private:
        std::chrono::high_resolution_clock::time_point start_time_, stop_time_;
};

template <typename t_data>
void is_equal(const t_data arg1, const t_data arg2, const std::string& message)
{
    if(arg1 == arg2){
        std::cout << message << " is ok" << '\n';
    }
    else{
        std::cerr << "-----Error!-----" << '\n';
        std::cerr << message << " is bad" << '\n';
        std::cerr << "-----Error!-----" << '\n';
    }
}

template <typename t_data1, template<typename> typename t_container1, typename t_data2, template<typename> typename t_container2>
void are_containers_equal(const t_container1<t_data1>& arg1, const t_container2<t_data2>& arg2, const std::string& message)
{
    const uint size1 = arg1.size();
    const uint size2 = arg2.size();
    if(size1 != size2){
        std::cerr << "-----Error!-----" << '\n';
        std::cerr << message << " is bad" << '\n';
        std::cerr << "-----Error!-----" << '\n';
    }
    else{
        bool error = false;
        typename t_container1<t_data1>::const_iterator it1 = std::begin(arg1);
        typename t_container1<t_data1>::const_iterator end = std::end(arg1);
        typename t_container2<t_data2>::const_iterator it2 = std::begin(arg2);
        while(it1 != end && !error){
            const t_data1& value1 = *it1;
            const t_data2& value2 = *it2;
            if(value1 != value2){
                std::cerr << "-----Error!-----" << '\n';
                std::cerr << message << " is bad" << '\n';
                std::cerr << "-----Error!-----" << '\n';
                error = true;
            }
            it1 = std::next(it1);
            it2 = std::next(it2);
        }
        if(!error){
            std::cout << message << " is ok" << '\n';
        }
    }
}

void test_default_construction()
{
    std::cout << "     Default construction test" << '\n';
    SkipList<uchar> list;
    std::list<uchar> correct;
    std::list<uchar> down = list.get_list();
    are_containers_equal(down, correct, "Empty");
}

void test_iterator_construction()
{
    std::cout << "     Iterator construction test" << '\n';
    const std::array<const uchar, 3> array{'a', 'b', 'c'};
    const std::list<uchar> correct{'a', 'b', 'c'};
    const std::array<const uchar, 3>::const_iterator begin_it = std::cbegin(array);
    const std::array<const uchar, 3>::const_iterator end_it = std::cend(array);
    SkipList<uchar> list(begin_it, end_it);
    std::list<uchar> down = list.get_list();
    are_containers_equal(correct, down, "Array of uchar");
}

void test_assignment_construction()
{
    std::cout << "     Assignment construction test" << '\n';
    SkipList<uchar> list;
    list.insert('a');
    list.insert('b');
    list.insert('c');
    SkipList<uchar> copy(list);
    std::list<uchar> correct{'a', 'b', 'c'};
    std::list<uchar> down = copy.get_list();
    are_containers_equal(down, correct, "3 uchar");
}

void test_move_construction()
{
    std::cout << "     Move construction test" << '\n';
    SkipList<uchar> list;
    list.insert('a');
    list.insert('b');
    list.insert('c');
    SkipList<uchar> copy(std::move(list));
    std::list<uchar> correct{'a', 'b', 'c'};
    std::list<uchar> empty;
    std::list<uchar> down = copy.get_list();
    std::list<uchar> list_down = list.get_list();
    are_containers_equal(down, correct, "3 uchar");
    are_containers_equal(list_down, empty, "Erase");
}

void test_insert()
{
    std::cout << "    Test insert" << '\n';
    SkipList<uchar> list;
    std::list<uchar> correct;
    const uchar amount = 100;
    for(uchar i = 0; i < amount; ++ i){
        const uchar value = amount - i;
        list.insert(value);
        correct.push_back(value);
    }
    correct.sort();
    std::list<uchar> down = list.get_list();
    are_containers_equal(down, correct, "100 uchar");
}

void test_insert_iterator()
{
    std::cout << "     Insert iterator test" << '\n';
    const std::array<const uchar, 3> array{'a', 'b', 'c'};
    const std::array<const uchar, 3>::const_iterator begin_it = std::cbegin(array);
    const std::array<const uchar, 3>::const_iterator end_it = std::cend(array);
    SkipList<uchar> list;
    list.insert(begin_it, end_it);
    std::list<uchar> correct{'a', 'b', 'c'};
    std::list<uchar> down = list.get_list();
    are_containers_equal(down, correct, "Array 3 uchar");
}

void test_search()
{
    std::cout << "    Test search" << '\n';
    SkipList<uchar> list;
    const uchar amount = 100;
    for(uchar i = 0; i < amount; ++ i){
        list.insert(i);
    }
    SkipIterator<uchar> find_it = list.search(99);
    SkipIterator<uchar> end_it = list.end();
    const uchar value = *find_it;
    const uchar correct = 99;
    is_equal(value, correct, "Exist");
    is_equal(list.search(100), end_it, "Not exist");
}

void test_remove()
{
    std::cout << "    Test remove" << '\n';
    SkipList<uchar> list;
    const uchar amount = 100;
    for(uchar i = 0; i < amount; ++ i){
        list.insert(i);
    }
    SkipIterator<uchar> find_it = list.search(99);
    SkipIterator<uchar> end_it = list.end();
    const uchar value = *find_it;
    const uchar correct = 99;
    is_equal(value, correct, "Exist");
    list.remove(99);
    is_equal(list.search(99), end_it, "Not exist");
}

void test_assignation()
{
    std::cout << "     Assignation test" << '\n';
    SkipList<uchar> list;
    list.insert('a');
    list.insert('b');
    list.insert('c');
    SkipList<uchar> copy;
    copy.insert('d');
    copy = list;
    std::list<uchar> list_down = list.get_list();
    std::list<uchar> copy_down = copy.get_list();
    are_containers_equal(list_down, copy_down, "3 uchar");
}

void test_move()
{
    std::cout << "     Move test" << '\n';
    SkipList<uchar> list;
    list.insert('a');
    list.insert('b');
    list.insert('c');
    SkipList<uchar> copy;
    copy.insert('d');
    copy = std::move(list);
    std::list<uchar> correct{'a', 'b', 'c'};
    std::list<uchar> empty;
    std::list<uchar> after{'e'};
    std::list<uchar> down = copy.get_list();
    std::list<uchar> list_down = list.get_list();
    list.insert('e');
    std::list<uchar> after_down = list.get_list();
    are_containers_equal(down, correct, "3 uchar");
    are_containers_equal(list_down, empty, "Erase");
    are_containers_equal(after_down, after_down, "After move");
}

void test_statistics()
{
    std::cout << "     Test statistics" << '\n';
    SkipList<uchar> list;
    list.insert('a');
    list.insert('b');
    list.insert('c');
    list.insert('d');
    list.get_statistics();
    list.remove('d');
    list.get_statistics();
}

void test_empty()
{
    std::cout << "     Empty method test" << '\n';
    SkipList<uchar> list;
    is_equal(list.empty(), true, "Empty");
    list.insert('a');
    is_equal(list.empty(), false, "Filled");
    list.remove('a');
    is_equal(list.empty(), true, "Remove");
    list.insert('a');
    SkipList<uchar> copy(std::move(list));
    is_equal(list.empty(), true, "Move");
}

void test_size()
{
    std::cout << "     Size test" << '\n';
    SkipList<uchar> list;
    size_t empty = 0;
    is_equal(list.size(), empty, "Empty");
    list.insert('a');
    list.insert('b');
    list.insert('c');
    size_t filled = 3;
    is_equal(list.size(), filled, "3 uchar");
}

void test_iterator()
{
    std::cout << "     Iterator test" << '\n';
    SkipList<uchar> list;
    list.insert('a');
    list.insert('b');
    list.insert('c');
    SkipIterator<uchar> begin_it = list.begin();
    SkipIterator<uchar> end_it = list.end();
    SkipIterator<uchar> it = begin_it;
    std::list<uchar> result;
    std::list<uchar> correct{'a', 'b', 'c'};
    for(it; it != end_it; it = std::next(it)){
        const uchar value = *it;
        result.push_back(value);
    }
    are_containers_equal(result, correct, "3 uchar");
}

void test_count()
{
    std::cout << "     Count test" << '\n';
    SkipList<uchar> list;
    list.insert('a');
    const size_t zero = list.count('b');
    const size_t zero_correct = 0;
    is_equal(zero, zero_correct, "Zero");
    list.insert('b');
    list.insert('b');
    list.insert('b');
    const size_t filled = list.count('b');
    const size_t correct = 3;
    is_equal(filled, correct, "Filled");
}

void test_search_time()
{
    const ushort repeat = 10000;
    std::list<ushort> sizes{1, 10, 100, 1'000, 10'000};
    std::vector<uint> repeats{1'000'000, 1'000'000, 1'000'000, 100'000, 10'000};
    std::ofstream out;
    TimeMeasure timer;
    out.open("result.txt");
    uchar j = 0;
    for(const ushort size: sizes){
        SkipList<ushort> list;
        for(ushort i = 0; i < size; ++ i){
            list.insert(i);
        }
        const ushort find = size / 2;
        uint repeat = repeats[j];
        timer.start();
        for(uint i = 0; i < repeat; ++ i){
            SkipIterator<ushort> find_it = list.search(find);
        }
        timer.stop();
        const size_t time = timer.elapsed();
        const float average = static_cast<float>(time) / repeat;
        out << size << ' ' << average << '\n';
        ++ j;
    }
    out.close();
}

void test_key_value()
{
    std::cout << "     Key value test" << '\n';
    SkipList<std::pair<const ushort, uchar>> map;
    std::pair<const ushort, uchar> pair1{0, 'a'};
    std::pair<const ushort, uchar> pair2{1, 'b'};
    std::pair<const ushort, uchar> pair3{2, 'c'};
    map.insert(pair1);
    map.insert(pair2);
    map.insert(pair3);
    SkipIterator<std::pair<const ushort, uchar>> it = map.search(pair1);
    const std::pair<const ushort, uchar> find_pair = *it;
    const uchar data = find_pair.second;
    std::cout << data << '\n';
}

int main()
{
    test_default_construction();
    test_iterator_construction();
    test_assignment_construction();
    test_move_construction();
    test_insert();
    test_insert_iterator();
    test_search();
    test_remove();
    test_assignation();
    test_move();
    test_statistics();
    test_empty();
    test_size();
    test_iterator();
    test_count();
    // test_search_time();
    test_key_value();
    return 0;
}
