/// @file
///	@ingroup   weft
///	@copyright Copyright 2020 Stephen Meyer. All rights reserved.
///	@licence	     Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "../weft.shared/weft.h"

using namespace c74::min;


class shifter : public object<shifter> {
public:
    MIN_DESCRIPTION	{"Shift a sequence of integers using another sequence based on simple vector addition."};
    MIN_TAGS		    {"sequences, transformations"};
    MIN_AUTHOR		{"Steve Meyer"};
    MIN_RELATED		{"zl"};


    inlet<>  input  { this, "(bang) output shifted sequence." };
    outlet<> output { this, "(list) the transformed sequence." };


    attribute< vector<int> > sequence { this, "sequence", {0}, description {"The primary sequence to shift."},
        setter { MIN_FUNCTION {
            if (args.size() == 0 || !only_ints(args))
                return this->sequence;
            else
                return args;
        }}
    };


    attribute< vector<int> > shift_pattern { this, "shift_pattern", {0}, description {"The shift pattern used to transform the primary sequence."},
        setter { MIN_FUNCTION {
            if (args.size() == 0 || !only_ints(args))
                return this->shift_pattern;
            else
                return args;
        }}
    };


    message<> bang { this, "bang", "Send out the shifted sequence.",
        MIN_FUNCTION {
            lock  lock {m_mutex};

            atoms shifted_seq;
            auto seq    = from_atoms<std::vector<int>>(this->sequence);
            auto shifts = from_atoms<std::vector<int>>(this->shift_pattern);
            for (int i = 0; i < seq.size(); i++)
                if (seq[i] == 0)
                    shifted_seq.push_back(seq[i]);
                else
                    shifted_seq.push_back(seq[i] + shifts[i % shifts.size()]);

            lock.unlock();
            output.send(shifted_seq);
            return {};
        }
    };


private:
    mutex m_mutex;
};


MIN_EXTERNAL(shifter);
