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


    enum class melodies : int { iv, xi, xv, xvi, enum_count };

    enum_map melodies_range = {"iv", "xi", "xv", "xvi"};

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
                case melodies::xv: {
                    melody_xv(transformed_seq);
                    break;
                }
                case melodies::xvi: {
                    melody_xvi(transformed_seq);
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

    // Logic: go N / 2 + 1 steps forward, N / 2 steps back through a melody
    // Given the melody: 1 2 3 4 5 6 7 8 9 10 11 12
    // Generate a sequence that is the concatenation of the following segments:
    // Segment 1:   1 2 3 4 5 6 7 7 6 5 4 3 2
    // Segment 2:   2 3 4 5 6 7 8 8 7 6 5 4 3
    // ...
    // Segment 11: 11 12 1 2 3 4 5 5 4 3 2 1 12
    // Segment 12: 12 1 2 3 4 5 6 6 5 4 3 2 1
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


    // Given: 1 2 3
    // Generate a sequence that is the concatenation of the following segments:
    // Segment 1: 1 0 2 0 3 0
    // Segment 2: 1 2 0 3 1 0 2 3 0
    // Segment 3: 1 2 3 0 1 2 3 0 1 2 3 0
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


    // Given the note series: A G F E D
    // Generate the self-similar sequence:
    //
    // A G G F G E F D
    // G A E G F F D E
    // G E A F E D G A
    // F G F G D A E F
    // G F E D A G F E
    // E F D A G G A F
    // F D G E F A G F
    // D E A F E F F
    //
    // Self-similar by powers of 2. Stepping through the sequence by every note, every 2nd note, every
    // 4th note, every 8th note will always play the same sequence. Notice in the generated example
    // above that the first row (every note) and first column (every 8th note) are identical sequences.
    //
    // Returns a 63 note sequence.
    void melody_xv(atoms& transformed_seq) {

        vector<int> seq = from_atoms<std::vector<int>>(this->sequence);

        int seq_steps = 63;
        int max_power = 7;
        int rational_melody[63];
        int count = 0;

        for (int i = 0; i < seq_steps; i++)
            rational_melody[i] = -1;

        do {
            // Get the current contiguous range from the beginning of the sequence, defined as
            // all steps that from the beginning that are not equal to the default value (-1).
            // This loop does not execute the first time through the do/while loop.
            int contiguous_end = next_empty_index(rational_melody, seq_steps);
            for (int i = 0; i < contiguous_end; i++) {

                // Use the contiguous sequence, which represents the steps at every note,
                // to fill out the pattern at every 2nd, 4th and/or 8th notes.
                for (int power = 1; power <= max_power; power++) {
                    int next_step = (int) (i * pow(2.0, power)) % seq_steps;
                    rational_melody[next_step] = rational_melody[i];
                }
            }

            // Finally, fill in the earliest empty step with the next note from the notes pattern.
            // Don't do this if there are no empty sequence steps.
            int next_empty = next_empty_index(rational_melody, seq_steps);
            if (next_empty < seq_steps)
                rational_melody[next_empty] = seq[count % seq.size()];

            count++;

        } while (!melody_xv_complete(rational_melody, seq_steps));

        for (int step = 0; step < seq_steps; step++)
            transformed_seq.push_back(rational_melody[step]);
    }


    int next_empty_index(int rational_melody[], int seq_steps) {
        int step = 0;
        while (step < seq_steps & rational_melody[step] != -1) { step++; }
        return step;
    }


    bool melody_xv_complete(int rational_melody[], int seq_steps) {
        int step = 0;
        while (step < seq_steps & rational_melody[step] != -1) { step++; }
        return step >= seq_steps;
    }


    void melody_xvi(atoms& transformed_seq) {

        vector<int> seq = from_atoms<std::vector<int>>(this->sequence);

        if (seq.size() < 2) {
            transformed_seq.push_back(seq[0]);
        } else {
            atoms seq_indices;
            atoms first_segment = {0, 1, 0};
            append_segment(seq_indices, first_segment);

            atoms prev_segment;
            append_segment(prev_segment, first_segment);

            for (int i = 2; i < seq.size(); i++) {
                int prev_segment_idx = 0;
                atoms next_segment;
                int next_length = pow(2, i) + 1;

                for (int j = 0; j < next_length; j++) {
                    if (j % 2 == 0) {
                        next_segment.push_back(prev_segment[prev_segment_idx]);
                        prev_segment_idx++;
                    } else {
                        int neighbors[] = { next_segment[j - 1], prev_segment[prev_segment_idx] };
                        std::sort(neighbors, neighbors + 2);

                        if (neighbors[1] - neighbors[0] == 1)
                            next_segment.push_back(neighbors[1] + 1);
                        else
                            next_segment.push_back(neighbors[1] - 1);
                    }
                }

                append_segment(seq_indices, next_segment);
                prev_segment.clear();
                append_segment(prev_segment, next_segment);
            }

            for (int i = 0; i < seq_indices.size(); i++)
                transformed_seq.push_back(seq[seq_indices[i]]);
        }
    }


    void append_segment(atoms& append_to, atoms& append_from) {
        for (int i = 0; i < append_from.size(); i++)
            append_to.push_back(append_from[i]);
    }
};


MIN_EXTERNAL(rational);
