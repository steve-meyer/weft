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

    GIVEN("A default instance of weft.shifter") {

        test_wrapper<shifter> an_instance;
        shifter&             my_object = an_instance;
        
        // Seed the initial sequence
        my_object.list({ 1, 2, 3, 4 });

        WHEN("a sequence is given and it is banged") {
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
        }
    }
}
