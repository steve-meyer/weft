/// @file
/// @ingroup   weft
/// @copyright Copyright 2020 Stephen Meyer. All rights reserved.
/// @license        Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min_unittest.h"  // required unit test header
#include "weft.gates.cpp"   // need the source of our object so that we can access it


SCENARIO("Object produces correct output") {
    ext_main(nullptr);    // every unit test must call ext_main() once to configure the class

    GIVEN("An instance of weft.repeater with a sequence") {

        test_wrapper<gates> an_instance;
        gates&             my_object = an_instance;
        atoms sequence = {1, 1, 5, 5, 6, 6};
        my_object.sequence = sequence;

        WHEN("it is given a gate pattern with non-integers and it is banged") {
            atoms gates;
            gates.push_back(1);
            gates.push_back("string");
            gates.push_back(0);
            my_object.gates_pattern = gates;
            my_object.bang();

            THEN("the gates are not stored and the original transformation is returned") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                atoms expected = {1, 1, 5, 5, 6, 6};
                REQUIRE(output.size() == 1);
                REQUIRE(output[0] == expected);
            }
        }

        WHEN("it is given a repeater pattern with non-integers and it is banged") {
            atoms gates = {1, 0, 0};
            my_object.gates_pattern = gates;
            my_object.bang();

            THEN("the gates are stored and the transformed sequence is returned") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                atoms expected = {1, 0, 0, 5, 0, 0};
                REQUIRE(output.size() == 1);
                REQUIRE(output[0] == expected);
            }
        }

        WHEN("the sequence that is not a multiple of the gates pattern") {
            atoms sequence = {1, 1, 5, 5, 7};
            atoms gates = {1, 0, 0};
            my_object.sequence = sequence;
            my_object.gates_pattern = gates;
            my_object.bang();

            THEN("the gates are applied to the steps in the sequence") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                atoms expected = {1, 0, 0, 5, 0};
                REQUIRE(output.size() == 1);
                REQUIRE(output[0] == expected);
            }
        }

        WHEN("the sequence is shorter than the gates pattern") {
            atoms sequence = {1, 5};
            atoms gates = {1, 0, 0};
            my_object.sequence = sequence;
            my_object.gates_pattern = gates;
            my_object.bang();

            THEN("the gates are applied to the steps in the sequence") {
                auto& output = *c74::max::object_getoutput(my_object, 0);
                atoms expected = {1, 0};
                REQUIRE(output.size() == 1);
                REQUIRE(output[0] == expected);
            }
        }
    }
}
