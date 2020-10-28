/// @file
///	@ingroup   weft
///	@copyright Copyright 2020 Stephen Meyer. All rights reserved.
///	@licence	     Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <cmath>

using namespace c74::min;


class shifter : public object<shifter> {
public:
    MIN_DESCRIPTION	{"Shift a sequence of integers using another sequence based on simple vector addition."};
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
                    if (step != 0) {
                        int shift_step = m_shifts[i % m_shifts.size()];
                        sequence_copy[i] = atom(step + shift_step);
                    }
                }
            }

            output.send(sequence_copy);
            return {};
        }
    };


    message<threadsafe::yes> list { this, "list", "In left inlet, sets the primary sequence; in right inlet, sets the shift sequence that transforms the primary sequence.",
        MIN_FUNCTION {

            if (only_ints(args))
            {
                lock lock {m_mutex};
                if (inlet == 0) {
                    m_sequence.clear();
                    m_sequence.reserve(m_sequence.size() + args.size());
                    m_sequence.insert(m_sequence.end(), args.begin(), args.end());
                } else {
                    m_shifts.clear();
                    m_shifts.reserve(m_shifts.size() + args.size());
                    m_shifts.insert(m_shifts.end(), args.begin(), args.end());
                }
            }
            else
                cerr << "Cannot process list with a non-integer element." << endl;

            return {};
        }
    };


private:
    atoms m_sequence;
    atoms m_shifts;
    mutex m_mutex;

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
};


MIN_EXTERNAL(shifter);
