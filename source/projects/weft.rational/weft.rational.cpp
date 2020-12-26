/// @file
///	@ingroup   weft
///	@copyright Copyright 2020 Stephen Meyer. All rights reserved.
///	@licence	     Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "../weft.shared/weft.h"

using namespace c74::min;


class rational : public object<rational> {
public:
    MIN_DESCRIPTION {"Transform a sequence by rational melody algorithms."};
    MIN_TAGS        {"sequences, transformations"};
    MIN_AUTHOR      {"Steve Meyer"};
    MIN_RELATED     {"zl"};


    inlet<>  input  { this, "(bang) send out transformed sequence" };
    outlet<> output { this, "(list) the transformed sequence as a list." };


    enum class melodies : int { iv, xi, enum_count };

    enum_map melodies_range = {"iv", "xi"};

    attribute<melodies> melody {this, "melody", melodies::xi, melodies_range,
        description {"The rational melody number (in lowercase roman numerals)."}
    };


    attribute< vector<int> > sequence { this, "sequence", {0}, description {"The primary sequence to transform."},
        setter { MIN_FUNCTION {
            if (args.size() == 0 || !only_ints(args))
                return this->sequence;
            else
                return args;
        }}
    };


    message<> bang { this, "bang", "Send out the transformed sequence with rational melody algorithm applied.",
        MIN_FUNCTION {
            lock  lock {m_mutex};
            atoms transformed_seq;

            switch(melody) {
                case melodies::xi: {
                    melody_xi(transformed_seq);
                    break;
                }
                case melodies::iv: {
                    melody_iv(transformed_seq);
                    break;
                }
                case melodies::enum_count:
                    break;
            }

            lock.unlock();
            output.send(transformed_seq);
            return {};
        }
    };


private:
    mutex m_mutex;

    void melody_xi(atoms& transformed_seq) {

        vector<int> seq   = from_atoms<std::vector<int>>(this->sequence);
        int segment_length = seq.size() / 2 + 1;

        // Do that dance Paula sang about...
        for (int segment = 0; segment < seq.size(); segment++) {

            // Take two steps forward,
            for (int index = 0; index < segment_length; index++)
                transformed_seq.push_back( seq[(index + segment) % seq.size()] );

            // Take one step back.
            for (int index = 0; index < segment_length - 1; index++)
                transformed_seq.push_back( transformed_seq[transformed_seq.size() - 1 - index - index] );
        }
    }


    void melody_iv(atoms& transformed_seq) {

        vector<int> seq = from_atoms<std::vector<int>>(this->sequence);

        for (int segment = 1; segment <= seq.size(); segment++) {

            vector<int> rhythm;
            for (int i = 0; i < segment; i++)
                rhythm.push_back(1);
            rhythm.push_back(0);

            int length = seq.size() * (segment + 1);
            apply_rhythm(transformed_seq, seq, rhythm, length, symbol("wrap"));
        }
    }
};


MIN_EXTERNAL(rational);
