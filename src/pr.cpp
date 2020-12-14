#include<iostream>
#include<cstring>

#include"ProjectRadius.hpp"
#include"unit_tests.hpp"

int main(int argc, char**argv) {

    if(argc > 1 ) {
        if(strcmp(argv[1],"-h")==0) {
            std::cout << "Help is on the way\n";
        } else if(strcmp(argv[1],"-d")==0) {
            UnitTests();
        }

        return 1;
    }

    // TEST(Test_Vec3_A, "Test_Vec3_A");
    // TEST(Test_Vec3_B, "Test_Vec3_B (x,y,z)", 8, 8, 8);
    // TEST_SUMMARY();


    return 0;
}