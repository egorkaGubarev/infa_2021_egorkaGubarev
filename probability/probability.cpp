#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <random>

typedef unsigned int uint;

class State
{
    public:
        virtual bool contains(int s) const = 0;

        virtual ~State() = default;
};

class DiscreteState: public State
{
    private:
        int const state_;

    public:
        DiscreteState(int state): state_{state}
        {

        }

        DiscreteState(std::string mode): state_{0}
        {
            if (contains(0) && !contains(1)){
                std::cout << "DiscreteState is ok" << '\n';
            }
            else{
                std::cerr << "Error! DiscreteState is bad" << '\n';
            }
        }

        bool contains(int s) const override
        {
            return s == state_;
        }
};

class SegmentState: public State
{
    private:
        int const beg_, end_;

    public:
        SegmentState(int beg, int end): beg_{beg}, end_{end}
        {

        }

        SegmentState(std::string mode): beg_{0}, end_{2}
        {
            if (!contains(-1) && contains(0) && contains(1) && contains(2) && !contains(3)){
                std::cout << "SegmentState is ok" << '\n';
            }
            else{
                std::cerr << "Error! SegmentState is bad" << '\n';
            }
        }

        bool contains(int s) const override
        {
            return s >= beg_ && s <= end_;
        }
};

class UnionState: public State
{
    private:
        State *state_1_, *state_2_;

    public:
        UnionState(State* state_1, State* state_2): state_1_{state_1}, state_2_{state_2}
        {

        }

        UnionState(std::string mode)
        {
            SegmentState segment_1(0, 2);
            SegmentState segment_2(1, 3);
            SegmentState* ptr_segment_1 = &segment_1;
            SegmentState* ptr_segment_2 = &segment_2;
            state_1_ = ptr_segment_1;
            state_2_ = ptr_segment_2;
            if (!contains(-1) && contains(0) && contains(1) && contains(2) && contains(3) && !contains(4)){
                std::cout << "UnionState is ok" << '\n';
            }
            else{
                std::cerr << "Error! UnionState is bad" << '\n';
            }
        }

        bool contains(int s) const override
        {
            if (state_1_->contains(s) || state_2_->contains(s))
            {
                return true;
            }
            return false;
        }
};

class SegmentSpacesState: public State
{
    private:
        State* segment_;
        State* space_;

    public:
        SegmentSpacesState(State* segment, State* space): segment_{segment}, space_{space}
        {

        }

        SegmentSpacesState(std::string mode)
        {
            SegmentState segment(0, 4);
            DiscreteState space(2);
            SegmentState* ptr_segment = &segment;
            DiscreteState* ptr_space = &space;
            segment_ = ptr_segment;
            space_ = ptr_space;
            if (!contains(-1) && contains(0) && contains(1) && !contains(2) && contains(3) && contains(4) && !contains(5)){
                std::cout << "SegmentSpacesState is ok" << '\n';
            }
            else{
                std::cerr << "Error! SegmentSpacesState is bad" << '\n';
            }
        }

        bool contains(int s) const override
        {
            if(space_->contains(s)){
                return false;
            }
            if(segment_->contains(s)){
                return true;
            }
            return false;
        }
};

class SegmentExtentionState: public State
{
    private:
        State* segment_;
        State* extention_;

    public:
        SegmentExtentionState(State* segment, State* extention): segment_{segment}, extention_{extention}
        {

        }

        SegmentExtentionState(std::string mode)
        {
            SegmentState segment(0, 2);
            DiscreteState extention(4);
            SegmentState* ptr_segment = &segment;
            DiscreteState* ptr_extention = &extention;
            segment_ = ptr_segment;
            extention_ = ptr_extention;
            if (!contains(-1) && contains(0) && contains(1) && contains(2) && !contains(3) && contains(4) && !contains(5)){
                std::cout << "SegmentExtentionState is ok" << '\n';
            }
            else{
                std::cerr << "Error! SegmentExtentionState is bad" << '\n';
            }
        }

        bool contains(int s) const override
        {
            UnionState segment_extention_state(segment_, extention_);
            return segment_extention_state.contains(s);
        }
};

class SegmentSpacesExtentionState: public State
{
    private:
        State* segment_;
        State* space_;
        State* extention_;

    public:
        SegmentSpacesExtentionState(State* segment, State* space, State* extention): segment_{segment}, space_{space}, extention_{extention}
        {

        }

        SegmentSpacesExtentionState(std::string mode)
        {
            SegmentState segment(0, 4);
            DiscreteState space(2);
            DiscreteState extention(6);
            SegmentState* ptr_segment = &segment;
            DiscreteState* ptr_space = &space;
            DiscreteState* ptr_extention = &extention;
            segment_ = ptr_segment;
            space_ = ptr_space;
            extention_ = ptr_extention;
            if (!contains(-1) && contains(0) && contains(1) && !contains(2) && contains(3) && contains(4) && !contains(5) && contains(6) && !contains(7)){
                std::cout << "SegmentSpacesExtentionState is ok" << '\n';
            }
            else{
                std::cerr << "Error! SegmentSpacesExtentionState is bad" << '\n';
            }
        }

        bool contains(int s) const override
        {
            if(space_->contains(s)){
                return false;
            }
            UnionState segment_extention(segment_, extention_);
            return segment_extention.contains(s);
        }
};

class SetState: public State
{
    private:
        std::set<int> states_;

    public:
        SetState(std::set<int> const& src): states_{src}
        {

        }

        SetState(std::string mode)
        {
            const std::set<int> set = {0, 1, 2};
            states_ = set;
            if (!contains(-1) && contains(0) && contains(1) && contains(2) && !contains(3)){
                std::cout << "SetState is ok" << '\n';
            }
            else{
                std::cerr << "Error! SetState is bad" << '\n';
            }
        }

        bool contains(int s) const override{
            return states_.count(s) > 0;
        }

};

class CrossState: public State
{
    private:
        State *state_1_, *state_2_;

    public:
        CrossState(State* state_1, State* state_2): state_1_{state_1}, state_2_{state_2}
        {

        }

        CrossState(std::string mode)
        {
            SegmentState segment_1(0, 3);
            SegmentState segment_2(1, 4);
            SegmentState* ptr_segment_1 = &segment_1;
            SegmentState* ptr_segment_2 = &segment_2;
            state_1_ = ptr_segment_1;
            state_2_ = ptr_segment_2;
            if (!contains(-1) && !contains(0) && contains(1) && contains(2) && contains(3) && !contains(4) && !contains(5)){
                std::cout << "CrossState is ok" << '\n';
            }
            else{
                std::cerr << "Error! CrossState is bad" << '\n';
            }
        }

        bool contains(int s) const override
        {
            if (state_1_->contains(s) && state_2_->contains(s)){
                return true;
            }
            return false;
        }
};

class ProbabilityTest
{
    private:
        unsigned seed_;
        int test_min_, test_max_;
        std::string path_;
    public:
        ProbabilityTest(unsigned seed, int test_min, int test_max, std::string path):
            seed_{seed}, test_min_{test_min},test_max_{test_max}, path_{path}
        {

        }

        float operator()(State const& s, float accuracy) const
        {
            std::default_random_engine rng(seed_);
            std::uniform_int_distribution<int> dstr(test_min_,test_max_);
            unsigned good = 0;
            std::ofstream file;
            file.open(path_, std::ios::trunc);
            if (!file.is_open()){
                std::cerr << "File is not opened. Program aborted" << std::endl;
                return -1;
            }
            uint number_experiment = 0;
            float probability_current, probability_delta;
            float probability_last = -1;
            float probability_delta_abs = 1;
            while (probability_delta_abs > accuracy || good == 0 || good == number_experiment){
                number_experiment ++;
                if (s.contains(dstr(rng))){
                        ++ good;
                }
                probability_current = static_cast<float>(good) / number_experiment;
                probability_delta = probability_current - probability_last;
                probability_delta_abs = std::abs(probability_delta);
                file << number_experiment <<"; " << probability_current << std::endl;
                probability_last = probability_current;
            }
            file.close();
            std::cout << "Number of experiments: " << number_experiment << std::endl;
            std::cout << "Probability: " << probability_current << std::endl;
            return probability_current;
        }
};

class Tester
{
    public:
        void make_test()
        {
            DiscreteState discrete_state("test");
            SegmentState segment_state("test");
            SetState set_state("test");
            SegmentSpacesState segment_spaces_state("test");
            SegmentExtentionState segmemt_extention_state("test");
            SegmentSpacesExtentionState segment_spaces_extention_state("test");
            UnionState union_state("test");
            CrossState cross_state("test");
        }
};

int main(int argc, const char* argv[])
{
    float accuracy = 0.000001;
    Tester unit_tester;
    ProbabilityTest probability_tester(0, 0, 20, "result.txt");
    unit_tester.make_test();
    SegmentState segment(0, 10);
    probability_tester(segment, accuracy);
    return 0;
}
