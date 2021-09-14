#include <chrono>
#include <iostream>
#include <random>
#include <vector>

typedef unsigned int uchar;
typedef unsigned int uint;

class DemoStr
{
    public:
        DemoStr(): data_{nullptr}, length_{0}, capacity_{0}
        {

        }

        DemoStr(DemoStr const& other): length_{other.length_}, capacity_{other.capacity_}
        {
            allocate(other.length_);
            copy_from(other.data_, 0, other.length_);
        }

        DemoStr(char const* raw_str, size_t length): length_{length}, capacity_{length}
        {
            allocate(length);
            copy_from(raw_str, 0, length);
            reformat();
        }

        explicit DemoStr(int value)
        {
            length_ = get_value_length(value);
            capacity_ = length_;
            allocate(length_);
            auto it = &data_[length_ - 1];
            while (value > 0) {
                *it = static_cast<char>(value % 10) + '0';
                -- it;
                value /= 10;
            }
        }

        void print() const
        {
            for (auto i = 0u; i < length_; ++ i){
                std::cout << data_[i];
            }
            std::cout << '\n';
        }

        // index <= length_ - 1
        char at(size_t index) const
        {
            return data_[index];
        }

        bool equal(DemoStr const& other) const
        {
            if (other.length_ != length_){
                return false;
            }
            for (auto i = 0u; i < length_; ++ i){
                if (data_[i] != other.data_[i]) {
                    return false;
                }
            }
            return true;
        }

        void read_from(std::istream& in)
        {
            char tmp;
            while (in >> tmp) {
                if (length_ >= capacity_){
                    reallocate((capacity_ == 0) ? 1 : capacity_ * 2);
                }
                data_[length_] = tmp;
                ++ length_;
            }
            reformat();
        }

        void shrink_to_fit()
        {
            reallocate(length_);
        }

        ~DemoStr()
        {
            clear();
        }

        void clear()
        {
            delete[] data_;
            data_ = nullptr;
            length_ = 0;
            capacity_ = 0;
        }

        char* get_data() const
        {
            return data_;
        }

    protected:
        void set_capacity(uint new_capacity)
        {
            capacity_ = new_capacity;
        }

        void set_data(uchar bit, uint index)
        {
            data_[index] = bit;
        }

        uint get_capacity() const
        {
            return capacity_;
        }

        void set_length(uint lenght_new)
        {
            length_ = lenght_new;
        }

        uint  get_length() const
        {
            return length_;
        }

        void reallocate(size_t n)
        {
            auto old_data = data_;
            data_ = new char[n];
            capacity_ = n;
            copy_from(old_data, 0, std::min(n, length_));
            delete[] old_data;
        }

        void reformat()
        {
            for (auto i = 0u; i < length_; ++ i){
                data_[i] = (i % 2 == 1) ? std::tolower(data_[i]) : std::toupper(data_[i]);
            }
        }

        size_t get_value_length(int value) const
        {
            auto len = 0ull;
            while (value > 0) {
                value /= 10;
                ++ len;
            }
            return len;
        }

        void allocate(size_t n)
        {
            data_ = new char[n];
        }

        void copy_from(char const* data, uint start, size_t n)
        {
            uint end = start + n;
            for (auto i = start; i < end; ++ i){
                uint index_old = i - start;
                data_[i] = data[index_old];
            }
        }

        friend std::ostream& operator << (std::ostream& out, DemoStr const& v)
        {
            for (auto i = 0u; i < v.length_; ++ i)
            {
                out << v.data_[i];
            }
            return out;
        }

    private:
        char *data_ = nullptr;
        size_t length_ = 0, capacity_ = 0;
};

class RandomGenerator
{
    public:
        static std::vector<int>create_sequence(size_t n, int min, int max)
        {
            std::mt19937_64 engine(std::random_device{}());
            std::uniform_int_distribution<int>distr(min, max);
            std::vector<int>v(n);
            for (auto i = 0u; i < n; ++ i){
                v[i] = distr(engine);
            }
            return v;
        }
    private:

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

class Str: public DemoStr
{
    public:
        using DemoStr::DemoStr;

        Str(std::vector<int>source, uint size)
        {
            set_length(size);
            set_capacity(size);
            allocate(size);
            for (uint index = 0; index < size; index ++){
                set_data(source[index], index);
            }
        }

        void append(Str const& other)
        {
            uint length = get_length();
            uint length_other = other.get_length();
            uint length_new = length + length_other;
            reallocate(length_new);
            char* data_other = other.get_data();
            copy_from(data_other, length, length_other);
            set_length(length_new);
        }

        Str copy(uint start, uint end) const
        {
            char* data = get_data();
            char* data_new_start = data + start;
            uint length = end - start;
            Str str_copied(data_new_start, length);
            return str_copied;
        }

        std::vector<Str> split(uchar separator) const
        {
            uint start = 0;
            uint length = get_length();
            char* data = get_data();
            std::vector<Str>str_splitted_vector;
            uchar separator_low = std::tolower(separator);
            for (uint index = 0; index < length; index ++){
                uchar element = data[index];
                uchar element_low = std::tolower(element);
                if (element_low == separator_low){
                    Str str_splitted(copy(start, index));
                    str_splitted_vector.push_back(str_splitted);
                    start = index + 1;
                }
            }
            Str str_splitted(copy(start, length));
            str_splitted_vector.push_back(str_splitted);
            return str_splitted_vector;
        }
};

int main(int argc, char** argv)
{
    uint length_append = 1;
    const uchar length_base = 1;
    const uint length_max = 1000000000;
    const uchar length_start = 1;
    const uchar max = 1;
    const uchar min = -1;
    const uchar step = 2;
    uint elapsed, length;
    char* raw = new char[length_start];
    raw[0] = '0';
    Str str(raw, length_start);
    TimeMeasure timer;
    while (length_append < length_max)
    {
        std::vector<int>string = RandomGenerator::create_sequence(length_append, min, max);
        Str string_append(string, length_append);
        timer.start();
        str.append(string_append);
        timer.stop();
        elapsed = timer.elapsed();
        std::cout << elapsed << '\n';
        length_append *= step;
    }
    delete[] raw;
    raw = NULL;
    return 0;
}
