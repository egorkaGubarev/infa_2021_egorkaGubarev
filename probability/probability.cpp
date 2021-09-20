#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <random>

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

        int get_state() const
        {
            return state_;
        }
};

class SegmentState: public State
{
    private:
        int const beg_, end_;

    public:
        SegmentState(): beg_{0}, end_{-1}
        {

        }

        SegmentState(int beg, int end): beg_{beg}, end_{end}
        {

        }

        bool contains(int s) const override
        {
            return s >= beg_ && s <= end_;
        }

        int get_begin() const
        {
            return beg_;
        }

        int get_end() const
        {
            return end_;
        }
};

class SegmentSpacesState: public State
{
    private:
        std::vector<State*> segments_;
        std::vector<State*> spaces_;

    public:
        SegmentSpacesState(): segments_{}, spaces_{}
        {

        };

        SegmentSpacesState(std::vector<State*> segments, std::vector<State*> spaces): segments_{segments}, spaces_{spaces}
        {

        }

        bool contains(int s) const override
        {
            for(State const* space: spaces_){
                if(space->contains(s)){
                    return false;
                }
            }
            for(State const* segment: segments_){
                if(segment->contains(s)){
                    return true;
                }
            }
            return false;
        }
};

class SegmentExtentionState: public State
{
    private:
        std::vector<State*> segments_;
        std::vector<State*> extentions_;

    public:
        SegmentExtentionState(): segments_{}, extentions_{}
        {

        }

        SegmentExtentionState(std::vector<State*> segments, std::vector<State*> extentions): segments_{segments}, extentions_{extentions}
        {

        }

        bool contains(int s) const override
        {
            for(State const* extention: extentions_){
                if(extention->contains(s)){
                    return true;
                }
            }
            for(State const* segment: segments_){
                if(segment->contains(s)){
                    return true;
                }
            }
            return false;
        }
};

class SegmentSpacesExtentionState: public State
{
    private:
        std::vector<State*> segments_;
        std::vector<State*> spaces_;
        std::vector<State*> extentions_;

    public:
        SegmentSpacesExtentionState(): segments_{}, spaces_{}, extentions_{}
        {

        }

        SegmentSpacesExtentionState(std::vector<State*> segments, std::vector<State*> spaces, std::vector<State*> extentions):
            segments_{segments}, spaces_{spaces}, extentions_{extentions}
        {

        }

        bool contains(int s) const override
        {
            for(State const* space: spaces_){
                if(space->contains(s)){
                    return false;
                }
            }
            for(State const* extention: extentions_){
                if(extention->contains(s)){
                    return true;
                }
            }
            for(State const* segment: segments_){
                if(segment->contains(s)){
                    return true;
                }
            }
            return false;
        }
};

class SetState: public State
{
    private:
        std::set<int> const states_;

    public:
        SetState(): states_{}
        {

        }

        SetState(std::set<int> const& src): states_{src}
        {

        }

        bool contains(int s) const override{
            return states_.count(s) > 0;
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
        unsigned test_count_;
        std::string path_;
    public:
        ProbabilityTest(unsigned seed, int test_min, int test_max, unsigned test_count, std::string path):
            seed_{seed}, test_min_{test_min},test_max_{test_max}, test_count_{test_count}, path_{path}
        {

        }

        float operator()(State const& s) const
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
            for (unsigned cnt = 0; cnt != test_count_; ++ cnt){
                if (s.contains(dstr(rng))){
                        ++ good;
                }
                file << cnt <<"; " << static_cast<float>(good) / (cnt + 1) << std::endl;
            }
            file.close();
            return static_cast<float>(good)/static_cast<float>(test_count_);
        }
};

int main(int argc, const char* argv[])
{
    return 0;
}
