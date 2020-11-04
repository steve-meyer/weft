/// @file
/// @ingroup   weft
/// @copyright Copyright 2020 Stephen Meyer. All rights reserved.
/// @license        Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min_unittest.h"  // required unit test header
#include "weft.rhythm.cpp"   // need the source of our object so that we can access it


SCENARIO("Object produces correct output") {
    ext_main(nullptr);    // every unit test must call ext_main() once to configure the class

    GIVEN("An instance of weft.rhythm with a sequence") {

        test_wrapper<rhythm> an_instance;
        rhythm&             my_object = an_instance;
        atoms sequence = {1, 1, 5, 5, 6, 6, 4, 4};
        my_object.sequence = sequence;

        WHEN("it is given a rhythm pattern with non-integers and it is banged") {
            atoms rhythm;
            rhythm.push_back(1);
            rhythm.push_back("string");
            rhythm.push_back(0);
            my_object.rhythm_pattern = rhythm;
            my_object.bang();

            THEN("the rhythm is not stored and the original transformation is returned") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                atoms expected = {1, 1, 5, 5, 6, 6, 4, 4};
                REQUIRE(output.size() == 1);
                REQUIRE(output[0] == expected);
            }
        }

        WHEN("it is given a length and a rhythm pattern and it is banged") {
            my_object.length = 16;
            atoms rhythm = {1, 1, 0};
            my_object.rhythm_pattern = rhythm;
            my_object.bang();

            THEN("the rhythm is stored and the transformed sequence is returned and wraps the sequence to fill out the bar length") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                atoms expected = {1, 1, 0, 5, 5, 0, 6, 6, 0, 4, 4, 0, 1, 1, 0, 5};
                REQUIRE(output.size() == 1);
                REQUIRE(output[0] == expected);
            }

            AND_WHEN("the fill mode is flipped to 'silence'") {
                my_object.fill_mode = "silence";
                my_object.bang();

                THEN("the transformed sequence has rests at the end") {
                    auto& output = *c74::max::object_getoutput(my_object, 0);
                    atoms expected = {1, 1, 0, 5, 5, 0, 6, 6, 0, 4, 4, 0, 0, 0, 0, 0};
                    REQUIRE(output.size() == 2);
                    REQUIRE(output[1] == expected);
                }
            }
        }

        WHEN("the rhythm has no specified length and the sequence is a multiple of the rhythm's hit counts (1's or non-zero values)") {
            atoms rhythm = {1, 1, 0};
            my_object.rhythm_pattern = rhythm;
            my_object.bang();

            THEN("the transformed sequence is a multiple of the entire rhythm") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                atoms expected = {1, 1, 0, 5, 5, 0, 6, 6, 0, 4, 4, 0};
                REQUIRE(output.size() == 1);
                REQUIRE(output[0] == expected);
            }
        }

        WHEN("the rhythm has no specified length and the sequence is NOT a multiple of the rhythm's hit counts (1's or non-zero values)") {
            atoms rhythm = {1, 1, 0, 1, 0};
            my_object.rhythm_pattern = rhythm;
            my_object.bang();

            THEN("the transformed sequence is a multiple of the entire rhythm and wraps the sequence") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                atoms expected = {1, 1, 0, 5, 0, 5, 6, 0, 6, 0, 4, 4, 0, 1, 0};
                REQUIRE(output.size() == 1);
                REQUIRE(output[0] == expected);
            }
        }
        
        WHEN("the rhythm has no specified length, a fill mode for silence and the sequence is NOT a multiple of the rhythm's hit counts (1's or non-zero values)") {
            atoms rhythm = {1, 1, 0, 1, 0};
            my_object.rhythm_pattern = rhythm;
            my_object.fill_mode = symbol("silence");
            my_object.bang();

            THEN("the transformed sequence is a multiple of the entire rhythm and wraps the sequence") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                atoms expected = {1, 1, 0, 5, 0, 5, 6, 0, 6, 0, 4, 4, 0, 0, 0};
                REQUIRE(output.size() == 1);
                REQUIRE(output[0] == expected);
            }
        }
    }
}
