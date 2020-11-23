/// @file
///	@ingroup   weft
///	@copyright Copyright 2020 Stephen Meyer. All rights reserved.
///	@licence	     Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "../weft.shared/weft.h"

using namespace c74::min;


class rhythm : public object<rhythm> {
public:
    MIN_DESCRIPTION {"Transform a sequence by applying a rhythmic pattern."};
    MIN_TAGS        {"sequences, transformations"};
    MIN_AUTHOR      {"Steve Meyer"};
    MIN_RELATED     {"zl"};


    inlet<>  input  { this, "(bang) send out transformed sequence" };
    outlet<> output { this, "(list) the transformed sequence as a list." };


    attribute<int> length { this, "length", -1, description {"The length of the transformed sequence in steps."}};
    attribute<symbol> fill_mode { this, "fill_mode", "wrap",
        description {"The mode used to fill out a sequence when the length is longer than the transformed sequence."},
        range {"wrap", "silence"}
    };


    attribute< vector<int> > sequence { this, "sequence", {0}, description {"The primary sequence to transform."},
        setter { MIN_FUNCTION {
            if (args.size() == 0 || !only_ints(args))
                return this->sequence;
            else
                return args;
        }}
    };


    attribute< vector<int> > rhythm_pattern { this, "rhythm", {1}, description {"The rhythm pattern used to transform the primary sequence."},
        setter { MIN_FUNCTION {
            if (args.size() == 0 || !only_ints(args))
                return this->rhythm_pattern;
            else
                return args;
        }}
    };


    message<> bang { this, "bang", "Send out the transformed sequence with repeats applied.",
        MIN_FUNCTION {
            lock  lock {m_mutex};

            atoms transformed_seq;
            vector<int> seq   = from_atoms<std::vector<int>>(this->sequence);
            vector<int> rhythm = from_atoms<std::vector<int>>(this->rhythm_pattern);
             

            int length;
            if (this->length >= 1)
                // If a length greater than or equal to 1 has been specified, use it.
                length = this->length;
            else
                // Otherwise calculate the length by applying the rhythmic transformation to all steps in the sequence
                length = calculate_length(seq, rhythm);

            int processed_step_index = 0;
            for (int i = 0; i < length; i++) {
                int rhythm_step = rhythm[i % rhythm.size()];

                if (rhythm_step == 0 || (processed_step_index >= seq.size() && this->fill_mode == symbol("silence")))
                    transformed_seq.push_back(0);
                else {
                    transformed_seq.push_back(seq[processed_step_index % seq.size()]);
                    processed_step_index++;
                }
            }

            lock.unlock();
            output.send(transformed_seq);
            return {};
        }
    };


private:
    mutex m_mutex;

    int calculate_length(vector<int> const &seq, vector<int> const &rhythm) {
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
};


MIN_EXTERNAL(rhythm);
