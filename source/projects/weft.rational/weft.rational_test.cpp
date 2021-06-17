/// @file
/// @ingroup   weft
/// @copyright Copyright 2020 Stephen Meyer. All rights reserved.
/// @license        Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min_unittest.h"  // required unit test header
#include "weft.rational.cpp"   // need the source of our object so that we can access it


SCENARIO("Object produces correct output") {
    ext_main(nullptr);    // every unit test must call ext_main() once to configure the class

     GIVEN("An instance of weft.rational with a sequence") {
    
         test_wrapper<rational> an_instance;
         rational&             my_object = an_instance;
         atoms sequence = {1, 2, 3, 4};
         my_object.sequence = sequence;

         WHEN("it is given melody number XI and it is banged") {
             my_object.melody = rational::melodies::xi;
             my_object.bang();
    
             THEN("the transformed sequence is returned") {
                 auto& output = *c74::max::object_getoutput(my_object, 0);
                 atoms expected = {
                     1, 2, 3, 3, 2,
                     2, 3, 4, 4, 3,
                     3, 4, 1, 1, 4,
                     4, 1, 2, 2, 1
                 };
                 REQUIRE(output.size() == 1);
                 REQUIRE(output[0] == expected);
             }
         }

         WHEN("it is given melody number IV and it is banged") {
             my_object.melody = rational::melodies::iv;
             my_object.bang();
    
             THEN("the transformed sequence is returned") {
                 auto& output = *c74::max::object_getoutput(my_object, 0);
                 atoms expected = {
                                 1, 0,          2, 0,          3, 0,          4, 0,
                              1, 2, 0,       3, 4, 0,       1, 2, 0,       3, 4, 0,
                           1, 2, 3, 0,    4, 1, 2, 0,    3, 4, 1, 0,    2, 3, 4, 0,
                        1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0
                 };
                 REQUIRE(output.size() == 1);
                 REQUIRE(output[0] == expected);
             }
         }

         WHEN("it is given melody number XV and it is banged") {
             atoms sequence = {1, 2, 3, 4, 5};
             my_object.sequence = sequence;
             my_object.melody = rational::melodies::xv;
             my_object.bang();

             THEN("the transformed sequence is returned") {
                 auto& output = *c74::max::object_getoutput(my_object, 0);
                 atoms expected = {
                     1, 2, 2, 3, 2, 4, 3, 5,
                     2, 1, 4, 2, 3, 3, 5, 4,
                     2, 4, 1, 3, 4, 5, 2, 1,
                     3, 2, 3, 2, 5, 1, 4, 3,
                     2, 3, 4, 5, 1, 2, 3, 4,
                     4, 3, 5, 1, 2, 2, 1, 3,
                     3, 5, 2, 4, 3, 1, 2, 3,
                     5, 4, 1, 3, 4, 3, 3
                 };
                 REQUIRE(output.size() == 1);
                 REQUIRE(output[0] == expected);
             }
         }
     }
}
