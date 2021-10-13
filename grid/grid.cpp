#include <iostream>

typedef unsigned char uchar;
typedef unsigned int uint;

class IHandler
{
    public:
        virtual void* value() = 0;
        virtual const void* get_value() const = 0;
        virtual ~IHandler() = default;
};

template<typename t_value>
class TrivialHandler: public IHandler
{
    private:
        t_value value_;
    public:
        TrivialHandler(const t_value value): value_{value}
        {

        }

        void* value() override
        {
            t_value* const ptr_value = &value_;
            void* const ptr_value_as_void = static_cast<void*>(ptr_value);
            return ptr_value_as_void;
        }

        const void* get_value() const override
        {
            const t_value* const ptr_value = &value_;
            const void* const ptr_value_as_void = static_cast<const void*>(ptr_value);
            return ptr_value_as_void;
        }
};

class Any
{
    private:
        IHandler* value_;

        void delete_value()
        {
            delete value_;
        }

    public:
        Any(): value_{NULL}
        {

        }

        template<typename t_value>
        Any(const t_value value): value_{new TrivialHandler<t_value>(value)}
        {

        }

        ~Any()
        {
            delete_value();
        }

        template<typename t_value>
        void set_value(const t_value value)
        {
            delete_value();
            value_ = new TrivialHandler<t_value>(value);
        }

        template<typename t_value>
        const t_value as() const
        {
            IHandler& r_value_interface = *value_;
            TrivialHandler<t_value>& r_value_casted = dynamic_cast<TrivialHandler<t_value>&>(r_value_interface);
            const void* const ptr_value_as_void = r_value_casted.get_value();
            const t_value* const ptr_value = static_cast<const t_value*>(ptr_value_as_void);
            const t_value value = *ptr_value;
            return value;
        }
};

template<typename t_data, typename t_index>
class Grid
{
    private:
        t_data* memory_;
        t_index amount_elements_, x_size_, y_size_;

        void clear()
        {
            for (t_index index = 0; index < amount_elements_; ++ index){
                memory_[index] = 0;
            }
        }

        t_index get_index(t_index x_index, t_index y_index) const
        {
            t_index index = y_index * x_size_ + x_index;
            return index;
        }

    public:
        Grid(t_index x_size, t_index y_size): x_size_{x_size}, y_size_{y_size}
        {
            amount_elements_ = x_size_ * y_size_;
            memory_ = new t_data[amount_elements_];
            clear();
        }

        ~Grid()
        {
            delete[] memory_;
        }

        t_index get_xsize() const
        {
            return x_size_;
        }

        t_index get_ysize() const
        {
            return y_size_;
        }

        Grid& make_subgrid(t_index x_index, t_index y_index, t_index x_sub_size, t_index y_sub_size)
        {
            const t_index index = get_index(x_index, y_index);
            const t_data value = memory_[index];
            Grid<t_data, t_index> subgrid(x_sub_size, y_sub_size);
            subgrid = value;
            Any universal_subgrid(subgrid);
            memory_[index] = universal_subgrid;
            return this;
        }

        t_data operator()(t_index x_index, t_index y_index) const
        {
            t_index index = get_index(x_index, y_index);
            t_data value = memory_[index];
            return value;
        }

        t_data& operator()(t_index x_index, t_index y_index)
        {
            t_index index = y_index * x_size_ + x_index;
            t_data& r_value = memory_[index];
            return r_value;
        }

        Grid& operator =(t_data value)
        {
            for (t_index index = 0; index < amount_elements_; ++ index){
                memory_[index] = value;
            }
            return *this;
        }

        friend std::ostream& operator <<(std::ostream& out, Grid const& grid)
        {
            t_index x_size = grid.get_xsize();
            t_index amount_elements = grid.amount_elements_;
            t_index index_last = amount_elements - 1;
            for (t_index index = 0; index < amount_elements; ++ index){
                t_data value = grid.memory_[index];
                out << value << ' ';
                t_index index_written = index + 1;
                t_index mod = index_written % x_size;

                if (mod == 0 && index < index_last){
                    out << '\n';
                }
            }
            return out;
        }

        friend std::istream& operator >>(std::istream& in, Grid& grid)
        {
            grid.clear();
            t_index amount_elements = grid.amount_elements_;
            t_index index = 0;
            t_data value;
            while(in.good() && index < amount_elements){
                in >> value;
                grid.memory_[index] = value;
                ++ index;
            }
            return in;
        }
};

template<typename t_value>
bool is_equal(const t_value value_1, const t_value value_2)
{
    if (value_1 == value_2){
        return true;
    }
    else{
        return false;
    }
}

class Tester
{
    public:
        template <typename t_data, typename t_index>
        static void test_parenthesis(const t_index x_index, const t_index y_index,
                                     const Grid<t_data, t_index>& grid,
                                     const t_data correct_result,
                                     const std::string message)
        {
            const t_data value = grid(x_index, y_index);
            const bool is_correct = is_equal(value, correct_result);
            if (is_correct){
                std::cout << message << " parenthesis is ok;" << '\n';
            }
            else{
                std::cerr << "----- Error! -----" << '\n';
                std::cerr << message << " parenthesis is bad!" << '\n';
                std::cerr << "----- Error! -----" << '\n';
            }
        }

        template <typename t_data, typename t_index>
        static void test_x_size(const Grid<t_data, t_index>& grid, const t_index correct_result, const std::string message)
        {
            const t_index x_size = grid.get_xsize();
            const bool is_correct = is_equal(x_size, correct_result);
            if (is_correct){
                std::cout << message << " x size is ok;" << '\n';
            }
            else{
                std::cerr << "----- Error! -----" << '\n';
                std::cerr << message << " x size is bad!" << '\n';
                std::cerr << "----- Error! -----" << '\n';
            }
        }

        template <typename t_data, typename t_index>
        static void test_y_size(const Grid<t_data, t_index>& grid, const t_index correct_result, const std::string message)
        {
            const t_index y_size = grid.get_ysize();
            const bool is_correct = is_equal(y_size, correct_result);
            if (is_correct){
                std::cout << message << " y size is ok;" << '\n';
            }
            else{
                std::cerr << "----- Error! -----" << '\n';
                std::cerr << message << " y size is bad!" << '\n';
                std::cerr << "----- Error! -----" << '\n';
            }
        }

        template <typename t_data, typename t_index>
        static void test_assigment(const Grid<t_data, t_index>& grid, const t_data correct_result, const std::string message)
        {
            bool is_correct = true;
            const t_index x_size = grid.get_xsize();
            const t_index y_size = grid.get_ysize();
            for (t_index x = 0; x < x_size; ++ x){
                for (t_index y = 0; y < y_size; ++ y){
                    const t_data value = grid(x, y);
                    if (!is_equal(value, correct_result)){
                        is_correct = false;
                    }
                }
            }
            if (is_correct){
                std::cout << message << " assignment is ok;" << '\n';
            }
            else{
                std::cerr << "----- Error! -----" << '\n';
                std::cerr << message << " assignment is bad!" << '\n';
                std::cerr << "----- Error! -----" << '\n';
            }
        }

        template <typename t_value>
        static void test_any(const t_value result, const t_value correct_result, const std::string message)
        {
            const bool is_correct = is_equal(result, correct_result);
            if (is_correct){
                std::cout << message << " type is ok;" << '\n';
            }
            else{
                std::cerr << "----- Error! -----" << '\n';
                std::cerr << message << " type is bad!" << '\n';
                std::cerr << "----- Error! -----" << '\n';
            }
        }
};

void make_primary_test(Tester tester)
{
    std::cout << "     Primary test" << '\n';
    Grid<float, size_t> one(1, 1);
    size_t x_index_one = 0;
    size_t y_index_one = 0;
    float value_one = 1;
    size_t x_size_one = 1;
    size_t y_size_one = 1;
    one(x_index_one, y_index_one) =  value_one;
    const std::string message_one = "Grid with one sell";
    tester.test_parenthesis(x_index_one, y_index_one, one, value_one, message_one);
    tester.test_x_size(one, x_size_one, message_one);
    tester.test_y_size(one, y_size_one, message_one);
    Grid<float, size_t> edge(2, 2);
    size_t x_index_edge = 1;
    size_t y_index_edge = 0;
    float value_edge = 2;
    size_t x_size_edge = 2;
    size_t y_size_edge = 2;
    edge(x_index_edge, y_index_edge) =  value_edge;
    const std::string message_edge = "Edge of grid";
    tester.test_parenthesis(x_index_edge, y_index_edge, edge, value_edge, message_edge);
    tester.test_x_size(edge, x_size_edge, message_edge);
    tester.test_y_size(edge, y_size_edge, message_edge);
    Grid<float, size_t> middle(3, 3);
    size_t x_index_middle = 1;
    size_t y_index_middle = 1;
    float value_middle = 3;
    size_t x_size_middle = 3;
    size_t y_size_middle = 3;
    middle(x_index_middle, y_index_middle) =  value_middle;
    const std::string message_middle = "Middle of grid";
    tester.test_parenthesis(x_index_middle, y_index_middle, middle, value_middle, message_middle);
    tester.test_x_size(middle, x_size_middle, message_middle);
    tester.test_y_size(middle, y_size_middle, message_middle);
    Grid<double, size_t> double_grid(1, 2);
    size_t x_index_double = 0;
    size_t y_index_double = 1;
    double value_double = 4;
    size_t x_size_double = 1;
    size_t y_size_double = 2;
    double_grid(x_index_double, y_index_double) =  value_double;
    const std::string message_double = "Grid of doubles";
    tester.test_parenthesis(x_index_double, y_index_double, double_grid, value_double, message_double);
    tester.test_x_size(double_grid, x_size_double, message_double);
    tester.test_y_size(double_grid, y_size_double, message_double);
    std::cout << "Debug" << '\n';
}

void make_assignment_test(Tester tester)
{
    std::cout << "     Assigment test" << '\n';
    Grid<float, size_t> one(1, 1);
    float value_one = 1;
    one = value_one;
    const std::string message_one = "Grid with one sell";
    tester.test_assigment(one, value_one, message_one);
    Grid<float, size_t> edge(2, 2);
    float value_edge = 2;
    edge = value_edge;
    const std::string message_edge = "Grid 2 x 2";
    tester.test_assigment(edge, value_edge, message_edge);
    Grid<float, size_t> middle(3, 3);
    float value_middle = 3;
    middle = value_middle;
    const std::string message_middle = "Grid 3 x 3";
    tester.test_assigment(middle, value_middle, message_middle);
    Grid<double, size_t> double_grid(1, 2);
    double value_double = 0.1;
    double_grid = value_double;
    const std::string message_double = "Double grid";
    tester.test_assigment(double_grid, value_double, message_double);
}

void make_test_of_any(Tester tester)
{
    std::cout << "     Test of any" << '\n';
    const uchar value_uchar = 'a';
    Any any(value_uchar);
    const uchar result_uchar = any.as<uchar>();
    tester.test_any(result_uchar, value_uchar, "Uchar");
    const char value_char = 'b';
    any.set_value(value_char);
    const char result_char = any.as<char>();
    tester.test_any(result_char, value_char, "Char");
    const uint value_uint = 1;
    any.set_value(value_uint);
    const uint result_uint = any.as<uint>();
    tester.test_any(result_uint, value_uint, "Uint");
    const int value_int = 2;
    any.set_value(value_int);
    const int result_int = any.as<int>();
    tester.test_any(result_int, value_int, "Int");
    const float value_float = 1;
    any.set_value(value_float);
    const float result_float = any.as<float>();
    tester.test_any(result_float, value_float, "Float");
}

int main(int argc, char** argv)
{
    Tester tester;
    make_primary_test(tester);
    make_assignment_test(tester);
    make_test_of_any(tester);
    return 0;
}
