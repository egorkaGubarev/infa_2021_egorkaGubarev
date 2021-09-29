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
        void test(State* state, int point, bool correct_result, std::string message)
        {
            bool result = state->contains(point);
            if (result == correct_result){
                std::cout << message << " is ok;" << '\n';
            }
            else{
                std::cerr << "----- Error! -----" << '\n';
                std::cerr << message << " is bad;"  << '\n';
                std::cerr << "----- Error! -----" << '\n';
            }
        }
};

void test(Tester tester)
{
    DiscreteState discrete_state(0);
    State* ptr_discrete_state = &discrete_state;
    tester.test(ptr_discrete_state, -1, false, "Discrete outside left range");
    tester.test(ptr_discrete_state, 0, true, "Discrete inside");
    tester.test(ptr_discrete_state, 1, false, "Discrete outside right range");
    SegmentState segment_state(1, 3);
    State* ptr_segment_state = &segment_state;
    tester.test(ptr_segment_state, 0, false, "Segment outside left range");
    tester.test(ptr_segment_state, 1, true, "Segment left edge");
    tester.test(ptr_segment_state, 2, true, "Segment inside");
    tester.test(ptr_segment_state, 3, true, "Segment right edge");
    tester.test(ptr_segment_state, 4, false, "Segment outside right range");
    UnionState union_state(ptr_discrete_state, ptr_segment_state);
    State* ptr_union_state = &union_state;
    tester.test(ptr_union_state, -1, false, "Union outside left range");
    tester.test(ptr_union_state, 0, true, "Union left edge");
    tester.test(ptr_union_state, 1, true, "Union inside");
    tester.test(ptr_union_state, 3, true, "Union right edge");
    tester.test(ptr_union_state, 4, false, "Union outside right range");
    DiscreteState point(2);
    State* ptr_point = &point;
    SegmentSpacesState segment_spaces_state(ptr_segment_state, ptr_point);
    State* ptr_segment_spaces_state = &segment_spaces_state;
    tester.test(ptr_segment_spaces_state, 0, false, "Segment with spaces outside left range");
    tester.test(ptr_segment_spaces_state, 1, true, "Segment with spaces left edge");
    tester.test(ptr_segment_spaces_state, 2, false, "Segment with spaces space");
    tester.test(ptr_segment_spaces_state, 3, true, "Segment with spaces right edge");
    tester.test(ptr_segment_spaces_state, 4, false, "Segment with spaces outside right range");
    DiscreteState point_2(5);
    State* ptr_point_2 = &point_2;
    SegmentExtentionState segment_extention_state(ptr_segment_state, ptr_point_2);
    State* ptr_segment_extention_state = &segment_extention_state;
    tester.test(ptr_segment_extention_state, 0, false, "Segment with extention outside left range");
    tester.test(ptr_segment_extention_state, 1, true, "Segment with extention left edge");
    tester.test(ptr_segment_extention_state, 2, true, "Segment with extention inside");
    tester.test(ptr_segment_extention_state, 3, true, "Segment with extention right edge");
    tester.test(ptr_segment_extention_state, 4, false, "Segment with extention outside in middle");
    tester.test(ptr_segment_extention_state, 5, true, "Segment with extention in extention");
    tester.test(ptr_segment_extention_state, 6, false, "Segment with extention outside right range");
    SegmentSpacesExtentionState segment_spaces_extention_state(ptr_segment_state, ptr_point, ptr_point_2);
    State* ptr_segment_spaces_extention_state = &segment_spaces_extention_state;
    tester.test(ptr_segment_spaces_extention_state, 0, false, "Segment with spaces and extention outside left range");
    tester.test(ptr_segment_spaces_extention_state, 1, true, "Segment with spaces and extention left edge");
    tester.test(ptr_segment_spaces_extention_state, 2, false, "Segment with spaces and extention in space");
    tester.test(ptr_segment_spaces_extention_state, 3, true, "Segment with spaces and extention right edge");
    tester.test(ptr_segment_spaces_extention_state, 4, false, "Segment with spaces and extention outside in middle");
    tester.test(ptr_segment_spaces_extention_state, 5, true, "Segment with spaces and extention in extention");
    tester.test(ptr_segment_spaces_extention_state, 6, false, "Segment with spaces and extention outside right range");
    SetState set_state(std::set<int>{4, 6, 7, 8});
    State* ptr_set_state = &set_state;
    tester.test(ptr_set_state, 3, false, "Set outside left range");
    tester.test(ptr_set_state, 4, true, "Set left in isolated point");
    tester.test(ptr_set_state, 5, false, "Set in space");
    tester.test(ptr_set_state, 6, true, "Set right edge");
    tester.test(ptr_set_state, 7, true, "Set in middle");
    tester.test(ptr_set_state, 8, true, "Set right edge");
    tester.test(ptr_set_state, 9, false, "Set outside right range");
    CrossState cross_state(ptr_segment_state, ptr_point);
    State* ptr_cross_state = &cross_state;
    tester.test(ptr_cross_state, 0, false, "Cross outside left range");
    tester.test(ptr_cross_state, 1, false, "Cross only in one state");
    tester.test(ptr_cross_state, 2, true, "Cross inside");
    tester.test(ptr_cross_state, 4, false, "Cross outside right range");
}

int main(int argc, const char* argv[])
{
    float accuracy = 0.0000001;
    ProbabilityTest tester(0, -100, 100, "result.txt");
    SegmentState segment(0, 10);
    tester(segment, accuracy);
    return 0;
}
