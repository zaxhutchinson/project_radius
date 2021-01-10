#include<iostream>
#include<cstring>

#include"unit_tests.hpp"

#ifdef DEBUG
    #define RUN_TEST 1
#else
    #define RUN_TEST 0
#endif

int main(int argc, char**argv) {

    if(argc > 1 ) {
        if(strcmp(argv[1],"-h")==0) {
            std::cout << "Help is on the way\n";
        } else if(strcmp(argv[1],"-d")==0) {
            UnitTests();
        }

        return 1;
    }

    if(RUN_TEST==1) {
        UnitTests();
    }


    return 0;
}