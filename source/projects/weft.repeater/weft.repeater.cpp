/// @file
///	@ingroup   weft
///	@copyright Copyright 2020 Stephen Meyer. All rights reserved.
///	@licence	     Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "../weft.shared/weft.h"

using namespace c74::min;


class repeater : public object<repeater> {
public:
    MIN_DESCRIPTION {"Transform a sequence by applying a repeater sequence."};
    MIN_TAGS        {"sequences, transformations"};
    MIN_AUTHOR      {"Steve Meyer"};
    MIN_RELATED     {"zl"};


    inlet<>  input  { this, "(bang) send out transformed sequence; (list) set the primary sequence." };
    outlet<> output { this, "(list) the transformed sequence as a list." };


    attribute< vector<int> > sequence { this, "sequence", {0}, description {"The primary sequence to transform."},
        setter { MIN_FUNCTION {
            if (args.size() == 0 || !only_ints(args))
                return this->sequence;
            else
                return args;
        }}
    };


    attribute< vector<int> > repeats_pattern { this, "repeats", {1}, description {"The repeats pattern used to transform the primary sequence."},
        setter { MIN_FUNCTION {
            if (args.size() == 0 || !only_ints(args))
                return this->repeats_pattern;
            else
                return args;
        }}
    };


    message<> bang { this, "bang", "Send out the transformed sequence with repeats applied.",
        MIN_FUNCTION {
            lock  lock {m_mutex};

            atoms transformed_seq;
            vector<int> seq     = from_atoms<std::vector<int>>(this->sequence);
            vector<int> repeats = from_atoms<std::vector<int>>(this->repeats_pattern);

            for (int i = 0; i < seq.size(); i++) {
                atom current_step = seq[i];
                int repeat_step   = repeats[i % repeats.size()];
                for (int j = 0; j < repeat_step; j++)
                    transformed_seq.push_back(current_step);
            }

            lock.unlock();
            output.send(transformed_seq);
            return {};
        }
    };


private:
    mutex m_mutex;
};


MIN_EXTERNAL(repeater);
