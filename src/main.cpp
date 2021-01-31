#include<iostream>
#include<cstring>

#include"sim.hpp"
#include"unit_tests.hpp"

// #ifdef DEBUG
//     #define RUN_TEST 1
// #else
//     #define RUN_TEST 0
// #endif


void Run(int simnum);


int main(int argc, char**argv) {

    int simnum = -1;

    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i],"-h")==0) {
            std::cout << "Help is on the way\n";
        } else if(strcmp(argv[i],"-s")==0) {
            simnum = std::stoi(argv[++i]);
            std::cout << "RUNNING SIM: " << simnum << std::endl;
        } else if(strcmp(argv[i],"-d")==0) {
            UnitTests();
        }
    }

    Run(simnum);

    return 0;
}

void Run(int simnum) {

}
