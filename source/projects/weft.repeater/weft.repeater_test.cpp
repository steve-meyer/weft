/// @file
/// @ingroup   weft
/// @copyright Copyright 2020 Stephen Meyer. All rights reserved.
/// @license        Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min_unittest.h"  // required unit test header
#include "weft.repeater.cpp"   // need the source of our object so that we can access it


SCENARIO("Object produces correct output") {
    ext_main(nullptr);    // every unit test must call ext_main() once to configure the class

    GIVEN("An instance of weft.repeater with a sequence") {

        test_wrapper<repeater> an_instance;
        repeater&             my_object = an_instance;
        atoms sequence = {1, 5, 6, 4};
        my_object.sequence = sequence;
        
        WHEN("it is given a repeater pattern with non-integers and it is banged") {
            atoms repeats;
            repeats.push_back(1);
            repeats.push_back("string");
            repeats.push_back(2);
            my_object.repeats_pattern = repeats;
            my_object.bang();

            THEN("the repeats are not stored and the original transformation is returned") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                atoms expected = {1, 5, 6, 4};
                REQUIRE(output.size() == 1);
                REQUIRE(output[0] == expected);
            }
        }

        WHEN("it is given a repeater pattern of integers and it is banged") {
            
            atoms repeats = {1, 2, 1, 2};
            my_object.repeats_pattern = repeats;
            my_object.bang();

            THEN("it applies the repetition transformation") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                atoms expected = {1, 5, 5, 6, 4, 4};
                REQUIRE(output.size() == 1);
                REQUIRE(output[0] == expected);
            }
        }

        WHEN("it is not given a repeater pattern of integers and it is banged") {
            
            my_object.bang();

            THEN("the original transformation is returned") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                atoms expected = {1, 5, 6, 4};
                REQUIRE(output.size() == 1);
                REQUIRE(output[0] == expected);
            }
        }

        WHEN("it is given a repeats pattern is not a multiple of the sequence") {
            atoms repeats = {3, 2, 1};
            my_object.repeats_pattern = repeats;
            my_object.bang();

            THEN("the resulting transformation uses a modulo/wrap around model") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                atoms expected = {1, 1, 1, 5, 5, 6, 4, 4, 4};
                REQUIRE(output.size() == 1);
                REQUIRE(output[0] == expected);
            }
        }

        WHEN("it is given a repeater pattern that is longer than the sequence") {
            atoms sequence = {1, 5};
            atoms repeats  = {2, 2, 1};
            my_object.sequence = sequence;
            my_object.repeats_pattern = repeats;
            my_object.bang();

            THEN("it only applies repeat steps for the primary sequence length") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                atoms expected = {1, 1, 5, 5};
                REQUIRE(output.size() == 1);
                REQUIRE(output[0] == expected);
            }
        }
    }
}
