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

    inlet<>  input_l { this, "(bang) send out transformed sequence; (list) set the primary sequence." };
    inlet<>  input_r { this, "(list) set the repeats pattern." };
    outlet<> output  { this, "(list) the transformed sequence as a list." };


    message<threadsafe::yes> bang { this, "bang", "Send out the transformed sequence with repeats applied.",
        MIN_FUNCTION {
            lock  lock {m_mutex};
            atoms sequence_copy = m_sequence;
            lock.unlock();
            
            if (m_repeats.size() > 0) {
                atoms transformed_seq;
                for (int i = 0; i < sequence_copy.size(); i++) {
                    atom current_step = sequence_copy[i];
                    int repeat_step = m_repeats[i % m_repeats.size()];
                    for (int j = 0; j < repeat_step; j++) {
                        transformed_seq.push_back(current_step);
                    }
                }
                output.send(transformed_seq);
            } else {
                cwarn << "No repeats pattern set. Outputting unmodified sequence." << endl;
                output.send(sequence_copy);
            }
            return {};
        }
    };


    message<threadsafe::yes> list { this, "list", "In left inlet, sets the primary sequence; in right inlet, sets the repeats pattern that transforms the primary sequence.",
        MIN_FUNCTION {

            lock lock {m_mutex};
            if (inlet == 0) {
                m_sequence.clear();
                m_sequence.reserve(args.size());
                m_sequence.insert(m_sequence.end(), args.begin(), args.end());
            } else {
                if (only_ints(args)) {
                    m_repeats.clear();
                    m_repeats.reserve(args.size());
                    m_repeats.insert(m_repeats.end(), args.begin(), args.end());
                }
                else
                    cerr << "A repeats pattern must only consist of integers." << endl;
            }
            return {};
        }
    };


private:
    atoms m_sequence;
    atoms m_repeats;
    mutex m_mutex;
};


MIN_EXTERNAL(repeater);
