/// @file
///	@ingroup   weft
///	@copyright Copyright 2020 Stephen Meyer. All rights reserved.
///	@licence	     Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "../weft.shared/weft.h"

using namespace c74::min;


class gates : public object<gates> {
public:
    MIN_DESCRIPTION {"Transform a sequence by applying a gate pattern."};
    MIN_TAGS        {"sequences, transformations"};
    MIN_AUTHOR      {"Steve Meyer"};
    MIN_RELATED     {"zl"};


    inlet<>  input  { this, "(bang) send out transformed sequence" };
    outlet<> output { this, "(list) the transformed sequence as a list." };


    attribute< vector<int> > sequence { this, "sequence", {0}, description {"The primary sequence to transform."},
        setter { MIN_FUNCTION {
            if (args.size() == 0 || !only_ints(args))
                return this->sequence;
            else
                return args;
        }}
    };


    attribute< vector<int> > gates_pattern { this, "gates", {1}, description {"The gates pattern used to transform the primary sequence."},
        setter { MIN_FUNCTION {
            if (args.size() == 0 || !only_ints(args))
                return this->gates_pattern;
            else
                return args;
        }}
    };


    message<> bang { this, "bang", "Send out the transformed sequence with repeats applied.",
        MIN_FUNCTION {
            lock  lock {m_mutex};

            atoms transformed_seq;
            vector<int> seq   = from_atoms<std::vector<int>>(this->sequence);
            vector<int> gates = from_atoms<std::vector<int>>(this->gates_pattern);

            for (int i = 0; i < seq.size(); i++)
                if (gates[i % gates.size()] == 0)
                    transformed_seq.push_back(0);
                else
                    transformed_seq.push_back(seq[i]);

            lock.unlock();
            output.send(transformed_seq);
            return {};
        }
    };


private:
    mutex m_mutex;
};


MIN_EXTERNAL(gates);
