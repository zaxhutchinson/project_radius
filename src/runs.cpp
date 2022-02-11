#include"runs.hpp"

void RunMNIST(
    Writer * writer,
    Network * network,
    RNG & rng
) {
    std::string LABELS_FILENAME("exp_mnist/train-labels-idx1-ubyte");
    std::string IMAGES_FILENAME("exp_mnist/train-images-idx3-ubyte");


    MNISTReader mnist;
    mnist.LoadData(LABELS_FILENAME,IMAGES_FILENAME);
    vec<MNISTData> data; 
    vec<MNISTData> dataA = mnist.GetDataWithLabel(3);
    vec<MNISTData> dataB = mnist.GetDataWithLabel(8);
    std::copy(dataA.begin(), dataA.end(), std::back_inserter(data));
    std::copy(dataB.begin(), dataB.end(), std::back_inserter(data));
    
    sizet num_iterations = 100;
    // sizet examples_per_iteration = 1;
    sizet iteration_size = 100;
    i64 time_per_example = 1000;
    sizet mask_interval = 10;
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();

    vec<sizet> image_indexes;
    for(sizet i = 0; i < mnist.GetData().size(); i++) {
        image_indexes.push_back(i);
    }

    uptr<InputGenerator_Poisson> ig_mask = std::make_unique<InputGenerator_Poisson>();
    ig_mask->id = "0";
    for(i64 i = 0; i < input_layer_size; i++) {
        ig_mask->decay.push_back(std::exp(-1.0/10));
        ig_mask->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig_mask->strength.push_back(500.0);
        ig_mask->signal.push_back(0.0);
        ig_mask->rate.push_back(0.05);
    }

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    vec<double> rates = {zxlb::CORRECT_EXPECTED};
    // for(sizet i = 0; i < 10; i++) {
    //     rates.push_back(zxlb::INCORRECT_EXPECTED);
    // }

    //-------------------------------------------------------------------------
    // Start the run
    // network->RebuildDendrites();

    writer->StartRecording();
    
    for(sizet i = 0; i < num_iterations; i++) {
        std::cout << "Iteration: " << i << std::endl;

        std::shuffle(data.begin(), data.end(), rng);


        for(sizet k = 0; k < iteration_size; k++) {
            std::cout << k << "\r" << std::flush;

            // Get the image
            uptr<InputGenerator_Poisson> ig = mnist.GetDataAsPoissonInputGenerator(data[k]);

            // Set the inputs to the pixel data
            input_layer->AddInputGenerator(ig.get());


            rates[0] = zxlb::CORRECT_EXPECTED;


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
            network->RandomizeOrder(rng);

            if(k%mask_interval==0) {
                input_layer->AddInputGenerator(ig_mask.get());
                rates[0] = zxlb::INCORRECT_EXPECTED;
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
                ig_mask->Reset();

                network->Reset();
                network->RandomizeOrder(rng);
            }
            
        }
        std::cout << std::endl;
        //network->RebuildDendrites();
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
        rates.push_back(zxlb::INCORRECT_EXPECTED);
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
            rates[correct_choice] = zxlb::INCORRECT_EXPECTED;
            correct_choice = labels_with_indexes[d.label]; // look up neuron index.
            rates[correct_choice] = zxlb::CORRECT_EXPECTED;


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
        zxlb::INCORRECT_EXPECTED
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
                rates[0] = zxlb::CORRECT_EXPECTED;
            } else if(pattern==1) {
                input_layer->AddInputGenerator(ig2.get());
                rates[1] = zxlb::CORRECT_EXPECTED;
            } else if(pattern==2) {
                input_layer->AddInputGenerator(ig3.get());
                rates[1] = zxlb::INCORRECT_EXPECTED;
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
        zxlb::INCORRECT_EXPECTED, 
        zxlb::INCORRECT_EXPECTED
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
                rates[0] = zxlb::CORRECT_EXPECTED;
                rates[1] = zxlb::INCORRECT_EXPECTED;
            } else if(pattern==1) {
                input_layer->AddInputGenerator(ig2.get());
                rates[0] = zxlb::INCORRECT_EXPECTED;
                rates[1] = zxlb::CORRECT_EXPECTED;
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

    sizet num_iterations = 5000;
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
        zxlb::INCORRECT_EXPECTED
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
                rates[0] = zxlb::CORRECT_EXPECTED;
            } else if(pattern==1) {
                input_layer->AddInputGenerator(ig2.get());
                rates[0] = zxlb::CORRECT_EXPECTED;
            } else if(pattern==2) {
                input_layer->AddInputGenerator(ig3.get());
                rates[0] = zxlb::INCORRECT_EXPECTED;
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
        zxlb::INCORRECT_EXPECTED
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
                rates[0] = zxlb::CORRECT_EXPECTED;
            } else if(pattern==1) {
                input_layer->AddInputGenerator(ig2.get());
                rates[0] = zxlb::CORRECT_EXPECTED;
            } else if(pattern==2) {
                input_layer->AddInputGenerator(ig3.get());
                rates[0] = zxlb::INCORRECT_EXPECTED;
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
        zxlb::INCORRECT_EXPECTED
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
                // rates[0] = zxlb::INCORRECT_EXPECTED;



            if(pattern==0) {
                input_layer->AddInputGenerator(ig1.get());
                rates[0] = zxlb::CORRECT_EXPECTED;
            } else if(pattern==1) {
                input_layer->AddInputGenerator(ig2.get());
                rates[0] = zxlb::CORRECT_EXPECTED;
            } else if(pattern==2) {
                input_layer->AddInputGenerator(ig3.get());
                rates[0] = zxlb::CORRECT_EXPECTED;
            } else if(pattern==3) {
                input_layer->AddInputGenerator(ig4.get());
                rates[0] = zxlb::CORRECT_EXPECTED;
            } else if(pattern==4) {
                input_layer->AddInputGenerator(ig5.get());
                rates[0] = zxlb::INCORRECT_EXPECTED;
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

    sizet num_iterations = 2000;
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
    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();

    double RATE_ON = 0.01;
    double RATE_OFF = 0.001;
    double RATE_MASK = 0.005;
    
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
        ig5->rate.push_back(RATE_MASK);

        if(i==39 || i <=10) {
            ig1->rate.push_back(RATE_ON);
        } else {
            ig1->rate.push_back(RATE_OFF);
        }
        
        if(i>=9 && i<=20) {
            ig2->rate.push_back(RATE_ON);
        } else {
            ig2->rate.push_back(RATE_OFF);
        }

        if(i>=19 && i<=30) {
            ig3->rate.push_back(RATE_ON);
        } else {
            ig3->rate.push_back(RATE_OFF);
        }

        if(i==0 || (i>=29 && i<40)) {
            ig4->rate.push_back(RATE_ON);
        } else {
            ig4->rate.push_back(RATE_OFF);
        }
    }

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    // Size of the rate vector is equal to the number of
    // output neurons.
    vec<double> rates = {
        zxlb::INCORRECT_EXPECTED
    };
    

    //-------------------------------------------------------------------------
    // Start the run
    

    writer->StartRecording();
    
    for(sizet i = 0; i < num_iterations; i++) {

        std::shuffle(patterns.begin(), patterns.end(), rng);

        network->RebuildDendrites();

        for(sizet k = 0; k < iteration_size; k++) {
            std::cout << i << " " << k << "\r" << std::flush;

            // Get the image
            sizet pattern = patterns[k];
            
            //std::cout << "/== Iteration " << i << "  Pattern2 " << pattern << " ==============================//\n";

                // input_layer->AddInputGenerator(ig5.get());
                // rates[0] = zxlb::INCORRECT_EXPECTED;



            if(pattern==0) {
                input_layer->AddInputGenerator(ig1.get());
                output_layer->SetTraining(true,true,true);
                rates[0] = zxlb::CORRECT_EXPECTED;
            } else if(pattern==1) {
                input_layer->AddInputGenerator(ig2.get());
                output_layer->SetTraining(true,true,true);
                rates[0] = zxlb::CORRECT_EXPECTED;
            } else if(pattern==2) {
                input_layer->AddInputGenerator(ig3.get());
                output_layer->SetTraining(true,true,true);
                rates[0] = zxlb::CORRECT_EXPECTED;
            } else if(pattern==3) {
                input_layer->AddInputGenerator(ig4.get());
                output_layer->SetTraining(true,true,true);
                rates[0] = zxlb::CORRECT_EXPECTED;
            } else if(pattern==4) {
                input_layer->AddInputGenerator(ig5.get());
                output_layer->SetTraining(false,true,false);
                rates[0] = zxlb::INCORRECT_EXPECTED;
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
            
            writer->AddExampleData(std::make_unique<ExampleData>(i, k, std::to_string(pattern)));

            network->Reset();
            
        }
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
        zxlb::INCORRECT_EXPECTED
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
                // rates[0] = zxlb::INCORRECT_EXPECTED;



            if(pattern==0) {
                input_layer->AddInputGenerator(ig1.get());
                rates[0] = zxlb::CORRECT_EXPECTED;
            } else if(pattern==1) {
                input_layer->AddInputGenerator(ig2.get());
                rates[0] = zxlb::CORRECT_EXPECTED;
            } else if(pattern==2) {
                input_layer->AddInputGenerator(ig3.get());
                rates[0] = zxlb::CORRECT_EXPECTED;
            } else if(pattern==3) {
                input_layer->AddInputGenerator(ig4.get());
                rates[0] = zxlb::CORRECT_EXPECTED;
            } else if(pattern==4) {
                input_layer->AddInputGenerator(ig5.get());
                rates[0] = zxlb::CORRECT_EXPECTED;
            } else if(pattern==5) {
                input_layer->AddInputGenerator(ig6.get());
                rates[0] = zxlb::CORRECT_EXPECTED;
            } else if(pattern==6) {
                input_layer->AddInputGenerator(ig7.get());
                rates[0] = zxlb::CORRECT_EXPECTED;
            } else if(pattern==7) {
                input_layer->AddInputGenerator(ig8.get());
                rates[0] = zxlb::INCORRECT_EXPECTED;
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
        zxlb::INCORRECT_EXPECTED
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
                // rates[0] = zxlb::INCORRECT_EXPECTED;



            if(pattern==0) {
                input_layer->AddInputGenerator(ig1.get());
                rates[0] = zxlb::CORRECT_EXPECTED;
            } else if(pattern==1) {
                input_layer->AddInputGenerator(ig2.get());
                rates[0] = zxlb::CORRECT_EXPECTED;
            } else if(pattern==2) {
                input_layer->AddInputGenerator(ig3.get());
                rates[0] = zxlb::CORRECT_EXPECTED;
            } else if(pattern==3) {
                input_layer->AddInputGenerator(ig4.get());
                rates[0] = zxlb::CORRECT_EXPECTED;
            } else if(pattern==4) {
                input_layer->AddInputGenerator(ig5.get());
                rates[0] = zxlb::INCORRECT_EXPECTED;
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

    sizet num_iterations = 50;
    sizet iteration_size = beacon.GetSizeLabeledEntries();
    i64 time_per_example = 1000;
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();
    sizet mask_interval = 1000000000000000;

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
        zxlb::INCORRECT_EXPECTED
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
            rates[0] = zxlb::CORRECT_EXPECTED;

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
                rates[0] = zxlb::INCORRECT_EXPECTED;
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

/*****************************************************************************
 * LINES VERT
 ****************************************************************************/

void RunLinesVert(
    Writer * writer,
    Network * network,
    RNG & rng
) {
    vec<sizet> patterns = {
        0,1,2,3,4,5,6,7,8,9
    };
    umap<sizet,vec<sizet>> inputs_by_pattern = {
        {0, vec<sizet>({0,1,2,3,4})},
        {1, vec<sizet>({5,6,7,8,9})},
        {2, vec<sizet>({10,11,12,13,14})},
        {3, vec<sizet>({15,16,17,18,19})},
        {4, vec<sizet>({20,21,22,23,24})},
        {5, vec<sizet>({0,5,10,15,20})},
        {6, vec<sizet>({1,6,11,16,21})},
        {7, vec<sizet>({2,7,12,17,22})},
        {8, vec<sizet>({3,8,13,18,23})},
        {9, vec<sizet>({4,9,14,19,24})}
    };
    sizet num_iterations = 10000;
    sizet iteration_size = patterns.size();
    i64 time_per_example = 1000;
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();

    Neuron * output_neuron = output_layer->GetNeuron(0);
    //output_neuron->SetInputMethod(InputMethod::Simple);
    sizet input_method_switch_to_full = 0;
    sizet start_iteration_rebuild_dendrites = 0;

    double RATE_ON = 0.1;
    double RATE_OFF = 0.0;

    vuptr<InputGenerator_Poisson> igens;

    for(sizet i = 0; i < patterns.size(); i++) {
        uptr<InputGenerator_Poisson> ig = std::make_unique<InputGenerator_Poisson>();
        ig->id = std::to_string(i);
        ig->dist=std::uniform_real_distribution<double>(0.0,1.0);
        for(i64 k = 0; k < input_layer_size; k++) {
            ig->decay.push_back(std::exp(-1.0/10.0));
            ig->strength.push_back(500.0);
            ig->signal.push_back(0.0);
            ig->rate.push_back(RATE_OFF);
        }
        igens.emplace_back(std::move(ig));
    }
    for(sizet i = 0; i < patterns.size(); i++) {
        for(sizet k = 0; k < inputs_by_pattern[i].size(); k++) {
                igens[i]->rate[inputs_by_pattern[i][k]] = RATE_ON;
                //std::cout << i << " " << inputs_by_pattern[i][k] << " ON.\n";
        }
    }
    vec<double> rates_by_pattern = {
        zxlb::CORRECT_EXPECTED,
        zxlb::CORRECT_EXPECTED,
        zxlb::CORRECT_EXPECTED,
        zxlb::CORRECT_EXPECTED,
        zxlb::CORRECT_EXPECTED,
        zxlb::INCORRECT_EXPECTED,
        zxlb::INCORRECT_EXPECTED,
        zxlb::INCORRECT_EXPECTED,
        zxlb::INCORRECT_EXPECTED,
        zxlb::INCORRECT_EXPECTED,
    };
    vec<double> rate = {zxlb::INCORRECT_EXPECTED};

    writer->StartRecording();

    // Build the dendrites the first time.
    network->InitDendrites();

    for(sizet i = 0; i < num_iterations; i++) {
        std::cout << i << "\r" << std::flush;

        network->RandomizeOrder(rng);

        if(i==input_method_switch_to_full) {
            output_neuron->SetInputMethod(
                InputMethod::Full
            );
        }

        std::shuffle(patterns.begin(), patterns.end(), rng);

        if(i >= start_iteration_rebuild_dendrites) {
            network->RebuildDendrites();
        }

        for(sizet k = 0; k < iteration_size; k++) {
            sizet pattern = patterns[k];

            input_layer->AddInputGenerator(igens[pattern].get());
            output_layer->SetTraining(true,true,true);
            rate[0] = rates_by_pattern[pattern];
            network->UpdateLayerErrorValues(
                rate, output_layer_index
            );

            i64 time = 1;
            for(; time <= time_per_example; time++) {
                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            igens[pattern]->Reset();

            writer->AddExampleData(std::make_unique<ExampleData>(i,k,std::to_string(pattern)));
            network->SaveData(-1);
            network->WriteData(writer);
            network->Reset();
        }

        
        

    }

    network->CleanUpData(writer);

    writer->StopRecording();
}



/*****************************************************************************
 * XOR
 ****************************************************************************/

void RunXOR(
    Writer * writer,
    Network * network,
    RNG & rng
) {
    vec<sizet> patterns = {
        0,1,2
    };
    umap<sizet,vec<sizet>> inputs_by_pattern = {
        {0, vec<sizet>({0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15})},
        {1, vec<sizet>({16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31})},
        {2, vec<sizet>({0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31})}
    };
    sizet num_iterations = 20000;
    sizet iteration_size = patterns.size();
    i64 time_per_example = 1000;
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();

    Neuron * output_neuron = output_layer->GetNeuron(0);
    //output_neuron->SetInputMethod(InputMethod::Simple);
    sizet input_method_switch_to_full = 0;
    sizet start_iteration_rebuild_dendrites = 0;

    sizet train_ang_start_it = 0;
    sizet train_rad_start_it = 0;
    sizet train_str_start_it = 100000000;
    bool train_ang = false;
    bool train_rad = false;
    bool train_str = false;


    double RATE_ON = 0.1;
    double RATE_OFF = 0.0;

    vuptr<InputGenerator_Poisson> igens;

    for(sizet i = 0; i < patterns.size(); i++) {
        uptr<InputGenerator_Poisson> ig = std::make_unique<InputGenerator_Poisson>();
        ig->id = std::to_string(i);
        ig->dist=std::uniform_real_distribution<double>(0.0,1.0);
        for(i64 k = 0; k < input_layer_size; k++) {
            ig->decay.push_back(std::exp(-1.0/10.0));
            ig->strength.push_back(500.0);
            ig->signal.push_back(0.0);
            ig->rate.push_back(RATE_OFF);
        }
        igens.emplace_back(std::move(ig));
    }
    for(sizet i = 0; i < patterns.size(); i++) {
        for(sizet k = 0; k < inputs_by_pattern[i].size(); k++) {
                igens[i]->rate[inputs_by_pattern[i][k]] = RATE_ON;
        }
    }
    vec<double> rates_by_pattern = {
        zxlb::CORRECT_EXPECTED,
        zxlb::CORRECT_EXPECTED,
        zxlb::INCORRECT_EXPECTED,
    };
    vec<double> rate = {zxlb::INCORRECT_EXPECTED};

    writer->StartRecording();

    // Build the dendrites the first time.
    network->InitDendrites();

    for(sizet i = 0; i < num_iterations; i++) {
        std::cout << i << "\r" << std::flush;

        network->RandomizeOrder(rng);

        if(i==input_method_switch_to_full) {
            output_neuron->SetInputMethod(
                InputMethod::Full
            );
        }

        if(i >= train_ang_start_it) train_ang=true;
        if(i >= train_rad_start_it) train_rad=true;
        if(i >= train_str_start_it) train_str=true;
        output_layer->SetTraining(train_rad, train_ang, train_str);

        std::shuffle(patterns.begin(), patterns.end(), rng);

        if(i >= start_iteration_rebuild_dendrites) {
            network->RebuildDendrites();
        }

        for(sizet k = 0; k < iteration_size; k++) {
            sizet pattern = patterns[k];

            input_layer->AddInputGenerator(igens[pattern].get());
            
            rate[0] = rates_by_pattern[pattern];
            network->UpdateLayerErrorValues(
                rate, output_layer_index
            );

            i64 time = 1;
            for(; time <= time_per_example; time++) {
                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            igens[pattern]->Reset();

            writer->AddExampleData(std::make_unique<ExampleData>(i,k,std::to_string(pattern)));
            network->SaveData(-1);
            network->WriteData(writer);
            network->Reset();
        }

        
        

    }

    network->CleanUpData(writer);

    writer->StopRecording();

}



void RunEEG(
    Writer * writer,
    Network * network,
    RNG & rng
) {

    // ExpEEG exp_eeg;
    // exp_eeg.LoadData("./exp_eeg/train", "./exp_eeg/test");
    // EEG * eeg = nullptr;
    // vec<sizet> test_indexes_A;
    // vec<sizet> test_indexes_C;
    // vec<sizet> train_indexes_A;
    // vec<sizet> train_indexes_C;
    // str A("a");
    // str C("c");
    // for(sizet i = 0; i < exp_eeg.GetSizeTestExamples(A); i++) {
    //     test_indexes_A.push_back(i);
    // }
    // for(sizet i = 0; i < exp_eeg.GetSizeTrainingExamples(A); i++) {
    //     train_indexes_A.push_back(i);
    // }
    // for(sizet i = 0; i < exp_eeg.GetSizeTestExamples(C); i++) {
    //     test_indexes_C.push_back(i);
    // }
    // for(sizet i = 0; i < exp_eeg.GetSizeTrainingExamples(C); i++) {
    //     train_indexes_C.push_back(i);
    // }

    // vec<sizet> patterns = {
    //     0,1
    // };

    // sizet num_iterations = 500;
    // sizet iteration_size = 100;
    // i64 time_per_example = 1000;
    // i64 output_layer_index = network->GetOutputLayerIndex();
    // Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());


    // for(i64 i = 0; i < output_layer->GetLayerSize(); i++) {
    //     Neuron * output_neuron = output_layer->GetNeuron(i);

    //     output_neuron->SetInputMethod(
    //         InputMethod::Full
    //     );
    // }

    // // sizet train_ang_start_it = 0;
    // // sizet train_rad_start_it = 0;
    // // sizet train_str_start_it = 100000000;
    // // bool train_ang = false;
    // // bool train_rad = false;
    // // bool train_str = false;

    // // str train_or_test = "train";

    // // vec<double> rates_by_pattern = {
    // //     zxlb::CORRECT_EXPECTED,
    // //     zxlb::INCORRECT_EXPECTED,
    // // };
    // vec<double> rate = {
    //     zxlb::INCORRECT_EXPECTED,
    //     zxlb::INCORRECT_EXPECTED
    // };

    // writer->StartRecording();

    // // Build the dendrites the first time.
    // network->InitDendrites();

    // for(sizet i = 0; i < num_iterations; i++) {
    //     std::cout << i << "\r" << std::flush;

        
        

    //     network->RandomizeOrder(rng);


        
        

    //     // if(i >= train_ang_start_it) train_ang=true;
    //     // if(i >= train_rad_start_it) train_rad=true;
    //     // if(i >= train_str_start_it) train_str=true;
    //     output_layer->SetTraining(true, true, false);



    //     network->RebuildDendrites();


        
    //     std::shuffle(train_indexes_A.begin(), train_indexes_A.end(), rng);
    //     std::shuffle(train_indexes_C.begin(), train_indexes_C.end(), rng);

    //     for(sizet k = 0; k < iteration_size; k++) {
            
    //         eeg = exp_eeg.GetTrainingExample(train_indexes[k]);
            
    //         if(eeg->ac=="a") {
    //             rate[0] = zxlb::CORRECT_EXPECTED;
    //             rate[1] = zxlb::INCORRECT_EXPECTED;
    //         } else if(eeg->ac=="c") {
    //             rate[0] = zxlb::INCORRECT_EXPECTED;
    //             rate[1] = zxlb::CORRECT_EXPECTED;
    //         }
    //         network->UpdateLayerErrorValues(
    //             rate, output_layer_index
    //         );

    //         sizet reading_index = 0;
    //         vec<double> readings = eeg->GetReading(reading_index++);
    //         network->SetInputs(readings);

    //         i64 time = 1;
    //         for(; time <= time_per_example; time++) {

    //             if(time%4==0) {
    //                 readings = eeg->GetReading(reading_index++);
    //                 network->SetInputs(readings);
    //             }

    //             network->Update(
    //                 time,
    //                 writer,
    //                 rng
    //             );
    //         }

    //         // writer->AddExampleData(std::make_unique<ExampleData>(i,k,eeg->ac));
    //         // network->SaveData(-1);
    //         // network->WriteData(writer);
    //         network->Reset();
    //     }
        
        
    //     /*------------------------------------------
    //     Test 10 random testing examples.
    //     --------------------------------------------*/

    //     // The training cycle produced data. We don't want to store it.
    //     // Purge.
    //     network->Reset(true);

    //     output_layer->SetTraining(false, false, false);

    //     network->RebuildDendrites();

    //     iteration_size = 10;
    //     std::shuffle(test_indexes.begin(), test_indexes.end(), rng);

    //     for(sizet k = 0; k < iteration_size; k++) {
    //         eeg = exp_eeg.GetTestingExample(test_indexes[k]);

            
            
    //         if(eeg->ac=="a") {
    //             rate[0] = zxlb::CORRECT_EXPECTED;
    //             rate[1] = zxlb::INCORRECT_EXPECTED;
    //         } else if(eeg->ac=="c") {
    //             rate[0] = zxlb::INCORRECT_EXPECTED;
    //             rate[1] = zxlb::CORRECT_EXPECTED;
    //         }
    //         network->UpdateLayerErrorValues(
    //             rate, output_layer_index
    //         );

    //         sizet reading_index = 0;
    //         vec<double> readings = eeg->GetReading(reading_index++);
    //         network->SetInputs(readings);

    //         i64 time = 1;
    //         for(; time <= time_per_example; time++) {

    //             if(time%4==0) {
    //                 readings = eeg->GetReading(reading_index++);
    //                 network->SetInputs(readings);
    //             }

    //             network->Update(
    //                 time,
    //                 writer,
    //                 rng
    //             );
    //         }

    //         writer->AddExampleData(std::make_unique<ExampleData>(i,k,eeg->ac));
    //         network->SaveData(-1);
    //         network->WriteData(writer);
    //         network->Reset();
    //     }

    // }

    // network->CleanUpData(writer);

    // writer->StopRecording();
}



void RunEEG2(
    Writer * writer,
    Network * network,
    RNG & rng
) {

    ExpEEG exp_eeg;
    exp_eeg.LoadData("./exp_eeg/train", "./exp_eeg/test");
    EEG * eeg = nullptr;
    vec<sizet> test_indexes_A;
    vec<sizet> train_indexes_A;
    vec<sizet> test_indexes_C;
    vec<sizet> train_indexes_C;
    str A("a");
    str C("c");
    for(sizet i = 0; i < exp_eeg.GetSizeTestExamples(A); i++) {
        test_indexes_A.push_back(i);
    }
    for(sizet i = 0; i < exp_eeg.GetSizeTrainingExamples(A); i++) {
        train_indexes_A.push_back(i);
    }
    for(sizet i = 0; i < exp_eeg.GetSizeTestExamples(C); i++) {
        test_indexes_C.push_back(i);
    }
    for(sizet i = 0; i < exp_eeg.GetSizeTrainingExamples(C); i++) {
        train_indexes_C.push_back(i);
    }

    vec<sizet> patterns = {
        0,1
    };

    sizet num_iterations = 10000;
    sizet iteration_size = 0;
    i64 time_per_example = zxlb::EEG_TASK_DURATION;
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();

    for(i64 i = 0; i < output_layer->GetLayerSize(); i++) {
        Neuron * output_neuron = output_layer->GetNeuron(i);

        output_neuron->SetInputMethod(
            InputMethod::Full
        );
    }

    vec<double> rate = {
        zxlb::INCORRECT_EXPECTED,
        zxlb::INCORRECT_EXPECTED
    };


    uptr<InputGenerator_Poisson> igen = std::make_unique<InputGenerator_Poisson>();
    igen->id = "0";
    igen->dist = std::uniform_real_distribution<double>(0.0,1.0);
    for(i64 k = 0; k < input_layer_size; k++) {
        igen->decay.push_back(std::exp(-1.0/10.0));
        igen->strength.push_back(200.0);
        igen->signal.push_back(0.0);
        igen->rate.push_back(0.0);
    }

    // input_layer->AddInputGenerator(igen.get());

    writer->StartRecording();

    // Build the dendrites the first time.
    network->InitDendrites();

    for(sizet i = 0; i < num_iterations; i++) {
        std::cout << i << "\r" << std::flush;

        
        

        network->RandomizeOrder(rng);


        
        

        // if(i >= train_ang_start_it) train_ang=true;
        // if(i >= train_rad_start_it) train_rad=true;
        // if(i >= train_str_start_it) train_str=true;
        output_layer->SetTraining(true, true, true);



        network->RebuildDendrites();


        
        iteration_size = train_indexes_A.size()+train_indexes_C.size();
        std::shuffle(train_indexes_A.begin(), train_indexes_A.end(), rng);
        std::shuffle(train_indexes_C.begin(), train_indexes_C.end(), rng);

        for(sizet k = 0; k < 50; k++) {
            if(k%2==0) {
                eeg = exp_eeg.GetTrainingExample(A,train_indexes_A[k]);
                rate[0] = zxlb::CORRECT_EXPECTED;
                rate[1] = zxlb::INCORRECT_EXPECTED;
            }
            else {
                eeg = exp_eeg.GetTrainingExample(C,train_indexes_C[k]);
                rate[0] = zxlb::INCORRECT_EXPECTED;
                rate[1] = zxlb::CORRECT_EXPECTED;
            }
            
            // if(eeg->ac=="a") {
                
            // } else if(eeg->ac=="c") {
                
            // }
            network->UpdateLayerErrorValues(
                rate, output_layer_index
            );

            sizet reading_index = 0;
            vec<double> readings;// = eeg->GetReading(reading_index++);
            // network->SetInputs(readings);
            //igen->SetRate(readings);

            i64 time = 0;
            for(; time < time_per_example; time++) {

                if(time%4==0) {
                    readings = eeg->GetReading(reading_index++);
                    network->SetInputs(readings);
                    //igen->SetRate(readings);
                    
                }

                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            igen->Reset();

            // writer->AddExampleData(std::make_unique<ExampleData>(i,k,eeg->ac));
            // network->SaveData(-1);
            // network->WriteData(writer);
            network->Reset();
        }
        
        
        /*------------------------------------------
        Test 10 random testing examples.
        --------------------------------------------*/

        // The training cycle produced data. We don't want to store it.
        // Purge.
        network->Reset(true);

        output_layer->SetTraining(false, false, false);

        network->RebuildDendrites();

        iteration_size = 10;
        std::shuffle(test_indexes_A.begin(), test_indexes_A.end(), rng);
        std::shuffle(test_indexes_C.begin(), test_indexes_C.end(), rng);

        for(sizet k = 0; k < iteration_size; k++) {

            if(k%2==0) {
                eeg = exp_eeg.GetTrainingExample(A,test_indexes_A[k]);
                rate[0] = zxlb::CORRECT_EXPECTED;
                rate[1] = zxlb::INCORRECT_EXPECTED;
            }
            else {
                eeg = exp_eeg.GetTrainingExample(C,test_indexes_C[k]);
                rate[0] = zxlb::INCORRECT_EXPECTED;
                rate[1] = zxlb::CORRECT_EXPECTED;
            }
            
            // if(eeg->ac=="a") {
            //     rate[0] = zxlb::CORRECT_EXPECTED;
            //     rate[1] = zxlb::INCORRECT_EXPECTED;
            // } else if(eeg->ac=="c") {
            //     rate[0] = zxlb::INCORRECT_EXPECTED;
            //     rate[1] = zxlb::CORRECT_EXPECTED;
            // }
            network->UpdateLayerErrorValues(
                rate, output_layer_index
            );

            sizet reading_index = 0;
            vec<double> readings;// = eeg->GetReading(reading_index++);
            // network->SetInputs(readings);
            // igen->SetRate(readings);

            i64 time = 0;
            for(; time < time_per_example; time++) {

                if(time%4==0) {
                    readings = eeg->GetReading(reading_index++);
                    network->SetInputs(readings);
                    // igen->SetRate(readings);
                }

                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            igen->Reset();

            writer->AddExampleData(std::make_unique<ExampleData>(i,k,eeg->ac));
            network->SaveData(-1);
            network->WriteData(writer);
            network->Reset();
        }

    }

    network->CleanUpData(writer);

    writer->StopRecording();
}

/******************************************************************************
 * ExpMove_Hand
 * This experiment will test whether an AD neuron can learn to differentiate
 * between two types of EEGs.
 *****************************************************************************/

void RunMove_Hand(
    Writer * writer,
    Network * network,
    RNG & rng
) {

    int time_start = -200;
    int time_end = 1300;

    vec<MoveType> move_types = {
        MoveType::LEFT,
        MoveType::RIGHT
    };

    ExpMove exp_move(
        10,
        10,
        0.5,
        200.0,
        time_start,
        time_end,
        move_types
    );
    vec<ExpMoveInstance> instances = exp_move.GetAllInstances();

    vec<sizet> patterns = {
        0,1
    };

    sizet num_iterations = 1000;
    i64 time_per_example = time_end-time_start;
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());
    // Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());

    vec<double> rate = {
        0,
        0
    };

    // input_layer->AddInputGenerator(igen.get());

    writer->StartRecording();

    // Build the dendrites the first time.
    //network->InitDendrites();

    Neuron * n0 = output_layer->GetNeuron(0);
    Neuron * n1 = output_layer->GetNeuron(1);

    double lon = -M_PI;
    for(sizet i = 0; i < 10; i++) {
        for(sizet j = 0; j < 10; j++) {
            n0->synapses[i*10+j].location.Lon(lon);
            n1->synapses[j*10+i].location.Lon(lon);

            n0->synapses[i*10+j].location.Rad((10-j)*100);
            n1->synapses[j*10+i].location.Rad((10-i)*100);

            if(j<9) {
                n0->synapses[i*10+j].parent=i*10+j+1;
            } else {
                n0->synapses[i*10+j].parent=-1;
            }

            if(i<9) {
                n1->synapses[j*10+i].parent=j*10+i+1;
            } else {
                n1->synapses[j*10+i].parent=-1;
            }
        }
        lon+=M_PI/10.0;


    }

    output_layer->SetTraining(false, false, false);

    for(sizet i = 0; i < num_iterations; i++) {
        std::cout << i << "\r" << std::flush;

        std::shuffle(instances.begin(), instances.end(), rng);

        network->RandomizeOrder(rng);

        //network->RebuildDendrites();

        for(sizet k = 0; k < instances.size(); k++) {

            ExpMoveInstance * emi = &(instances[k]);
            str type;

            if(emi->type==MoveType::LEFT) {
                type = "H";
                rate[0] = 1;
                rate[1] = 0;
            }
            else if(emi->type==MoveType::RIGHT) {
                type = "V";
                rate[0] = 0;
                rate[1] = 1;
            }
            
            network->UpdateLayerErrorValues(
                rate, output_layer_index
            );

            i64 time = 0;
            for(; time < time_per_example; time++) {

                network->SetInputs(emi->signals[time]);


                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            writer->AddExampleData(std::make_unique<ExampleData>(i,k,type));
            network->SaveData(-1);
            network->WriteData(writer);
            network->Reset();
        }
    }

    network->CleanUpData(writer);

    writer->StopRecording();
}


/******************************************************************************
 * ExpMove
 * This experiment will test whether an AD neuron can learn to differentiate
 * between two types of EEGs.
 *****************************************************************************/

void RunMove(
    Writer * writer,
    Network * network,
    RNG & rng
) {

    int time_start = -200;
    int time_end = 1300;

    vec<MoveType> move_types = {
        MoveType::DOWN,
        MoveType::RIGHT
    };

    ExpMove exp_move(
        10,
        10,
        0.5,
        200.0,
        time_start,
        time_end,
        move_types
    );
    vec<ExpMoveInstance> instances = exp_move.GetAllInstances();

    vec<sizet> patterns = {
        0,1
    };

    sizet num_iterations = zxlb::NUM_ITERATIONS;
    i64 time_per_example = zxlb::TASK_DURATION;

    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());
    // Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());

    vec<double> rate = {
        0,
        0
    };

    // input_layer->AddInputGenerator(igen.get());

    writer->StartRecording();

    // Build the dendrites the first time.
    network->InitDendrites();

    output_layer->SetTraining(
        zxlb::TRAIN_RAD,
        zxlb::TRAIN_ANG,
        zxlb::TRAIN_STR
    );

    for(sizet i = 0; i < num_iterations; i++) {
        std::cout << i << "\r" << std::flush;

        std::shuffle(instances.begin(), instances.end(), rng);

        network->RandomizeOrder(rng);

        network->RebuildDendrites();

        for(sizet k = 0; k < instances.size(); k++) {

            ExpMoveInstance * emi = &(instances[k]);
            str type;

            if(emi->type==MoveType::RIGHT) {
                type = "RIGHT";
                rate[0] = zxlb::CORRECT_EXPECTED;
                rate[1] = zxlb::INCORRECT_EXPECTED;
            }
            else if(emi->type==MoveType::DOWN) {
                type = "DOWN";
                rate[0] = zxlb::INCORRECT_EXPECTED;
                rate[1] = zxlb::CORRECT_EXPECTED;
            }
            
            network->UpdateLayerErrorValues(
                rate, output_layer_index
            );

            i64 time = 0;
            for(; time < time_per_example; time++) {

                network->SetInputs(emi->signals[time]);


                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            writer->AddExampleData(std::make_unique<ExampleData>(i,k,type));
            network->SaveData(-1);
            network->WriteData(writer);
            network->Reset();
        }
    }

    network->CleanUpData(writer);

    writer->StopRecording();
}