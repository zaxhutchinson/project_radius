#include"runs.hpp"

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
    sizet num_iterations = 1000;
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
/*****************************
 * POIS001
 *****************************/
void RunPoisson000(
    Writer * writer,
    Network * network,
    RNG & rng
) {

    zxlog::Debug("RunPatter() called.");

    // Label, Index of output neuron
    std::uniform_int_distribution<int> dist(0,2);

    sizet num_iterations = 10000;
    sizet iteration_size = 2;
    i64 time_per_example = 1000;
    i64 output_layer_index = network->GetOutputLayerIndex();

    // Build the input vector.
    uptr<InputGenerator_Poisson> ig1 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig2 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig3 = std::make_unique<InputGenerator_Poisson>();
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

        ig3->decay.push_back(std::exp(-1.0/10));
        ig3->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig3->strength.push_back(500.0);
        ig3->signal.push_back(0.0);
        ig3->rate.push_back(0.1);

        if(i==0) {
            ig1->rate.push_back(0.1);
        } else {
            ig1->rate.push_back(0.0);
        }
        
        if(i==1) {
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
    vec<sizet> patterns = {0, 1, 2, 2};

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
                rates[1] = config::CORRECT_EXPECTED;
            } else if(pattern==2) {
                input_layer->AddInputGenerator(ig3.get());
                rates[1] = config::INCORRECT_EXPECTED;
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

/*****************************************************************************
 * POIS002
 * Three patterns using spike trains. Two correct patterns (100hz-0hz) and
 * one incorrect random mask (100hz).
 ****************************************************************************/
void RunPoisson002(
    Writer * writer,
    Network * network,
    RNG & rng
) {

    zxlog::Debug("RunPatter() called.");

    // Label, Index of output neuron
    vec<sizet> patterns = {0, 1, 2};

    sizet num_iterations = 2000;
    sizet iteration_size = patterns.size();
    i64 time_per_example = 1000;
    i64 output_layer_index = network->GetOutputLayerIndex();

    // Build the input vector.
    uptr<InputGenerator_Poisson> ig1 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig2 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig3 = std::make_unique<InputGenerator_Poisson>();
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

        ig3->decay.push_back(std::exp(-1.0/10));
        ig3->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig3->strength.push_back(500.0);
        ig3->signal.push_back(0.0);
        ig3->rate.push_back(0.05);

        if(i<10) {
            ig1->rate.push_back(0.1);
        } else if(i%2==0) {
            ig1->rate.push_back(0.1);
        } else {
            ig1->rate.push_back(0.01);
        }
        
        if(i<10) {
            ig2->rate.push_back(0.1);
        } else if(i%2==1) {
            ig2->rate.push_back(0.1);
        } else {
            ig2->rate.push_back(0.01);
        }
    }

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    // Size of the rate vector is equal to the number of
    // output neurons.
    vec<double> rates = {
        config::INCORRECT_EXPECTED
    };
    

    //-------------------------------------------------------------------------
    // Start the run
    network->RebuildDendrites();

    writer->StartRecording();
    
    for(sizet i = 0; i < num_iterations; i++) {
        std::cout << i << "\r" << std::flush;

        std::shuffle(patterns.begin(), patterns.end(), rng);

        for(sizet k = 0; k < iteration_size; k++) {

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

            
            
            writer->AddExampleData(std::make_unique<ExampleData>(i, k, std::to_string(pattern)));

            // network->SaveData(time);
            // network->WriteData(writer);

            network->Reset();
            
        }
        network->RebuildDendrites();
        network->SaveData(-1);
        network->WriteData(writer);
        network->RandomizeOrder(rng);
        
    }

    network->CleanUpData(writer);

    writer->StopRecording();

}


/*****************************************************************************
 * POIS003
 * Similar to two except.
 * Three patterns using spike trains. Two correct patterns (100hz-10hz) and
 * one incorrect random mask (100hz).
 ****************************************************************************/
void RunPoisson003(
    Writer * writer,
    Network * network,
    RNG & rng
) {

    zxlog::Debug("RunPatter() called.");

    // Label, Index of output neuron
    vec<sizet> patterns = {0, 1, 2, 2};

    sizet num_iterations = 2000;
    sizet iteration_size = patterns.size();
    i64 time_per_example = 1000;
    i64 output_layer_index = network->GetOutputLayerIndex();

    // Build the input vector.
    uptr<InputGenerator_Poisson> ig1 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig2 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig3 = std::make_unique<InputGenerator_Poisson>();
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

        ig3->decay.push_back(std::exp(-1.0/10));
        ig3->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig3->strength.push_back(500.0);
        ig3->signal.push_back(0.0);
        ig3->rate.push_back(0.1);

        if(i<55) {
            ig1->rate.push_back(0.1);
        } else {
            ig1->rate.push_back(0.01);
        }
        
        if(i>=45) {
            ig2->rate.push_back(0.1);
        } else {
            ig2->rate.push_back(0.01);
        }
    }

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    // Size of the rate vector is equal to the number of
    // output neurons.
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



/*****************************************************************************
 * POIS004
 ****************************************************************************/
void RunPoisson004(
    Writer * writer,
    Network * network,
    RNG & rng
) {

    zxlog::Debug("RunPatter() called.");

    // Label, Index of output neuron
    vec<sizet> patterns = {0, 1, 2, 3, 4};

    sizet num_iterations = 3000;
    sizet iteration_size = patterns.size();
    i64 time_per_example = 1000;
    i64 output_layer_index = network->GetOutputLayerIndex();

    // Build the input vector.
    uptr<InputGenerator_Poisson> ig1 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig2 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig3 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig4 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig5 = std::make_unique<InputGenerator_Poisson>();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();
    
    for(i64 i = 0; i < input_layer_size; i++) {

        ig1->id = "1";
        ig1->decay.push_back(std::exp(-1.0/10));
        ig1->dist=std::uniform_real_distribution<double>(0.0,1.0);
        ig1->strength.push_back(500.0);
        ig1->signal.push_back(0.0);

        ig2->id = "2";
        ig2->decay.push_back(std::exp(-1.0/10));
        ig2->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig2->strength.push_back(500.0);
        ig2->signal.push_back(0.0);

        ig3->id = "3";
        ig3->decay.push_back(std::exp(-1.0/10));
        ig3->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig3->strength.push_back(500.0);
        ig3->signal.push_back(0.0);

        ig4->id = "4";
        ig4->decay.push_back(std::exp(-1.0/10));
        ig4->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig4->strength.push_back(500.0);
        ig4->signal.push_back(0.0);

        ig5->id = "5";
        ig5->decay.push_back(std::exp(-1.0/10));
        ig5->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig5->strength.push_back(500.0);
        ig5->signal.push_back(0.0);
        ig5->rate.push_back(0.005);

        if(i<10) {
            ig1->rate.push_back(0.1);
        } else {
            ig1->rate.push_back(0.0);
        }
        
        if(i>=10 && i<20) {
            ig2->rate.push_back(0.1);
        } else {
            ig2->rate.push_back(0.0);
        }

        if(i>=20 && i<30) {
            ig3->rate.push_back(0.1);
        } else {
            ig3->rate.push_back(0.0);
        }

        if(i>=30 && i<40) {
            ig4->rate.push_back(0.1);
        } else {
            ig4->rate.push_back(0.0);
        }
    }

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    // Size of the rate vector is equal to the number of
    // output neurons.
    vec<double> rates = {
        config::INCORRECT_EXPECTED
    };
    

    //-------------------------------------------------------------------------
    // Start the run
    network->RebuildDendrites();

    writer->StartRecording();
    
    for(sizet i = 0; i < num_iterations; i++) {
        std::cout << i << "\r" << std::flush;
        zxlog::Debug("Iteration " + std::to_string(i));

        std::shuffle(patterns.begin(), patterns.end(), rng);

        for(sizet k = 0; k < iteration_size; k++) {
            zxlog::Debug("   Image " + std::to_string(k));

            // Get the image
            sizet pattern = patterns[k];
            
            //std::cout << "/== Iteration " << i << "  Pattern2 " << pattern << " ==============================//\n";

                // input_layer->AddInputGenerator(ig5.get());
                // rates[0] = config::INCORRECT_EXPECTED;



            if(pattern==0) {
                input_layer->AddInputGenerator(ig1.get());
                rates[0] = config::CORRECT_EXPECTED;
            } else if(pattern==1) {
                input_layer->AddInputGenerator(ig2.get());
                rates[0] = config::CORRECT_EXPECTED;
            } else if(pattern==2) {
                input_layer->AddInputGenerator(ig3.get());
                rates[0] = config::CORRECT_EXPECTED;
            } else if(pattern==3) {
                input_layer->AddInputGenerator(ig4.get());
                rates[0] = config::CORRECT_EXPECTED;
            } else if(pattern==4) {
                input_layer->AddInputGenerator(ig5.get());
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


            if(pattern==0) {
                ig1->Reset();
            } else if(pattern==1) {
                ig2->Reset();
            } else if(pattern==2) {
                ig3->Reset();
            } else if(pattern==3) {
                ig4->Reset();
            } else if(pattern==4) {
                ig5->Reset();
            }



            vec<double> error_rates = network->GetErrorRates(output_layer_index);
            
            // for(sizet m = 0; m < error_rates.size(); m++) {
            //     std::cout << m << ":" << error_rates[m] << std::endl;
            // }
            
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



/*****************************************************************************
 * POIS004_B
 ****************************************************************************/
void RunPoisson004_B(
    Writer * writer,
    Network * network,
    RNG & rng
) {

    zxlog::Debug("RunPatter() called.");

    // Label, Index of output neuron
    vec<sizet> patterns = {0, 1, 2, 3, 4};

    sizet num_iterations = 4000;
    sizet iteration_size = patterns.size();
    i64 time_per_example = 1000;
    i64 output_layer_index = network->GetOutputLayerIndex();

    // Build the input vector.
    uptr<InputGenerator_Poisson> ig1 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig2 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig3 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig4 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig5 = std::make_unique<InputGenerator_Poisson>();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();
    
    for(i64 i = 0; i < input_layer_size; i++) {

        ig1->id = "1";
        ig1->decay.push_back(std::exp(-1.0/10));
        ig1->dist=std::uniform_real_distribution<double>(0.0,1.0);
        ig1->strength.push_back(500.0);
        ig1->signal.push_back(0.0);

        ig2->id = "2";
        ig2->decay.push_back(std::exp(-1.0/10));
        ig2->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig2->strength.push_back(500.0);
        ig2->signal.push_back(0.0);

        ig3->id = "3";
        ig3->decay.push_back(std::exp(-1.0/10));
        ig3->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig3->strength.push_back(500.0);
        ig3->signal.push_back(0.0);

        ig4->id = "4";
        ig4->decay.push_back(std::exp(-1.0/10));
        ig4->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig4->strength.push_back(500.0);
        ig4->signal.push_back(0.0);

        ig5->id = "5";
        ig5->decay.push_back(std::exp(-1.0/10));
        ig5->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig5->strength.push_back(500.0);
        ig5->signal.push_back(0.0);
        ig5->rate.push_back(0.01);

        if(i==39 || i <=10) {
            ig1->rate.push_back(0.1);
        } else {
            ig1->rate.push_back(0.0);
        }
        
        if(i>=9 && i<=20) {
            ig2->rate.push_back(0.1);
        } else {
            ig2->rate.push_back(0.0);
        }

        if(i>=19 && i<=30) {
            ig3->rate.push_back(0.1);
        } else {
            ig3->rate.push_back(0.0);
        }

        if(i==0 || (i>=29 && i<40)) {
            ig4->rate.push_back(0.1);
        } else {
            ig4->rate.push_back(0.0);
        }
    }

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    // Size of the rate vector is equal to the number of
    // output neurons.
    vec<double> rates = {
        config::INCORRECT_EXPECTED
    };
    

    //-------------------------------------------------------------------------
    // Start the run
    network->RebuildDendrites();

    writer->StartRecording();
    
    for(sizet i = 0; i < num_iterations; i++) {
        std::cout << i << "\r" << std::flush;
        zxlog::Debug("Iteration " + std::to_string(i));

        std::shuffle(patterns.begin(), patterns.end(), rng);

        for(sizet k = 0; k < iteration_size; k++) {
            zxlog::Debug("   Image " + std::to_string(k));

            // Get the image
            sizet pattern = patterns[k];
            
            //std::cout << "/== Iteration " << i << "  Pattern2 " << pattern << " ==============================//\n";

                // input_layer->AddInputGenerator(ig5.get());
                // rates[0] = config::INCORRECT_EXPECTED;



            if(pattern==0) {
                input_layer->AddInputGenerator(ig1.get());
                rates[0] = config::CORRECT_EXPECTED;
            } else if(pattern==1) {
                input_layer->AddInputGenerator(ig2.get());
                rates[0] = config::CORRECT_EXPECTED;
            } else if(pattern==2) {
                input_layer->AddInputGenerator(ig3.get());
                rates[0] = config::CORRECT_EXPECTED;
            } else if(pattern==3) {
                input_layer->AddInputGenerator(ig4.get());
                rates[0] = config::CORRECT_EXPECTED;
            } else if(pattern==4) {
                input_layer->AddInputGenerator(ig5.get());
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


            if(pattern==0) {
                ig1->Reset();
            } else if(pattern==1) {
                ig2->Reset();
            } else if(pattern==2) {
                ig3->Reset();
            } else if(pattern==3) {
                ig4->Reset();
            } else if(pattern==4) {
                ig5->Reset();
            }



            vec<double> error_rates = network->GetErrorRates(output_layer_index);
            
            // for(sizet m = 0; m < error_rates.size(); m++) {
            //     std::cout << m << ":" << error_rates[m] << std::endl;
            // }
            
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

/*****************************************************************************
 * POIS004_C
 ****************************************************************************/
void RunPoisson004_C(
    Writer * writer,
    Network * network,
    RNG & rng
) {

    zxlog::Debug("RunPatter() called.");

    // Label, Index of output neuron
    vec<sizet> patterns = {0, 1, 2, 3, 4, 5, 6, 7};

    sizet num_iterations = 20000;
    sizet iteration_size = patterns.size();
    i64 time_per_example = 1000;
    i64 output_layer_index = network->GetOutputLayerIndex();

    // Build the input vector.
    uptr<InputGenerator_Poisson> ig1 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig2 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig3 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig4 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig5 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig6 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig7 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig8 = std::make_unique<InputGenerator_Poisson>();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();
    
    for(i64 i = 0; i < input_layer_size; i++) {

        ig1->id = "1";
        ig1->decay.push_back(std::exp(-1.0/10));
        ig1->dist=std::uniform_real_distribution<double>(0.0,1.0);
        ig1->strength.push_back(500.0);
        ig1->signal.push_back(0.0);

        ig2->id = "2";
        ig2->decay.push_back(std::exp(-1.0/10));
        ig2->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig2->strength.push_back(500.0);
        ig2->signal.push_back(0.0);

        ig3->id = "3";
        ig3->decay.push_back(std::exp(-1.0/10));
        ig3->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig3->strength.push_back(500.0);
        ig3->signal.push_back(0.0);

        ig4->id = "4";
        ig4->decay.push_back(std::exp(-1.0/10));
        ig4->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig4->strength.push_back(500.0);
        ig4->signal.push_back(0.0);

        ig5->id = "5";
        ig5->decay.push_back(std::exp(-1.0/10));
        ig5->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig5->strength.push_back(500.0);
        ig5->signal.push_back(0.0);

        ig6->id = "6";
        ig6->decay.push_back(std::exp(-1.0/10));
        ig6->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig6->strength.push_back(500.0);
        ig6->signal.push_back(0.0);

        ig7->id = "7";
        ig7->decay.push_back(std::exp(-1.0/10));
        ig7->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig7->strength.push_back(500.0);
        ig7->signal.push_back(0.0);

        ig8->id = "8";
        ig8->decay.push_back(std::exp(-1.0/10));
        ig8->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig8->strength.push_back(500.0);
        ig8->signal.push_back(0.0);
        ig8->rate.push_back(0.005);

        if(i>=0 && i<=6) {
            ig1->rate.push_back(0.1);
        } else {
            ig1->rate.push_back(0.0);
        }
        
        if(i==0) {
            ig2->rate.push_back(0.1);
        } else if(i>=7 && i<=12) {
            ig2->rate.push_back(0.07);
        } else {
            ig2->rate.push_back(0.0);
        }

        if(i==6) {
            ig3->rate.push_back(0.1);
        } else if(i>=13 && i<=18) {
            ig3->rate.push_back(0.07);
        } else {
            ig3->rate.push_back(0.0);
        }

        if(i==7) {
            ig4->rate.push_back(0.07);
        } else if(i>=19 && i<=24) {
            ig4->rate.push_back(0.04);
        } else {
            ig4->rate.push_back(0.0);
        }
        
        if(i==12) {
            ig5->rate.push_back(0.07);
        } else if(i>=25 && i<=30) {
            ig5->rate.push_back(0.04);
        } else {
            ig5->rate.push_back(0.0);
        }

        if(i==13) {
            ig6->rate.push_back(0.07);
        } else if(i>=31 && i<=36) {
            ig6->rate.push_back(0.04);
        } else {
            ig6->rate.push_back(0.0);
        }

        if(i==18) {
            ig7->rate.push_back(0.07);
        } else if(i>=37 && i<=42) {
            ig7->rate.push_back(0.04);
        } else {
            ig7->rate.push_back(0.0);
        }

    }

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    // Size of the rate vector is equal to the number of
    // output neurons.
    vec<double> rates = {
        config::INCORRECT_EXPECTED
    };
    

    //-------------------------------------------------------------------------
    // Start the run
    network->RebuildDendrites();

    writer->StartRecording();
    
    for(sizet i = 0; i < num_iterations; i++) {
        std::cout << i << "\r" << std::flush;
        zxlog::Debug("Iteration " + std::to_string(i));

        std::shuffle(patterns.begin(), patterns.end(), rng);

        for(sizet k = 0; k < iteration_size; k++) {
            zxlog::Debug("   Image " + std::to_string(k));

            // Get the image
            sizet pattern = patterns[k];
            
            //std::cout << "/== Iteration " << i << "  Pattern2 " << pattern << " ==============================//\n";

                // input_layer->AddInputGenerator(ig5.get());
                // rates[0] = config::INCORRECT_EXPECTED;



            if(pattern==0) {
                input_layer->AddInputGenerator(ig1.get());
                rates[0] = config::CORRECT_EXPECTED;
            } else if(pattern==1) {
                input_layer->AddInputGenerator(ig2.get());
                rates[0] = config::CORRECT_EXPECTED;
            } else if(pattern==2) {
                input_layer->AddInputGenerator(ig3.get());
                rates[0] = config::CORRECT_EXPECTED;
            } else if(pattern==3) {
                input_layer->AddInputGenerator(ig4.get());
                rates[0] = config::CORRECT_EXPECTED;
            } else if(pattern==4) {
                input_layer->AddInputGenerator(ig5.get());
                rates[0] = config::CORRECT_EXPECTED;
            } else if(pattern==5) {
                input_layer->AddInputGenerator(ig6.get());
                rates[0] = config::CORRECT_EXPECTED;
            } else if(pattern==6) {
                input_layer->AddInputGenerator(ig7.get());
                rates[0] = config::CORRECT_EXPECTED;
            } else if(pattern==7) {
                input_layer->AddInputGenerator(ig8.get());
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


            if(pattern==0) {
                ig1->Reset();
            } else if(pattern==1) {
                ig2->Reset();
            } else if(pattern==2) {
                ig3->Reset();
            } else if(pattern==3) {
                ig4->Reset();
            } else if(pattern==4) {
                ig5->Reset();
            } else if(pattern==5) {
                ig6->Reset();
            } else if(pattern==6) {
                ig7->Reset();
            } else if(pattern==7) {
                ig8->Reset();
            }



            vec<double> error_rates = network->GetErrorRates(output_layer_index);
            
            // for(sizet m = 0; m < error_rates.size(); m++) {
            //     std::cout << m << ":" << error_rates[m] << std::endl;
            // }
            
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

/*****************************************************************************
 * POIS004_D
 ****************************************************************************/
void RunPoisson004_D(
    Writer * writer,
    Network * network,
    RNG & rng
) {

    zxlog::Debug("RunPatter() called.");

    // Label, Index of output neuron
    vec<sizet> patterns = {0, 1, 2, 3, 4};

    sizet num_iterations = 5000;
    sizet iteration_size = patterns.size();
    i64 time_per_example = 1000;
    i64 output_layer_index = network->GetOutputLayerIndex();

    // Build the input vector.
    uptr<InputGenerator_Poisson> ig1 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig2 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig3 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig4 = std::make_unique<InputGenerator_Poisson>();
    uptr<InputGenerator_Poisson> ig5 = std::make_unique<InputGenerator_Poisson>();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();
    
    for(i64 i = 0; i < input_layer_size; i++) {

        ig1->id = "1";
        ig1->decay.push_back(std::exp(-1.0/10));
        ig1->dist=std::uniform_real_distribution<double>(0.0,1.0);
        ig1->strength.push_back(500.0);
        ig1->signal.push_back(0.0);

        ig2->id = "2";
        ig2->decay.push_back(std::exp(-1.0/10));
        ig2->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig2->strength.push_back(500.0);
        ig2->signal.push_back(0.0);

        ig3->id = "3";
        ig3->decay.push_back(std::exp(-1.0/10));
        ig3->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig3->strength.push_back(500.0);
        ig3->signal.push_back(0.0);

        ig4->id = "4";
        ig4->decay.push_back(std::exp(-1.0/10));
        ig4->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig4->strength.push_back(500.0);
        ig4->signal.push_back(0.0);

        ig5->id = "5";
        ig5->decay.push_back(std::exp(-1.0/10));
        ig5->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig5->strength.push_back(500.0);
        ig5->signal.push_back(0.0);
        ig5->rate.push_back(0.005);

        if(i==39 || i <=10) {
            if(i>=0 && i <=2) ig1->rate.push_back(0.1);
            else if(i>=3 && i <=5) ig1->rate.push_back(0.05);
            else if(i>=6 && i <=9) ig1->rate.push_back(0.025);
            else ig1->rate.push_back(0.0);
        } else {
            ig1->rate.push_back(0.0);
        }
        
        if(i>=9 && i<=20) {
            if(i>=10 && i<=12) ig2->rate.push_back(0.1);
            else if(i>=13 && i<=15) ig2->rate.push_back(0.05);
            else if(i>=16 && i<=19) ig2->rate.push_back(0.025);
            else ig2->rate.push_back(0.0);
        } else {
            ig2->rate.push_back(0.0);
        }

        if(i>=19 && i<=30) {
            if(i>=20 && i<=22) ig3->rate.push_back(0.1);
            else if(i>=23 && i<=25) ig3->rate.push_back(0.05);
            else if(i>=26 && i<=29) ig3->rate.push_back(0.025);
            else ig3->rate.push_back(0.0);
        } else {
            ig3->rate.push_back(0.0);
        }

        if(i==0 || (i>=29 && i<40)) {
            if(i>=30 && i<=32) ig4->rate.push_back(0.1);
            else if(i>=33 && i<=35) ig4->rate.push_back(0.05);
            else if(i>=36 && i<=39) ig4->rate.push_back(0.025);
            else ig4->rate.push_back(0.0);
        } else {
            ig4->rate.push_back(0.0);
        }
    }

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    // Size of the rate vector is equal to the number of
    // output neurons.
    vec<double> rates = {
        config::INCORRECT_EXPECTED
    };
    

    //-------------------------------------------------------------------------
    // Start the run
    network->RebuildDendrites();

    writer->StartRecording();
    
    for(sizet i = 0; i < num_iterations; i++) {
        std::cout << i << "\r" << std::flush;
        zxlog::Debug("Iteration " + std::to_string(i));

        std::shuffle(patterns.begin(), patterns.end(), rng);

        for(sizet k = 0; k < iteration_size; k++) {
            zxlog::Debug("   Image " + std::to_string(k));

            // Get the image
            sizet pattern = patterns[k];
            
            //std::cout << "/== Iteration " << i << "  Pattern2 " << pattern << " ==============================//\n";

                // input_layer->AddInputGenerator(ig5.get());
                // rates[0] = config::INCORRECT_EXPECTED;



            if(pattern==0) {
                input_layer->AddInputGenerator(ig1.get());
                rates[0] = config::CORRECT_EXPECTED;
            } else if(pattern==1) {
                input_layer->AddInputGenerator(ig2.get());
                rates[0] = config::CORRECT_EXPECTED;
            } else if(pattern==2) {
                input_layer->AddInputGenerator(ig3.get());
                rates[0] = config::CORRECT_EXPECTED;
            } else if(pattern==3) {
                input_layer->AddInputGenerator(ig4.get());
                rates[0] = config::CORRECT_EXPECTED;
            } else if(pattern==4) {
                input_layer->AddInputGenerator(ig5.get());
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


            if(pattern==0) {
                ig1->Reset();
            } else if(pattern==1) {
                ig2->Reset();
            } else if(pattern==2) {
                ig3->Reset();
            } else if(pattern==3) {
                ig4->Reset();
            } else if(pattern==4) {
                ig5->Reset();
            }



            vec<double> error_rates = network->GetErrorRates(output_layer_index);
            
            // for(sizet m = 0; m < error_rates.size(); m++) {
            //     std::cout << m << ":" << error_rates[m] << std::endl;
            // }
            
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



/*****************************************************************************
 * ExpBeacon
 ****************************************************************************/
void RunExpBeacon(
    Writer * writer,
    Network * network,
    RNG & rng
) {
    zxlog::Debug("ExpBeacon Run.");

    ExpBeacon beacon;
    beacon.Init(rng());
    ExpBeaconEntry * entry;

    sizet num_iterations = 200;
    sizet iteration_size = beacon.GetSizeLabeledEntries();
    i64 time_per_example = 1000;
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();
    sizet mask_interval = 10000000000000;

    uptr<InputGenerator_Poisson> ig = std::make_unique<InputGenerator_Poisson>();
    ig->id = "0";
    for(i64 i = 0; i < input_layer_size; i++) {
        ig->decay.push_back(std::exp(-1.0/10));
        ig->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig->strength.push_back(500.0);
        ig->signal.push_back(0.0);
        ig->rate.push_back(0.005);
    }

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    // Size of the rate vector is equal to the number of
    // output neurons.
    vec<double> rates = {
        config::INCORRECT_EXPECTED
    };
    //-------------------------------------------------------------------------
    // Start the run
    network->RebuildDendrites();

    vec<double> zero_inputs;
    for(int i = 0; i < input_layer_size; i++) {
        zero_inputs.push_back(0.0);
    }

    writer->StartRecording();
    //vec<double> input;
    //input.reserve(13);
    for(sizet i = 0; i < num_iterations; i++) {
        std::cout << i << "\r" << std::flush;
        zxlog::Debug("Iteration " + std::to_string(i));

        //-----------------------------------------------------------
        // Exp BEACON start
        beacon.ShuffleLabeledEntries();
        // Get the entry

        for(sizet k = 0; k < iteration_size; k++) {
            entry = beacon.GetLabeledEntry(k);

            //input.clear();
            //std::copy(std::begin(entry->beacons), std::end(entry->beacons),std::back_inserter(input));
            
            network->SetInputs(entry->beacons);

            // Set the rate to correct
            rates[0] = config::CORRECT_EXPECTED;

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
            network->Reset();


            // MASK
            if(k%mask_interval==0) {
                network->SetInputs(zero_inputs);
                input_layer->AddInputGenerator(ig.get());
                rates[0] = config::INCORRECT_EXPECTED;
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

                // Reset the input generator
                ig->Reset();

                network->Reset();
                network->RandomizeOrder(rng);
            }
        }

    
        // END OF ITERATION
        //------------------------------------------------------   

        network->RebuildDendrites();
        network->SaveData(-1);
        network->WriteData(writer);
        
    }

    network->CleanUpData(writer);

    writer->StopRecording();

}