/// @file
///	@ingroup   weft
///	@copyright Copyright 2020 Stephen Meyer. All rights reserved.
///	@licence	     Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"

using namespace c74::min;


class shifter : public object<shifter> {
public:
    MIN_DESCRIPTION	{"Shift the elements in a sequence."};
    MIN_TAGS		    {"sequences, transformations"};
    MIN_AUTHOR		{"Steve Meyer"};
    MIN_RELATED		{"zl"};

    inlet<>  input_l { this, "(bang) send shifted sequence; (list) set the sequence" };
    inlet<>  input_r { this, "(list) set the sequence shifts" };
    outlet<> output	{ this, "(list) the transformed list, shifted where applicable" };
    
    
    message<threadsafe::yes> bang { this, "bang", "Send out the shifted sequence",
        MIN_FUNCTION {
            lock  lock {m_mutex};
            atoms sequence_copy = m_sequence;
            lock.unlock();
            
            if (m_shifts.size() > 0) {
                for (int i = 0; i < sequence_copy.size(); i++) {
                    int step = sequence_copy[i];
                    int shift_step = m_shifts[i % m_shifts.size()];
                    sequence_copy[i] = atom(step + shift_step);
                }
            }

            output.send(sequence_copy);
            return {};
        }
    };
    
    
    message<threadsafe::yes> list { this, "list", "Perform the shift operation on the list",
        MIN_FUNCTION {
            lock lock {m_mutex};
            if (inlet == 0) {
                m_sequence.reserve(m_sequence.size() + args.size());
                m_sequence.insert(m_sequence.end(), args.begin(), args.end());
            } else {
                m_shifts.reserve(m_shifts.size() + args.size());
                m_shifts.insert(m_shifts.end(), args.begin(), args.end());
            }
            return {};
        }
    };


private:
    atoms m_sequence;
    atoms m_shifts;
    mutex m_mutex;
};


MIN_EXTERNAL(shifter);
