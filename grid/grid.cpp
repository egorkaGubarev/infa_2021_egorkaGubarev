#include <iostream>

typedef unsigned char uchar;
typedef unsigned int uint;

template<typename t_data, typename t_index>
class Grid
{
    private:
        t_index amount_, x_size_, y_size_;
        t_data value_;
        Grid* memory_;

        t_index get_index(t_index x_index, t_index y_index) const
        {
            t_index index = y_index * x_size_ + x_index;
            return index;
        }

        void clear_inside()
        {
            value_ = get_average();
            const t_index amount = amount_;
            for(t_index i = 0; i < amount; ++ i){
                Grid& element = memory_[i];
                if(element.is_subgrid()){
                    element.clear_inside();
                }
                else{
                    delete[] element.memory_;
                    element.memory_ = nullptr;
                }
            }
        }

        void copy(const Grid& other)
        {
            clear_inside();
            delete[] memory_;
            amount_ = other.amount_;
            x_size_ = other.x_size_;
            y_size_ = other.y_size_;
            memory_ = new Grid[amount_];
            for(uint i = 0; i < amount_; ++ i){
                Grid& element = memory_[i];
                const Grid& other_element = other.memory_[i];
                if(other_element.is_subgrid()){
                    element.copy(other_element);
                }
                else{
                    element.amount_ = 0;
                    element.x_size_ = 0;
                    element.y_size_ = 0;
                    element.value_ = other_element.value_;
                }
            }
        }

    public:
        Grid(t_index x_size, t_index y_size, t_data value): x_size_{x_size}, y_size_{y_size}, value_{value}
        {
            amount_ = x_size_ * y_size_;
            if(amount_ == 0){
                memory_ = nullptr;
            }
            else{
                memory_ = new Grid[amount_];
            }
        }

        Grid(): Grid<t_data, t_index>(0, 0, 0)
        {

        }

        Grid(t_index x_size, t_index y_size): Grid<t_data, t_index>(x_size, y_size, 0)
        {

        }

        Grid(const Grid& other): Grid()
        {
            copy(other);
        }

        ~Grid()
        {
            clear_inside();
            delete[] memory_;
            memory_ = nullptr;
        }

        t_data get_average() const
        {
            if(amount_ == 0){
                return 0;
            }
            else{
                t_data sum = 0;
                for(t_index i = 0; i < amount_; ++ i){
                    t_data number;
                    const Grid& element = memory_[i];
                    if(element.is_subgrid()){
                        number = element.get_average();
                    }
                    else{
                        number = element.value_;
                    }
                    sum += number;
                }
                const t_data average = sum / amount_;
                return average;
            }
        }

        t_index get_xsize() const
        {
            return x_size_;
        }

        t_index get_ysize() const
        {
            return y_size_;
        }

        t_index get_value() const
        {
            return value_;
        }

        bool is_subgrid() const
        {
            if(memory_ == nullptr){
                return false;
            }
            else{
                return true;
            }
        }

        Grid& make_subgrid(const t_data x_index, const t_data y_index, const t_data x_sub_size, const t_data y_sub_size)
        {
            collapse_subgrid(x_index, y_index);
            const Grid<t_data, t_index>& this_const_ref = *this;
            const t_data value = this_const_ref(x_index, y_index);
            const t_index index = get_index(x_index, y_index);
            Grid& insert = memory_[index];
            insert.x_size_ = x_sub_size;
            insert.y_size_ = y_sub_size;
            const t_index amount = x_sub_size * y_sub_size;
            insert.amount_ = amount;
            insert.memory_ = new Grid[amount];
            insert = value;
            return *this;
        }

        Grid& collapse_subgrid(const t_data x_index, const t_data y_index)
        {
            const t_index index = get_index(x_index, y_index);
            const Grid& this_grid = *this;
            const t_data value = this_grid(x_index, y_index);
            Grid& element = memory_[index];
            element.clear_inside();
            delete[] element.memory_;
            element.memory_ = nullptr;
            element.amount_ = 0;
            element.x_size_ = 0;
            element.y_size_ = 0;
            element.value_ = value;
            return *this;
        }

        Grid& get_subgrid(const t_data x_index, const t_data y_index)
        {
            const t_index index = get_index(x_index, y_index);
            Grid& subgrid = memory_[index];
            return subgrid;
        }

        const Grid& get_subgrid(const t_data x_index, const t_data y_index) const
        {
            const t_index index = get_index(x_index, y_index);
            const Grid& subgrid = memory_[index];
            return subgrid;
        }

        t_data operator()(t_index x_index, t_index y_index) const
        {
            t_index index = get_index(x_index, y_index);
            t_data value;
            const Grid& element = memory_[index];
            if(element.is_subgrid()){
                value = element.get_average();
            }
            else{
                value = element.value_;
            }
            return value;
        }

        Grid& operator()(t_index x_index, t_index y_index)
        {
            t_index index = y_index * x_size_ + x_index;
            Grid& element = memory_[index];
            return element;
        }

        Grid& operator=(t_data value)
        {
            if(this->is_subgrid()){
                for (t_index index = 0; index < amount_; ++ index){
                    Grid& element = memory_[index];
                    if(element.is_subgrid()){
                        element = value;
                    }
                    else{
                        element.value_ = value;
                    }
                }
            }
            else{
                value_ = value;
            }
            return *this;
        }

        friend std::ostream& operator<<(std::ostream& out, const Grid& grid)
        {
            t_index x_size = grid.get_xsize();
            t_index amount_elements = grid.amount_;
            t_index index_last = amount_elements - 1;
            for (t_index index = 0; index < amount_elements; ++ index){
                const Grid& element = grid.memory_[index];
                if (!element.is_subgrid()){
                    const t_data value = element.value_;
                    out << value;
                }
                else{
                    const t_data average = element.get_average();
                    std::cout << average;
                }
                out << ' ';
                t_index index_written = index + 1;
                t_index mod = index_written % x_size;
                if (mod == 0 && index < index_last){
                    out << '\n';
                }
            }
            return out;
        }

        friend std::istream& operator>>(std::istream& in, Grid& grid)
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
bool is_equal(const t_value& value_1, const t_value& value_2)
{
    if (value_1 == value_2){
        return true;
    }
    else{
        return false;
    }
}

template<>
bool is_equal(const double& value_1, const double& value_2)
{
    const double error = 0.00001;
    const double delta = value_2 - value_1;
    const double abs = std::abs(delta);
    if (abs < error){
        return true;
    }
    else{
        return false;
    }
}

template<typename t_value>
void test(const t_value& value_1, const t_value& value_2, const std::string& message)
{
    const bool equal = is_equal(value_1, value_2);
    if(equal){
        std::cout << message << " is ok;" << '\n';
    }
    else{
        std::cerr << "----- Error! -----" << '\n';
        std::cerr << message << " is bad!" << '\n';
        std::cerr << "----- Error! -----" << '\n';
    }
}

void average_test()
{
    std::cout << "     Average test" << '\n';
    const Grid<float, uchar> empty;
    test(empty.get_average(), 0.0f, "Empty");
    Grid<float, uchar> one(1, 1);
    test(one.get_average(), 0.0f, "One");
    one = 0.1;
    test(one.get_average(), 0.1f, "Assined");
    Grid<double, char> big(3, 4);
    test(big.get_average(), 0.0, "Big");
    big = 0.2;
    test(big.get_average(), 0.2, "Big assined");
    big.make_subgrid(1, 2, 5, 6);
    Grid<double, char>& subgrid = big.get_subgrid(1, 2);
    subgrid = 0.3;
    test(big.get_average(), 0.20833, "Subgrid");
    subgrid(1, 2) = 0.4;
    test(big.get_average(), 0.20861, "Various");
    subgrid.make_subgrid(3, 4, 7, 8);
    Grid<double, char>& second_level = subgrid.get_subgrid(3, 4);
    second_level = 0.5;
    test(big.get_average(), 0.20917, "Second level");
    subgrid.collapse_subgrid(3, 4);
    test(big.get_average(), 0.20917, "Collapse");
    big.collapse_subgrid(1, 2);
    test(big.get_average(), 0.20917, "Primary");

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

void test_copy()
{
    std::cout << "     Copy test" << '\n';
    Grid<float, size_t> my_grid(3, 4);
    my_grid = 0.1;
    my_grid.make_subgrid(1, 2, 5, 6);
    Grid<float, size_t>& subgrid = my_grid.get_subgrid(1, 2);
    subgrid = 0.2;
    subgrid(3, 4) = 0.3;
    Grid<float, size_t> copy(my_grid);
    const Grid<float, size_t>& copy_const_ref = copy;
    const Grid<float, size_t>& my_grid_const_ref = my_grid;
    test(copy_const_ref(0, 0), my_grid_const_ref(0, 0), "Copy from main grid");
    test(copy_const_ref(1, 2), my_grid_const_ref(1, 2), "Average copy from subgrid");
    const Grid<float, size_t>& copy_subgrid_const_ref = copy.get_subgrid(1, 2);
    const Grid<float, size_t>& subgrid_const_ref = subgrid;
    test(subgrid_const_ref(3, 4), copy_subgrid_const_ref(3, 4), "Exact copy from subgrid");
    std::cout << "If there's no warnings or exceptions, destruction of grid with subgrid inside has gone successfully" << '\n';
}

int main(int argc, char** argv)
{
    Tester tester;
    make_primary_test(tester);
    make_assignment_test(tester);
    average_test();
    test_copy();
    return 0;
}
