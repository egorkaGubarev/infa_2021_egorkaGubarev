#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

typedef unsigned char uchar;
typedef unsigned int uint;

class Polinom
{
    private:
        uint size_, resize_factor_;
        int coefficient_, power_;
        int* const ptr_coefficient_;
        int* const ptr_power_;
        int* ptr_read_number_;
        bool is_cap_read_, is_coefficient_explicit_, is_coefficient_ready_, is_positive_, is_x_read_;
        std::string string_;
        std::vector<int>coefficients_;

        void apply_sign_to_previous_coefficient()
        {
            if (!is_positive_){
                coefficient_ = -coefficient_;
            }
        }

        void get_coefficients_from_string()
        {
            for (uchar symbol: string_){
                if (symbol == '+'){
                    process_previous_coefficient();
                    is_positive_ = true;
                    prepare_to_get_next_coefficient();
                }
                if (symbol == '-'){
                    process_previous_coefficient();
                    is_positive_ = false;
                    prepare_to_get_next_coefficient();
                }
                if (symbol >= '0' && symbol <= '9'){
                    get_digit(symbol);
                    if (ptr_read_number_ == ptr_coefficient_){
                        is_coefficient_explicit_ = true;
                    }
                }
                if (symbol == 'x'){
                    is_coefficient_ready_ = true;
                    is_x_read_ = true;
                }
                if (symbol == '^'){
                    is_cap_read_ = true;
                    ptr_read_number_ = ptr_power_;
                }
            }
            process_previous_coefficient();
        }

        void get_digit(uchar symbol)
        {
            uint number = symbol - '0';
            *ptr_read_number_ = *ptr_read_number_ * 10 + number;
        }

        static uint get_digits_amount(const int number)
        {
            if (number == 0){
                return 1;
            }
            int copy_number = number;
            uint digits_amount = 0;
            if (copy_number < 0){
                copy_number *= -1;
                digits_amount ++;
            }
            while(copy_number > 0){
                copy_number /= 10;
                digits_amount ++;
            }
            return digits_amount;
        }

        void make_coefficient_explicit()
        {
            if (!is_coefficient_explicit_){
                coefficient_ = 1;
            }
        }

        void make_previous_power_explicit()
        {
            if (is_x_read_ && !is_cap_read_){
                power_ = 1;
            }
            if (!is_x_read_ && !is_cap_read_){
                power_ = 0;
            }
        }

        void prepare_coefficients_vector()
        {
            size_ = string_.size();
            coefficients_.resize(size_);
        }

        void prepare_to_get_next_coefficient()
        {
            coefficient_ = 0;
            power_ = 0;
            is_cap_read_ = false;
            is_coefficient_explicit_ = false;
            is_x_read_ = false;
            ptr_read_number_ = ptr_coefficient_;
        }

        void process_previous_coefficient()
        {
            if (is_coefficient_ready_){
                make_coefficient_explicit();
                apply_sign_to_previous_coefficient();
            }
            make_previous_power_explicit();
            is_coefficient_ready_ = true;
            write_previous_coefficient_to_vector();
        }

        void resize_coefficients_vector_if_needed()
        {
            if (size_ <= power_){
                uint new_size = resize_factor_ * power_;
                uint new_size_checked = std::max(size_, new_size);
                coefficients_.resize(new_size_checked);
                size_ = new_size_checked;
            }
        }

        void write_previous_coefficient_to_vector()
        {
            if (is_coefficient_ready_){
                resize_coefficients_vector_if_needed();
                coefficients_[power_] += coefficient_;
            }
        }

    public:
        Polinom(const std::string polinom): size_{0}, resize_factor_{2},
                                            coefficient_{0}, power_{0},
                                            ptr_coefficient_{&coefficient_}, ptr_power_{&power_}, ptr_read_number_{ptr_coefficient_},
                                            coefficients_{0},
                                            is_cap_read_{false}, is_coefficient_explicit_{false}, is_coefficient_ready_{false}, is_positive_{true}, is_x_read_{false},
                                            string_{polinom}
        {
            prepare_coefficients_vector();
            get_coefficients_from_string();
        }

        friend std::istream& operator>>(std::istream& in, Polinom& polinom)
        {
            std::getline(in, polinom.string_);
            return in;
        }

        friend std::ostream& operator<<(std::ostream& out, Polinom const& polinom)
        {
            uint power = 0;
            for (int coefficient: polinom.coefficients_){
                if (coefficient != 0){
                    out << coefficient << "x^" << power << '\n';
                }
                power ++;
            }
            return out;
        }

        std::vector<int> get_coefficients() const
        {
            return coefficients_;
        }

        void print()
        {
            std::ostringstream coefficients, powers;
            for (int power = size_ - 1; power >= 0; power --){
                int coefficient = coefficients_[power];
                if (coefficient != 0){
                    uint digits_amount_in_coefficient = get_digits_amount(coefficient);
                    uint digits_amount_in_power = get_digits_amount(power);
                    uint digits_amount_in_power_with_x = digits_amount_in_power + 2;
                    if (digits_amount_in_power_with_x >= digits_amount_in_coefficient){
                        uint digits_delta = digits_amount_in_power_with_x - digits_amount_in_coefficient;
                        uint spaces_amount = digits_delta / 2;
                        uint mod = digits_delta % 2;
                        powers << "|x^" << power;
                        coefficients << '|';
                        coefficients << std::setw(spaces_amount);
                        coefficients << "";
                        coefficients << std::setw(1);
                        if (mod != 0){
                            coefficients << ' ';
                        }
                        coefficients << coefficient;
                        coefficients << std::setw(spaces_amount);
                        coefficients << "";
                    }
                    else{
                        uint digits_delta = digits_amount_in_coefficient - digits_amount_in_power_with_x;
                        uint spaces_amount = digits_delta / 2;
                        uint mod = digits_delta % 2;
                        coefficients << '|' << coefficient;
                        powers << '|';
                        powers << std::setw(spaces_amount);
                        powers << "";
                        powers << std::setw(1);
                        if (mod != 0){
                            powers << ' ';
                        }
                        powers << "x^" << power;
                        powers << std::setw(spaces_amount);
                        powers << "";
                    }
                    coefficients << std::setw(1);
                    powers << std::setw(1);
                }
            }
            coefficients << '|';
            powers << '|';
            std::string coefficients_to_print = coefficients.str();
            std::string powers_to_print = powers.str();
            std::cout << powers_to_print << '\n';
            std::cout << coefficients_to_print << '\n';
        }
};

class Tester
{
    private:
        static bool is_equal(const std::vector<int> coefficients, const std::vector<int> correct_coefficients)
        {
            uint size = correct_coefficients.size();
            for (uint power = 0; power < size; power ++){
                if (coefficients[power] != correct_coefficients[power]){
                    return false;
                }
            }
            return true;
        }

    public:
        void test(const std::string string_polinom, const std::vector<int> correct_coefficients, const std::string message) const
        {
            Polinom polinom(string_polinom);
            std::vector<int>coefficiensts = polinom.get_coefficients();
            if(is_equal(coefficiensts, correct_coefficients)){
                std::cout << message << " is ok;" << '\n';
            }
            else{
                std::cerr << "----- Error! -----" << '\n';
                std::cerr << message << " is bad;" << '\n';
                std::cerr << "----- Error! -----" << '\n';
            }
        }
};

void test(Tester tester)
{
    tester.test("1", std::vector<int> {1}, "One constant");
    tester.test("-1", std::vector<int> {-1}, "One negative constant");
    tester.test("1-2", std::vector<int> {-1}, "Two constants");
    tester.test("1-2+3", std::vector<int> {2}, "Three constants");
    tester.test("x", std::vector<int> {0, 1}, "One first degree");
    tester.test("-x", std::vector<int> {0, -1}, "One negative first degree");
    tester.test("x-2x", std::vector<int> {0, -1}, "Two first degrees");
    tester.test("x-2x+3x", std::vector<int> {0, 2}, "Three first degrees");
    tester.test("x^2", std::vector<int> {0, 0, 1}, "One square");
    tester.test("-x^2", std::vector<int> {0, 0, -1}, "One negative square");
    tester.test("x^2-2x^2", std::vector<int> {0, 0, -1}, "Two squares");
    tester.test("x^2-2x^2+3x^2", std::vector<int> {0, 0, 2}, "Three squares");
    tester.test("10", std::vector<int> {10}, "Multi-character constant");
    tester.test("1-2+3x-4x+5x^2-6x^2", std::vector<int> {-1, -1, -1}, "Multi-degree polinom");
    tester.test("1 - 2 * x + 3 * x ^ 2", std::vector<int> {1, -2, 3}, "Polinom with additional symbols");
}

int main()
{
    Polinom polinom("1-2x+3x^2");
    polinom.print();
    return 0;
}
