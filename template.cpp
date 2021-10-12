#include <iostream>
#include <string>
#include <vector>

typedef unsigned char uchar;
typedef unsigned int uint;

template <typename t_number>
bool is_equal(const t_number number_1, const t_number number_2)
{
    if (number_1 == number_2){
        return true;
    }
    else{
        return false;
    }
}

template <>
bool is_equal(const double number_1, const double number_2)
{
    const double error_max = 0.0001;
    const double delta = number_2 - number_1;
    const double delta_abs = std::abs(delta);
    if (delta_abs < error_max){
        return true;
    }
    else{
        return false;
    }
}

template <typename t_value, typename t_index>
t_index search(const t_value target, const t_index begin, const t_index end)
{
    t_index index = begin;
    for (index; index != end; ++ index){
        t_value value = *index;
        if (is_equal(value, target)){
            return index;
        }
    }
    return index;
}

template <typename t_index>
t_index make_hoare_partition(const t_index begin, const t_index end)
{
    const uint size = std::distance(begin, end);
    const uint half_size = size / 2;
    t_index base_index = begin;
    std::advance(base_index, half_size);
    auto base_value = *base_index;
    t_index begin_to_end = begin;
    t_index end_to_begin = end;
    while (true){
        auto value_1 = *begin_to_end;
        while (value_1 < base_value){
            ++ begin_to_end;
            value_1 = *begin_to_end;
        }
        auto value_2 = *end_to_begin;
        while (value_2 > base_value){
            -- end_to_begin;
            value_2 = *end_to_begin;
        }
        if (begin_to_end >= end_to_begin){
            return end_to_begin;
        }
        auto swap = *begin_to_end;
        *begin_to_end = *end_to_begin;
        *end_to_begin = swap;
        ++ begin_to_end;
        -- end_to_begin;
    }
}

template <typename t_index>
void sort(const t_index begin, const t_index end)
{
    if (begin < end){
        const t_index partition = make_hoare_partition(begin, end - 1);
        sort(begin, partition);
        const t_index partition_next = partition + 1;
        sort(partition_next, end);
    }
}

template <typename t_value, typename t_key>
class Dictionary
{
    private:
        const std::vector<t_key> keys_;
        const std::vector<t_value> values_;
    public:
        Dictionary(std::vector<t_key> keys, std::vector<t_value> values): keys_{keys}, values_{values}
        {

        }

        t_value get_value(t_key key)
        {
            auto begin = keys_.begin();
            auto end = keys_.end();
            auto key_found = search(key, begin, end);
            uint distance = std::distance(begin, key_found);
            t_value value_found = values_[distance];
            return value_found;
        }
};

class Tester
{
    public:
        template <typename t_value, typename t_index>
        static void test_search(const t_value target, const t_index begin, const t_index end, const t_index correct_result, const std::string message)
        {
            const t_index result = search(target, begin, end);
            if (result == correct_result){
                std::cout << message << " is ok;" << '\n';
            }
            else{
                std::cerr << "----- Error! -----" << '\n';
                std::cerr << message << " is bad;" << '\n';
                std::cerr << "----- Error! -----" << '\n';
            }
        }

        template <typename t_value, typename t_index>
        static void test_sort(const t_index begin, const t_index end, const std::vector<t_value> correct_result, const std::string message)
        {
            const uint size = end - begin;
            std::vector<t_value> result(size);
            sort(begin, end);
            t_index index = begin;
            uint position = 0;
            while (index != end){
                result[position] = *index;
                ++ index;
                ++ position;
            }
            if (result == correct_result){
                std::cout << message << " is ok;" << '\n';
            }
            else{
                std::cerr << "----- Error! -----" << '\n';
                std::cerr << message << " is bad;" << '\n';
                std::cerr << "----- Error! -----" << '\n';
            }
        }

        template <typename t_value, typename t_key>
        static void test_dictionary(Dictionary<t_value, t_key> dictionary, const t_key key, const t_value correct_result, const std::string message)
        {
            t_value result = dictionary.get_value(key);
            if (result == correct_result){
                std::cout << message << " is ok;" << '\n';
            }
            else{
                std::cerr << "----- Error! -----" << '\n';
                std::cerr << message << " is bad;" << '\n';
                std::cerr << "----- Error! -----" << '\n';
            }
        }
};

void test_search(Tester tester)
{
    std::cout << "     Search test" << '\n';
    std::vector<uchar> data_empty;
    auto ptr_begin_empty = data_empty.begin();
    auto ptr_end_empty = data_empty.end();
    tester.test_search(0, ptr_begin_empty, ptr_end_empty, ptr_end_empty, "Empty vector");
    std::vector<uchar> data_uchar{0, 1};
    uchar* ptr_begin_uchar = &data_uchar[0];
    uchar* ptr_end_uchar = &data_uchar[1] + 1;
    tester.test_search(0, ptr_begin_uchar, ptr_end_uchar, ptr_begin_uchar, "Begin of vector of uchar");
    tester.test_search(1, ptr_begin_uchar, ptr_end_uchar, ptr_end_uchar - 1, "End of vector of uchar");
    tester.test_search(2, ptr_begin_uchar, ptr_end_uchar, ptr_end_uchar, "Invalid in vector of uchar");
    std::vector<char> data_char{0, 1};
    char* ptr_begin_char = &data_char[0];
    char* ptr_end_char = &data_char[1] + 1;
    tester.test_search(0, ptr_begin_char, ptr_end_char, ptr_begin_char, "Begin of vector of char");
    tester.test_search(1, ptr_begin_char, ptr_end_char, ptr_end_char - 1, "End of vector of char");
    tester.test_search(2, ptr_begin_char, ptr_end_char, ptr_end_char, "Invalid in vector of char");
    std::vector<int> data_int{0, 1};
    int* ptr_begin_int = &data_int[0];
    int* ptr_end_int = &data_int[1] + 1;
    tester.test_search(0, ptr_begin_int, ptr_end_int, ptr_begin_int, "Begin of vector of int");
    tester.test_search(1, ptr_begin_int, ptr_end_int, ptr_end_int - 1, "End of vector of int");
    tester.test_search(2, ptr_begin_int, ptr_end_int, ptr_end_int, "Invalid in vector of int");
    std::vector<double> data_double{0, 1};
    double* ptr_begin_double = &data_double[0];
    double* ptr_end_double = &data_double[1] + 1;
    tester.test_search(0, ptr_begin_double, ptr_end_double, ptr_begin_double, "Begin of vector of double");
    tester.test_search(1, ptr_begin_double, ptr_end_double, ptr_end_double - 1, "End of vector of double");
    tester.test_search(1.00005, ptr_begin_double, ptr_end_double, ptr_end_double - 1, "Almost equal values in vector of double");
    tester.test_search(2, ptr_begin_double, ptr_end_double, ptr_end_double, "Invalid in vector of double");
}

void test_sort(Tester tester)
{
    std::cout << "     Sort test" << '\n';
    std::vector<uchar> empty_uchar;
    auto ptr_begin_empty_uchar = empty_uchar.begin();
    auto ptr_end_empty_uchar = empty_uchar.end();
    tester.test_sort(ptr_begin_empty_uchar, ptr_end_empty_uchar, empty_uchar, "Empty vector of uchar");
    std::vector<uchar> sorted_uchar_one{0};
    auto ptr_begin_sorted_uchar_one = sorted_uchar_one.begin();
    auto ptr_end_sorted_uchar_one = sorted_uchar_one.end();
    tester.test_sort(ptr_begin_sorted_uchar_one, ptr_end_sorted_uchar_one, sorted_uchar_one, "Vector of uchar of one element");
    std::vector<uchar> sorted_uchar{0, 1, 2};
    auto ptr_begin_sorted_uchar = sorted_uchar.begin();
    auto ptr_end_sorted_uchar = sorted_uchar.end();
    tester.test_sort(ptr_begin_sorted_uchar, ptr_end_sorted_uchar, sorted_uchar, "Sorted vector of uchar");
    std::vector<uchar> vector_uchar{2, 0, 1};
    auto ptr_begin_vector_uchar = vector_uchar.begin();
    auto ptr_end_vector_uchar = vector_uchar.end();
    tester.test_sort(ptr_begin_vector_uchar, ptr_end_vector_uchar, std::vector <uchar> {0, 1, 2}, "Vector of uchar");
    std::vector<double> empty_double;
    auto ptr_begin_empty_double = empty_double.begin();
    auto ptr_end_empty_double = empty_double.end();
    tester.test_sort(ptr_begin_empty_double, ptr_end_empty_double, empty_double, "Empty vector of double");
    std::vector<double> sorted_double_one{0};
    auto ptr_begin_sorted_double_one = sorted_double_one.begin();
    auto ptr_end_sorted_double_one = sorted_double_one.end();
    tester.test_sort(ptr_begin_sorted_double_one, ptr_end_sorted_double_one, sorted_double_one, "Vector of double of one element");
    std::vector<double> sorted_double{0, 1, 2};
    auto ptr_begin_sorted_double = sorted_double.begin();
    auto ptr_end_sorted_double = sorted_double.end();
    tester.test_sort(ptr_begin_sorted_double, ptr_end_sorted_double, sorted_double, "Sorted vector of double");
    std::vector<double> vector_double{2, 0, 1};
    auto ptr_begin_vector_double = vector_double.begin();
    auto ptr_end_vector_double = vector_double.end();
    tester.test_sort(ptr_begin_vector_double, ptr_end_vector_double, std::vector <double> {0, 1, 2}, "Vector of double");
}

void test_search_dictionary(Tester tester)
{
    std::cout << "     Dictionary test" << '\n';
    Dictionary <uchar, uchar> dictionary_uchar (std::vector <uchar> {'a', 'b', 'c'}, std::vector <uchar> {'d', 'e', 'f'});
    const uchar key_uchar_1 = 'a';
    const uchar value_uchar_1 = 'd';
    const uchar key_uchar_middle = 'b';
    const uchar value_uchar_middle = 'e';
    const uchar key_uchar_end = 'c';
    const uchar value_uchar_end = 'f';
    tester.test_dictionary(dictionary_uchar, key_uchar_1, value_uchar_1, "1-st element in dictionary of uchar");
    tester.test_dictionary(dictionary_uchar, key_uchar_middle, value_uchar_middle, "Middle element in dictionary of uchar");
    tester.test_dictionary(dictionary_uchar, key_uchar_end, value_uchar_end, "End element in dictionary of uchar");
    Dictionary <double, double> dictionary_double (std::vector <double> {0.1, 0.2, 0.3}, std::vector <double> {0.4, 0.5, 0.6});
    const double key_double_1 = 0.1;
    const double value_double_1 = 0.4;
    const double key_double_middle = 0.2;
    const double value_double_middle = 0.5;
    const double key_double_end = 0.3;
    const double value_double_end = 0.6;
    tester.test_dictionary(dictionary_double, key_double_1, value_double_1, "1-st element in dictionary of double");
    tester.test_dictionary(dictionary_double, key_double_middle, value_double_middle, "Middle element in dictionary of double");
    tester.test_dictionary(dictionary_double, key_double_end, value_double_end, "End element in dictionary of double");
}

int main(int argc, char** argv)
{
    Tester tester;
    test_search(tester);
    test_sort(tester);
    test_search_dictionary(tester);
    return 0;
}
