#include<iostream>
#include<cstring>
#include<omp.h>
#include<random>

#include"sim.hpp"
#include"unit_tests.hpp"
#include"mnist_reader.hpp"
#include"log.hpp"
#include"network_template.hpp"
#include"builder.hpp"

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

    // LOGS
    zxlog::Init(true);

    // MNIST
    zxlog::Debug("MAIN: Loading MNIST data");
    std::string LABELS_FILENAME("mnist/train-labels-idx1-ubyte");
    std::string IMAGES_FILENAME("mnist/train-images-idx3-ubyte");
    MNISTReader mnist_reader;
    mnist_reader.LoadData(LABELS_FILENAME,IMAGES_FILENAME);

    //-------------------------------------------------------------------------
    // RANDOM
    zxlog::Debug("MAIN: Random init.");
    std::random_device rd;
    RNG rng(rd());

    //-------------------------------------------------------------------------
    // WRITER
    zxlog::Debug("MAIN: Setting up writer.");
    Writer writer(std::string("/home/zax/Projects/project_radius_output/output"));

    //-------------------------------------------------------------------------
    // Process command line args.
    str network_id = "-1";

    zxlog::Debug("MAIN: Processing cmd line args.");
    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i],"-h")==0) {
            std::cout << "Help is on the way\n";
        } else if(strcmp(argv[i],"-n")==0) {
            network_id = argv[++i];
            std::cout << "RUNNING SIM: " << network_id << std::endl;
        } else if(strcmp(argv[i],"-d")==0) {
            UnitTests();
        }
    }

    //-------------------------------------------------------------------------
    // Load the network templates
    zxlog::Debug("MAIN: Begin reading network templates.");
    tmps::ReadNetworkTemplate("example_template_file.json");
    zxlog::Debug("MAIN: End reading network templates.");

    //-------------------------------------------------------------------------
    // Network
    
    zxlog::Debug("MAIN: Begin building network " + network_id + ".");
    uptr<Network> network = BuildNetwork(network_id, rng);
    zxlog::Debug("MAIN: End building network " + network_id + ".");

    //-------------------------------------------------------------------------
    // Run network
    Run();

    return 0;
}

void Run() {
    std::cout << "RUN STARTED\n";
}
