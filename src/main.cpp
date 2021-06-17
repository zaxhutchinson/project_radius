#include<iostream>
#include<cstring>
#include<omp.h>
#include<random>

#include"zxlb.hpp"
#include"config.hpp"
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



void RunMNIST(
    Writer * writer,
    Network * network,
    MNISTReader & mnist,
    RNG & rng
);

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
    Writer writer(str("/home/zax/Projects/project_radius_output/output"));

    //-------------------------------------------------------------------------
    // Process command line args.
    str network_id = "mnist_network_001";

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
    // Init the config data
    config::InitConfig();

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

    network->InitWriteData();

    //-------------------------------------------------------------------------
    // Run network
    RunMNIST(
        &writer,
        network.get(),
        mnist_reader,
        rng
    );

    return 0;
}

void RunMNIST(
    Writer * writer,
    Network * network,
    MNISTReader & mnist,
    RNG & rng
) {
    zxlog::Debug("RunMNIST() called.");

    // Label, Index of output neuron
    vec<unsigned> labels = {0,1,2,3,4,5,6,7,8,9};
    umap<unsigned, sizet> labels_with_indexes = {
        {0,0},
        {1,1},
        {2,2},
        {3,3},
        {4,4},
        {5,5},
        {6,6},
        {7,7},
        {8,8},
        {9,9}
    };
    sizet num_iterations = 10;
    sizet examples_per_iteration = 1;
    sizet iteration_size = examples_per_iteration * labels.size();
    i64 time_per_example = 1000;
    sizet correct_choice = 0;
    i64 output_layer_index = network->GetOutputLayerIndex();
    i64 error_rate_start_time = 100;

    zxlog::Debug("Get MNIST data.");
    vec<vec<MNISTData>> data = mnist.GetDataAsIteration(
        labels,
        num_iterations,
        examples_per_iteration,
        rng
    );

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    vec<double> rates;
    for(sizet i = 0; i < labels.size(); i++) {
        rates.push_back(config::INCORRECT_EXPECTED);
    }

    //-------------------------------------------------------------------------
    // Start the run
    network->RebuildDendrites();

    //network->GetLayer(1)->GetNeuron(0)->PrintDendrite();

    writer->StartRecording();
    
    for(sizet i = 0; i < num_iterations; i++) {
        zxlog::Debug("Iteration " + std::to_string(i));

        for(sizet k = 0; k < iteration_size; k++) {
            zxlog::Debug("   Image " + std::to_string(k));

            // Get the image
            MNISTData & d = data[i][k];

            // Set the inputs to the pixel data
            network->SetInputs(d.image);

            // Update the error rates before and after swapping
            // to the new correct choice
            rates[correct_choice] = config::INCORRECT_EXPECTED;
            correct_choice = labels_with_indexes[d.label]; // look up neuron index.
            rates[correct_choice] = config::CORRECT_EXPECTED;


            network->UpdateLayerErrorValues(
                rates, error_rate_start_time, output_layer_index
            );



            for(i64 time = 1; time <= time_per_example; time++) {
                network->Update(
                    time,
                    writer
                );
            }

            vec<double> error_rates = network->GetErrorRates(output_layer_index);
            std::cout << "/== Iteration " << i << "  Image " << k << " ==============================//\n";
            for(sizet m = 0; m < error_rates.size(); m++) {
                std::cout << m << ":" << error_rates[m] << std::endl;
            }

            network->Reset();
        }
        network->RebuildDendrites();

        //network->GetLayer(1)->GetNeuron(0)->PrintDendrite();
    }

    network->CleanUpData(writer);

    writer->StopRecording();

}
