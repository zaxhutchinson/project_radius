#include<iostream>
#include<cstring>
#include<omp.h>
#include<random>

#include"sim.hpp"
#include"unit_tests.hpp"
#include"mnist_reader.hpp"

// #ifdef DEBUG
//     #define RUN_TEST 1
// #else
//     #define RUN_TEST 0
// #endif



void Run();
uptr<Network> BuildNetwork(int network_id);

int main(int argc, char**argv) {
    //-------------------------------------------------------------------------
    // INIT STUFF
    omp_set_num_threads(7);

    // MNIST
    std::string LABELS_FILENAME("mnist/train-labels-idx1-ubyte");
    std::string IMAGES_FILENAME("mnist/train-images-idx3-ubyte");
    MNISTReader mnist_reader;
    mnist_reader.LoadData(LABELS_FILENAME,IMAGES_FILENAME);

    // RANDOM
    std::random_device rd;
    RNG rng(rd());

    //-------------------------------------------------------------------------
    // WRITER
    Writer writer(std::string("/home/zax/Projects/project_radius_output/output"));

    //-------------------------------------------------------------------------
    // Process command line args.
    int network_id = -1;

    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i],"-h")==0) {
            std::cout << "Help is on the way\n";
        } else if(strcmp(argv[i],"-n")==0) {
            network_id = std::stoi(argv[++i]);
            std::cout << "RUNNING SIM: " << network_id << std::endl;
        } else if(strcmp(argv[i],"-d")==0) {
            UnitTests();
        }
    }

    //-------------------------------------------------------------------------
    // Network
    uptr<Network> network = BuildNetwork(network_id);


    //-------------------------------------------------------------------------
    // Run network
    Run();

    return 0;
}

void Run() {

}

uptr<Network> BuildNetwork(int network_id) {

    

}
