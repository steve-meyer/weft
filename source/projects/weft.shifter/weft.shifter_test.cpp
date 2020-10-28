/// @file
/// @ingroup   weft
/// @copyright Copyright 2020 Stephen Meyer. All rights reserved.
/// @license        Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min_unittest.h"     // required unit test header
#include "weft.shifter.cpp"    // need the source of our object so that we can access it

// Unit tests are written using the Catch framework as described at
// https://github.com/philsquared/Catch/blob/master/docs/tutorial.md

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
            my_object.list(sequence);
            my_object.bang();

            THEN("it does not update the sequence") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                REQUIRE(output.size() == 0);
            }
        }

        WHEN("It is given a list containing a float") {

            atoms sequence;
            sequence.push_back(1);
            sequence.push_back(2);
            sequence.push_back(3.1456);
            my_object.list(sequence);
            my_object.bang();

            THEN("it does not update the sequence") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                REQUIRE(output.size() == 0);
            }
        }
    }

    GIVEN("An instance of weft.shifter given a list of integers") {

        test_wrapper<shifter> an_instance;
        shifter&             my_object = an_instance;

        // Seed the initial sequence
        my_object.list({ 1, 2, 3, 4 });

        WHEN("it is banged") {
            my_object.bang();
            THEN("the sequence is returned") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                REQUIRE(output.size() == 1);
                REQUIRE(output[0].size() == 4);
                atoms expected = { 1, 2, 3, 4 };
                REQUIRE(output[0] == expected);
            }
        }

        WHEN("a shift sequence is provided and it is banged") {
            atoms shift_seq = { 1, 0 };
            my_object.list(shift_seq, 1);
            my_object.bang();

            THEN("the returned sequence has shifts applied") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                REQUIRE(output.size() == 1);
                REQUIRE(output[0].size() == 4);
                atoms expected = { 2, 2, 4, 4 };
                REQUIRE(output[0] == expected);
            }

            AND_WHEN("a new shift sequence provided and it is banged") {
                atoms shift_seq = { 0, 1 };
                my_object.list(shift_seq, 1);
                my_object.bang();

                THEN("the returned sequence has shifts applied") {
                    auto& output = *c74::max::object_getoutput(my_object, 0);
                    REQUIRE(output.size() == 2);
                    REQUIRE(output[1].size() == 4);
                    atoms expected = { 1, 3, 3, 5 };
                    REQUIRE(output[1] == expected);
                }
            }
        }

        WHEN("it is given a list again and it is banged") {
            my_object.list({4, 3, 2, 1});
            my_object.bang();
            THEN("the new list replaces the old list and is returned") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                REQUIRE(output.size() == 1);
                REQUIRE(output[0].size() == 4);
                atoms expected = {4, 3, 2, 1};
                REQUIRE(output[0] == expected);
            }
        }

        WHEN("it is given a shift sequence that is not a multiple of the sequence") {
            my_object.list({1, 0, 1}, 1);
            my_object.bang();
            THEN("the resulting transformation uses a modulo/wrap around model") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                REQUIRE(output.size() == 1);
                REQUIRE(output[0].size() == 4);
                atoms expected = {2, 2, 4, 5};
                REQUIRE(output[0] == expected);
            }
        }

        WHEN("it is given a shift sequence that is longer than the primary sequence") {
            my_object.list({1, 0, 1, 0, 1}, 1);
            my_object.bang();
            THEN("the resulting transformation uses a modulo/wrap around model") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                REQUIRE(output.size() == 1);
                REQUIRE(output[0].size() == 4);
                atoms expected = {2, 2, 4, 4};
                REQUIRE(output[0] == expected);
            }
        }

        WHEN("the primary sequences has 0-value rest steps") {
            my_object.list({1, 0, 0, 0});
            my_object.list({1, 0}, 1);
            my_object.bang();
            THEN("the resulting transformation does not modify the rests") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                REQUIRE(output.size() == 1);
                REQUIRE(output[0].size() == 4);
                atoms expected = {2, 0, 0, 0};
                REQUIRE(output[0] == expected);
            }
        }
    }
}
