/// @file
/// @ingroup   weft
/// @copyright Copyright 2020 Stephen Meyer. All rights reserved.
/// @license        Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min_unittest.h"     // required unit test header
#include "weft.shifter.cpp"    // need the source of our object so that we can access it


SCENARIO("object produces correct output") {
    ext_main(nullptr);    // every unit test must call ext_main() once to configure the class

    GIVEN("An instance of weft.shifter") {

        test_wrapper<shifter> an_instance;
        shifter&             my_object = an_instance;

        WHEN("It is given a list containing a symbol") {

            atoms sequence;
            sequence.push_back(1);
            sequence.push_back("string");
            sequence.push_back(2);
            my_object.sequence = sequence;
            my_object.bang();

            THEN("it does not update the sequence") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                atoms expected = {0};
                REQUIRE(output.size() == 1);
                REQUIRE(output[0] == expected);
            }
        }

        WHEN("It is given a list containing a float") {

            atoms sequence;
            sequence.push_back(1);
            sequence.push_back(2);
            sequence.push_back(3.1456);
            my_object.sequence = sequence;
            my_object.bang();

            THEN("it does not update the sequence") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                atoms expected = {0};
                REQUIRE(output.size() == 1);
                REQUIRE(output[0] == expected);
            }
        }
    }

    GIVEN("An instance of weft.shifter given a list of integers") {

        test_wrapper<shifter> an_instance;
        shifter&             my_object = an_instance;

        // Seed the initial sequence
        atoms sequence = {1, 2, 3, 4};
        my_object.sequence = sequence;

        WHEN("it is banged") {
            my_object.bang();

            THEN("the sequence is returned") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                atoms expected = { 1, 2, 3, 4 };
                REQUIRE(output.size() == 1);
                REQUIRE(output[0] == expected);
            }
        }

        WHEN("it is given an invalid sequence") {
            atoms sequence;
            sequence.push_back(1);
            sequence.push_back(2);
            sequence.push_back(3.1456);
            my_object.sequence = sequence;
            my_object.bang();

            THEN("it does not update the current, valid sequence") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                atoms expected = { 1, 2, 3, 4 };
                REQUIRE(output.size() == 1);
                REQUIRE(output[0] == expected);
            }
        }

        WHEN("a shift sequence is provided and it is banged") {
            atoms shift_seq = { 1, 0 };
            my_object.shift_pattern = shift_seq;
            my_object.bang();

            THEN("the returned sequence has shifts applied") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                atoms expected = { 2, 2, 4, 4 };
                REQUIRE(output.size() == 1);
                REQUIRE(output[0] == expected);
            }

            AND_WHEN("a new shift sequence provided and it is banged") {
                atoms shift_seq = { 0, 1 };
                my_object.shift_pattern = shift_seq;
                my_object.bang();

                THEN("the returned sequence has shifts applied") {
                    auto& output = *c74::max::object_getoutput(my_object, 0);
                    atoms expected = { 1, 3, 3, 5 };
                    REQUIRE(output.size() == 2);
                    REQUIRE(output[1] == expected);
                }
            }
        }

        WHEN("it is given a list again and it is banged") {
            atoms sequence = {4, 3, 2, 1};
            my_object.sequence = sequence;
            my_object.bang();

            THEN("the new list replaces the old list and is returned") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                atoms expected = {4, 3, 2, 1};
                REQUIRE(output.size() == 1);
                REQUIRE(output[0] == expected);
            }
        }

        WHEN("it is given a shift sequence that is not a multiple of the sequence") {
            atoms shift_seq = {1, 0, 1};
            my_object.shift_pattern = shift_seq;
            my_object.bang();

            THEN("the resulting transformation uses a modulo/wrap around model") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                atoms expected = {2, 2, 4, 5};
                REQUIRE(output.size() == 1);
                REQUIRE(output[0] == expected);
            }
        }

        WHEN("it is given a shift sequence that is longer than the primary sequence") {
            atoms shift_seq = {1, 0, 1, 0, 1};
            my_object.shift_pattern = shift_seq;
            my_object.bang();

            THEN("the resulting transformation uses a modulo/wrap around model") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                atoms expected = {2, 2, 4, 4};
                REQUIRE(output.size() == 1);
                REQUIRE(output[0] == expected);
            }
        }

        WHEN("the primary sequences has 0-value rest steps") {
            atoms sequence  = {1, 0, 0, 0};
            atoms shift_seq = {1, 0};
            my_object.sequence = sequence;
            my_object.shift_pattern = shift_seq;
            my_object.bang();

            THEN("the resulting transformation does not modify the rests") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                atoms expected = {2, 0, 0, 0};
                REQUIRE(output.size() == 1);
                REQUIRE(output[0] == expected);
            }
        }
    }
}
