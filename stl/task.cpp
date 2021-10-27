#include <chrono>
#include <deque>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>

typedef unsigned char uchar;
typedef unsigned int uint;

class TimeMeasure
{
    private:
        std::chrono::high_resolution_clock::time_point start_time_, stop_time_;
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
};

template <typename t_container>
class Tester
{
    private:
        const float step_;
        const uint elements_amount_max_, elements_amount_min_;

        static const typename t_container::iterator get_iterrator_end(t_container& my_vector)
        {
            const typename t_container::iterator it_end = my_vector.end();
            return it_end;
        }

        static const typename t_container::iterator get_iterrator_middle(t_container& my_vector)
        {
            typename t_container::iterator it = my_vector.begin();
            const typename t_container::iterator it_end = my_vector.end();
            const uint distance = std::distance(it, it_end);
            const uint middle = distance / 2;
            std::advance(it, middle);
            return it;
        }

        void make_test(const uint tests_amount, const std::string& path, const typename t_container::iterator get_iterrator(t_container&)) const
        {
            t_container my_container;
            TimeMeasure timer;
            std::ofstream out;
            out.open(path);
            for (uint elements_amount = elements_amount_min_; elements_amount < elements_amount_max_; elements_amount *= step_){
                my_container.clear();
                my_container.resize(elements_amount);
                size_t elapsed_total = 0;
                timer.start();
                for (uint test_number = 0; test_number < tests_amount; ++ test_number){
                    const typename t_container::iterator iterrator = get_iterrator(my_container);
                    my_container.insert(iterrator, 0);
                }
                timer.stop();
                const size_t elapsed = timer.elapsed();
                const double elapsed_total_double = static_cast<const double>(elapsed);
                const double average = elapsed_total_double / tests_amount;
                out << elements_amount << ';' << average << '\n';
                std::cout << "Size " << elements_amount << " is tested" << '\n';
            }
            out.close();
        }

    public:
        Tester(const float step, const uint elements_amount_min, const uint elements_amount_max): step_{step}, elements_amount_min_{elements_amount_min},
                                                                                                        elements_amount_max_{elements_amount_max}
        {

        }

        void test_insert_back(const std::string& path) const
        {
            std::cout << "Container insert back test is started" << '\n';
            const uint tests_amount = 1000000;
            make_test(tests_amount, path, get_iterrator_end);
            std::cout << "Container insert back test is done" << '\n';
        }

        void test_insert_middle(const std::string& path)
        {
            std::cout << "Container insert back test is started" << '\n';
            const uint tests_amount = 2000;
            make_test(tests_amount, path, get_iterrator_middle);
            std::cout << "Container insert middle test is done";
        }
};

template <typename t_container, typename t_data>
const t_data transform_reduse(const typename t_container::iterator it_begin, const typename t_container::iterator it_end, const t_data& initial_value,
                               const t_data(*unary_function)(const t_data&), const t_data(*binary_function)(const t_data&, const t_data&))
{
    t_data element;
    t_data partial_result;
    t_data result = initial_value;
    for(typename t_container::iterator it = it_begin; it != it_end; it = std::next(it)){
        element = *it;
        partial_result = unary_function(element);
        result = binary_function(result, partial_result);
    }
    return result;
}

template<typename t_data>
const t_data do_nothig(const t_data& arg)
{
    return arg;
}

template <typename t_data>
const t_data get_square(const t_data& arg)
{
    const t_data square = std::pow(arg, 2);
    return square;
}

template<typename t_data>
const t_data add(const t_data& arg1, const t_data& arg2)
{
    const t_data sum = arg1 + arg2;
    return sum;
}

template<typename t_data>
const t_data multiply(const t_data& arg1, const t_data& arg2)
{
    const t_data product = arg1 * arg2;
    return product;
}

class UnitTest
{
    public:
        template<typename t_data>
        static void compare(const t_data& result, const t_data& correct_result, const std::string& message)
        {
            if (result == correct_result){
                std::cout << message << " is ok!" << '\n';
             }
             else{
                std::cerr << "----- Error! -----" << '\n';
                std::cerr << message << " is bad!" << '\n';
                std::cerr << "----- Error! -----" << '\n';
             }
        }
};

void test_transform()
{
    UnitTest tester;
    std::vector<uchar> vector_sum{1, 2, 3, 4, 5, 6, 7, 8};
    std::deque<uint> deque_product{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    const std::vector<uchar>::iterator it_begin_sum = vector_sum.begin();
    const std::vector<uchar>::iterator it_end_sum = vector_sum.end();
    const std::deque<uint>::iterator it_begin_product = deque_product.begin();
    const std::deque<uint>::iterator it_end_product = deque_product.end();
    const std::string message_sum = "Sum in vector";
    const std::string message_product = "Product in deque";
    const std::string message_sum_square = "Sum of squares in vector";
    const uchar initial_sum = 0;
    const uint initial_product = 1;
    const uchar(*const pt_nothing_uchar)(const uchar&) = &do_nothig<uchar>;
    const uint(*const pt_nothing_uint)(const uint&) = &do_nothig<uint>;
    const uchar(*const pt_add)(const uchar&, const uchar&) = &add<uchar>;
    const uint(*const pt_multiply)(const uint&, const uint&) = &multiply<uint>;
    const uchar(*const pt_get_square)(const uchar&) = &get_square<uchar>;
    const uchar result_sum = transform_reduse<std::vector<uchar>, uchar>(it_begin_sum, it_end_sum, initial_sum, pt_nothing_uchar, pt_add);
    const uint result_product = transform_reduse<std::deque<uint>, uint>(it_begin_product, it_end_product, initial_product, pt_nothing_uint, pt_multiply);
    const uchar result_sum_square = transform_reduse<std::vector<uchar>, uchar>(it_begin_sum, it_end_sum, initial_sum, pt_get_square, pt_add);
    const uchar correct_result_sum = 36;
    const uint correct_result_product = 3628800;
    const uchar correct_result_sum_square = 204;
    tester.compare(result_sum, correct_result_sum, message_sum);
    tester.compare(result_product, correct_result_product, message_product);
    tester.compare(result_sum_square, correct_result_sum_square, message_sum_square);
}

void make_benchmark()
{
    const float step = 1.1;
    const uint elements_amount_min = 1000000;
    const uint elements_amount_max = 7000000;
    const std::string vector_back = "vector_insert_back.txt";
    const std::string vector_middle = "vector_insert_middle.txt";
    const std::string deque_back = "deque_insert_back.txt";
    const std::string deque_middle = "deque_insert_middle.txt";
    Tester<std::vector<uchar>> vector_tester(step, elements_amount_min, elements_amount_max);
    Tester<std::deque<uchar>> deque_tester(step, elements_amount_min, elements_amount_max);
    vector_tester.test_insert_back(vector_back);
    vector_tester.test_insert_middle(vector_middle);
    deque_tester.test_insert_back(deque_back);
    deque_tester.test_insert_middle(deque_middle);
}

int main(const int argc, const char** const argv)
{
    test_transform();
    make_benchmark();
    return 0;
}
