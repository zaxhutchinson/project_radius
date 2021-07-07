#include<fstream>
#include<iostream>
#include<cstring>
#include<omp.h>
#include<random>
#include<cassert>

#include"zxlb.hpp"
#include"config.hpp"
#include"sim.hpp"
#include"unit_tests.hpp"
#include"mnist_reader.hpp"
#include"network_template.hpp"
#include"builder.hpp"
#include"pattern_maker.hpp"

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

void RunPattern001(
    Writer * writer,
    Network * network,
    PatternMaker * pattern_maker,
    RNG & rng
);

void RunPoisson001(
    Writer * writer,
    Network * network,
    RNG & rng
);
void RunPoisson002(
    Writer * writer,
    Network * network,
    RNG & rng
);

int main(int argc, char**argv) {
    // std::cout << std::sqrt(0) << std::endl;
    // VecS v1( -1.22428754752690549523208574101,2.83163092469335131440288932936,89.6130481824433360316106700338);
    // VecS v2( 1.22428754943845707892080554302,-0.309961738331212655062074645684,87.6133275637629225229829899035);
    // // VecS v3(  -M_PI/4.0, M_PI/2.0, 100);
    // // VecS v4(  M_PI/4.0, 0.0, 100);

    // std::cout << v2.Distance(v1) << std::endl;
    // v1.Orbit(v1.HeadingTo(v2),v1.Distance(v2));
    // std::cout << v1.Distance(v2) << std::endl;
    // return 0;

    // // std::cout << v2.VectorTo(v1).to_string() << std::endl;
    // // double head = v2.HeadingTo(v1);
    // // std::cout << Vec3(std::cos(head), std::sin(head)).to_string() << std::endl;

    // // std::cout << v1.VectorTo(v2).ToVecS().to_string() << std::endl;

    // // double hv1v2 = v1.HeadingTo(v2);
    // // double hv1v3 = v1.HeadingTo(v3);
    // // double hv1v4 = v1.HeadingTo(v4);
    // // double hv1v23 = std::abs( hv1v2-hv1v3);
    // // double hv2v1 = v2.HeadingTo(v1);
    // std::cout << "V1: " << v1.to_string() << std::endl;
    // std::cout << "V2: " << v2.to_string() << std::endl;
    // std::cout << "V3: " << v3.to_string() << std::endl;
    // std::cout << "V4: " << v4.to_string() << std::endl;
    // double dist = v2.DistanceWithRadius(v3);
    // double delta = 0.01;
    // double x= 0.0;
    // while( x < dist) {

    //     v2.Orbit(v2.HeadingTo(v3), -delta);
    //     std::cout << "V2: " << v2.to_string() << std::endl;
    //     x += delta;
    // }
    // std::cout << "V1: " << v1.to_string() << std::endl;
    // std::cout << "V2: " << v2.to_string() << std::endl;
    // std::cout << "V3: " << v3.to_string() << std::endl;
    // std::cout << "V4: " << v4.to_string() << std::endl;
    // // std::cout << "V1->V2: " << hv1v2 << std::endl;
    // // std::cout << "V1->V3: " << hv1v3 << std::endl;
    // // std::cout << "V1->V23: " << hv1v23 << std::endl;
    // // std::cout << "V1->V4: " << hv1v4 << std::endl;
    // return 0;

    // Vec3 v3a(-10,-10,-10);
    // VecS vSa = v3a.ToVecS();
    // Vec3 v3b = vSa.ToVec3();
    // VecS vSb = v3b.ToVecS();
    // std::cout << "V3A: " << v3a.to_string() << std::endl;
    // std::cout << "VSA: " << vSa.to_string() << std::endl;
    // std::cout << "V3B: " << v3b.to_string() << std::endl;
    // std::cout << "VSB: " << vSb.to_string() << std::endl;

    // return 0;
    //-------------------------------------------------------------------------
    // INIT STUFF
    omp_set_num_threads(7);

    // LOGS
    zxlog::Init(false);

    

    //-------------------------------------------------------------------------
    // RANDOM
    zxlog::Debug("MAIN: Random init.");
    std::random_device rd;
    RNG rng(rd());

    //-------------------------------------------------------------------------
    // WRITER
    zxlog::Debug("MAIN: Setting up writer.");
    Writer writer(str("/run/media/zax/a06347ed-42d6-48d5-a380-ddcfcb7fcf75/output/project_radius/output/"));

    //-------------------------------------------------------------------------
    // Process command line args.
    str network_id = "poisson_net_002";

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
    // MNIST
    //zxlog::Debug("MAIN: Loading MNIST data");
    // std::string LABELS_FILENAME("mnist/train-labels-idx1-ubyte");
    // std::string IMAGES_FILENAME("mnist/train-images-idx3-ubyte");
    // MNISTReader mnist_reader;
    // mnist_reader.LoadData(LABELS_FILENAME,IMAGES_FILENAME);

    // RunMNIST(
    //     &writer,
    //     network.get(),
    //     mnist_reader,
    //     rng
    // );

    //----------------------------------------------------------------------------
    // Pattern 001
    // zxlog::Debug("MAIN: Loading Pattern data");
    // PatternMaker pattern_maker = BuildPattern001(rng);
    // RunPattern001(
    //     &writer,
    //     network.get(),
    //     &pattern_maker,
    //     rng
    // );


    //----------------------------------------------------------------------------
    // Poisson Spike Trains
    RunPoisson002(
        &writer,
        network.get(),
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
    sizet num_iterations = 100;
    sizet examples_per_iteration = 1;
    sizet iteration_size = examples_per_iteration * labels.size();
    i64 time_per_example = 1000;
    sizet correct_choice = 0;
    i64 output_layer_index = network->GetOutputLayerIndex();

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
                rates, output_layer_index
            );


            i64 time = 1;
            for(; time <= time_per_example; time++) {
                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            vec<double> error_rates = network->GetErrorRates(output_layer_index);
            std::cout << "/== Iteration " << i << "  Image " << k << " ==============================//\n";
            for(sizet m = 0; m < error_rates.size(); m++) {
                std::cout << m << ":" << error_rates[m] << std::endl;
            }

            writer->AddExampleData(std::make_unique<ExampleData>(i, k, std::to_string(d.label)));

            network->SaveData(time);
            network->WriteData(writer);

            network->Reset();
            
        }
        network->RebuildDendrites();
        network->SaveData(-1);
        network->WriteData(writer);
        
    }

    network->CleanUpData(writer);

    writer->StopRecording();

}

void RunPattern001(
    Writer * writer,
    Network * network,
    PatternMaker * pattern_maker,
    RNG & rng
) {

    zxlog::Debug("RunPatter() called.");

    // Label, Index of output neuron
    vec<str> labels = pattern_maker->GetAllLabels();
    umap<str, sizet> labels_with_indexes;
    for(sizet i = 0; i < labels.size(); i++) {
        labels_with_indexes.emplace(labels[i], i);
    }

    sizet num_iterations = 5000;
    sizet iteration_size = pattern_maker->GetIterationSize();
    i64 time_per_example = 1000;
    sizet correct_choice = 0;
    i64 output_layer_index = network->GetOutputLayerIndex();

    zxlog::Debug("Get PatternMaker data.");
    vec<vec<Pattern>> data = pattern_maker->GetDataAsIteration(
        labels,
        num_iterations,
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

    writer->StartRecording();
    
    for(sizet i = 0; i < num_iterations; i++) {
        zxlog::Debug("Iteration " + std::to_string(i));

        for(sizet k = 0; k < iteration_size; k++) {
            zxlog::Debug("   Image " + std::to_string(k));

            // Get the image
            Pattern & d = data[i][k];

            // Set the inputs to the pixel data
            network->SetInputs(d.data);

            // Update the error rates before and after swapping
            // to the new correct choice
            rates[correct_choice] = config::INCORRECT_EXPECTED;
            correct_choice = labels_with_indexes[d.label]; // look up neuron index.
            rates[correct_choice] = config::CORRECT_EXPECTED;


            network->UpdateLayerErrorValues(
                rates, output_layer_index
            );


            i64 time = 1;
            for(; time <= time_per_example; time++) {
                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            vec<double> error_rates = network->GetErrorRates(output_layer_index);
            std::cout << "/== Iteration " << i << "  Image " << correct_choice << ":" << d.label << " ==============================//\n";
            for(sizet m = 0; m < error_rates.size(); m++) {
                std::cout << m << ":" << error_rates[m] << std::endl;
            }
            
            writer->AddExampleData(std::make_unique<ExampleData>(i, k, d.label));

            // network->SaveData(time);
            // network->WriteData(writer);

            network->Reset();
            
        }
        network->RebuildDendrites();
        network->SaveData(-1);
        network->WriteData(writer);
        
    }

    network->CleanUpData(writer);

    writer->StopRecording();

}

void RunPoisson001(
    Writer * writer,
    Network * network,
    RNG & rng
) {

    zxlog::Debug("RunPatter() called.");

    // Label, Index of output neuron
    std::uniform_int_distribution<int> dist(0,1);

    sizet num_iterations = 100;
    sizet iteration_size = 2;
    i64 time_per_example = 1000;
    i64 output_layer_index = network->GetOutputLayerIndex();

    // Build the input vector.
    uptr<InputGenerator_Poisson> ig1 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig2 = std::make_unique<InputGenerator_Poisson>();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();
    for(i64 i = 0; i < input_layer_size; i++) {

        ig1->decay.push_back(std::exp(-1.0/10));
        ig1->dist=std::uniform_real_distribution<double>(0.0,1.0);
        ig1->strength.push_back(500.0);
        ig1->signal.push_back(0.0);

        ig2->decay.push_back(std::exp(-1.0/10));
        ig2->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig2->strength.push_back(500.0);
        ig2->signal.push_back(0.0);

        if(i<20) {
            ig1->rate.push_back(0.2);
        } else {
            ig1->rate.push_back(0.01);
        }
        
        if(i>79) {
            ig2->rate.push_back(0.2);
        } else {
            ig2->rate.push_back(0.01);
        }
    }

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    vec<double> rates = {
        config::INCORRECT_EXPECTED, 
        config::INCORRECT_EXPECTED
    };
    vec<sizet> patterns = {0, 1};

    //-------------------------------------------------------------------------
    // Start the run
    network->RebuildDendrites();

    writer->StartRecording();
    
    for(sizet i = 0; i < num_iterations; i++) {
        zxlog::Debug("Iteration " + std::to_string(i));

        std::shuffle(patterns.begin(), patterns.end(), rng);

        for(sizet k = 0; k < iteration_size; k++) {
            zxlog::Debug("   Image " + std::to_string(k));

            // Get the image
            sizet pattern = patterns[k];

            if(pattern==0) {
                input_layer->AddInputGenerator(ig1.get());
                rates[0] = config::CORRECT_EXPECTED;
                rates[1] = config::INCORRECT_EXPECTED;
            } else if(pattern==1) {
                input_layer->AddInputGenerator(ig2.get());
                rates[0] = config::INCORRECT_EXPECTED;
                rates[1] = config::CORRECT_EXPECTED;
            }


            network->UpdateLayerErrorValues(
                rates, output_layer_index
            );


            i64 time = 1;
            for(; time <= time_per_example; time++) {
                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            vec<double> error_rates = network->GetErrorRates(output_layer_index);
            std::cout << "/== Iteration " << i << "  Image " << pattern << " ==============================//\n";
            for(sizet m = 0; m < error_rates.size(); m++) {
                std::cout << m << ":" << error_rates[m] << std::endl;
            }
            
            writer->AddExampleData(std::make_unique<ExampleData>(i, k, std::to_string(pattern)));

            // network->SaveData(time);
            // network->WriteData(writer);

            network->Reset();
            
        }
        network->RebuildDendrites();
        network->SaveData(-1);
        network->WriteData(writer);
        
    }

    network->CleanUpData(writer);

    writer->StopRecording();

}


void RunPoisson002(
    Writer * writer,
    Network * network,
    RNG & rng
) {

    zxlog::Debug("RunPatter() called.");

    // Label, Index of output neuron
    vec<sizet> patterns = {0, 1, 2};

    sizet num_iterations = 1000;
    sizet iteration_size = patterns.size();
    i64 time_per_example = 1000;
    i64 output_layer_index = network->GetOutputLayerIndex();

    // Build the input vector.
    uptr<InputGenerator_Poisson> ig1 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig2 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig3 = std::make_unique<InputGenerator_Poisson>();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();
    std::cout << input_layer_size << std::endl;
    for(i64 i = 0; i < input_layer_size; i++) {

        ig1->decay.push_back(std::exp(-1.0/10));
        ig1->dist=std::uniform_real_distribution<double>(0.0,1.0);
        ig1->strength.push_back(500.0);
        ig1->signal.push_back(0.0);

        ig2->decay.push_back(std::exp(-1.0/10));
        ig2->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig2->strength.push_back(500.0);
        ig2->signal.push_back(0.0);

        ig3->decay.push_back(std::exp(-1.0/10));
        ig3->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig3->strength.push_back(500.0);
        ig3->signal.push_back(0.0);
        ig3->rate.push_back(0.1);

        if(i<55) {
            ig1->rate.push_back(0.1);
        } else {
            ig1->rate.push_back(0.0);
        }
        
        if(i>=45) {
            ig2->rate.push_back(0.1);
        } else {
            ig2->rate.push_back(0.0);
        }
    }

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    vec<double> rates = {
        config::INCORRECT_EXPECTED
    };
    

    //-------------------------------------------------------------------------
    // Start the run
    network->RebuildDendrites();

    writer->StartRecording();
    
    for(sizet i = 0; i < num_iterations; i++) {
        zxlog::Debug("Iteration " + std::to_string(i));

        std::shuffle(patterns.begin(), patterns.end(), rng);

        for(sizet k = 0; k < iteration_size; k++) {
            zxlog::Debug("   Image " + std::to_string(k));

            // Get the image
            sizet pattern = patterns[k];

            if(pattern==0) {
                input_layer->AddInputGenerator(ig1.get());
                rates[0] = config::CORRECT_EXPECTED;
            } else if(pattern==1) {
                input_layer->AddInputGenerator(ig2.get());
                rates[0] = config::CORRECT_EXPECTED;
            } else if(pattern==2) {
                input_layer->AddInputGenerator(ig3.get());
                rates[0] = config::INCORRECT_EXPECTED;
            }


            network->UpdateLayerErrorValues(
                rates, output_layer_index
            );


            i64 time = 1;
            for(; time <= time_per_example; time++) {
                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            vec<double> error_rates = network->GetErrorRates(output_layer_index);
            std::cout << "/== Iteration " << i << "  Image " << pattern << " ==============================//\n";
            for(sizet m = 0; m < error_rates.size(); m++) {
                std::cout << m << ":" << error_rates[m] << std::endl;
            }
            
            writer->AddExampleData(std::make_unique<ExampleData>(i, k, std::to_string(pattern)));

            // network->SaveData(time);
            // network->WriteData(writer);

            network->Reset();
            
        }
        network->RebuildDendrites();
        network->SaveData(-1);
        network->WriteData(writer);
        
    }

    network->CleanUpData(writer);

    writer->StopRecording();

}