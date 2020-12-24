#include "c74_min.h"
#include <cmath>


using namespace c74::min;


int calculate_length(const vector<int> &seq, const vector<int> &rhythm);


bool only_ints(atoms const &args) {
    bool ints_only = true;
    for (int i = 0; i < args.size(); i++)
        try
        {
            float num = std::stof(args[i]);
            if (std::floor(num) != num)
            {
                ints_only = false;
                break;
            }
        }
        catch (...)
        {
            ints_only = false;
        }

    return ints_only;
}


int calculate_length(const vector<int> &seq, const vector<int> &rhythm) {
    int rhythm_hits = 0;
    float seq_length = seq.size();
    for (int i = 0; i < rhythm.size(); i++) {
        if (rhythm[i] != 0)
            rhythm_hits++;
    }

    if (rhythm_hits == 0)
        return 0;
    else {
        int step_hits = std::ceil(seq_length / rhythm_hits);
        return rhythm.size() * step_hits;
    }
}


void apply_rhythm(atoms *transformed_seq, const vector<int> &seq, const vector<int> &rhythm, const int &length, const symbol fill_mode) {

    int transformed_seq_length;
    if (length >= 1)
        // If a length greater than or equal to 1 has been specified, use it.
        transformed_seq_length = length;
    else
        // Otherwise calculate the length by applying the rhythmic transformation to all steps in the sequence
        transformed_seq_length = calculate_length(seq, rhythm);

    int processed_step_index = 0;
    for (int i = 0; i < transformed_seq_length; i++) {
        int rhythm_step = rhythm[i % rhythm.size()];

        if (rhythm_step == 0 || (processed_step_index >= seq.size() && fill_mode == symbol("silence")))
            transformed_seq->push_back(0);
        else {
            transformed_seq->push_back(seq[processed_step_index % seq.size()]);
            processed_step_index++;
        }
    }
}
