#include"runs.hpp"

void RunMNIST(
    Writer * writer,
    Network * network,
    RNG & rng
) {
    std::string TRAIN_LABELS_FILENAME("exp_mnist/train-labels-idx1-ubyte");
    std::string TRAIN_IMAGES_FILENAME("exp_mnist/train-images-idx3-ubyte");
    std::string TEST_LABELS_FILENAME("exp_mnist/t10k-labels-idx1-ubyte");
    std::string TEST_IMAGES_FILENAME("exp_mnist/t10k-images-idx3-ubyte");

    vec<unsigned> labels = {0,1,2,3,4,5,6,7,8,9};

    std::cout << "START: LOADING MNIST DATA\n";

    MNISTReader train_mnist;
    MNISTReader test_mnist;
    train_mnist.LoadData(TRAIN_LABELS_FILENAME,TRAIN_IMAGES_FILENAME,true);
    test_mnist.LoadData(TEST_LABELS_FILENAME,TEST_IMAGES_FILENAME,true);
    // vec<MNISTData> train_data;
    // vec<MNISTData> test_data;
    vec<vec<MNISTData>> test_data_by_id;
    vec<vec<MNISTData>> train_data_by_id;
    // vec<int> labels;
    for(int i = 0; i < 10; i++) {
        // labels.push_back(i);
        vec<MNISTData> train_tmp = train_mnist.GetDataWithLabel(i);
        vec<MNISTData> test_tmp = test_mnist.GetDataWithLabel(i);
        test_data_by_id.push_back(std::move(test_tmp));
        train_data_by_id.push_back(std::move(train_tmp));
        // std::copy(train_tmp.begin(), train_tmp.end(), std::back_inserter(train_data));
        // std::copy(test_tmp.begin(), test_tmp.end(), std::back_inserter(test_data));
    }



    std::cout << "DONE: LOADING MNIST DATA\n";
    
    sizet num_epochs = 1000;
    sizet train_iteration_size = 100; // don't make larger than num examples
    sizet test_iteration_size = 10; // don't make larger than num examples

    i64 time_per_example = 1000;
    i64 time;

    double correct_rate = 10.0;
    double incorrect_rate = 1.0;
    vec<int> spikes_per_neuron;
    
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();
    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());

    // vec<sizet> train_image_indexes;
    // vec<sizet> test_image_indexes;
    // for(sizet i = 0; i < train_mnist.GetData().size(); i++) {
    //     train_image_indexes.push_back(i);
    // }
    // for(sizet i = 0; i < test_mnist.GetData().size(); i++) {
    //     test_image_indexes.push_back(i);
    // }


    vec<vec<double>> spike_history;
    vec<double> prates;
    for(int j = 0; j < 10; j++) {
        vec<double> srh;
        for(i64 i = 0; i < output_layer->GetLayerSize(); i++) {
            srh.push_back(correct_rate);
        }
        prates.push_back(1.0);
        spike_history.push_back(std::move(srh));
    }

    uptr<InputGenerator_Poisson> ig_mask = std::make_unique<InputGenerator_Poisson>();
    ig_mask->id = "0";
    for(i64 i = 0; i < input_layer_size; i++) {
        ig_mask->decay.push_back(std::exp(-1.0/10));
        ig_mask->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig_mask->strength.push_back(500.0);
        ig_mask->signal.push_back(0.0);
        ig_mask->rate.push_back(0.01);
    }

    

    i64 mask_time = 1000;
    double MASK_ERROR = -1;
    vec<double> MASK_RATES;
    for(unsigned int l = 0; l < 10; l++) {
        MASK_RATES.push_back(MASK_ERROR);
    }
    

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    vec<double> rates;
    for(i64 i = 0; i < output_layer->GetLayerSize(); i++) {
        rates.push_back(correct_rate);
    }

    //-------------------------------------------------------------------------
    // Start the run

    network->RebuildDendrites();

    writer->StartRecording();

    output_layer->SetRecordData(false);

    for(sizet e = 0; e < num_epochs; e++) {

        std::cout << "EPOCH: " << e << std::endl;

        

        // std::shuffle(test_data.begin(), test_data.end(), rng);

        std::shuffle(labels.begin(), labels.end(), rng);

        for(int i = 0; i < 10; i++) {
            std::shuffle(test_data_by_id[i].begin(), test_data_by_id[i].end(), rng);
            std::shuffle(train_data_by_id[i].begin(), train_data_by_id[i].end(), rng);
        }
    
        // TRAINING SECTION
        {

            
            

            network->RandomizeOrder(rng);

            for(sizet k = 0; k < train_iteration_size; k++) {
                std::cout << "TRAIN " << k << "\r" << std::flush;

                

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = train_data_by_id[k][labels[k%10]].label;

                

                ///////////////////////////////////////////////////////////////
                /// TRAINING RUN

                output_layer->SetTraining(
                    zxlb::TRAIN_RAD,
                    zxlb::TRAIN_ANG,
                    zxlb::TRAIN_STR
                );

                
                vec<double> wrate = spike_history[label];
                for(unsigned int l = 0; l < 10; l++) {
                    if(l==label) {
                        prates[l] = 1.0 - (wrate[l] / correct_rate);
                        wrate[l] = 1.0 - (wrate[l] / correct_rate);
                        // rates[l] = spikes_per_neuron[l] > correct_rate ? 
                        //                 0.0 : 
                        //                 zxlb::CORRECT_EXPECTED * (1.0 - (spikes_per_neuron[l]/correct_rate));
                    }
                    else {
                        prates[l] = 0.0;//1.0 - (wrate[l] / incorrect_rate);
                        wrate[l] = 1.0 - (wrate[l] / incorrect_rate);
                        // rates[l] = spikes_per_neuron[l] < incorrect_rate ? 
                        //                 0.0 : 
                        //                 zxlb::INCORRECT_EXPECTED * (1.0 - (incorrect_rate / spikes_per_neuron[l]));
                    }
                }

                network->UpdateLayerErrorValuesSeparate(
                    prates, wrate, output_layer_index
                );

                
                input_layer->SetInputs(train_data_by_id[k][labels[k%10]].image);

                time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }

                vec<int> output_rate = output_layer->GetSpikesPerNeuron();
                for(int i = 0; i < output_rate.size(); i++) {
                    spike_history[label][i] = output_rate[i];
                }

                network->Reset();
            }
        }

        std::cout << std::endl;

        // TESTING SECTION
        {

            // Don't train anything
            output_layer->SetTraining(
                false,
                false,
                false // DON'T TRAIN STRENGTH
            );

            // Start saving data
            output_layer->SetRecordData(true);

            network->RandomizeOrder(rng);

            network->RebuildDendrites();

            for(sizet k = 0; k < test_iteration_size; k++) {
                std::cout << "TEST " << k << "\r" << std::flush;

                
                input_layer->SetInputs(test_data_by_id[k][k/10].image);

                i64 time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }

                writer->AddExampleData(std::make_unique<ExampleData>(e,test_data_by_id[k][k/10].label,"None"));
        
                network->SaveData(-1);
                network->WriteData(writer);

                network->Reset();
                
            }

            output_layer->SetRecordData(false);

        }

        std::cout << std::endl;


    }

    std::cout << "ALL DONE\n" << std::flush;

    network->CleanUpData(writer);
    writer->StopRecording();

}


void RunMNISTSingle(
    Writer * writer,
    Network * network,
    RNG & rng
) {
    std::string TRAIN_LABELS_FILENAME("exp_mnist/train-labels-idx1-ubyte");
    std::string TRAIN_IMAGES_FILENAME("exp_mnist/train-images-idx3-ubyte");
    std::string TEST_LABELS_FILENAME("exp_mnist/t10k-labels-idx1-ubyte");
    std::string TEST_IMAGES_FILENAME("exp_mnist/t10k-images-idx3-ubyte");

    vec<unsigned> labels = {4,9};

    std::cout << "START: LOADING MNIST DATA\n";

    MNISTReader train_mnist;
    MNISTReader test_mnist;
    train_mnist.LoadData(TRAIN_LABELS_FILENAME,TRAIN_IMAGES_FILENAME,true);
    test_mnist.LoadData(TEST_LABELS_FILENAME,TEST_IMAGES_FILENAME,true);
    // vec<MNISTData> train_data;
    // vec<MNISTData> test_data;
    vec<vec<MNISTData>> test_data_by_id;
    vec<vec<MNISTData>> train_data_by_id;
    vec<int> indexes;
    for(int i = 0; i < labels.size(); i++) {
        indexes.push_back(i);
        vec<MNISTData> train_tmp = train_mnist.GetDataWithLabel(labels[i]);
        vec<MNISTData> test_tmp = test_mnist.GetDataWithLabel(labels[i]);
        test_data_by_id.push_back(std::move(test_tmp));
        train_data_by_id.push_back(std::move(train_tmp));
        // std::copy(train_tmp.begin(), train_tmp.end(), std::back_inserter(train_data));
        // std::copy(test_tmp.begin(), test_tmp.end(), std::back_inserter(test_data));
    }



    std::cout << "DONE: LOADING MNIST DATA\n";
    
    sizet num_epochs = 3000;
    sizet batch = 1;
    sizet train_iteration_size = 100; // don't make larger than num examples
    sizet test_iteration_size = labels.size()*batch; // don't make larger than num examples

    i64 time_per_example = 1000;
    i64 time;

    double correct_rate = 5.0;
    double incorrect_rate = 1.0;
    vec<int> spikes_per_neuron;
    
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();
    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());

    // vec<sizet> train_image_indexes;
    // vec<sizet> test_image_indexes;
    // for(sizet i = 0; i < train_mnist.GetData().size(); i++) {
    //     train_image_indexes.push_back(i);
    // }
    // for(sizet i = 0; i < test_mnist.GetData().size(); i++) {
    //     test_image_indexes.push_back(i);
    // }


    vec<vec<double>> spike_history;
    vec<double> prates;
    for(int j = 0; j < labels.size(); j++) {
        vec<double> srh;
        for(i64 i = 0; i < output_layer->GetLayerSize(); i++) {
            srh.push_back(0.0);
        }
        prates.push_back(1.0);
        spike_history.push_back(std::move(srh));
    }

    uptr<InputGenerator_Poisson> ig_mask = std::make_unique<InputGenerator_Poisson>();
    ig_mask->id = "0";
    for(i64 i = 0; i < input_layer_size; i++) {
        ig_mask->decay.push_back(std::exp(-1.0/10));
        ig_mask->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig_mask->strength.push_back(500.0);
        ig_mask->signal.push_back(0.0);
        ig_mask->rate.push_back(0.01);
    }

    

    i64 mask_time = 1000;
    double MASK_ERROR = -1;
    vec<double> MASK_RATES;
    for(unsigned int l = 0; l < labels.size(); l++) {
        MASK_RATES.push_back(MASK_ERROR);
    }
    

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    vec<double> rates;
    for(i64 i = 0; i < output_layer->GetLayerSize(); i++) {
        rates.push_back(correct_rate);
    }

    //-------------------------------------------------------------------------
    // Start the run

    network->RebuildDendrites();

    writer->StartRecording();

    output_layer->SetRecordData(false);

    for(sizet e = 0; e < num_epochs; e++) {

        std::cout << "EPOCH: " << e << std::endl;

        

        // std::shuffle(test_data.begin(), test_data.end(), rng);

        std::shuffle(indexes.begin(), indexes.end(), rng);

        for(int i = 0; i < labels.size(); i++) {
            std::shuffle(test_data_by_id[i].begin(), test_data_by_id[i].end(), rng);
            std::shuffle(train_data_by_id[i].begin(), train_data_by_id[i].end(), rng);
        }
    
        // TRAINING SECTION
        {

            
            

            network->RandomizeOrder(rng);

            for(sizet k = 0; k < train_iteration_size; k++) {
                std::cout << "TRAIN " << k << "\r" << std::flush;

                int neuron = indexes[k%indexes.size()];
                int example = k/indexes.size();

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = train_data_by_id[neuron][example].label;

                

                ///////////////////////////////////////////////////////////////
                /// TRAINING RUN

                output_layer->SetTraining(
                    true,
                    true,
                    true
                );

                
                vec<double> wrate = spike_history[neuron];
                for(unsigned int l = 0; l < indexes.size(); l++) {
                    if(l==neuron) {
                        wrate[l] = 1.0 - (wrate[l] / correct_rate);
                        prates[l] = 1.0 - (wrate[l] / correct_rate);
                        
                        // std::cout << prates[l] << std::endl;
                        // rates[l] = spikes_per_neuron[l] > correct_rate ? 
                        //                 0.0 : 
                        //                 zxlb::CORRECT_EXPECTED * (1.0 - (spikes_per_neuron[l]/correct_rate));
                    }
                    else {
                        prates[l] = 1.0 - (wrate[l] / incorrect_rate);
                        wrate[l] = 1.0 - (wrate[l] / incorrect_rate);
                        // std::cout << prates[l] << std::endl;
                        // rates[l] = spikes_per_neuron[l] < incorrect_rate ? 
                        //                 0.0 : 
                        //                 zxlb::INCORRECT_EXPECTED * (1.0 - (incorrect_rate / spikes_per_neuron[l]));
                    }
                }

                network->UpdateLayerErrorValuesSeparate(
                    prates, wrate, output_layer_index
                );

                
                input_layer->SetInputs(train_data_by_id[neuron][example].image);

                time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }

                vec<int> output_rate = output_layer->GetSpikesPerNeuron();
                for(int i = 0; i < output_rate.size(); i++) {
                    spike_history[neuron][i] = (output_rate[i] + spike_history[neuron][i]) / 2.0;
                }

                network->Reset();
            }
        }

        std::cout << std::endl;

        // TESTING SECTION
        {

            // Don't train anything
            output_layer->SetTraining(
                false,
                false,
                false // DON'T TRAIN STRENGTH
            );

            // Start saving data
            output_layer->SetRecordData(true);

            network->RandomizeOrder(rng);

            network->RebuildDendrites();

            for(sizet k = 0; k < test_iteration_size; k++) {
                std::cout << "TEST " << k << "\r" << std::flush;

                int neuron = indexes[k%indexes.size()];
                int example = k/indexes.size();

                input_layer->SetInputs(test_data_by_id[neuron][example].image);

                i64 time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }

                writer->AddExampleData(std::make_unique<ExampleData>(neuron,test_data_by_id[neuron][example].label,"None"));
        
                network->SaveData(-1);
                network->WriteData(writer);

                network->Reset();
                
            }

            output_layer->SetRecordData(false);

        }

        std::cout << std::endl;

    }

    std::cout << "ALL DONE\n" << std::flush;

    network->CleanUpData(writer);
    writer->StopRecording();

}


/*****************************
 * INPUT POSITION2
 *****************************/
void RunInputPosition2(
    Writer * writer,
    Network * network,
    RNG & rng
) {
    // std::string TRAIN_LABELS_FILENAME("exp_mnist/train-labels-idx1-ubyte");
    // std::string TRAIN_IMAGES_FILENAME("exp_mnist/train-images-idx3-ubyte");
    // std::string TEST_LABELS_FILENAME("exp_mnist/t10k-labels-idx1-ubyte");
    // std::string TEST_IMAGES_FILENAME("exp_mnist/t10k-images-idx3-ubyte");

    vec<unsigned> labels = {0,
                            1,2,3,4,5,
                            6,7,8,9,10,
                            11,12,13,14,15,
                            16,17,18,19,20,
                            21,22,23,24};

    vec<vec<unsigned>> patterns = {
        {0,24,1,2,3,4,5,21},
        {0,21,6,7,8,9,10,22},
        {0,22,11,12,13,14,15,23},
        {0,23,16,17,18,19,20,24}
    };

    // std::cout << "START: LOADING MNIST DATA\n";

    // MNISTReader train_mnist;
    // MNISTReader test_mnist;
    // train_mnist.LoadData(TRAIN_LABELS_FILENAME,TRAIN_IMAGES_FILENAME,true);
    // test_mnist.LoadData(TEST_LABELS_FILENAME,TEST_IMAGES_FILENAME,true);
    // vec<MNISTData> train_data;
    // vec<MNISTData> test_data;
    // for(int i = 0; i < 10; i++) {
    //     vec<MNISTData> train_tmp = train_mnist.GetDataWithLabel(i);
    //     vec<MNISTData> test_tmp = test_mnist.GetDataWithLabel(i);
    //     std::copy(train_tmp.begin(), train_tmp.end(), std::back_inserter(train_data));
    //     std::copy(test_tmp.begin(), test_tmp.end(), std::back_inserter(test_data));
    // }



    // std::cout << "DONE: LOADING MNIST DATA\n";
    
    sizet num_epochs = 100;
    sizet train_iteration_size = 100; // don't make larger than num examples
    sizet test_iteration_size = 1; // don't make larger than num examples
    i64 time_per_example = 1000;
    i64 time;
    double correct_rate = 20.0;
    double incorrect_rate = 1.0;
    vec<int> spikes_per_neuron;
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();

    vuptr<InputGenerator_Poisson> ig_patterns;

    // for(i64 k = 0; k < input_layer_size; k++) {
    //     uptr<InputGenerator_Poisson> pat = std::make_unique<InputGenerator_Poisson>();
    //     pat->id = std::to_string(k);
    //     if(std::find(patterns[0].begin(),patterns[0].end(),k)!=patterns[0].end()) {

    //     }
    // }
    for(sizet i = 0; i < patterns.size(); i++) {
        double r = 0.1;
        uptr<InputGenerator_Poisson> pat = std::make_unique<InputGenerator_Poisson>();
        pat->id = std::to_string(i);
        for(i64 k = 0; k < input_layer_size; k++) {
            // If this input is part of the pattern.
            if(std::find(patterns[i].begin(),patterns[i].end(),k)!=patterns[i].end()) {
                pat->decay.push_back(std::exp(-1.0/10));
                pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
                pat->strength.push_back(500.0);
                pat->signal.push_back(0.0);
                pat->rate.push_back(r);
            } else {
                pat->decay.push_back(std::exp(-1.0/10));
                pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
                pat->strength.push_back(500.0);
                pat->signal.push_back(0.0);
                pat->rate.push_back(0.0);
            }
            
        }
        ig_patterns.push_back(std::move(pat));
    }

    uptr<InputGenerator_Poisson> ig_mask = std::make_unique<InputGenerator_Poisson>();
    ig_mask->id = "mask";
    for(i64 i = 0; i < input_layer_size; i++) {
        ig_mask->decay.push_back(std::exp(-1.0/10));
        ig_mask->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig_mask->strength.push_back(500.0);
        ig_mask->signal.push_back(0.0);
        ig_mask->rate.push_back(0.0);
    }

    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());

    sizet mask_time = 10000;
    // double MASK_ERROR = -1;
    vec<double> MASK_RATES = {-1.0};
    // for(unsigned int l = 0; l < 10; l++) {
    //     MASK_RATES.push_back(MASK_ERROR);
    // }
    

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    vec<double> rates;
    for(i64 i = 0; i < output_layer->GetLayerSize(); i++) {
        rates.push_back(zxlb::CORRECT_EXPECTED);
    }


    vec<sizet> examples;
    for(sizet i = 0; i < patterns.size(); i++) {
        examples.push_back(i);
    }
    //-------------------------------------------------------------------------
    // Start the run

    network->RebuildDendrites();

    network->InitWriteData();

    writer->StartRecording();

    output_layer->SetRecordData(false);

    for(sizet e = 0; e < num_epochs; e++) {

        std::cout << "EPOCH: " << e << std::endl;

        
        std::shuffle(examples.begin(), examples.end(), rng);
    
        // TRAINING SECTION
        {

            
            

            network->RandomizeOrder(rng);

            for(sizet k = 0; k < train_iteration_size; k++) {
                std::cout << "TRAIN " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = examples[k%examples.size()];

                // Set the inputs to the pixel data
                // input_layer->AddInputGenerator(ig_patterns[label].get());
                // // input_layer->SetInputs(train_data[k].image);

                ////////////////////////////////////////////////////////////
                // GET ERROR RATE RUN //////////////////////////////////////
                // output_layer->SetTraining(
                //     false,
                //     false,
                //     false
                // );

                // time = 1;
                // for(; time <= time_per_example; time++) {
                //     network->Update(
                //         time,
                //         writer,
                //         rng
                //     );
                // }
                

                ///////////////////////////////////////////////////////////////
                /// TRAINING RUN

                output_layer->SetTraining(
                    zxlb::TRAIN_RAD,
                    zxlb::TRAIN_ANG,
                    zxlb::TRAIN_STR
                );

                spikes_per_neuron = output_layer->GetSpikesPerNeuron();

                // network->Reset();

                // // for(sizet l = 0; l < labels.size(); l++) {
                // if(std::find(patterns[label].begin(),patterns[label].end(),label)!=patterns[label].end()) {
                //     rates[0] = spikes_per_neuron[0] > correct_rate ? 
                //                     0.0 : 
                //                     zxlb::CORRECT_EXPECTED * (1.0 - (spikes_per_neuron[0]/correct_rate));
                // }
                // else {
                //     rates[0] = spikes_per_neuron[0] < incorrect_rate ? 
                //                     0.0 : 
                //                     zxlb::INCORRECT_EXPECTED * (1.0 - (incorrect_rate / spikes_per_neuron[0]));
                // }
                // }

                network->UpdateLayerErrorValues(
                    rates, output_layer_index
                );

                input_layer->AddInputGenerator(ig_patterns[label].get());
                // input_layer->SetInputs(train_data[k].image);

                time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }

                network->Reset();
            }
        }

        std::cout << std::endl;

        std::shuffle(examples.begin(), examples.end(), rng);

        // TESTING SECTION
        {

            // Don't train anything
            output_layer->SetTraining(
                false,
                false,
                false // DON'T TRAIN STRENGTH
            );

            // Start saving data
            output_layer->SetRecordData(true);

            network->RandomizeOrder(rng);

            network->RebuildDendrites();

            for(sizet k = 0; k < test_iteration_size; k++) {
                std::cout << "TEST " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = examples[k%examples.size()];

                input_layer->AddInputGenerator(ig_patterns[label].get());

                i64 time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }
                //uptr<ExampleData> exdata = std::make_unique<ExampleData>(e,label,str("None"));
                //writer->AddExampleData(std::move(exdata));
                //exdata = nullptr;
                network->SaveData(-1);
                network->WriteData(writer);

                network->Reset();
                
            }

            output_layer->SetRecordData(false);

        }

        std::cout << std::endl;

        // MASK SECTION

        {

            std::cout << "MASK" << std::flush;

            output_layer->SetTraining(
                false,
                true,
                false
            );

            input_layer->AddInputGenerator(ig_mask.get());
            
            network->UpdateLayerErrorValues(
                MASK_RATES, output_layer_index
            );


            i64 time = 1;
            for(; time <= mask_time; time++) {
                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            // Reset the input generator
            ig_mask->Reset();
            network->Reset();
            
            // Remove input generator. Only needed for mask.
            input_layer->AddInputGenerator(nullptr);
        }

        std::cout << std::endl;

    }

    std::cout << "ALL DONE\n" << std::flush;

    network->CleanUpData(writer);
    writer->StopRecording();

}



/*****************************
 * INPUT POSITION1
 *****************************/
void RunInputPosition1(
    Writer * writer,
    Network * network,
    RNG & rng
) {
    
    vec<unsigned> labels = {0,1,2,3,4,
                            5,6,7,8,9,
                            10,11,12,13,14,
                            15,16,17,18,19,
                            20};

    vec<vec<unsigned>> patterns = {
        {0,1,2,3,4,5},
        {0,6,7,8,9,10},
        {0,11,12,13,14,15},
        {0,16,17,18,19,20}
    };

    
    sizet num_epochs = 1000;
    sizet train_iteration_size = 100; // don't make larger than num examples
    sizet test_iteration_size = 1; // don't make larger than num examples
    i64 time_per_example = 1000;
    i64 time;
    double correct_rate = 20.0;
    double incorrect_rate = 1.0;
    vec<int> spikes_per_neuron;
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();

    vuptr<InputGenerator_Poisson> ig_patterns;


    for(sizet i = 0; i < patterns.size(); i++) {
        double r = 0.1;
        uptr<InputGenerator_Poisson> pat = std::make_unique<InputGenerator_Poisson>();
        pat->id = std::to_string(i);
        for(i64 k = 0; k < input_layer_size; k++) {
            // If this input is part of the pattern.
            if(std::find(patterns[i].begin(),patterns[i].end(),k)!=patterns[i].end()) {
                pat->decay.push_back(std::exp(-1.0/10));
                pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
                pat->strength.push_back(500.0);
                pat->signal.push_back(0.0);
                pat->rate.push_back(r);
            } else {
                pat->decay.push_back(std::exp(-1.0/10));
                pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
                pat->strength.push_back(500.0);
                pat->signal.push_back(0.0);
                pat->rate.push_back(0.0);
            }
            
        }
        ig_patterns.push_back(std::move(pat));
    }

    uptr<InputGenerator_Poisson> ig_mask = std::make_unique<InputGenerator_Poisson>();
    ig_mask->id = "mask";
    for(i64 i = 0; i < input_layer_size; i++) {
        ig_mask->decay.push_back(std::exp(-1.0/10));
        ig_mask->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig_mask->strength.push_back(500.0);
        ig_mask->signal.push_back(0.0);
        ig_mask->rate.push_back(0.00);
    }

    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());

    sizet mask_time = 10000;
    vec<double> MASK_RATES = {-1.0};

    

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    vec<double> rates;
    for(i64 i = 0; i < output_layer->GetLayerSize(); i++) {
        rates.push_back(zxlb::CORRECT_EXPECTED);
    }


    vec<sizet> examples;
    for(sizet i = 0; i < patterns.size(); i++) {
        examples.push_back(i);
    }
    //-------------------------------------------------------------------------
    // Start the run

    network->RebuildDendrites();

    network->InitWriteData();

    writer->StartRecording();

    output_layer->SetRecordData(true);
    network->SaveData(-1);
    network->WriteData(writer);

    output_layer->SetRecordData(false);

    for(sizet e = 0; e < num_epochs; e++) {

        std::cout << "EPOCH: " << e << std::endl;

        
        std::shuffle(examples.begin(), examples.end(), rng);
    
        // TRAINING SECTION
        {

            
            

            network->RandomizeOrder(rng);

            for(sizet k = 0; k < train_iteration_size; k++) {
                std::cout << "TRAIN " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = examples[k%examples.size()];

                

                output_layer->SetTraining(
                    zxlb::TRAIN_RAD,
                    zxlb::TRAIN_ANG,
                    zxlb::TRAIN_STR
                );

                spikes_per_neuron = output_layer->GetSpikesPerNeuron();

               

                network->UpdateLayerErrorValues(
                    rates, output_layer_index
                );

                input_layer->AddInputGenerator(ig_patterns[label].get());
                // input_layer->SetInputs(train_data[k].image);

                time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }

                network->Reset();
            }
        }

        std::cout << std::endl;

        std::shuffle(examples.begin(), examples.end(), rng);

        // TESTING SECTION
        {

            // Don't train anything
            output_layer->SetTraining(
                false,
                false,
                false // DON'T TRAIN STRENGTH
            );

            // Start saving data
            output_layer->SetRecordData(true);

            network->RandomizeOrder(rng);

            network->RebuildDendrites();

            for(sizet k = 0; k < test_iteration_size; k++) {
                std::cout << "TEST " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = examples[k%examples.size()];

                input_layer->AddInputGenerator(ig_patterns[label].get());

                i64 time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }
                //uptr<ExampleData> exdata = std::make_unique<ExampleData>(e,label,str("None"));
                //writer->AddExampleData(std::move(exdata));
                //exdata = nullptr;
                network->SaveData(-1);
                network->WriteData(writer);

                network->Reset();
                
            }

            output_layer->SetRecordData(false);

        }

        std::cout << std::endl;

        // MASK SECTION

        {

            std::cout << "MASK" << std::flush;

            output_layer->SetTraining(
                false,
                true,
                false
            );

            input_layer->AddInputGenerator(ig_mask.get());
            
            network->UpdateLayerErrorValues(
                MASK_RATES, output_layer_index
            );


            i64 time = 1;
            for(; time <= mask_time; time++) {
                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            // Reset the input generator
            ig_mask->Reset();
            network->Reset();
            
            // Remove input generator. Only needed for mask.
            input_layer->AddInputGenerator(nullptr);
        }

        std::cout << std::endl;

    }

    std::cout << "ALL DONE\n" << std::flush;

    network->CleanUpData(writer);
    writer->StopRecording();

}



/*****************************
 * INPUT POSITION3
 *****************************/
void RunInputPosition3(
    Writer * writer,
    Network * network,
    RNG & rng
) {
    
    vec<unsigned> labels = {0,1,2,3,4,
                            5,6,7,8,9,
                            10,11,12,13,14,
                            15,16,17,18,19,
                            20,21,22,23,24,
                            25,26,27,28,29,
                            30,31,32,33,34,
                            35,36,37,38,39,
                            40,41,42,43,44,
                            45,46,47,48,49
                            };

    vec<vec<unsigned>> patterns = {
        {
        0,1,2,3,4,
        5,6,7,8,9,
        10,11,12,13,14,
        15,16,17,18,19,
        20,21,22,23,24,
        25,26,27,28,29,
        30,31,32,33,34,
        35,36,37,38,39,
        40,41,42,43,44,
        45,46,47,48,49
        }
    };

    
    sizet num_epochs = 100;
    sizet train_iteration_size = 100; // don't make larger than num examples
    sizet test_iteration_size = 1; // don't make larger than num examples
    i64 time_per_example = 1000;
    i64 time;
    double correct_rate = 20.0;
    double incorrect_rate = 1.0;
    vec<int> spikes_per_neuron;
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();

    vuptr<InputGenerator_Poisson> ig_patterns;


    for(sizet i = 0; i < patterns.size(); i++) {
        double r = 0.1;
        uptr<InputGenerator_Poisson> pat = std::make_unique<InputGenerator_Poisson>();
        pat->id = std::to_string(i);
        for(i64 k = 0; k < input_layer_size; k++) {
            // If this input is part of the pattern.
            if(std::find(patterns[i].begin(),patterns[i].end(),k)!=patterns[i].end()) {
                pat->decay.push_back(std::exp(-1.0/10));
                pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
                pat->strength.push_back(500.0);
                pat->signal.push_back(0.0);
                pat->rate.push_back(r);
            } else {
                pat->decay.push_back(std::exp(-1.0/10));
                pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
                pat->strength.push_back(500.0);
                pat->signal.push_back(0.0);
                pat->rate.push_back(0.0);
            }
            
        }
        ig_patterns.push_back(std::move(pat));
    }

    uptr<InputGenerator_Poisson> ig_mask = std::make_unique<InputGenerator_Poisson>();
    ig_mask->id = "mask";
    for(i64 i = 0; i < input_layer_size; i++) {
        ig_mask->decay.push_back(std::exp(-1.0/10));
        ig_mask->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig_mask->strength.push_back(500.0);
        ig_mask->signal.push_back(0.0);
        ig_mask->rate.push_back(0.00);
    }

    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());

    sizet mask_time = 0;
    vec<double> MASK_RATES = {-1.0};

    

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    vec<double> rates;
    for(i64 i = 0; i < output_layer->GetLayerSize(); i++) {
        rates.push_back(zxlb::CORRECT_EXPECTED);
    }


    vec<sizet> examples;
    for(sizet i = 0; i < patterns.size(); i++) {
        examples.push_back(i);
    }
    //-------------------------------------------------------------------------
    // Start the run

    network->RebuildDendrites();

    network->InitWriteData();

    writer->StartRecording();

    output_layer->SetRecordData(true);
    network->SaveData(-1);
    network->WriteData(writer);

    output_layer->SetRecordData(false);

    for(sizet e = 0; e < num_epochs; e++) {

        std::cout << "EPOCH: " << e << std::endl;

        
        std::shuffle(examples.begin(), examples.end(), rng);
    
        // TRAINING SECTION
        {

            
            

            network->RandomizeOrder(rng);

            for(sizet k = 0; k < train_iteration_size; k++) {
                std::cout << "TRAIN " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = examples[k%examples.size()];

                

                output_layer->SetTraining(
                    zxlb::TRAIN_RAD,
                    zxlb::TRAIN_ANG,
                    zxlb::TRAIN_STR
                );

                spikes_per_neuron = output_layer->GetSpikesPerNeuron();

               

                network->UpdateLayerErrorValues(
                    rates, output_layer_index
                );

                input_layer->AddInputGenerator(ig_patterns[label].get());
                // input_layer->SetInputs(train_data[k].image);

                time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }

                network->Reset();
            }
        }

        std::cout << std::endl;

        std::shuffle(examples.begin(), examples.end(), rng);

        // TESTING SECTION
        {

            // Don't train anything
            output_layer->SetTraining(
                false,
                false,
                false // DON'T TRAIN STRENGTH
            );

            // Start saving data
            output_layer->SetRecordData(true);

            network->RandomizeOrder(rng);

            network->RebuildDendrites();

            for(sizet k = 0; k < test_iteration_size; k++) {
                std::cout << "TEST " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = examples[k%examples.size()];

                input_layer->AddInputGenerator(ig_patterns[label].get());

                i64 time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }
                //uptr<ExampleData> exdata = std::make_unique<ExampleData>(e,label,str("None"));
                //writer->AddExampleData(std::move(exdata));
                //exdata = nullptr;
                network->SaveData(-1);
                network->WriteData(writer);

                network->Reset();
                
            }

            output_layer->SetRecordData(false);

        }

        std::cout << std::endl;

        // MASK SECTION

        {

            std::cout << "MASK" << std::flush;

            output_layer->SetTraining(
                false,
                true,
                false
            );

            input_layer->AddInputGenerator(ig_mask.get());
            
            network->UpdateLayerErrorValues(
                MASK_RATES, output_layer_index
            );


            i64 time = 1;
            for(; time <= mask_time; time++) {
                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            // Reset the input generator
            ig_mask->Reset();
            network->Reset();
            
            // Remove input generator. Only needed for mask.
            input_layer->AddInputGenerator(nullptr);
        }

        std::cout << std::endl;

    }

    std::cout << "ALL DONE\n" << std::flush;

    network->CleanUpData(writer);
    writer->StopRecording();

}


/*****************************
 * INPUT POSITION4
 *****************************/
void RunInputPosition4(
    Writer * writer,
    Network * network,
    RNG & rng
) {
    
    vec<unsigned> labels = {0,1};

    vec<vec<unsigned>> patterns = {
        {
        0,1
        }
    };

    
    sizet num_epochs = 100;
    sizet train_iteration_size = 100; // don't make larger than num examples
    sizet test_iteration_size = 1; // don't make larger than num examples
    i64 time_per_example = 1000;
    i64 time;
    double correct_rate = 20.0;
    double incorrect_rate = 1.0;
    vec<int> spikes_per_neuron;
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();

    vuptr<InputGenerator_Poisson> ig_patterns;


    for(sizet i = 0; i < patterns.size(); i++) {
        double r = 0.01;
        uptr<InputGenerator_Poisson> pat = std::make_unique<InputGenerator_Poisson>();
        pat->id = std::to_string(i);
        for(i64 k = 0; k < input_layer_size; k++) {
            // If this input is part of the pattern.
            if(std::find(patterns[i].begin(),patterns[i].end(),k)!=patterns[i].end()) {
                pat->decay.push_back(std::exp(-1.0/10));
                pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
                pat->strength.push_back(500.0);
                pat->signal.push_back(0.0);
                pat->rate.push_back(r);
            } else {
                pat->decay.push_back(std::exp(-1.0/10));
                pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
                pat->strength.push_back(500.0);
                pat->signal.push_back(0.0);
                pat->rate.push_back(0.0);
            }
            
        }
        ig_patterns.push_back(std::move(pat));
    }

    uptr<InputGenerator_Poisson> ig_mask = std::make_unique<InputGenerator_Poisson>();
    ig_mask->id = "mask";
    for(i64 i = 0; i < input_layer_size; i++) {
        ig_mask->decay.push_back(std::exp(-1.0/10));
        ig_mask->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig_mask->strength.push_back(500.0);
        ig_mask->signal.push_back(0.0);
        ig_mask->rate.push_back(0.00);
    }

    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());

    sizet mask_time = 0;
    vec<double> MASK_RATES = {-1.0};

    

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    vec<double> rates;
    for(i64 i = 0; i < output_layer->GetLayerSize(); i++) {
        rates.push_back(zxlb::CORRECT_EXPECTED);
    }


    vec<sizet> examples;
    for(sizet i = 0; i < patterns.size(); i++) {
        examples.push_back(i);
    }
    //-------------------------------------------------------------------------
    // Start the run

    network->RebuildDendrites();

    network->InitWriteData();

    writer->StartRecording();

    output_layer->SetRecordData(true);
    network->SaveData(-1);
    network->WriteData(writer);

    output_layer->SetRecordData(false);

    for(sizet e = 0; e < num_epochs; e++) {

        std::cout << "EPOCH: " << e << std::endl;

        
        std::shuffle(examples.begin(), examples.end(), rng);
    
        // TRAINING SECTION
        {

            
            

            network->RandomizeOrder(rng);

            for(sizet k = 0; k < train_iteration_size; k++) {
                std::cout << "TRAIN " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = examples[k%examples.size()];

                

                output_layer->SetTraining(
                    zxlb::TRAIN_RAD,
                    zxlb::TRAIN_ANG,
                    zxlb::TRAIN_STR
                );

                spikes_per_neuron = output_layer->GetSpikesPerNeuron();

               

                network->UpdateLayerErrorValues(
                    rates, output_layer_index
                );

                input_layer->AddInputGenerator(ig_patterns[label].get());
                // input_layer->SetInputs(train_data[k].image);

                time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }

                network->Reset();
            }
        }

        std::cout << std::endl;

        std::shuffle(examples.begin(), examples.end(), rng);

        // TESTING SECTION
        {

            // Don't train anything
            output_layer->SetTraining(
                false,
                false,
                false // DON'T TRAIN STRENGTH
            );

            // Start saving data
            output_layer->SetRecordData(true);

            network->RandomizeOrder(rng);

            network->RebuildDendrites();

            for(sizet k = 0; k < test_iteration_size; k++) {
                std::cout << "TEST " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = examples[k%examples.size()];

                input_layer->AddInputGenerator(ig_patterns[label].get());

                i64 time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }
                //uptr<ExampleData> exdata = std::make_unique<ExampleData>(e,label,str("None"));
                //writer->AddExampleData(std::move(exdata));
                //exdata = nullptr;
                network->SaveData(-1);
                network->WriteData(writer);

                network->Reset();
                
            }

            output_layer->SetRecordData(false);

        }

        std::cout << std::endl;

        // MASK SECTION

        {

            std::cout << "MASK" << std::flush;

            output_layer->SetTraining(
                false,
                true,
                false
            );

            input_layer->AddInputGenerator(ig_mask.get());
            
            network->UpdateLayerErrorValues(
                MASK_RATES, output_layer_index
            );


            i64 time = 1;
            for(; time <= mask_time; time++) {
                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            // Reset the input generator
            ig_mask->Reset();
            network->Reset();
            
            // Remove input generator. Only needed for mask.
            input_layer->AddInputGenerator(nullptr);
        }

        std::cout << std::endl;

    }

    std::cout << "ALL DONE\n" << std::flush;

    network->CleanUpData(writer);
    writer->StopRecording();

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
    str TRIAL_TYPE("S1 obj ");
    // str TRIAL_TYPE("S2 match "); // This must have the space after match.
    // str TRIAL_TYPE("S2 nomatch");
    for(sizet i = 0; i < exp_eeg.GetSizeTestExamples(A); i++) {
        //std::cout << TRIAL_TYPE << ":" << exp_eeg.GetTestingExampleTrialType(A,i) << "X";
        if(exp_eeg.GetTestingExampleTrialType(A,i)==TRIAL_TYPE) {
            test_indexes_A.push_back(i);
        }
    }
    for(sizet i = 0; i < exp_eeg.GetSizeTrainingExamples(A); i++) {
        if(exp_eeg.GetTrainingExampleTrialType(A,i)==TRIAL_TYPE) {
            train_indexes_A.push_back(i);
        }
    }
    for(sizet i = 0; i < exp_eeg.GetSizeTestExamples(C); i++) {
        if(exp_eeg.GetTestingExampleTrialType(C,i)==TRIAL_TYPE) {
            test_indexes_C.push_back(i);
        }
    }
    for(sizet i = 0; i < exp_eeg.GetSizeTrainingExamples(C); i++) {
        if(exp_eeg.GetTrainingExampleTrialType(C,i)==TRIAL_TYPE) {
            train_indexes_C.push_back(i);
        }
    }

    i32 TRIAL_ID = -1;
    vec<EEG*> traineegs = exp_eeg.GetAllTraningExamplesByID(TRIAL_ID,TRIAL_TYPE,"c");
    vec<EEG*> testeegs = exp_eeg.GetAllTestingExamplesByID(TRIAL_ID,TRIAL_TYPE,"c");

    std::cout << traineegs.size() << std::endl;

    vec<sizet> patterns = {
        0,1
    };

    sizet num_iterations = 5000;
    sizet iteration_size = 0;
    i64 time_per_example = zxlb::EEG_TASK_DURATION;
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    output_layer->SetTraining(false, false, false);
    i64 input_layer_size = input_layer->GetLayerSize();

    for(i64 i = 0; i < output_layer->GetLayerSize(); i++) {
        Neuron * output_neuron = output_layer->GetNeuron(i);

        output_neuron->SetInputMethod(
            InputMethod::Full
        );
    }

    vec<double> rate = {
        zxlb::CORRECT_EXPECTED,
        zxlb::CORRECT_EXPECTED
    };


    uptr<InputGenerator_Poisson> igen = std::make_unique<InputGenerator_Poisson>();
    igen->id = "0";
    igen->dist = std::uniform_real_distribution<double>(0.0,1.0);
    for(i64 k = 0; k < input_layer_size; k++) {
        igen->decay.push_back(std::exp(-1.0/10.0));
        igen->strength.push_back(200.0);
        igen->signal.push_back(0.0);
        igen->rate.push_back(0.01);
    }

    i64 mask_time = 10000;
    vec<double> mask_rate = {
        -1,-1
    };

    vec<int> spikes_per_neuron={20,20};
    double correct_rate = 20.0;
    double incorrect_rate = 1.0;

    // input_layer->AddInputGenerator(igen.get());

    writer->StartRecording();

    // Build the dendrites the first time.
    network->InitDendrites();

    for(sizet i = 0; i < num_iterations; i++) {
        std::cout << i << "\r" << std::flush;

        
        

        network->RandomizeOrder(rng);
        
        network->RebuildDendrites();
        
        // std::shuffle(train_indexes_A.begin(), train_indexes_A.end(), rng);
        // std::shuffle(train_indexes_C.begin(), train_indexes_C.end(), rng);

        std::shuffle(traineegs.begin(), traineegs.end(), rng);
        // std::shuffle(testeegs.begin(), testeegs.end(), rng);

        for(sizet k = 0, counter=0; k < traineegs.size(); k++) {

            eeg = traineegs[counter];

            // Simple Error
            network->Reset();
            output_layer->SetTraining(true, true, false);
            if(eeg->ac=="a") {
                rate[0] = 1.0;
                rate[1] = -1.0;
            } else {
                rate[0] = -1.0;
                rate[1] = 1.0;
            }

            // Rate-based error (must also multiple traineegs.size() by 2
            // in the for-loop header).
            // if(k%2==1) {
            //     spikes_per_neuron = output_layer->GetSpikesPerNeuron();
            //     output_layer->SetTraining(true, true, false);
            //     if(eeg->ac=="a") {
                    
            //         rate[0] = spikes_per_neuron[0] > correct_rate ?
            //                         0.0 :
            //                         zxlb::CORRECT_EXPECTED * (1.0-(spikes_per_neuron[0]/correct_rate));
            //         rate[1] = spikes_per_neuron[1] < incorrect_rate ? 
            //                         0.0 : 
            //                         zxlb::INCORRECT_EXPECTED * (1.0 - (incorrect_rate / spikes_per_neuron[1]));
            //     }
            //     else {
            //         rate[1] = spikes_per_neuron[1] > correct_rate ?
            //                         0.0 :
            //                         zxlb::CORRECT_EXPECTED * (1.0-(spikes_per_neuron[1]/correct_rate));
            //         rate[0] = spikes_per_neuron[0] < incorrect_rate ? 
            //                         0.0 : 
            //                         zxlb::INCORRECT_EXPECTED * (1.0 - (incorrect_rate / spikes_per_neuron[0]));
            //     }
                
            //     network->Reset();
            //     counter++;
            // } else {
            //     output_layer->SetTraining(false,false,false);
            //     network->Reset();
            // }
            
            network->UpdateLayerErrorValues(
                rate, output_layer_index
            );

            sizet reading_index = 0;
            vec<double> readings;

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
            
        }
        
        
        /*------------------------------------------
        Test 10 random testing examples.
        --------------------------------------------*/

        // The training cycle produced data. We don't want to store it.
        // Purge.
        network->Reset(true);

        output_layer->SetTraining(false, false, false);

        network->RebuildDendrites();

        iteration_size = 1;
        // std::shuffle(test_indexes_A.begin(), test_indexes_A.end(), rng);
        // std::shuffle(test_indexes_C.begin(), test_indexes_C.end(), rng);

        // std::shuffle(traineegs.begin(), traineegs.end(), rng);
        std::shuffle(testeegs.begin(), testeegs.end(), rng);

        for(sizet k = 0; k < 1; k++) {
            eeg = testeegs[k];
            // if(testeegs[k]->ac=="a") {
            //     eeg = testeegs[k];//exp_eeg.GetTestingExample(A,test_indexes_A[k]);
            //     rate[0] = zxlb::CORRECT_EXPECTED;
            //     rate[1] = zxlb::INCORRECT_EXPECTED;
            // }
            // else {
            //     eeg = testeegs[k];//exp_eeg.GetTestingExample(C,test_indexes_C[k]);
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


        // MASK
        // {

            

        //     output_layer->SetTraining(
        //         false,
        //         true,
        //         false
        //     );

        //     input_layer->AddInputGenerator(igen.get());
            
        //     network->UpdateLayerErrorValues(
        //         mask_rate, output_layer_index
        //     );


        //     i64 time = 1;
        //     for(; time <= mask_time; time++) {
        //         network->Update(
        //             time,
        //             writer,
        //             rng
        //         );
        //     }

        //     // Reset the input generator
        //     igen->Reset();
        //     network->Reset();
            
        //     // Remove input generator. Only needed for mask.
        //     input_layer->AddInputGenerator(nullptr);
        // }

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

    sizet num_iterations = 2000;
    sizet iteration_size = beacon.GetSizeLabeledEntries() / 10;
    i64 time_per_example = 1000;
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    Layer * output_layer = network->GetLayer(output_layer_index);
    i64 input_layer_size = input_layer->GetLayerSize();
    sizet mask_time = 1000*(iteration_size/10);

    uptr<InputGenerator_Poisson> ig = std::make_unique<InputGenerator_Poisson>();
    ig->id = "0";
    for(i64 i = 0; i < input_layer_size; i++) {
        ig->decay.push_back(std::exp(-1.0/10));
        ig->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig->strength.push_back(500.0);
        ig->signal.push_back(0.0);
        ig->rate.push_back(0.01);
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
    // network->RebuildDendrites();

    vec<double> zero_inputs;
    for(int i = 0; i < input_layer_size; i++) {
        zero_inputs.push_back(0.0);
    }

    writer->StartRecording();
    
    

    for(sizet i = 0; i < num_iterations; i++) {
        std::cout << i << "\r" << std::flush;

        //-----------------------------------------------------------
        // TRAINING 
        beacon.ShuffleLabeledEntries();
        network->RebuildDendrites();
        network->RandomizeOrder(rng);
        output_layer->SetTraining(true,true,false);

        for(sizet k = 0; k < iteration_size; k++) {
            entry = beacon.GetLabeledEntry(k);
            
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
        }


        // TEST
        {

            network->Reset(true);
            output_layer->SetTraining(false, false, false);
            network->RebuildDendrites();


            entry = beacon.GetLabeledEntry(0);
            
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


            writer->AddExampleData(std::make_unique<ExampleData>(i,0,"NONE"));
            network->SaveData(-1);
            network->WriteData(writer);
            network->Reset(true);
        }

        ///////////////////////////////////////////////////////////
        // MASK
        if(i < num_iterations-1) {
            output_layer->SetTraining(false,true,false);
            input_layer->AddInputGenerator(ig.get());
            rates[0] = zxlb::INCORRECT_EXPECTED;
            network->UpdateLayerErrorValues(
                rates, output_layer_index
            );
            network->SetInputs(zero_inputs);

            i64 time = 1;
            for(; time <= mask_time; time++) {
                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            // Reset the input generator
            ig->Reset();
            network->Reset(true);
            network->RandomizeOrder(rng);
            input_layer->AddInputGenerator(nullptr);
        }
        

    
        // END OF ITERATION
        //------------------------------------------------------   

        // network->RebuildDendrites();
        // network->SaveData(-1);
        // network->WriteData(writer);
        
    }

    network->CleanUpData(writer);

    writer->StopRecording();

}



/*****************************
 * RADIAL POSITION 0
 *****************************/
void RunRadialPosition0(
    Writer * writer,
    Network * network,
    RNG & rng
) {
    
    vec<unsigned> labels = {0
                            };

    vec<vec<unsigned>> patterns = {
        {
        0
        }
    };

    
    sizet num_epochs = 1000;
    sizet train_iteration_size = 100; // don't make larger than num examples
    sizet test_iteration_size = 1; // don't make larger than num examples
    i64 time_per_example = 1000;
    i64 time;
    double correct_rate = 20.0;
    double incorrect_rate = 1.0;
    vec<int> spikes_per_neuron;
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();

    vuptr<InputGenerator_Poisson> ig_patterns;


    for(sizet i = 0; i < patterns.size(); i++) {
        double r = 0.04;
        uptr<InputGenerator_Poisson> pat = std::make_unique<InputGenerator_Poisson>();
        pat->id = std::to_string(i);
        for(i64 k = 0; k < input_layer_size; k++) {
            // If this input is part of the pattern.
            if(std::find(patterns[i].begin(),patterns[i].end(),k)!=patterns[i].end()) {
                pat->decay.push_back(std::exp(-1.0/10));
                pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
                pat->strength.push_back(500.0);
                pat->signal.push_back(0.0);
                pat->rate.push_back(r);
                r+=0.01;
            } else {
                pat->decay.push_back(std::exp(-1.0/10));
                pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
                pat->strength.push_back(500.0);
                pat->signal.push_back(0.0);
                pat->rate.push_back(0.0);
            }
            
        }
        ig_patterns.push_back(std::move(pat));
    }

    uptr<InputGenerator_Poisson> ig_mask = std::make_unique<InputGenerator_Poisson>();
    ig_mask->id = "mask";
    for(i64 i = 0; i < input_layer_size; i++) {
        ig_mask->decay.push_back(std::exp(-1.0/10));
        ig_mask->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig_mask->strength.push_back(500.0);
        ig_mask->signal.push_back(0.0);
        ig_mask->rate.push_back(0.00);
    }

    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());

    sizet mask_time = 0;
    vec<double> MASK_RATES = {-1.0};

    

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    vec<double> rates;
    for(i64 i = 0; i < output_layer->GetLayerSize(); i++) {
        rates.push_back(zxlb::CORRECT_EXPECTED);
    }


    vec<sizet> examples;
    for(sizet i = 0; i < patterns.size(); i++) {
        examples.push_back(i);
    }
    //-------------------------------------------------------------------------
    // Start the run

    network->RebuildDendrites();

    network->InitWriteData();

    writer->StartRecording();

    output_layer->SetRecordData(true);
    network->SaveData(-1);
    network->WriteData(writer);

    output_layer->SetRecordData(false);

    for(sizet e = 0; e < num_epochs; e++) {

        std::cout << "EPOCH: " << e << "\r" << std::flush;

        
        std::shuffle(examples.begin(), examples.end(), rng);
    
        // TRAINING SECTION
        {

            output_layer->SetTraining(
                true,
                false,
                false
            );
            

            network->RandomizeOrder(rng);

            for(sizet k = 0; k < train_iteration_size; k++) {
                // std::cout << "TRAIN " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = examples[k%examples.size()];


                network->UpdateLayerErrorValues(
                    rates, output_layer_index
                );

                input_layer->AddInputGenerator(ig_patterns[label].get());
                // input_layer->SetInputs(train_data[k].image);

                time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }


                network->Reset();
            }
        }

        // std::cout << std::endl;

        std::shuffle(examples.begin(), examples.end(), rng);

        // TESTING SECTION
        {

            // Don't train anything
            output_layer->SetTraining(
                false,
                false,
                false // DON'T TRAIN STRENGTH
            );

            // Start saving data
            output_layer->SetRecordData(true);

            network->RandomizeOrder(rng);

            network->RebuildDendrites();

            for(sizet k = 0; k < test_iteration_size; k++) {
                // std::cout << "TEST " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = examples[k%examples.size()];

                input_layer->AddInputGenerator(ig_patterns[label].get());

                i64 time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }
                //uptr<ExampleData> exdata = std::make_unique<ExampleData>(e,label,str("None"));
                //writer->AddExampleData(std::move(exdata));
                //exdata = nullptr;
                network->SaveData(-1);
                network->WriteData(writer);

                network->Reset();
                
            }

            output_layer->SetRecordData(false);

        }

        // std::cout << std::endl;

        // MASK SECTION

        // {

        //     // std::cout << "MASK" << std::flush;

        //     output_layer->SetTraining(
        //         false,
        //         false,
        //         false
        //     );

        //     input_layer->AddInputGenerator(ig_mask.get());
            
        //     network->UpdateLayerErrorValues(
        //         MASK_RATES, output_layer_index
        //     );


        //     i64 time = 1;
        //     for(; time <= mask_time; time++) {
        //         network->Update(
        //             time,
        //             writer,
        //             rng
        //         );
        //     }

        //     // Reset the input generator
        //     ig_mask->Reset();
        //     network->Reset();
            
        //     // Remove input generator. Only needed for mask.
        //     input_layer->AddInputGenerator(nullptr);
        // }

        // std::cout << std::endl;

    }

    // std::cout << "ALL DONE\n" << std::flush;

    network->CleanUpData(writer);
    writer->StopRecording();

}




/*****************************
 * RADIAL POSITION 1
 *****************************/
void RunRadialPosition1(
    Writer * writer,
    Network * network,
    RNG & rng
) {
    
    vec<unsigned> labels = {0,1,2,3
                            };

    vec<vec<unsigned>> patterns = {
        {
        0,1,2,3,4
        }
    };

    
    sizet num_epochs = 1000;
    sizet train_iteration_size = 100; // don't make larger than num examples
    sizet test_iteration_size = 1; // don't make larger than num examples
    i64 time_per_example = 1000;
    i64 time;
    double correct_rate = 20.0;
    double incorrect_rate = 1.0;
    vec<int> spikes_per_neuron;
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();

    vuptr<InputGenerator_Poisson> ig_patterns;


    for(sizet i = 0; i < patterns.size(); i++) {
        double r = 0.04;
        uptr<InputGenerator_Poisson> pat = std::make_unique<InputGenerator_Poisson>();
        pat->id = std::to_string(i);
        for(i64 k = 0; k < input_layer_size; k++) {
            // If this input is part of the pattern.
            if(std::find(patterns[i].begin(),patterns[i].end(),k)!=patterns[i].end()) {
                pat->decay.push_back(std::exp(-1.0/10));
                pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
                pat->strength.push_back(500.0);
                pat->signal.push_back(0.0);
                pat->rate.push_back(r);
                r+=0.01;
            } else {
                pat->decay.push_back(std::exp(-1.0/10));
                pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
                pat->strength.push_back(500.0);
                pat->signal.push_back(0.0);
                pat->rate.push_back(0.0);
            }
            
        }
        ig_patterns.push_back(std::move(pat));
    }

    uptr<InputGenerator_Poisson> ig_mask = std::make_unique<InputGenerator_Poisson>();
    ig_mask->id = "mask";
    for(i64 i = 0; i < input_layer_size; i++) {
        ig_mask->decay.push_back(std::exp(-1.0/10));
        ig_mask->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig_mask->strength.push_back(500.0);
        ig_mask->signal.push_back(0.0);
        ig_mask->rate.push_back(0.00);
    }

    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());

    sizet mask_time = 0;
    vec<double> MASK_RATES = {-1.0};

    

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    vec<double> rates;
    for(i64 i = 0; i < output_layer->GetLayerSize(); i++) {
        rates.push_back(zxlb::CORRECT_EXPECTED);
    }


    vec<sizet> examples;
    for(sizet i = 0; i < patterns.size(); i++) {
        examples.push_back(i);
    }
    //-------------------------------------------------------------------------
    // Start the run

    network->RebuildDendrites();

    network->InitWriteData();

    writer->StartRecording();

    output_layer->SetRecordData(true);
    network->SaveData(-1);
    network->WriteData(writer);

    output_layer->SetRecordData(false);

    for(sizet e = 0; e < num_epochs; e++) {

        std::cout << "EPOCH: " << e << "\r" << std::flush;

        
        std::shuffle(examples.begin(), examples.end(), rng);
    
        // TRAINING SECTION
        {

            output_layer->SetTraining(
                true,
                false,
                false
            );
            

            network->RandomizeOrder(rng);

            for(sizet k = 0; k < train_iteration_size; k++) {
                // std::cout << "TRAIN " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = examples[k%examples.size()];


                network->UpdateLayerErrorValues(
                    rates, output_layer_index
                );

                input_layer->AddInputGenerator(ig_patterns[label].get());
                // input_layer->SetInputs(train_data[k].image);

                time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }


                network->Reset();
            }
        }

        // std::cout << std::endl;

        std::shuffle(examples.begin(), examples.end(), rng);

        // TESTING SECTION
        {

            // Don't train anything
            output_layer->SetTraining(
                false,
                false,
                false // DON'T TRAIN STRENGTH
            );

            // Start saving data
            output_layer->SetRecordData(true);

            network->RandomizeOrder(rng);

            network->RebuildDendrites();

            for(sizet k = 0; k < test_iteration_size; k++) {
                // std::cout << "TEST " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = examples[k%examples.size()];

                input_layer->AddInputGenerator(ig_patterns[label].get());

                i64 time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }
                //uptr<ExampleData> exdata = std::make_unique<ExampleData>(e,label,str("None"));
                //writer->AddExampleData(std::move(exdata));
                //exdata = nullptr;
                network->SaveData(-1);
                network->WriteData(writer);

                network->Reset();
                
            }

            output_layer->SetRecordData(false);

        }

    }

    // std::cout << "ALL DONE\n" << std::flush;

    network->CleanUpData(writer);
    writer->StopRecording();

}

/*****************************
 * RADIAL POSITION2
 *****************************/
void RunRadialPosition2(
    Writer * writer,
    Network * network,
    RNG & rng
) {
    
    vec<unsigned> labels = {0,1,2,3,4,
                            5,6,7,8,9,
                            10,11,12,13,14,
                            15,16,17,18,19,
                            };

    vec<vec<unsigned>> patterns = {
        {0,1,2,3,4},
        {5,6,7,8,9},
        {10,11,12,13,14},
        {15,16,17,18,19}
    };

    
    sizet num_epochs = 1000;
    sizet train_iteration_size = 100; // don't make larger than num examples
    sizet test_iteration_size = 1; // don't make larger than num examples
    i64 time_per_example = 1000;
    i64 time;
    double correct_rate = 20.0;
    double incorrect_rate = 1.0;
    vec<int> spikes_per_neuron;
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();

    vuptr<InputGenerator_Poisson> ig_patterns;


    for(sizet i = 0; i < patterns.size(); i++) {
        double r = 0.1;
        uptr<InputGenerator_Poisson> pat = std::make_unique<InputGenerator_Poisson>();
        pat->id = std::to_string(i);
        for(i64 k = 0; k < input_layer_size; k++) {
            // If this input is part of the pattern.
            if(std::find(patterns[i].begin(),patterns[i].end(),k)!=patterns[i].end()) {
                pat->decay.push_back(std::exp(-1.0/10));
                pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
                pat->strength.push_back(500.0);
                pat->signal.push_back(0.0);
                pat->rate.push_back(r);
                r-=0.02;
            } else {
                pat->decay.push_back(std::exp(-1.0/10));
                pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
                pat->strength.push_back(500.0);
                pat->signal.push_back(0.0);
                pat->rate.push_back(0.0);
            }
            
        }
        ig_patterns.push_back(std::move(pat));
    }

    uptr<InputGenerator_Poisson> ig_mask = std::make_unique<InputGenerator_Poisson>();
    ig_mask->id = "mask";
    for(i64 i = 0; i < input_layer_size; i++) {
        ig_mask->decay.push_back(std::exp(-1.0/10));
        ig_mask->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig_mask->strength.push_back(500.0);
        ig_mask->signal.push_back(0.0);
        ig_mask->rate.push_back(0.005);
    }

    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());

    sizet mask_time = 10000;
    vec<double> MASK_RATES = {-1.0};

    

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    vec<double> rates;
    for(i64 i = 0; i < output_layer->GetLayerSize(); i++) {
        rates.push_back(zxlb::CORRECT_EXPECTED);
    }


    vec<sizet> examples;
    for(sizet i = 0; i < patterns.size(); i++) {
        examples.push_back(i);
    }
    //-------------------------------------------------------------------------
    // Start the run

    network->RebuildDendrites();

    network->InitWriteData();

    writer->StartRecording();

    output_layer->SetRecordData(true);
    network->SaveData(-1);
    network->WriteData(writer);

    output_layer->SetRecordData(false);

    for(sizet e = 0; e < num_epochs; e++) {

        std::cout << "EPOCH: " << e << "\r" << std::flush;

        
        std::shuffle(examples.begin(), examples.end(), rng);
    
        // TRAINING SECTION
        {

            
            

            network->RandomizeOrder(rng);

            for(sizet k = 0; k < train_iteration_size; k++) {
                // std::cout << "TRAIN " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = examples[k%examples.size()];

                

                output_layer->SetTraining(
                    true,
                    true,
                    false
                );

                spikes_per_neuron = output_layer->GetSpikesPerNeuron();

               

                network->UpdateLayerErrorValues(
                    rates, output_layer_index
                );

                input_layer->AddInputGenerator(ig_patterns[label].get());
                // input_layer->SetInputs(train_data[k].image);

                time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }

                network->Reset();
                ig_patterns[label]->Reset();
            }
        }

        // std::cout << std::endl;

        std::shuffle(examples.begin(), examples.end(), rng);

        // TESTING SECTION
        {

            // Don't train anything
            output_layer->SetTraining(
                false,
                false,
                false // DON'T TRAIN STRENGTH
            );

            // Start saving data
            output_layer->SetRecordData(true);

            network->RandomizeOrder(rng);

            network->RebuildDendrites();

            for(sizet k = 0; k < test_iteration_size; k++) {
                // std::cout << "TEST " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = examples[k%examples.size()];

                input_layer->AddInputGenerator(ig_patterns[label].get());

                i64 time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }
                //uptr<ExampleData> exdata = std::make_unique<ExampleData>(e,label,str("None"));
                //writer->AddExampleData(std::move(exdata));
                //exdata = nullptr;
                network->SaveData(-1);
                network->WriteData(writer);

                network->Reset();
                
            }

            output_layer->SetRecordData(false);

        }

        // std::cout << std::endl;

        // MASK SECTION

        {

            // std::cout << "MASK" << std::flush;

            output_layer->SetTraining(
                false,
                true,
                false
            );

            input_layer->AddInputGenerator(ig_mask.get());
            
            network->UpdateLayerErrorValues(
                MASK_RATES, output_layer_index
            );


            i64 time = 1;
            for(; time <= mask_time; time++) {
                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            // Reset the input generator
            ig_mask->Reset();
            network->Reset();
            
            // Remove input generator. Only needed for mask.
            input_layer->AddInputGenerator(nullptr);
        }

        // std::cout << std::endl;

    }

    // std::cout << "ALL DONE\n" << std::flush;

    network->CleanUpData(writer);
    writer->StopRecording();

}

/*****************************
 * TREE BUILDING 0
 *****************************/
void RunTree0(
    Writer * writer,
    Network * network,
    RNG & rng
) {
    
    vec<unsigned> labels = {0,1,2,3,4,
                            5,6,7,8,9,
                            10,11,12,13,14,
                            15,16,17,18,19,
                            };

    vec<vec<unsigned>> patterns = {
        {0,1,2,3,4},
        {5,6,7,8,9},
        {10,11,12,13,14},
        {15,16,17,18,19}
    };

    
    sizet num_epochs = 1;
    sizet train_iteration_size = 0; // don't make larger than num examples
    sizet test_iteration_size = 1; // don't make larger than num examples
    i64 time_per_example = 1000;
    i64 time;
    double correct_rate = 20.0;
    double incorrect_rate = 1.0;
    vec<int> spikes_per_neuron;
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();

    vuptr<InputGenerator_Poisson> ig_patterns;


    for(sizet i = 0; i < patterns.size(); i++) {
        double r = 0.1;
        uptr<InputGenerator_Poisson> pat = std::make_unique<InputGenerator_Poisson>();
        pat->id = std::to_string(i);
        for(i64 k = 0; k < input_layer_size; k++) {
            // If this input is part of the pattern.
            if(std::find(patterns[i].begin(),patterns[i].end(),k)!=patterns[i].end()) {
                pat->decay.push_back(std::exp(-1.0/10));
                pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
                pat->strength.push_back(500.0);
                pat->signal.push_back(0.0);
                pat->rate.push_back(r);
                r-=0.02;
            } else {
                pat->decay.push_back(std::exp(-1.0/10));
                pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
                pat->strength.push_back(500.0);
                pat->signal.push_back(0.0);
                pat->rate.push_back(0.0);
            }
            
        }
        ig_patterns.push_back(std::move(pat));
    }

    uptr<InputGenerator_Poisson> ig_mask = std::make_unique<InputGenerator_Poisson>();
    ig_mask->id = "mask";
    for(i64 i = 0; i < input_layer_size; i++) {
        ig_mask->decay.push_back(std::exp(-1.0/10));
        ig_mask->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig_mask->strength.push_back(500.0);
        ig_mask->signal.push_back(0.0);
        ig_mask->rate.push_back(0.005);
    }

    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());

    sizet mask_time = 10000;
    vec<double> MASK_RATES = {-1.0};

    

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    vec<double> rates;
    for(i64 i = 0; i < output_layer->GetLayerSize(); i++) {
        rates.push_back(zxlb::CORRECT_EXPECTED);
    }


    vec<sizet> examples;
    for(sizet i = 0; i < patterns.size(); i++) {
        examples.push_back(i);
    }
    //-------------------------------------------------------------------------
    // Start the run

    network->RebuildDendrites();

    network->InitWriteData();

    writer->StartRecording();

    output_layer->SetRecordData(true);
    network->SaveData(-1);
    network->WriteData(writer);

    output_layer->SetRecordData(false);

    for(sizet e = 0; e < num_epochs; e++) {

        std::cout << "EPOCH: " << e << "\r" << std::flush;

        
        std::shuffle(examples.begin(), examples.end(), rng);
    
        // TRAINING SECTION
        {

            
            

            network->RandomizeOrder(rng);

            for(sizet k = 0; k < train_iteration_size; k++) {
                // std::cout << "TRAIN " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = examples[k%examples.size()];

                

                output_layer->SetTraining(
                    true,
                    true,
                    false
                );

                spikes_per_neuron = output_layer->GetSpikesPerNeuron();

               

                network->UpdateLayerErrorValues(
                    rates, output_layer_index
                );

                input_layer->AddInputGenerator(ig_patterns[label].get());
                // input_layer->SetInputs(train_data[k].image);

                time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }

                network->Reset();
                ig_patterns[label]->Reset();
            }
        }

        // std::cout << std::endl;

        std::shuffle(examples.begin(), examples.end(), rng);

        // TESTING SECTION
        {

            // Don't train anything
            output_layer->SetTraining(
                false,
                false,
                false // DON'T TRAIN STRENGTH
            );

            // Start saving data
            output_layer->SetRecordData(true);

            network->RandomizeOrder(rng);

            network->RebuildDendrites();

            for(sizet k = 0; k < test_iteration_size; k++) {
                // std::cout << "TEST " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = examples[k%examples.size()];

                input_layer->AddInputGenerator(ig_patterns[label].get());

                i64 time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }
                //uptr<ExampleData> exdata = std::make_unique<ExampleData>(e,label,str("None"));
                //writer->AddExampleData(std::move(exdata));
                //exdata = nullptr;
                network->SaveData(-1);
                network->WriteData(writer);

                network->Reset();
                
            }

            output_layer->SetRecordData(false);

        }

        // std::cout << std::endl;

        // MASK SECTION

        {

            // std::cout << "MASK" << std::flush;

            output_layer->SetTraining(
                false,
                true,
                false
            );

            input_layer->AddInputGenerator(ig_mask.get());
            
            network->UpdateLayerErrorValues(
                MASK_RATES, output_layer_index
            );


            i64 time = 1;
            for(; time <= mask_time; time++) {
                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            // Reset the input generator
            ig_mask->Reset();
            network->Reset();
            
            // Remove input generator. Only needed for mask.
            input_layer->AddInputGenerator(nullptr);
        }

        // std::cout << std::endl;

    }
    network->RebuildDendrites();
    // std::cout << "ALL DONE\n" << std::flush;

    network->CleanUpData(writer);
    writer->StopRecording();

}
/*****************************
 * TREE BUILDING 1
 *****************************/
void RunTree1(
    Writer * writer,
    Network * network,
    RNG & rng
) {
    
    vec<unsigned> labels = {0,1,2,3,4,
                            5,6,7,8,9,
                            10,11,12,13,14,
                            15,16,17,18,19,
                            20,21,22,23,24,
                            25,26,27,28,29,
                            30,31,32,33,34,
                            35,36,37,38,39
                            };

    vec<vec<unsigned>> patterns = {
        {0,1,2,3,4,
        5,6,7,8,9},
        {10,11,12,13,14,
        15,16,17,18,19},
        {20,21,22,23,24,
        25,26,27,28,29},
        {30,31,32,33,34,
        35,36,37,38,39}
    };

    
    sizet num_epochs = 1000;
    sizet train_iteration_size = 100; // don't make larger than num examples
    sizet test_iteration_size = 1; // don't make larger than num examples
    i64 time_per_example = 1000;
    i64 time;
    double correct_rate = 20.0;
    double incorrect_rate = 1.0;
    vec<int> spikes_per_neuron;
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();

    vuptr<InputGenerator_Poisson> ig_patterns;


    for(sizet i = 0; i < patterns.size(); i++) {
        double r = 0.05;
        uptr<InputGenerator_Poisson> pat = std::make_unique<InputGenerator_Poisson>();
        pat->id = std::to_string(i);
        for(i64 k = 0; k < input_layer_size; k++) {
            // If this input is part of the pattern.
            if(std::find(patterns[i].begin(),patterns[i].end(),k)!=patterns[i].end()) {
                pat->decay.push_back(std::exp(-1.0/10));
                pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
                pat->strength.push_back(500.0);
                pat->signal.push_back(0.0);
                pat->rate.push_back(r);
                //r-=0.02;
            } else {
                pat->decay.push_back(std::exp(-1.0/10));
                pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
                pat->strength.push_back(500.0);
                pat->signal.push_back(0.0);
                pat->rate.push_back(0.0);
            }
            
        }
        ig_patterns.push_back(std::move(pat));
    }

    uptr<InputGenerator_Poisson> ig_mask = std::make_unique<InputGenerator_Poisson>();
    ig_mask->id = "mask";
    for(i64 i = 0; i < input_layer_size; i++) {
        ig_mask->decay.push_back(std::exp(-1.0/10));
        ig_mask->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig_mask->strength.push_back(500.0);
        ig_mask->signal.push_back(0.0);
        ig_mask->rate.push_back(0.01);
    }

    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());

    sizet mask_time = 10000;
    vec<double> MASK_RATES = {-0.1};

    

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    vec<double> rates;
    for(i64 i = 0; i < output_layer->GetLayerSize(); i++) {
        rates.push_back(zxlb::CORRECT_EXPECTED);
    }


    vec<sizet> examples;
    for(sizet i = 0; i < patterns.size(); i++) {
        examples.push_back(i);
    }
    //-------------------------------------------------------------------------
    // Start the run

    network->RebuildDendrites();

    network->InitWriteData();

    writer->StartRecording();

    output_layer->SetRecordData(true);
    network->SaveData(-1);
    network->WriteData(writer);

    output_layer->SetRecordData(false);

    for(sizet e = 0; e < num_epochs; e++) {

        std::cout << "EPOCH: " << e << "\r" << std::flush;

        
        std::shuffle(examples.begin(), examples.end(), rng);
    
        // TRAINING SECTION
        {

            
            

            network->RandomizeOrder(rng);

            for(sizet k = 0; k < train_iteration_size; k++) {
                // std::cout << "TRAIN " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = examples[k%examples.size()];

                

                output_layer->SetTraining(
                    true,
                    true,
                    false
                );

                spikes_per_neuron = output_layer->GetSpikesPerNeuron();

               

                network->UpdateLayerErrorValues(
                    rates, output_layer_index
                );

                input_layer->AddInputGenerator(ig_patterns[label].get());
                // input_layer->SetInputs(train_data[k].image);

                time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }

                network->Reset();
                ig_patterns[label]->Reset();
            }
        }

        // std::cout << std::endl;

        std::shuffle(examples.begin(), examples.end(), rng);

        // TESTING SECTION
        {

            // Don't train anything
            output_layer->SetTraining(
                false,
                false,
                false // DON'T TRAIN STRENGTH
            );

            // Start saving data
            output_layer->SetRecordData(true);

            network->RandomizeOrder(rng);

            network->RebuildDendrites();

            for(sizet k = 0; k < test_iteration_size; k++) {
                // std::cout << "TEST " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = examples[k%examples.size()];

                input_layer->AddInputGenerator(ig_patterns[label].get());

                i64 time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }
                //uptr<ExampleData> exdata = std::make_unique<ExampleData>(e,label,str("None"));
                //writer->AddExampleData(std::move(exdata));
                //exdata = nullptr;
                network->SaveData(-1);
                network->WriteData(writer);

                network->Reset();
                
            }

            output_layer->SetRecordData(false);

        }

        // std::cout << std::endl;

        // MASK SECTION

        {

            // std::cout << "MASK" << std::flush;

            output_layer->SetTraining(
                false,
                true,
                false
            );

            input_layer->AddInputGenerator(ig_mask.get());
            
            network->UpdateLayerErrorValues(
                MASK_RATES, output_layer_index
            );


            i64 time = 1;
            for(; time <= mask_time; time++) {
                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            // Reset the input generator
            ig_mask->Reset();
            network->Reset();
            
            // Remove input generator. Only needed for mask.
            input_layer->AddInputGenerator(nullptr);
        }

        // std::cout << std::endl;

    }
    network->RebuildDendrites();
    // std::cout << "ALL DONE\n" << std::flush;

    network->CleanUpData(writer);
    writer->StopRecording();

}

/*****************************
 * TREE BUILDING 2
 *****************************/
void RunTree2(
    Writer * writer,
    Network * network,
    RNG & rng
) {
    
    vec<unsigned> labels = {0,1,2,3,4,
                            5,6,7,8,9,
                            10,11,12,13,14,
                            15,16,17,18,19,
                            20,21,22,23,24,
                            25,26,27,28,29,
                            30,31,32,33,34,
                            35,36,37,38,39
                            };

    vec<vec<unsigned>> patterns = {
        {0,1,2,3,4,
        5,6,7,8,9},
        {10,11,12,13,14,
        15,16,17,18,19},
        {20,21,22,23,24,
        25,26,27,28,29},
        {30,31,32,33,34,
        35,36,37,38,39}
    };

    sizet mask_time = 10000;
    sizet num_epochs = 1000;
    sizet train_iteration_size = 100; // don't make larger than num examples
    sizet test_iteration_size = 4; // one per patter
    i64 time_per_example = 1000;
    i64 time;
    double correct_rate = 20.0;
    double incorrect_rate = 1.0;
    vec<int> spikes_per_neuron;
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();

    vuptr<InputGenerator_Poisson> ig_patterns;

    vec<double> r = {0.005, 0.01, 0.02, 0.04};
    for(sizet i = 0; i < patterns.size(); i++) {
        
        uptr<InputGenerator_Poisson> pat = std::make_unique<InputGenerator_Poisson>();
        pat->id = std::to_string(i);
        for(i64 k = 0; k < input_layer_size; k++) {
            // If this input is part of the pattern.
            if(std::find(patterns[i].begin(),patterns[i].end(),k)!=patterns[i].end()) {
                pat->decay.push_back(std::exp(-1.0/10));
                pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
                pat->strength.push_back(500.0);
                pat->signal.push_back(0.0);
                pat->rate.push_back(r[i]);
                //r-=0.02;
            } else {
                pat->decay.push_back(std::exp(-1.0/10));
                pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
                pat->strength.push_back(500.0);
                pat->signal.push_back(0.0);
                pat->rate.push_back(0.0);
            }
            
        }
        ig_patterns.push_back(std::move(pat));
        // r+=0.025;
    }

    uptr<InputGenerator_Poisson> ig_mask = std::make_unique<InputGenerator_Poisson>();
    ig_mask->id = "mask";
    for(i64 i = 0; i < input_layer_size; i++) {
        ig_mask->decay.push_back(std::exp(-1.0/10));
        ig_mask->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig_mask->strength.push_back(500.0);
        ig_mask->signal.push_back(0.0);
        ig_mask->rate.push_back(0.01);
    }

    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());

    
    vec<double> MASK_RATES = {-0.1};

    

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    vec<double> rates;
    for(i64 i = 0; i < output_layer->GetLayerSize(); i++) {
        rates.push_back(zxlb::CORRECT_EXPECTED);
    }


    vec<sizet> examples;
    for(sizet i = 0; i < patterns.size(); i++) {
        examples.push_back(i);
    }
    //-------------------------------------------------------------------------
    // Start the run

    network->RebuildDendrites();

    network->InitWriteData();

    writer->StartRecording();

    output_layer->SetRecordData(true);
    network->SaveData(-1);
    network->WriteData(writer);

    output_layer->SetRecordData(false);

    for(sizet e = 0; e < num_epochs; e++) {

        std::cout << "EPOCH: " << e << "\r" << std::flush;

        
        std::shuffle(examples.begin(), examples.end(), rng);
    
        // TRAINING SECTION
        {

            
            

            network->RandomizeOrder(rng);

            for(sizet k = 0; k < train_iteration_size; k++) {
                // std::cout << "TRAIN " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = examples[k%examples.size()];

                

                output_layer->SetTraining(
                    true,
                    true,
                    false
                );

                spikes_per_neuron = output_layer->GetSpikesPerNeuron();

               

                network->UpdateLayerErrorValues(
                    rates, output_layer_index
                );

                input_layer->AddInputGenerator(ig_patterns[label].get());
                // input_layer->SetInputs(train_data[k].image);

                time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }

                network->Reset();
                ig_patterns[label]->Reset();
            }
        }

        // std::cout << std::endl;

        std::shuffle(examples.begin(), examples.end(), rng);

        // TESTING SECTION
        {

            // Don't train anything
            output_layer->SetTraining(
                false,
                false,
                false // DON'T TRAIN STRENGTH
            );

            // Start saving data
            output_layer->SetRecordData(true);

            network->RandomizeOrder(rng);

            network->RebuildDendrites();

            for(sizet k = 0; k < test_iteration_size; k++) {
                // std::cout << "TEST " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = examples[k%examples.size()];

                input_layer->AddInputGenerator(ig_patterns[label].get());

                i64 time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }
                //uptr<ExampleData> exdata = std::make_unique<ExampleData>(e,label,str("None"));
                //writer->AddExampleData(std::move(exdata));
                //exdata = nullptr;
                writer->AddExampleData(std::make_unique<ExampleData>(k,label,std::to_string(label)));
                network->SaveData(-1);
                network->WriteData(writer);

                network->Reset();
                
            }

            output_layer->SetRecordData(false);

        }

        // std::cout << std::endl;

        // MASK SECTION

        {

            // std::cout << "MASK" << std::flush;

            output_layer->SetTraining(
                false,
                true,
                false
            );

            input_layer->AddInputGenerator(ig_mask.get());
            
            network->UpdateLayerErrorValues(
                MASK_RATES, output_layer_index
            );


            i64 time = 1;
            for(; time <= mask_time; time++) {
                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            // Reset the input generator
            ig_mask->Reset();
            network->Reset();
            
            // Remove input generator. Only needed for mask.
            input_layer->AddInputGenerator(nullptr);
        }

        // std::cout << std::endl;

    }
    network->RebuildDendrites();
    // std::cout << "ALL DONE\n" << std::flush;

    network->CleanUpData(writer);
    writer->StopRecording();

}




/*****************************
 * TREE BUILDING 3
 *****************************/
void RunTree3(
    Writer * writer,
    Network * network,
    RNG & rng
) {
    
    // vec<unsigned> labels;
    // for(sizet i = 0; i < 100; i++) labels.push_back(i);

    // vec<vec<unsigned>> patterns = {
    //     {0,1,2,3,4,
    //     5,6,7,8,9},
    //     {10,11,12,13,14,
    //     15,16,17,18,19},
    //     {20,21,22,23,24,
    //     25,26,27,28,29},
    //     {30,31,32,33,34,
    //     35,36,37,38,39}
    // };

    sizet num_patterns = 1;
    sizet num_inputs = 100;
    sizet num_epochs = 1000;
    sizet train_iteration_size = 100; // don't make larger than num examples
    sizet test_iteration_size = 10; // don't make larger than num examples
    i64 time_per_example = 1000;
    i64 time;
    double correct_rate = 20.0;
    double incorrect_rate = 1.0;
    vec<int> spikes_per_neuron;
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();

    vuptr<InputGenerator_Poisson> ig_patterns;

    double center = 49.5;
    for(sizet i = 0; i < num_patterns; i++) {
        uptr<InputGenerator_Poisson> pat = std::make_unique<InputGenerator_Poisson>();
        pat->id = std::to_string(i);
        for(i64 k = 0; k < num_inputs; k++) {
            pat->decay.push_back(std::exp(-1.0/10));
            pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
            pat->strength.push_back(500.0);
            pat->signal.push_back(0.0);

            pat->rate.push_back(
                0.1 / (std::pow(0.1*((k-center)),2.0)+1.0)
            );
        }
        ig_patterns.push_back(std::move(pat));
    }

    vuptr<InputGenerator_Poisson> ig_testing;
    sizet start = 0;
    sizet end = 10;
    for(sizet i = 0; i < 10; i++) {
        uptr<InputGenerator_Poisson> pat = std::make_unique<InputGenerator_Poisson>();
        pat->id = std::to_string(i);
        for(i64 k = 0; k < num_inputs; k++) {
            pat->decay.push_back(std::exp(-1.0/10));
            pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
            pat->strength.push_back(500.0);
            pat->signal.push_back(0.0);
            if(k >= start && k < end) {
                pat->rate.push_back(0.05);
            } else {
                pat->rate.push_back(0.0);
            }
        }
        ig_testing.push_back(std::move(pat));
        start = end;
        end += 10;
    }

    uptr<InputGenerator_Poisson> ig_mask = std::make_unique<InputGenerator_Poisson>();
    ig_mask->id = "mask";
    for(i64 i = 0; i < input_layer_size; i++) {
        ig_mask->decay.push_back(std::exp(-1.0/10));
        ig_mask->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig_mask->strength.push_back(500.0);
        ig_mask->signal.push_back(0.0);
        ig_mask->rate.push_back(0.01);
    }

    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());

    sizet mask_time = 10000;
    vec<double> MASK_RATES = {-0.1};

    

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    vec<double> rates;
    for(i64 i = 0; i < output_layer->GetLayerSize(); i++) {
        rates.push_back(zxlb::CORRECT_EXPECTED);
    }


    vec<sizet> examples;
    for(sizet i = 0; i < num_patterns; i++) {
        examples.push_back(i);
    }
    vec<sizet> testing_examples;
    for(sizet i = 0; i < 10; i++) {
        testing_examples.push_back(i);
    }
    //-------------------------------------------------------------------------
    // Start the run

    network->RebuildDendrites();

    network->InitWriteData();

    writer->StartRecording();

    output_layer->SetRecordData(true);
    network->SaveData(-1);
    network->WriteData(writer);

    output_layer->SetRecordData(false);

    for(sizet e = 0; e < num_epochs; e++) {

        std::cout << "EPOCH: " << e << "\r" << std::flush;

        
        std::shuffle(examples.begin(), examples.end(), rng);
    
        // TRAINING SECTION
        {

            
            

            network->RandomizeOrder(rng);

            for(sizet k = 0; k < train_iteration_size; k++) {
                // std::cout << "TRAIN " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = examples[k%examples.size()];

                

                output_layer->SetTraining(
                    true,
                    true,
                    false
                );

                spikes_per_neuron = output_layer->GetSpikesPerNeuron();

               

                network->UpdateLayerErrorValues(
                    rates, output_layer_index
                );
                 
                input_layer->AddInputGenerator(ig_patterns[label].get());
                // input_layer->SetInputs(train_data[k].image);

                time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }

                network->Reset();
                ig_patterns[label]->Reset();
            }
        }

        // std::cout << std::endl;

        //std::shuffle(testing_examples.begin(), testing_examples.end(), rng);

        // TESTING SECTION
        {

            // Don't train anything
            output_layer->SetTraining(
                false,
                false,
                false // DON'T TRAIN STRENGTH
            );

            // Start saving data
            output_layer->SetRecordData(true);

            network->RandomizeOrder(rng);

            network->RebuildDendrites();

            for(sizet k = 0; k < test_iteration_size; k++) {
                // std::cout << "TEST " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = testing_examples[k%testing_examples.size()];

                input_layer->AddInputGenerator(ig_testing[label].get());

                i64 time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }
                //uptr<ExampleData> exdata = std::make_unique<ExampleData>(e,label,str("None"));
                //writer->AddExampleData(std::move(exdata));
                //exdata = nullptr;
                network->SaveData(-1);
                network->WriteData(writer);

                network->Reset();
                
            }

            output_layer->SetRecordData(false);

        }

        // std::cout << std::endl;

        // MASK SECTION

        {

            // std::cout << "MASK" << std::flush;

            output_layer->SetTraining(
                false,
                true,
                false
            );

            input_layer->AddInputGenerator(ig_mask.get());
            
            network->UpdateLayerErrorValues(
                MASK_RATES, output_layer_index
            );


            i64 time = 1;
            for(; time <= mask_time; time++) {
                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            // Reset the input generator
            ig_mask->Reset();
            network->Reset();
            
            // Remove input generator. Only needed for mask.
            input_layer->AddInputGenerator(nullptr);
        }

        // std::cout << std::endl;

    }
    network->RebuildDendrites();
    // std::cout << "ALL DONE\n" << std::flush;

    network->CleanUpData(writer);
    writer->StopRecording();

}

/*****************************
 * TREE BUILDING 3 TESTB
 *****************************/
void RunTree3_TestB(
    Writer * writer,
    Network * network,
    RNG & rng
) {
    
    // vec<unsigned> labels;
    // for(sizet i = 0; i < 100; i++) labels.push_back(i);

    // vec<vec<unsigned>> patterns = {
    //     {0,1,2,3,4,
    //     5,6,7,8,9},
    //     {10,11,12,13,14,
    //     15,16,17,18,19},
    //     {20,21,22,23,24,
    //     25,26,27,28,29},
    //     {30,31,32,33,34,
    //     35,36,37,38,39}
    // };

    sizet num_patterns = 1;
    sizet num_inputs = 100;
    sizet num_epochs = 100;
    sizet train_iteration_size = 0; // don't make larger than num examples
    sizet test_iteration_size = 10; // don't make larger than num examples
    i64 time_per_example = 1000;
    i64 time;
    double correct_rate = 20.0;
    double incorrect_rate = 1.0;
    vec<int> spikes_per_neuron;
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();

    vuptr<InputGenerator_Poisson> ig_patterns;

    double center = 49.5;
    for(sizet i = 0; i < num_patterns; i++) {
        uptr<InputGenerator_Poisson> pat = std::make_unique<InputGenerator_Poisson>();
        pat->id = std::to_string(i);
        for(i64 k = 0; k < num_inputs; k++) {
            pat->decay.push_back(std::exp(-1.0/10));
            pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
            pat->strength.push_back(500.0);
            pat->signal.push_back(0.0);

            pat->rate.push_back(
                0.1 / (std::pow(0.1*((k-center)),2.0)+1.0)
            );
        }
        ig_patterns.push_back(std::move(pat));
    }

    // TEST A
    vuptr<InputGenerator_Poisson> ig_testing;
    sizet start = 0;
    sizet end = 10;
    for(sizet i = 0; i < 10; i++) {
        uptr<InputGenerator_Poisson> pat = std::make_unique<InputGenerator_Poisson>();
        pat->id = std::to_string(i);
        for(i64 k = 0; k < num_inputs; k++) {
            pat->decay.push_back(std::exp(-1.0/10));
            pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
            pat->strength.push_back(500.0);
            pat->signal.push_back(0.0);
            if(k >= start && k < end) {
                pat->rate.push_back(0.05);
            } else {
                pat->rate.push_back(0.0);
            }
        }
        ig_testing.push_back(std::move(pat));
        start = end;
        end += 10;
    }

    // TEST B
    // vuptr<InputGenerator_Poisson> ig_testing;
    // sizet remainder = 1;
    // for(sizet i = 0; i < 10; i++) {
    //     uptr<InputGenerator_Poisson> pat = std::make_unique<InputGenerator_Poisson>();
    //     pat->id = std::to_string(i);
    //     for(i64 k = 0; k < num_inputs; k++) {
    //         pat->decay.push_back(std::exp(-1.0/10));
    //         pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
    //         pat->strength.push_back(500.0);
    //         pat->signal.push_back(0.0);
    //         if(k/10 == 2 || k/10 == 1) {
    //             pat->rate.push_back(0.05);
    //         }
    //         else {
    //             pat->rate.push_back(0.0);
    //         }

    //     }
    //     ig_testing.push_back(std::move(pat));
    //     remainder++;
    // }

    uptr<InputGenerator_Poisson> ig_mask = std::make_unique<InputGenerator_Poisson>();
    ig_mask->id = "mask";
    for(i64 i = 0; i < input_layer_size; i++) {
        ig_mask->decay.push_back(std::exp(-1.0/10));
        ig_mask->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig_mask->strength.push_back(500.0);
        ig_mask->signal.push_back(0.0);
        ig_mask->rate.push_back(0.01);
    }

    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());

    sizet mask_time = 0;
    vec<double> MASK_RATES = {-0.1};

    

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    vec<double> rates;
    for(i64 i = 0; i < output_layer->GetLayerSize(); i++) {
        rates.push_back(zxlb::CORRECT_EXPECTED);
    }


    vec<sizet> examples;
    for(sizet i = 0; i < num_patterns; i++) {
        examples.push_back(i);
    }
    vec<sizet> testing_examples;
    for(sizet i = 0; i < 10; i++) {
        testing_examples.push_back(i);
    }
    //-------------------------------------------------------------------------
    // Start the run

    network->RebuildDendrites();

    network->InitWriteData();

    writer->StartRecording();

    // output_layer->SetRecordData(true);
    // network->SaveData(-1);
    // network->WriteData(writer);

    output_layer->SetRecordData(false);

    for(sizet e = 0; e < num_epochs; e++) {

        std::cout << "EPOCH: " << e << "\r" << std::flush;

        
        std::shuffle(examples.begin(), examples.end(), rng);
    
        // TRAINING SECTION
        {

            
            

            network->RandomizeOrder(rng);

            for(sizet k = 0; k < train_iteration_size; k++) {
                // std::cout << "TRAIN " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = examples[k%examples.size()];

                

                output_layer->SetTraining(
                    true,
                    true,
                    false
                );

                spikes_per_neuron = output_layer->GetSpikesPerNeuron();

               

                network->UpdateLayerErrorValues(
                    rates, output_layer_index
                );
                 
                input_layer->AddInputGenerator(ig_patterns[label].get());
                // input_layer->SetInputs(train_data[k].image);

                time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }

                network->Reset();
                ig_patterns[label]->Reset();
            }
        }

        // std::cout << std::endl;

        //std::shuffle(testing_examples.begin(), testing_examples.end(), rng);

        // TESTING SECTION
        {

            // Don't train anything
            output_layer->SetTraining(
                false,
                false,
                false // DON'T TRAIN STRENGTH
            );

            // Start saving data
            output_layer->SetRecordData(true);

            network->RandomizeOrder(rng);

            network->RebuildDendrites();

            std::shuffle(testing_examples.begin(), testing_examples.end(),rng);

            for(sizet k = 0; k < test_iteration_size; k++) {
            // for(int k = 9; k >= 0; k--) {
                // std::cout << "TEST " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = testing_examples[k];

                input_layer->AddInputGenerator(ig_testing[label].get());

                i64 time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }
                writer->AddExampleData(std::make_unique<ExampleData>(e,label,str("None")));
                network->SaveData(-1);
                network->WriteData(writer);
                ig_testing[label]->Reset();
                network->Reset();
                
            }

            output_layer->SetRecordData(false);

        }

        // std::cout << std::endl;

        // MASK SECTION

        {

            // std::cout << "MASK" << std::flush;

            output_layer->SetTraining(
                false,
                true,
                false
            );

            input_layer->AddInputGenerator(ig_mask.get());
            
            network->UpdateLayerErrorValues(
                MASK_RATES, output_layer_index
            );


            i64 time = 1;
            for(; time <= mask_time; time++) {
                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            // Reset the input generator
            ig_mask->Reset();
            network->Reset();
            
            // Remove input generator. Only needed for mask.
            input_layer->AddInputGenerator(nullptr);
        }

        // std::cout << std::endl;

    }
    network->RebuildDendrites();
    // std::cout << "ALL DONE\n" << std::flush;

    network->CleanUpData(writer);
    writer->StopRecording();

}

/*****************************
 * WEIGHT A
 *****************************/
void RunWeightA(
    Writer * writer,
    Network * network,
    RNG & rng
) {

    sizet num_patterns = 2;
    sizet num_inputs = 100;
    sizet num_epochs = 1000;
    sizet train_iteration_size = 0; // don't make larger than num examples
    sizet test_iteration_size = 2; // don't make larger than num examples
    i64 time_per_example = 1000;
    i64 time;
    double correct_rate = 20.0;
    double incorrect_rate = 1.0;
    vec<int> spikes_per_neuron;
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();

    vuptr<InputGenerator_Poisson> ig_patterns;

    
    // for(sizet i = 0; i < num_patterns; i++) {
    //     uptr<InputGenerator_Poisson> pat = std::make_unique<InputGenerator_Poisson>();
    //     pat->id = std::to_string(i);
    //     for(i64 k = 0; k < num_inputs; k++) {
    //         pat->decay.push_back(std::exp(-1.0/10));
    //         pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
    //         pat->strength.push_back(500.0);
    //         pat->signal.push_back(0.0);

    //         pat->rate.push_back(
    //             0.1 / (std::pow(0.1*((k-center)),2.0)+1.0)
    //         );
    //     }
    //     ig_patterns.push_back(std::move(pat));
    // }

    // TEST A
    // vuptr<InputGenerator_Poisson> ig_testing;
    // sizet start = 0;
    // sizet end = 10;
    // double center = 25;
    // for(sizet i = 0; i < 2; i++) {
    //     uptr<InputGenerator_Poisson> pat = std::make_unique<InputGenerator_Poisson>();
    //     pat->id = std::to_string(i);
    //     for(i64 k = 0; k < num_inputs; k++) {
    //         pat->decay.push_back(std::exp(-1.0/10));
    //         pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
    //         pat->strength.push_back(500.0);
    //         pat->signal.push_back(0.0);
            
    //         pat->rate.push_back(
    //             0.1 / (std::pow(0.1*((k-center)),2.0)+1.0)
    //         );

    //     }
    //     ig_patterns.push_back(std::move(pat));
    //     start = end;
    //     end += 10;
    //     center += 50;
    // }

    // TEST B
    vuptr<InputGenerator_Poisson> ig_testing;
    sizet hot = 38;
    for(sizet i = 0; i < num_patterns; i++) {
        uptr<InputGenerator_Poisson> pat = std::make_unique<InputGenerator_Poisson>();
        pat->id = std::to_string(i);
        for(i64 k = 0; k < num_inputs; k++) {
            pat->decay.push_back(std::exp(-1.0/10));
            pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
            pat->strength.push_back(500.0);
            pat->signal.push_back(0.0);
            if(k==hot) {
                pat->rate.push_back(0.1);
            }
            else {
                pat->rate.push_back(0.0);
            }

        }
        ig_testing.push_back(std::move(pat));
        hot = 65;
    }

    uptr<InputGenerator_Poisson> ig_mask = std::make_unique<InputGenerator_Poisson>();
    ig_mask->id = "mask";
    for(i64 i = 0; i < input_layer_size; i++) {
        ig_mask->decay.push_back(std::exp(-1.0/10));
        ig_mask->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig_mask->strength.push_back(500.0);
        ig_mask->signal.push_back(0.0);
        ig_mask->rate.push_back(0.01);
    }

    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());

    sizet mask_time = 0;
    vec<double> MASK_RATES = {-0.1};

    

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    vec<vec<double>> position_error_rates;
    vec<vec<double>> weight_error_rates;
    for(int j = 0; j < num_patterns; j++) {
        vec<double> prates;
        vec<double> wrates;
        for(i64 i = 0; i < output_layer->GetLayerSize(); i++) {
            prates.push_back(zxlb::CORRECT_EXPECTED);
            wrates.push_back(zxlb::CORRECT_EXPECTED);
            // if(j==0) wrates.push_back(zxlb::CORRECT_EXPECTED);
            // else wrates.push_back(zxlb::INCORRECT_EXPECTED);
        }
        position_error_rates.push_back(std::move(prates));
        weight_error_rates.push_back(std::move(wrates));
    }


    vec<sizet> examples;
    for(sizet i = 0; i < num_patterns; i++) {
        examples.push_back(i);
    }
    //-------------------------------------------------------------------------
    // Start the run

    network->RebuildDendrites();

    network->InitWriteData();

    writer->StartRecording();

    // output_layer->SetRecordData(true);
    // network->SaveData(-1);
    // network->WriteData(writer);

    output_layer->SetRecordData(false);

    vec<vec<double>> spike_history;
    for(int i = 0; i < 2; i++) {
        vec<double> v = {10.0};
        spike_history.push_back(std::move(v));
    }

    for(sizet e = 0; e < num_epochs; e++) {

        std::cout << "EPOCH: " << e << "\r" << std::flush;

        
        std::shuffle(examples.begin(), examples.end(), rng);
    
        // TRAINING SECTION
        {

            
            

            network->RandomizeOrder(rng);

            for(sizet k = 0; k < train_iteration_size; k++) {
                // std::cout << "TRAIN " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = examples[k%examples.size()];

                output_layer->SetTraining(
                    true,
                    true,
                    true
                );

                
                vec<double> wrate = spike_history[k%examples.size()];
                if(label==0) {
                    wrate[0] = 1.0 - (wrate[0] / 10.0);// * position_error_rates[label][0];
                } else {
                    wrate[0] = 1.0 - (wrate[0] / 5.0);// * position_error_rates[label][0];
                }

                network->UpdateLayerErrorValuesSeparate(
                    position_error_rates[label], wrate, output_layer_index
                );
                 
                input_layer->AddInputGenerator(ig_patterns[label].get());
                // input_layer->SetInputs(train_data[k].image);

                time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }

                spike_history[k%examples.size()][0] = output_layer->GetSpikesPerNeuron()[0];

                network->Reset();
                ig_patterns[label]->Reset();
            }
        }

        // std::cout << std::endl;

        //std::shuffle(testing_examples.begin(), testing_examples.end(), rng);

        // TESTING SECTION
        {

            // Don't train anything
            output_layer->SetTraining(
                false,
                false,
                false // DON'T TRAIN STRENGTH
            );

            // Start saving data
            output_layer->SetRecordData(true);

            network->RandomizeOrder(rng);

            network->RebuildDendrites();

            std::shuffle(examples.begin(), examples.end(),rng);

            for(sizet k = 0; k < test_iteration_size; k++) {
            // for(int k = 9; k >= 0; k--) {
                // std::cout << "TEST " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = examples[k];

                input_layer->AddInputGenerator(ig_testing[label].get());

                i64 time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }
                writer->AddExampleData(std::make_unique<ExampleData>(e,label,str("None")));
                network->SaveData(-1);
                network->WriteData(writer);
                ig_testing[label]->Reset();
                network->Reset();
                
            }

            output_layer->SetRecordData(false);

        }

        // std::cout << std::endl;

        // MASK SECTION

        {

            // std::cout << "MASK" << std::flush;

            output_layer->SetTraining(
                false,
                true,
                false
            );

            input_layer->AddInputGenerator(ig_mask.get());
            
            network->UpdateLayerErrorValues(
                MASK_RATES, output_layer_index
            );


            i64 time = 1;
            for(; time <= mask_time; time++) {
                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            // Reset the input generator
            ig_mask->Reset();
            network->Reset();
            
            // Remove input generator. Only needed for mask.
            input_layer->AddInputGenerator(nullptr);
        }

        // std::cout << std::endl;

    }
    network->RebuildDendrites();
    // std::cout << "ALL DONE\n" << std::flush;

    network->CleanUpData(writer);
    writer->StopRecording();

}

/*****************************
 * WEIGHT B
 *****************************/
void RunWeightB(
    Writer * writer,
    Network * network,
    RNG & rng
) {

    sizet num_patterns = 1;
    sizet num_inputs = 20;
    sizet num_epochs = 1000;
    sizet train_iteration_size = 100; // don't make larger than num examples
    sizet test_iteration_size = num_patterns; // don't make larger than num examples
    i64 time_per_example = 1000;
    i64 time;
    double correct_rate = 20.0;
    double incorrect_rate = 1.0;
    vec<int> spikes_per_neuron;
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();

    vuptr<InputGenerator_Poisson> ig_patterns;

    
    // for(sizet i = 0; i < num_patterns; i++) {
    //     uptr<InputGenerator_Poisson> pat = std::make_unique<InputGenerator_Poisson>();
    //     pat->id = std::to_string(i);
    //     for(i64 k = 0; k < num_inputs; k++) {
    //         pat->decay.push_back(std::exp(-1.0/10));
    //         pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
    //         pat->strength.push_back(500.0);
    //         pat->signal.push_back(0.0);

    //         pat->rate.push_back(
    //             0.1 / (std::pow(0.1*((k-center)),2.0)+1.0)
    //         );
    //     }
    //     ig_patterns.push_back(std::move(pat));
    // }

    // TEST A
    // vuptr<InputGenerator_Poisson> ig_testing;
    sizet start = 0;
    sizet end = 100;
    double center = 0;
    for(sizet i = 0; i < num_patterns; i++) {
        uptr<InputGenerator_Poisson> pat = std::make_unique<InputGenerator_Poisson>();
        pat->id = std::to_string(i);
        for(i64 k = 0; k < num_inputs; k++) {
            pat->decay.push_back(std::exp(-1.0/10));
            pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
            pat->strength.push_back(500.0);
            pat->signal.push_back(0.0);
            
            if(k >= start && k <= end) {
                pat->rate.push_back(
                    0.1 / (std::pow(0.2*((k-center)),2.0)+1.0)
                );
            } else {
                pat->rate.push_back(0.0);
            }

            std::cout << k << " " << 0.1 / (std::pow(0.2*((k-center)),2.0)+1.0) << std::endl;
        }
        ig_patterns.push_back(std::move(pat));
        // start = end;
        // end += 10;
    }

    // TEST B
    // vuptr<InputGenerator_Poisson> ig_testing;
    // sizet remainder = 1;
    // for(sizet i = 0; i < 10; i++) {
    //     uptr<InputGenerator_Poisson> pat = std::make_unique<InputGenerator_Poisson>();
    //     pat->id = std::to_string(i);
    //     for(i64 k = 0; k < num_inputs; k++) {
    //         pat->decay.push_back(std::exp(-1.0/10));
    //         pat->dist = std::uniform_real_distribution<double>(0.0,1.0);
    //         pat->strength.push_back(500.0);
    //         pat->signal.push_back(0.0);
    //         if(k/10 == 2 || k/10 == 1) {
    //             pat->rate.push_back(0.05);
    //         }
    //         else {
    //             pat->rate.push_back(0.0);
    //         }

    //     }
    //     ig_testing.push_back(std::move(pat));
    //     remainder++;
    // }

    uptr<InputGenerator_Poisson> ig_mask = std::make_unique<InputGenerator_Poisson>();
    ig_mask->id = "mask";
    for(i64 i = 0; i < input_layer_size; i++) {
        ig_mask->decay.push_back(std::exp(-1.0/10));
        ig_mask->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig_mask->strength.push_back(500.0);
        ig_mask->signal.push_back(0.0);
        ig_mask->rate.push_back(0.01);
    }

    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());

    sizet mask_time = 0;
    vec<double> MASK_RATES = {-0.1};

    

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    vec<vec<double>> position_error_rates;
    vec<vec<double>> weight_error_rates;
    for(int j = 0; j < num_patterns; j++) {
        vec<double> prates;
        vec<double> wrates;
        for(i64 i = 0; i < output_layer->GetLayerSize(); i++) {
            prates.push_back(zxlb::CORRECT_EXPECTED);
            wrates.push_back(zxlb::CORRECT_EXPECTED);
            // if(j==0) wrates.push_back(zxlb::CORRECT_EXPECTED);
            // else wrates.push_back(zxlb::INCORRECT_EXPECTED);
        }
        position_error_rates.push_back(std::move(prates));
        weight_error_rates.push_back(std::move(wrates));
    }


    vec<sizet> examples;
    for(sizet i = 0; i < num_patterns; i++) {
        examples.push_back(i);
    }
    //-------------------------------------------------------------------------
    // Start the run

    network->RebuildDendrites();

    network->InitWriteData();

    writer->StartRecording();

    // output_layer->SetRecordData(true);
    // network->SaveData(-1);
    // network->WriteData(writer);

    output_layer->SetRecordData(false);

    vec<vec<double>> spike_history;
    for(int i = 0; i < num_patterns; i++) {
        vec<double> v = {10.0};
        spike_history.push_back(std::move(v));
    }

    for(sizet e = 0; e < num_epochs; e++) {

        std::cout << "EPOCH: " << e << "\r" << std::flush;

        
        std::shuffle(examples.begin(), examples.end(), rng);
    
        // TRAINING SECTION
        {

            
            

            network->RandomizeOrder(rng);

            for(sizet k = 0; k < train_iteration_size; k++) {
                // std::cout << "TRAIN " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = examples[k%examples.size()];

                output_layer->SetTraining(
                    true,
                    true,
                    true
                );

                
                vec<double> wrate = spike_history[k%examples.size()];
                //if(label==0) {
                wrate[0] = 1.0 - (wrate[0] / 10.0);// * position_error_rates[label][0];
                //} else {
                //    wrate[0] = 1.0 - (wrate[0] / 5.0);// * position_error_rates[label][0];
                //}

                network->UpdateLayerErrorValuesSeparate(
                    position_error_rates[label], wrate, output_layer_index
                );
                 
                input_layer->AddInputGenerator(ig_patterns[label].get());
                // input_layer->SetInputs(train_data[k].image);

                time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }

                spike_history[k%examples.size()][0] = output_layer->GetSpikesPerNeuron()[0];

                network->Reset();
                ig_patterns[label]->Reset();
            }
        }

        // std::cout << std::endl;

        //std::shuffle(testing_examples.begin(), testing_examples.end(), rng);

        // TESTING SECTION
        {

            // Don't train anything
            output_layer->SetTraining(
                false,
                false,
                false // DON'T TRAIN STRENGTH
            );

            // Start saving data
            output_layer->SetRecordData(true);

            network->RandomizeOrder(rng);

            network->RebuildDendrites();

            std::shuffle(examples.begin(), examples.end(),rng);

            for(sizet k = 0; k < test_iteration_size; k++) {
            // for(int k = 9; k >= 0; k--) {
                // std::cout << "TEST " << k << "\r" << std::flush;

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = examples[k];

                input_layer->AddInputGenerator(ig_patterns[label].get());

                i64 time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }
                writer->AddExampleData(std::make_unique<ExampleData>(e,label,str("None")));
                network->SaveData(-1);
                network->WriteData(writer);
                ig_patterns[label]->Reset();
                network->Reset();
                
            }

            output_layer->SetRecordData(false);

        }

        // std::cout << std::endl;

        // MASK SECTION

        {

            // std::cout << "MASK" << std::flush;

            output_layer->SetTraining(
                false,
                true,
                false
            );

            input_layer->AddInputGenerator(ig_mask.get());
            
            network->UpdateLayerErrorValues(
                MASK_RATES, output_layer_index
            );


            i64 time = 1;
            for(; time <= mask_time; time++) {
                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            // Reset the input generator
            ig_mask->Reset();
            network->Reset();
            
            // Remove input generator. Only needed for mask.
            input_layer->AddInputGenerator(nullptr);
        }

        // std::cout << std::endl;

    }
    network->RebuildDendrites();
    // std::cout << "ALL DONE\n" << std::flush;

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

    int time_start = 0;
    int time_end = 1300;
    int time_total = 1500;

    vec<MoveType> move_types = {
        MoveType::DOWN,
        MoveType::RIGHT
    };

    // ExpMove exp_move(
    //     10,
    //     10,
    //     0.5,
    //     1.0,
    //     time_start,
    //     time_end,
    //     move_types
    // );
    // vec<ExpMoveInstance> instances = exp_move.GetAllInstances();

    vec<sizet> patterns = {
        0,1
    };

    vec<int> ymoves = {1,2,3,4,5,6,7,8,9,10};
    vec<int> xmoves = {1,2,3,4,5,6,7,8,9,10};

    sizet num_epochs = 2000;
    sizet num_iterations = 200;
    i64 time_per_example = 1100;

    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());

    vec<double> w_rate = {
        0,
        0
    };
    vec<double> p_rate = {
        1,
        1
    };

    // input_layer->AddInputGenerator(igen.get());

    writer->StartRecording();

    // Build the dendrites the first time.
    if(zxlb::FULL_REBUILD) network->InitDendrites();

    sizet mask_time = 0.0;//1000;
    vec<double> MASK_RATES = {-0.01, -0.01};
    uptr<InputGenerator_Poisson> ig_mask = std::make_unique<InputGenerator_Poisson>();
    ig_mask->id = "mask";
    for(i64 i = 0; i < input_layer->GetLayerSize(); i++) {
        ig_mask->decay.push_back(std::exp(-1.0/10));
        ig_mask->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig_mask->strength.push_back(500.0);
        ig_mask->signal.push_back(0.0);
        ig_mask->rate.push_back(0.001);
    }


    /* REMOVE TO STOP IGEN */
    // uptr<InputGenerator_Poisson> igen = std::make_unique<InputGenerator_Poisson>();
    // igen->id = "0";
    // igen->dist = std::uniform_real_distribution<double>(0.0,1.0);
    // for(int i = 0; i < input_layer->GetLayerSize(); i++) {
    //     igen->decay.push_back(std::exp(-1.0/10.0));
    //     igen->strength.push_back(500.0);
    //     igen->signal.push_back(0.0);
    //     igen->rate.push_back(0.0);
    // }
    // input_layer->AddInputGenerator(igen.get());

    vec<double> input_rates;
    MoveExp mexp;
    mexp.maxrate = 500;
    for(int x = 100; x <= 1000; x+=100) {
        for(int y = 100; y <= 1000; y+=100) {
            Coord c;
            c.x = x; c.y=y;
            mexp.cells.push_back(std::move(c));
            input_rates.push_back(0.0);
        }
    }

    std::uniform_real_distribution<double> xDist(1,1000);
    std::uniform_real_distribution<double> yDist(1,1000);

    double CORRECT_RATE = 2.0;
    double INCORRECT_RATE = 1.0;

    vec<vec<double>> spike_rates;
    for(int i = 0; i < move_types.size(); i++) {
        spike_rates.push_back({CORRECT_RATE/2,CORRECT_RATE/2});
    }

    // std::shuffle(instances.begin(), instances.end(), rng);

    network->RandomizeOrder(rng);

    network->RebuildDendrites();

    output_layer->SetRecordData(false);

    for(sizet e = 0; e < num_epochs; e++) {
        std::cout << e << "\r" << std::flush;

        // std::shuffle(move_types.begin(), move_types.end(), rng);

        output_layer->SetTraining(
            true,
            true,
            true
        );
        

        std::shuffle(xmoves.begin(), xmoves.end(), rng);
        std::shuffle(ymoves.begin(), ymoves.end(), rng);
            
        for(sizet k = 0; k < num_iterations; k++) {
            

            int index = k%move_types.size();

            MoveType mt = move_types[index];
            str type;

            if(mt==MoveType::RIGHT) {
                type = "RIGHT";
                w_rate[1] = (1.0 - spike_rates[index][1]/CORRECT_RATE);
                w_rate[0] = (spike_rates[index][0] > 0) ? (INCORRECT_RATE/spike_rates[index][0] - 1.0) : 0.0;
                p_rate[1] = 1;
                p_rate[0] = 0;//w_rate[1]/2;
                mexp.curpos.x = time_start;
                mexp.curpos.y =  yDist(rng);
                // mexp.curpos.y =  ymoves[(k/2)%10]*100;
                mexp.move_vector.x = 1.0;
                mexp.move_vector.y = 0.0;
            }
            else if(mt==MoveType::DOWN) {
                type = "DOWN";
                w_rate[1] = (spike_rates[index][1] > 0) ? (INCORRECT_RATE/spike_rates[index][1] - 1.0) : 0.0;
                w_rate[0] = (1.0 - spike_rates[index][0]/CORRECT_RATE);
                p_rate[1] = 0;//w_rate[0]/2;
                p_rate[0] = 1;
                mexp.curpos.x = xDist(rng);
                // mexp.curpos.x = xmoves[(k/2)%10]*100;
                mexp.curpos.y = time_start;
                mexp.move_vector.x = 0.0;
                mexp.move_vector.y = 1.0;
            }

            //std::cout << w_rate[0] << " " << w_rate[1] << " " << type << std::endl;
            
            network->UpdateLayerErrorValuesSeparate(
                p_rate, w_rate, output_layer_index
            );

            i64 time = 0;
            for(; time < time_per_example; time++) {

                mexp.GetRates(input_rates,type);
                // IGEN VERSION
                // igen->SetRate(input_rates);

                // NON-IGEN
                input_layer->SetInputs(input_rates);


                network->Update(
                    time,
                    writer,
                    rng
                );

                mexp.Move();
            }

            vec<int> spikes = output_layer->GetSpikesPerNeuron();
            for(int s = 0; s < spikes.size(); s++) {
                spike_rates[index][s] = (spike_rates[index][s] + spikes[s])/2.0;
            }

            // writer->AddExampleData(std::make_unique<ExampleData>(e,type,type));
            // network->SaveData(-1);
            // network->WriteData(writer);
            network->Reset();
            // igen->Reset();
        }

        network->RandomizeOrder(rng);

        network->RebuildDendrites();

        output_layer->SetRecordData(true);

        output_layer->SetTraining(
            false,
            false,
            false
        );

        std::shuffle(xmoves.begin(), xmoves.end(), rng);
        std::shuffle(ymoves.begin(), ymoves.end(), rng);

        for(sizet k = 0; k < 20; k++) {

            int index = k%move_types.size();

            MoveType mt = move_types[index];
            str type;

            if(mt==MoveType::DOWN) {
                type = "DOWN";
                mexp.curpos.x = time_start;
                mexp.curpos.y = ymoves[(k/2)%10]*100;
                mexp.move_vector.x = 1.0;
                mexp.move_vector.y = 0.0;
            }
            else if(mt==MoveType::RIGHT) {
                type = "RIGHT";
                mexp.curpos.x = xmoves[(k/2)%10]*100;
                mexp.curpos.y = time_start;
                mexp.move_vector.x = 0.0;
                mexp.move_vector.y = 1.0;
            } else {
                std::cout << "ERROR\n";
            }
            
            network->UpdateLayerErrorValuesSeparate(
                p_rate, w_rate, output_layer_index
            );

            i64 time = 0;
            for(; time < time_per_example; time++) {
                
                mexp.GetRates(input_rates,type);
                // IGEN VERSION
                // igen->SetRate(input_rates);

                // NON-IGEN
                input_layer->SetInputs(input_rates);


                network->Update(
                    time,
                    writer,
                    rng
                );

                mexp.Move();
            }

            // std::cout << mexp.curpos.x << " "<< mexp.curpos.y << std::endl;

            // std::cout << type << " ";
            vec<int> spikes = output_layer->GetSpikesPerNeuron();
            for(int s = 0; s < spikes.size(); s++) {
                spike_rates[index][s] = (spike_rates[index][s] + spikes[s])/2.0;
                // std::cout << spikes[s] << " ";
            }
            // std::cout << std::endl;

            writer->AddExampleData(std::make_unique<ExampleData>(((k/2)%10)+1,index,type));
            network->SaveData(-1);
            network->WriteData(writer);
            network->Reset();
            // igen->Reset();
        }

        output_layer->SetRecordData(false);


        // MASK SECTION

        {

            // std::cout << "MASK" << std::flush;

            output_layer->SetTraining(
                false,
                true,
                false
            );

            input_layer->AddInputGenerator(ig_mask.get());
            
            network->UpdateLayerErrorValues(
                MASK_RATES, output_layer_index
            );


            i64 time = 1;
            for(; time <= mask_time; time++) {
                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            // Reset the input generator
            ig_mask->Reset();
            network->Reset();
            
            // Remove input generator. Only needed for mask.
            input_layer->AddInputGenerator(nullptr);
        }

        // }
    }

    network->CleanUpData(writer);

    writer->StopRecording();
}






















/*****************************
 * PATTERN 001
 *****************************/
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

            vec<double> error_rates = network->GetPosErrorRates(output_layer_index);
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

            vec<double> error_rates = network->GetPosErrorRates(output_layer_index);
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

            vec<double> error_rates = network->GetPosErrorRates(output_layer_index);
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

            vec<double> error_rates = network->GetPosErrorRates(output_layer_index);
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



            vec<double> error_rates = network->GetPosErrorRates(output_layer_index);
            
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



            vec<double> error_rates = network->GetPosErrorRates(output_layer_index);
            
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



            vec<double> error_rates = network->GetPosErrorRates(output_layer_index);
            
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



            vec<double> error_rates = network->GetPosErrorRates(output_layer_index);
            
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





/******************************************************************************
 * ExpMove_Hand
 * This experiment will test whether an AD neuron can learn to differentiate
 * between two types of EEGs.
 *****************************************************************************/
void RunMove_Hand_Config(
    Network * network
) {
    Layer * out = network->GetLayer(network->GetOutputLayerIndex());

    Neuron * hN = out->GetNeuron(0);
    Neuron * vN = out->GetNeuron(1);

    int start = 100;
    int step = -10;

    for(int p = 0; p < 10; p++) {
        int r = start;
        for(int i = p; i < 100; i+=10) {
            Synapse * hS = hN->GetSynapse(i);
            hS->location.Lat(0.0); 
            hS->location.Lon(0.0); 
            hS->location.Rad(r);
            hS->dist_to_parent = std::abs(zxlb::MAX_RAD_TEMPORAL_DIFFERENCE/step);
            hS->compartment=p;
            if(i < 10) {
                hS->parent = i+10;
            } else if(i < 90) {
                hS->parent = i+10;
                hS->children.push_back(i-10);
            } else {
                hS->parent = -1;
                hS->children.push_back(i-10);
                hN->dendrites.push_back(i);
            }
            r+=step;

            // std::cout << "Neuron: " << hS->parent << " " << i << " ";
            // for(sizet x = 0; x < hS->children.size(); x++)
            //     std::cout << hS->children[x] << " ";
            // std::cout << std::endl; 
        }
    }

    for(int p = 0; p < 100; p+=10) {
        int r = start;
        for(int i = p; i < p+10; i++) {
            Synapse * vS = vN->GetSynapse(i);
            vS->location.Lat(0.0); 
            vS->location.Lon(0.0); 
            vS->location.Rad(r);
            vS->dist_to_parent = std::abs(zxlb::MAX_RAD_TEMPORAL_DIFFERENCE/step);
            vS->compartment=p;

            if(i%10==0) {
                vS->parent = i+1;
            } else if(i%10 < 9) {
                vS->parent = i+1;
                vS->children.push_back(i-1);
            } else {
                vS->parent = -1;
                vS->children.push_back(i-1);
                vN->dendrites.push_back(i);
            }
            r+=step;

            // std::cout << "Neuron: " << vS->parent << " " << i << " ";
            // for(sizet x = 0; x < vS->children.size(); x++)
            //     std::cout << vS->children[x] << " ";
            // std::cout << std::endl; 
        }
    }
}
void RunMove_Hand_Config_Reverse(
    Network * network
) {
    Layer * out = network->GetLayer(network->GetOutputLayerIndex());

    Neuron * hN = out->GetNeuron(0);
    Neuron * vN = out->GetNeuron(1);

    int start = 10;
    int step = 10;

    for(int p = 0; p < 10; p++) {
        int r = start;
        for(int i = p; i < 100; i+=10) {
            Synapse * hS = hN->GetSynapse(i);
            hS->location.Lat(0.0); 
            hS->location.Lon(0.0); 
            hS->location.Rad(r);
            hS->dist_to_parent = std::abs(zxlb::MAX_RAD_TEMPORAL_DIFFERENCE/step);
            hS->compartment=p;
            if(i < 10) {
                hS->parent = -1;
                hS->children.push_back(i+10);
                hN->dendrites.push_back(i);
                
            } else if(i < 90) {
                hS->parent = i-10;
                hS->children.push_back(i+10);
            } else {
                hS->parent = i-10;
            }
            r+=step;

            // std::cout << "Neuron: " << hS->parent << " " << i << " ";
            // for(sizet x = 0; x < hS->children.size(); x++)
            //     std::cout << hS->children[x] << " ";
            // std::cout << std::endl; 
        }
    }

    for(int p = 0; p < 100; p+=10) {
        int r = start;
        for(int i = p; i < p+10; i++) {
            Synapse * vS = vN->GetSynapse(i);
            vS->location.Lat(0.0); 
            vS->location.Lon(0.0); 
            vS->location.Rad(r);
            vS->dist_to_parent = std::abs(zxlb::MAX_RAD_TEMPORAL_DIFFERENCE/step);
            vS->compartment=p;

            if(i%10==0) {
                vS->parent = -1;
                vS->children.push_back(i+1);
                vN->dendrites.push_back(i);
            } else if(i%10 < 9) {
                vS->parent = i-1;
                vS->children.push_back(i+1);
            } else {
                vS->parent = i-1;
            }
            r+=step;

            // std::cout << "Neuron: " << vS->parent << " " << i << " ";
            // for(sizet x = 0; x < vS->children.size(); x++)
            //     std::cout << vS->children[x] << " ";
            // std::cout << std::endl; 
        }
    }
}
void RunMove_Hand_Random(
    Network * network,
    RNG & rng
) {
    Layer * out = network->GetLayer(network->GetOutputLayerIndex());

    Neuron * hN = out->GetNeuron(0);
    Neuron * vN = out->GetNeuron(1);

    int start = 10;
    int step = 10;

    int offset = 5;
    vec<int> synids;
    for(int i = 0; i < 100; i+=10) {
        vec<int> v;
        for(int k = i; k < i+10; k++) {

            v.push_back(k);

        }

        std::shuffle(v.begin()+offset, v.end(), rng);
        for(sizet k = 0; k < v.size(); k++) synids.push_back(v[k]);
    }
    

    for(int p = 0; p < 10; p++) {
        int r = start;
        for(int i = p; i < 100; i+=10) {
            Synapse * hS = hN->GetSynapse(synids[i]);
            hS->location.Lat(0.0); 
            hS->location.Lon(0.0); 
            hS->location.Rad(r);
            hS->compartment=p;
            if(i < 10) {
                hS->parent = -1;
                hS->children.push_back(i+10);
                hN->dendrites.push_back(i);
                
            } else if(i < 90) {
                hS->parent = i-10;
                hS->children.push_back(i+10);
            } else {
                hS->parent = i-10;
            }
            r+=step;

            // std::cout << "Neuron: " << hS->parent << " " << i << " ";
            // for(sizet x = 0; x < hS->children.size(); x++)
            //     std::cout << hS->children[x] << " ";
            // std::cout << std::endl; 
        }
    }

    synids.clear();
    for(int i = 0; i < 100; i+=10) {
        vec<int> v;
        for(int k = i; k < i+10; k++) {

            v.push_back(k);

        }

        std::shuffle(v.begin()+offset, v.end(), rng);
        for(sizet k = 0; k < v.size(); k++) synids.push_back(v[k]);
    }


    for(int p = 0; p < 100; p+=10) {
        int r = start;
        for(int i = p; i < p+10; i++) {
            Synapse * vS = vN->GetSynapse(synids[i]);
            vS->location.Lat(0.0); 
            vS->location.Lon(0.0); 
            vS->location.Rad(r);
            vS->compartment=p;

            if(i%10==0) {
                vS->parent = -1;
                vS->children.push_back(i+1);
                vN->dendrites.push_back(i);
            } else if(i%10 < 9) {
                vS->parent = i-1;
                vS->children.push_back(i+1);
            } else {
                vS->parent = i-1;
            }
            r+=step;

            // std::cout << "Neuron: " << vS->parent << " " << i << " ";
            // for(sizet x = 0; x < vS->children.size(); x++)
            //     std::cout << vS->children[x] << " ";
            // std::cout << std::endl; 
        }
    }
}
void RunMove_Hand(
    Writer * writer,
    Network * network,
    RNG & rng
) {

    RunMove_Hand_Config_Reverse(network);

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
        1.0,
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
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());

    vec<double> rate = {
        0,
        0
    };

    // input_layer->AddInputGenerator(igen.get());

    writer->StartRecording();

    // Build the dendrites the first time.
    //network->InitDendrites();

    output_layer->SetTraining(
        zxlb::TRAIN_RAD,
        zxlb::TRAIN_ANG,
        zxlb::TRAIN_STR
    );

    /* REMOVE TO STOP IGEN */
    uptr<InputGenerator_Poisson> igen = std::make_unique<InputGenerator_Poisson>();
    igen->id = "0";
    igen->dist = std::uniform_real_distribution<double>(0.0,1.0);
    for(int i = 0; i < input_layer->GetLayerSize(); i++) {
        igen->decay.push_back(std::exp(-1.0/10.0));
        igen->strength.push_back(200.0);
        igen->signal.push_back(0.0);
        igen->rate.push_back(0.0);
    }
    input_layer->AddInputGenerator(igen.get());

    for(sizet i = 0; i < num_iterations; i++) {
        std::cout << i << "\r" << std::flush;

        std::shuffle(instances.begin(), instances.end(), rng);

        network->RandomizeOrder(rng);

        //network->RebuildDendrites();

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

                // IGEN VERSION
                igen->SetRate(emi->signals[time]);

                // NON-IGEN
                // network->SetInputs(emi->signals[time]);


                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            writer->AddExampleData(std::make_unique<ExampleData>(i,emi->rc,type));
            network->SaveData(-1);
            network->WriteData(writer);
            network->Reset();
        }
    }

    network->CleanUpData(writer);

    writer->StopRecording();
}







void RunFMNIST(
    Writer * writer,
    Network * network,
    RNG & rng
) {
    std::string TRAIN_LABELS_FILENAME("exp_fmnist/train-labels-idx1-ubyte");
    std::string TRAIN_IMAGES_FILENAME("exp_fmnist/train-images-idx3-ubyte");
    std::string TEST_LABELS_FILENAME("exp_fmnist/t10k-labels-idx1-ubyte");
    std::string TEST_IMAGES_FILENAME("exp_fmnist/t10k-images-idx3-ubyte");

    vec<unsigned> labels = {0,1,2,3,4,5,6,7,8,9};

    std::cout << "START: LOADING MNIST DATA\n";

    MNISTReader train_mnist;
    MNISTReader test_mnist;
    train_mnist.LoadData(TRAIN_LABELS_FILENAME,TRAIN_IMAGES_FILENAME,true);
    test_mnist.LoadData(TEST_LABELS_FILENAME,TEST_IMAGES_FILENAME,true);
    // vec<MNISTData> train_data;
    // vec<MNISTData> test_data;
    vec<vec<MNISTData>> test_data_by_id;
    vec<vec<MNISTData>> train_data_by_id;
    // vec<int> labels;
    for(int i = 0; i < 10; i++) {
        // labels.push_back(i);
        vec<MNISTData> train_tmp = train_mnist.GetDataWithLabel(i);
        vec<MNISTData> test_tmp = test_mnist.GetDataWithLabel(i);
        test_data_by_id.push_back(std::move(test_tmp));
        train_data_by_id.push_back(std::move(train_tmp));
        // std::copy(train_tmp.begin(), train_tmp.end(), std::back_inserter(train_data));
        // std::copy(test_tmp.begin(), test_tmp.end(), std::back_inserter(test_data));
    }



    std::cout << "DONE: LOADING MNIST DATA\n";
    
    sizet num_epochs = 1000;
    sizet train_iteration_size = 100; // don't make larger than num examples
    sizet test_iteration_size = 10; // don't make larger than num examples

    i64 time_per_example = 1000;
    i64 time;

    double correct_rate = 10.0;
    double incorrect_rate = 1.0;
    vec<int> spikes_per_neuron;
    
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();
    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());

    // vec<sizet> train_image_indexes;
    // vec<sizet> test_image_indexes;
    // for(sizet i = 0; i < train_mnist.GetData().size(); i++) {
    //     train_image_indexes.push_back(i);
    // }
    // for(sizet i = 0; i < test_mnist.GetData().size(); i++) {
    //     test_image_indexes.push_back(i);
    // }


    vec<vec<double>> spike_history;
    vec<double> prates;
    for(int j = 0; j < 10; j++) {
        vec<double> srh;
        for(i64 i = 0; i < output_layer->GetLayerSize(); i++) {
            srh.push_back(correct_rate);
        }
        prates.push_back(1.0);
        spike_history.push_back(std::move(srh));
    }

    uptr<InputGenerator_Poisson> ig_mask = std::make_unique<InputGenerator_Poisson>();
    ig_mask->id = "0";
    for(i64 i = 0; i < input_layer_size; i++) {
        ig_mask->decay.push_back(std::exp(-1.0/10));
        ig_mask->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig_mask->strength.push_back(500.0);
        ig_mask->signal.push_back(0.0);
        ig_mask->rate.push_back(0.01);
    }

    

    i64 mask_time = 1000;
    double MASK_ERROR = -1;
    vec<double> MASK_RATES;
    for(unsigned int l = 0; l < 10; l++) {
        MASK_RATES.push_back(MASK_ERROR);
    }
    

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    vec<double> rates;
    for(i64 i = 0; i < output_layer->GetLayerSize(); i++) {
        rates.push_back(correct_rate);
    }

    //-------------------------------------------------------------------------
    // Start the run

    network->RebuildDendrites();

    writer->StartRecording();

    output_layer->SetRecordData(false);

    for(sizet e = 0; e < num_epochs; e++) {

        std::cout << "EPOCH: " << e << std::endl;

        

        // std::shuffle(test_data.begin(), test_data.end(), rng);

        std::shuffle(labels.begin(), labels.end(), rng);

        for(int i = 0; i < 10; i++) {
            std::shuffle(test_data_by_id[i].begin(), test_data_by_id[i].end(), rng);
            std::shuffle(train_data_by_id[i].begin(), train_data_by_id[i].end(), rng);
        }
    
        // TRAINING SECTION
        {

            
            

            network->RandomizeOrder(rng);

            for(sizet k = 0; k < train_iteration_size; k++) {
                std::cout << "TRAIN " << k << "\r" << std::flush;

                

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = train_data_by_id[k][labels[k%10]].label;

                

                ///////////////////////////////////////////////////////////////
                /// TRAINING RUN

                output_layer->SetTraining(
                    zxlb::TRAIN_RAD,
                    zxlb::TRAIN_ANG,
                    zxlb::TRAIN_STR
                );

                
                vec<double> wrate = spike_history[label];
                for(unsigned int l = 0; l < 10; l++) {
                    if(l==label) {
                        prates[l] = 1.0 - (wrate[l] / correct_rate);
                        wrate[l] = 1.0 - (wrate[l] / correct_rate);
                        // rates[l] = spikes_per_neuron[l] > correct_rate ? 
                        //                 0.0 : 
                        //                 zxlb::CORRECT_EXPECTED * (1.0 - (spikes_per_neuron[l]/correct_rate));
                    }
                    else {
                        prates[l] = 0.0;//1.0 - (wrate[l] / incorrect_rate);
                        wrate[l] = 1.0 - (wrate[l] / incorrect_rate);
                        // rates[l] = spikes_per_neuron[l] < incorrect_rate ? 
                        //                 0.0 : 
                        //                 zxlb::INCORRECT_EXPECTED * (1.0 - (incorrect_rate / spikes_per_neuron[l]));
                    }
                }

                network->UpdateLayerErrorValuesSeparate(
                    prates, wrate, output_layer_index
                );

                
                input_layer->SetInputs(train_data_by_id[k][labels[k%10]].image);

                time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }

                vec<int> output_rate = output_layer->GetSpikesPerNeuron();
                for(int i = 0; i < output_rate.size(); i++) {
                    spike_history[label][i] = output_rate[i];
                }

                network->Reset();
            }
        }

        std::cout << std::endl;

        // TESTING SECTION
        {

            // Don't train anything
            output_layer->SetTraining(
                false,
                false,
                false // DON'T TRAIN STRENGTH
            );

            // Start saving data
            output_layer->SetRecordData(true);

            network->RandomizeOrder(rng);

            network->RebuildDendrites();

            for(sizet k = 0; k < test_iteration_size; k++) {
                std::cout << "TEST " << k << "\r" << std::flush;

                
                input_layer->SetInputs(test_data_by_id[k][k/10].image);

                i64 time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }

                writer->AddExampleData(std::make_unique<ExampleData>(e,test_data_by_id[k][k/10].label,"None"));
        
                network->SaveData(-1);
                network->WriteData(writer);

                network->Reset();
                
            }

            output_layer->SetRecordData(false);

        }

        std::cout << std::endl;


    }

    std::cout << "ALL DONE\n" << std::flush;

    network->CleanUpData(writer);
    writer->StopRecording();

}


void RunFMNISTSingle(
    Writer * writer,
    Network * network,
    RNG & rng
) {
    std::string TRAIN_LABELS_FILENAME("exp_fmnist/train-labels-idx1-ubyte");
    std::string TRAIN_IMAGES_FILENAME("exp_fmnist/train-images-idx3-ubyte");
    std::string TEST_LABELS_FILENAME("exp_fmnist/t10k-labels-idx1-ubyte");
    std::string TEST_IMAGES_FILENAME("exp_fmnist/t10k-images-idx3-ubyte");

    vec<str> label_desc = {
        "t-shirt/top",
        "trouser",
        "pullover",
        "dress",
        "coat",
        "sandal",
        "shirt",
        "sneaker",
        "bag",
        "ankle boot",
    };

    vec<unsigned> labels = {0,5};

    std::cout << "START: LOADING FMNIST DATA\n";

    MNISTReader train_mnist;
    MNISTReader test_mnist;
    train_mnist.LoadData(TRAIN_LABELS_FILENAME,TRAIN_IMAGES_FILENAME,true);
    test_mnist.LoadData(TEST_LABELS_FILENAME,TEST_IMAGES_FILENAME,true);
    // vec<MNISTData> train_data;
    // vec<MNISTData> test_data;
    umap<int,vec<MNISTData>> test_data_by_id;
    umap<int,vec<MNISTData>> train_data_by_id;
    vec<int> indexes;
    for(int i = 0; i < labels.size(); i++) {
        indexes.push_back(i);
        vec<MNISTData> train_tmp = train_mnist.GetDataWithLabel(labels[i]);
        vec<MNISTData> test_tmp = test_mnist.GetDataWithLabel(labels[i]);
        test_data_by_id.emplace(labels[i],std::move(test_tmp));
        train_data_by_id.emplace(labels[i],std::move(train_tmp));
        // std::copy(train_tmp.begin(), train_tmp.end(), std::back_inserter(train_data));
        // std::copy(test_tmp.begin(), test_tmp.end(), std::back_inserter(test_data));
    }



    std::cout << "DONE: LOADING FMNIST DATA\n";
    
    sizet num_epochs = 1000;
    sizet num_train_iteration = 100; // don't make larger than num examples
    sizet num_test_iteration = 10*labels.size(); // don't make larger than num examples
    i64 time_per_example = 1000;
    i64 time;
    
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();
    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());

    vec<double> w_rate;
    vec<double> p_rate;
    for(int i = 0; i < labels.size(); i++) {
        w_rate.push_back(0);
        p_rate.push_back(1);
    }

    vec<vec<double>> p_error_rates;
    vec<vec<double>> w_error_rates;
    for(int i = 0; i < labels.size(); i++) {
        vec<double> v1;
        vec<double> v2;
        for(int j = 0; j < labels.size(); j++) {
            v1.push_back(0.0);
            v2.push_back(0.0);
        }
        p_error_rates.push_back(v1);
        w_error_rates.push_back(v2);
    }

    // vec<vec<double>> spike_history;
    // vec<double> prates;
    // for(int j = 0; j < labels.size(); j++) {
    //     vec<double> srh;
    //     for(i64 i = 0; i < output_layer->GetLayerSize(); i++) {
    //         srh.push_back(0.0);
    //     }
    //     prates.push_back(1.0);
    //     spike_history.push_back(std::move(srh));
    // }

    uptr<InputGenerator_Poisson> ig_mask = std::make_unique<InputGenerator_Poisson>();
    ig_mask->id = "0";
    for(i64 i = 0; i < input_layer_size; i++) {
        ig_mask->decay.push_back(std::exp(-1.0/10));
        ig_mask->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig_mask->strength.push_back(500.0);
        ig_mask->signal.push_back(0.0);
        ig_mask->rate.push_back(0.01);
    }

    

    i64 mask_time = 0;
    double MASK_ERROR = -0.1;
    vec<double> MASK_RATES;
    for(unsigned int l = 0; l < labels.size(); l++) {
        MASK_RATES.push_back(MASK_ERROR);
    }
    

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    double CORRECT_RATE = 10.0;
    double INCORRECT_RATE = 1.0;

    vec<vec<double>> spike_rates;
    vec<vec<double>> spike_history;
    for(i64 i = 0; i < labels.size(); i++) {
        vec<double> s;
        for(i64 k = 0; k < labels.size(); k++) {
            s.push_back(0);
        }
        spike_rates.push_back(std::move(s));
    }

    //-------------------------------------------------------------------------
    // Start the run


    network->RebuildDendrites();

    writer->StartRecording();

    output_layer->SetRecordData(false);

    for(sizet e = 0; e < num_epochs; e++) {

        std::cout << e << "\r" << std::flush;


        // Calculate Error
        for(int i = 0; i < labels.size(); i++) {
            for(int j = 0; j < labels.size(); j++) {
                double rate = ( spike_rates[i][j]/ 10 );
                double target = (i==j) ? CORRECT_RATE : INCORRECT_RATE;
                // if(i==j) {
                //     w_error_rates[i][j] = (1.0 - rate/CORRECT_RATE);
                //     p_error_rates[i][j] = (1.0 - rate/CORRECT_RATE);
                // } else {
                //     w_error_rates[i][j] = (rate >= INCORRECT_RATE) ? (INCORRECT_RATE/rate-1.0) : 1.0-rate;
                //     p_error_rates[i][j] = 0.0;//(1.0 - rate/CORRECT_RATE);
                //     //p_error_rates[i][j] = (rate >= 1) ? (INCORRECT_RATE/rate-1.0) : -rate;
                // }
                
                if(rate <= target) {
                    w_error_rates[i][j] = (1.0 - rate/target);
                    p_error_rates[i][j] = 1.0;//(1.0 - rate/target);
                } else {
                    w_error_rates[i][j] = (target/rate-1.0);
                    p_error_rates[i][j] = 1.0;//(target/rate-1.0);
                }
            }
        }

        //  Reset spike rates
        for(int i = 0; i < labels.size(); i++) {
            for(int j = 0; j < labels.size(); j++) {
                spike_rates[i][j]=0;
            }
        }
        

        std::shuffle(indexes.begin(), indexes.end(), rng);
        for(int i = 0; i < labels.size(); i++) {
            std::shuffle(test_data_by_id[i].begin(), test_data_by_id[i].end(), rng);
            std::shuffle(train_data_by_id[i].begin(), train_data_by_id[i].end(), rng);
        }
    
        // TRAINING SECTION
        {
            output_layer->SetRecordData(false);

            output_layer->SetTraining(
                true,
                true,
                true
            );

            network->RandomizeOrder(rng);

            for(sizet k = 0; k < num_train_iteration; k++) {
                // std::cout << "TRAIN " << k << "\r" << std::flush;

                

                int neuron = indexes[k%indexes.size()];
                int example = k/indexes.size();

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = train_data_by_id[labels[neuron]][example].label;

                

                ///////////////////////////////////////////////////////////////
                /// TRAINING RUN

                
                // ORIGINAL
                // for(unsigned int l = 0; l < indexes.size(); l++) {
                //     if(l==neuron) {
                //         w_rate[l] = (1.0 - spike_rates[neuron][l]/CORRECT_RATE);
                //         p_rate[l] = (1.0 - spike_rates[neuron][l]/CORRECT_RATE);
                //     }
                //     else {
                //         w_rate[l] = (spike_rates[neuron][l] > 0) ? (INCORRECT_RATE/spike_rates[neuron][l]-1.0) : 0.0;
                //         p_rate[l] = 0.0;
                //     }

                // }
                // network->UpdateLayerErrorValuesSeparate(
                //     p_rate, w_rate, output_layer_index
                // );

                network->UpdateLayerErrorValuesSeparate(
                    p_error_rates[neuron], w_error_rates[neuron], output_layer_index
                );

                
                input_layer->SetInputs(train_data_by_id[labels[neuron]][example].image);

                time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }

                

                network->Reset();
            }
        }

        // TESTING SECTION
        {

            // Don't train anything
            output_layer->SetTraining(
                false,
                false,
                false
            );

            // Start saving data
            output_layer->SetRecordData(true);

            network->RandomizeOrder(rng);

            network->RebuildDendrites();

            for(sizet k = 0; k < num_test_iteration; k++) {

                int neuron = indexes[k%indexes.size()];
                int example = k/indexes.size();
                unsigned int label = test_data_by_id[labels[neuron]][example].label;

                input_layer->SetInputs(test_data_by_id[labels[neuron]][example].image);

                i64 time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }

                vec<int> spikes = output_layer->GetSpikesPerNeuron();
                for(int s = 0; s < spikes.size(); s++) {
                    // spike_rates[neuron][s] = spikes[s]; // ORIGINAL
                    spike_rates[neuron][s] += spikes[s];
                }

                writer->AddExampleData(std::make_unique<ExampleData>(neuron,label,label_desc[label]));
                network->SaveData(-1);
                network->WriteData(writer);
                network->Reset();
                
            }

        }

        

        // MASK
        {

            output_layer->SetRecordData(false);

            output_layer->SetTraining(
                false,
                true,
                false
            );

            input_layer->AddInputGenerator(ig_mask.get());
            
            network->UpdateLayerErrorValues(
                MASK_RATES, output_layer_index
            );


            i64 time = 1;
            for(; time <= mask_time; time++) {
                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            // Reset the input generator
            ig_mask->Reset();
            network->Reset();
            
            // Remove input generator. Only needed for mask.
            input_layer->AddInputGenerator(nullptr);
        }

    }

    std::cout << "ALL DONE\n" << std::flush;

    network->CleanUpData(writer);
    writer->StopRecording();

}





void RunFMNISTDouble(
    Writer * writer,
    Network * network,
    RNG & rng
) {
    std::string TRAIN_LABELS_FILENAME("exp_fmnist/train-labels-idx1-ubyte");
    std::string TRAIN_IMAGES_FILENAME("exp_fmnist/train-images-idx3-ubyte");
    std::string TEST_LABELS_FILENAME("exp_fmnist/t10k-labels-idx1-ubyte");
    std::string TEST_IMAGES_FILENAME("exp_fmnist/t10k-images-idx3-ubyte");

    vec<str> label_desc = {
        "t-shirt/top",
        "trouser",
        "pullover",
        "dress",
        "coat",
        "sandal",
        "shirt",
        "sneaker",
        "bag",
        "ankle boot",
    };

    vec<unsigned> labels = {0,5};

    std::cout << "START: LOADING FMNIST DATA\n";

    MNISTReader train_mnist;
    MNISTReader test_mnist;
    train_mnist.LoadData(TRAIN_LABELS_FILENAME,TRAIN_IMAGES_FILENAME,true);
    test_mnist.LoadData(TEST_LABELS_FILENAME,TEST_IMAGES_FILENAME,true);
    // vec<MNISTData> train_data;
    // vec<MNISTData> test_data;
    umap<int,vec<MNISTData>> test_data_by_id;
    umap<int,vec<MNISTData>> train_data_by_id;
    vec<int> indexes;
    for(int i = 0; i < labels.size(); i++) {
        indexes.push_back(i);
        vec<MNISTData> train_tmp = train_mnist.GetDataWithLabel(labels[i]);
        vec<MNISTData> test_tmp = test_mnist.GetDataWithLabel(labels[i]);
        test_data_by_id.emplace(labels[i],std::move(test_tmp));
        train_data_by_id.emplace(labels[i],std::move(train_tmp));
        // std::copy(train_tmp.begin(), train_tmp.end(), std::back_inserter(train_data));
        // std::copy(test_tmp.begin(), test_tmp.end(), std::back_inserter(test_data));
    }



    std::cout << "DONE: LOADING FMNIST DATA\n";
    
    sizet num_epochs = 550;
    sizet num_train_iteration = 100; // don't make larger than num examples
    sizet num_test_iteration = 10*labels.size(); // don't make larger than num examples
    i64 time_per_example = 1000;
    i64 time;
    
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();
    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());

    vec<double> w_rate;
    vec<double> p_rate;
    for(int i = 0; i < labels.size(); i++) {
        w_rate.push_back(0);
        p_rate.push_back(1);
    }

    vec<vec<double>> p_error_rates;
    vec<vec<double>> w_error_rates;
    for(int i = 0; i < labels.size(); i++) {
        vec<double> v1;
        vec<double> v2;
        for(int j = 0; j < output_layer->GetLayerSize(); j++) {
            v1.push_back(0.0);
            v2.push_back(0.0);
        }
        p_error_rates.push_back(v1);
        w_error_rates.push_back(v2);
    }

    // vec<vec<double>> spike_history;
    // vec<double> prates;
    // for(int j = 0; j < labels.size(); j++) {
    //     vec<double> srh;
    //     for(i64 i = 0; i < output_layer->GetLayerSize(); i++) {
    //         srh.push_back(0.0);
    //     }
    //     prates.push_back(1.0);
    //     spike_history.push_back(std::move(srh));
    // }

    uptr<InputGenerator_Poisson> ig_mask = std::make_unique<InputGenerator_Poisson>();
    ig_mask->id = "0";
    for(i64 i = 0; i < input_layer_size; i++) {
        ig_mask->decay.push_back(std::exp(-1.0/10));
        ig_mask->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig_mask->strength.push_back(500.0);
        ig_mask->signal.push_back(0.0);
        ig_mask->rate.push_back(0.01);
    }

    

    i64 mask_time = 0;//1000;
    double MASK_ERROR = -0.01;
    vec<double> MASK_RATES;
    for(unsigned int l = 0; l < labels.size(); l++) {
        MASK_RATES.push_back(MASK_ERROR);
    }
    

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    double CORRECT_RATE = 10.0;
    double INCORRECT_RATE = 1.0;

    vec<double> TARGET_RATES = {10.0, 5.0};

    vec<double> spike_rates;
    vec<double> spike_history;

    for(i64 k = 0; k < labels.size(); k++) {
        spike_rates.push_back(0);
    }
        
    //-------------------------------------------------------------------------
    // Start the run


    network->RebuildDendrites();

    writer->StartRecording();

    output_layer->SetRecordData(false);

    for(sizet e = 0; e < num_epochs; e++) {

        std::cout << e << "\r" << std::flush;


        // Calculate Error
        // for(int i = 0; i < labels.size(); i++) {
        //     for(int j = 0; j < labels.size(); j++) {
        //         double rate = ( spike_rates[i][j]/ (num_test_iteration) );
        //         if(i==j) {
        //             w_error_rates[i][j] = (1.0 - rate/CORRECT_RATE);
        //             p_error_rates[i][j] = (1.0 - rate/CORRECT_RATE);
        //         } else {
        //             w_error_rates[i][j] = (rate >= INCORRECT_RATE) ? (INCORRECT_RATE/rate-1.0) : 1.0-rate;
        //             p_error_rates[i][j] = 0.0;//(1.0 - rate/CORRECT_RATE);
        //             //p_error_rates[i][j] = (rate >= 1) ? (INCORRECT_RATE/rate-1.0) : -rate;
        //         }
        //         // std::cout << i << " " << j << " " << rate << std::endl;
        //     }
        // }

        // //  Reset spike rates
        // for(int i = 0; i < labels.size(); i++) {
        //     for(int j = 0; j < labels.size(); j++) {
        //         spike_rates[i][j]=0;
        //     }
        // }
        

        std::shuffle(indexes.begin(), indexes.end(), rng);
        for(int i = 0; i < labels.size(); i++) {
            std::shuffle(test_data_by_id[i].begin(), test_data_by_id[i].end(), rng);
            std::shuffle(train_data_by_id[i].begin(), train_data_by_id[i].end(), rng);
        }
    
        // TRAINING SECTION
        {
            output_layer->SetRecordData(false);

            output_layer->SetTraining(
                false,
                false,
                true
            );

            network->RandomizeOrder(rng);

            for(sizet k = 0; k < num_train_iteration; k++) {
                // std::cout << "TRAIN " << k << "\r" << std::flush;

                

                int neuron = indexes[k%indexes.size()];
                int example = k/indexes.size();

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = train_data_by_id[labels[neuron]][example].label;

                

                ///////////////////////////////////////////////////////////////
                /// TRAINING RUN

                
                // ORIGINAL

                if(spike_rates[neuron] <= TARGET_RATES[neuron]) {
                    w_error_rates[neuron][0] = (1.0 - spike_rates[neuron]/TARGET_RATES[neuron]);
                    p_error_rates[neuron][0] = 1.0;//(1.0 - spike_rates[neuron]/TARGET_RATES[neuron]);
                } else {
                    w_error_rates[neuron][0] = (TARGET_RATES[neuron]/spike_rates[neuron]-1.0);
                    p_error_rates[neuron][0] = 1.0;//(TARGET_RATES[neuron]/spike_rates[neuron]-1.0);
                }

                // std::cout << w_error_rates[neuron][0] << " " << p_error_rates[neuron][0] << std::endl;

                network->UpdateLayerErrorValuesSeparate(
                    p_error_rates[neuron], w_error_rates[neuron], output_layer_index
                );

                // network->UpdateLayerErrorValuesSeparate(
                //     p_error_rates[neuron], w_error_rates[neuron], output_layer_index
                // );

                
                input_layer->SetInputs(train_data_by_id[labels[neuron]][example].image);

                time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }

                vec<int> spikes = output_layer->GetSpikesPerNeuron();
                // for(int s = 0; s < spikes.size(); s++) {
                //     // spike_rates[neuron][s] = spikes[s]; // ORIGINAL
                spike_rates[neuron] = spikes[0];
                // }

                network->Reset();
            }
        }

        // TESTING SECTION
        {

            // Don't train anything
            output_layer->SetTraining(
                false,
                false,
                false
            );

            // Start saving data
            output_layer->SetRecordData(true);

            network->RandomizeOrder(rng);

            network->RebuildDendrites();

            for(sizet k = 0; k < num_test_iteration; k++) {

                int neuron = indexes[k%indexes.size()];
                int example = k/indexes.size();
                unsigned int label = test_data_by_id[labels[neuron]][example].label;

                input_layer->SetInputs(test_data_by_id[labels[neuron]][example].image);

                i64 time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }

                

                writer->AddExampleData(std::make_unique<ExampleData>(neuron,label,label_desc[label]));
                network->SaveData(-1);
                network->WriteData(writer);
                network->Reset();
                
            }

        }

        

        // MASK
        {

            output_layer->SetRecordData(false);

            output_layer->SetTraining(
                false,
                true,
                false
            );

            input_layer->AddInputGenerator(ig_mask.get());
            
            network->UpdateLayerErrorValues(
                MASK_RATES, output_layer_index
            );


            i64 time = 1;
            for(; time <= mask_time; time++) {
                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            // Reset the input generator
            ig_mask->Reset();
            network->Reset();
            
            // Remove input generator. Only needed for mask.
            input_layer->AddInputGenerator(nullptr);
        }

    }

    std::cout << "ALL DONE\n" << std::flush;

    network->CleanUpData(writer);
    writer->StopRecording();

}





void RunFMNISTTriple(
    Writer * writer,
    Network * network,
    RNG & rng
) {
    std::string TRAIN_LABELS_FILENAME("exp_fmnist/train-labels-idx1-ubyte");
    std::string TRAIN_IMAGES_FILENAME("exp_fmnist/train-images-idx3-ubyte");
    std::string TEST_LABELS_FILENAME("exp_fmnist/t10k-labels-idx1-ubyte");
    std::string TEST_IMAGES_FILENAME("exp_fmnist/t10k-images-idx3-ubyte");

    vec<str> label_desc = {
        "t-shirt/top",
        "trouser",
        "pullover",
        "dress",
        "coat",
        "sandal",
        "shirt",
        "sneaker",
        "bag",
        "ankle boot",
    };

    vec<unsigned> labels = {0,1,5};

    std::cout << "START: LOADING FMNIST DATA\n";

    MNISTReader train_mnist;
    MNISTReader test_mnist;
    train_mnist.LoadData(TRAIN_LABELS_FILENAME,TRAIN_IMAGES_FILENAME,true);
    test_mnist.LoadData(TEST_LABELS_FILENAME,TEST_IMAGES_FILENAME,true);
    // vec<MNISTData> train_data;
    // vec<MNISTData> test_data;
    umap<int,vec<MNISTData>> test_data_by_id;
    umap<int,vec<MNISTData>> train_data_by_id;
    vec<int> indexes;
    for(int i = 0; i < labels.size(); i++) {
        indexes.push_back(i);
        vec<MNISTData> train_tmp = train_mnist.GetDataWithLabel(labels[i]);
        vec<MNISTData> test_tmp = test_mnist.GetDataWithLabel(labels[i]);
        test_data_by_id.emplace(labels[i],std::move(test_tmp));
        train_data_by_id.emplace(labels[i],std::move(train_tmp));
        // std::copy(train_tmp.begin(), train_tmp.end(), std::back_inserter(train_data));
        // std::copy(test_tmp.begin(), test_tmp.end(), std::back_inserter(test_data));
    }



    std::cout << "DONE: LOADING FMNIST DATA\n";
    
    sizet num_epochs = 1000;
    sizet num_train_iteration = 100; // don't make larger than num examples
    sizet num_test_iteration = 10*labels.size(); // don't make larger than num examples
    i64 time_per_example = 1000;
    i64 time;
    
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();
    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());

    vec<double> w_rate;
    vec<double> p_rate;
    for(int i = 0; i < labels.size(); i++) {
        w_rate.push_back(0);
        p_rate.push_back(1);
    }

    vec<vec<double>> p_error_rates;
    vec<vec<double>> w_error_rates;
    for(int i = 0; i < labels.size(); i++) {
        vec<double> v1;
        vec<double> v2;
        for(int j = 0; j < output_layer->GetLayerSize(); j++) {
            v1.push_back(0.0);
            v2.push_back(0.0);
        }
        p_error_rates.push_back(v1);
        w_error_rates.push_back(v2);
    }

    // vec<vec<double>> spike_history;
    // vec<double> prates;
    // for(int j = 0; j < labels.size(); j++) {
    //     vec<double> srh;
    //     for(i64 i = 0; i < output_layer->GetLayerSize(); i++) {
    //         srh.push_back(0.0);
    //     }
    //     prates.push_back(1.0);
    //     spike_history.push_back(std::move(srh));
    // }

    uptr<InputGenerator_Poisson> ig_mask = std::make_unique<InputGenerator_Poisson>();
    ig_mask->id = "0";
    for(i64 i = 0; i < input_layer_size; i++) {
        ig_mask->decay.push_back(std::exp(-1.0/10));
        ig_mask->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig_mask->strength.push_back(500.0);
        ig_mask->signal.push_back(0.0);
        ig_mask->rate.push_back(0.01);
    }

    

    i64 mask_time = 0;
    double MASK_ERROR = -0.1;
    vec<double> MASK_RATES;
    for(unsigned int l = 0; l < labels.size(); l++) {
        MASK_RATES.push_back(MASK_ERROR);
    }
    

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    double CORRECT_RATE = 10.0;
    double INCORRECT_RATE = 1.0;

    vec<double> TARGET_RATES = {5.0, 7.5, 10.0};

    vec<double> spike_rates;
    vec<double> spike_history;

    for(i64 k = 0; k < labels.size(); k++) {
        spike_rates.push_back(0);
    }
        
    //-------------------------------------------------------------------------
    // Start the run


    network->RebuildDendrites();

    writer->StartRecording();

    output_layer->SetRecordData(false);

    for(sizet e = 0; e < num_epochs; e++) {

        std::cout << e << "\r" << std::flush;


        // Calculate Error
        // for(int i = 0; i < labels.size(); i++) {
        //     for(int j = 0; j < labels.size(); j++) {
        //         double rate = ( spike_rates[i][j]/ (num_test_iteration) );
        //         if(i==j) {
        //             w_error_rates[i][j] = (1.0 - rate/CORRECT_RATE);
        //             p_error_rates[i][j] = (1.0 - rate/CORRECT_RATE);
        //         } else {
        //             w_error_rates[i][j] = (rate >= INCORRECT_RATE) ? (INCORRECT_RATE/rate-1.0) : 1.0-rate;
        //             p_error_rates[i][j] = 0.0;//(1.0 - rate/CORRECT_RATE);
        //             //p_error_rates[i][j] = (rate >= 1) ? (INCORRECT_RATE/rate-1.0) : -rate;
        //         }
        //         // std::cout << i << " " << j << " " << rate << std::endl;
        //     }
        // }

        // //  Reset spike rates
        // for(int i = 0; i < labels.size(); i++) {
        //     for(int j = 0; j < labels.size(); j++) {
        //         spike_rates[i][j]=0;
        //     }
        // }

        for(int i = 0; i < labels.size(); i++) {
            spike_rates[i] /= 10.0;
            // w_error_rates[i][0] = (1.0 - spike_rates[i]/TARGET_RATES[i]);
            // p_error_rates[i][0] = (1.0 - spike_rates[i]/TARGET_RATES[i]);
            if(spike_rates[i] <= TARGET_RATES[i]) {
                w_error_rates[i][0] = (1.0 - spike_rates[i]/TARGET_RATES[i]);
                p_error_rates[i][0] = 1.0;//(1.0 - spike_rates[i]/TARGET_RATES[i]);
            } else {
                w_error_rates[i][0] = (TARGET_RATES[i]/spike_rates[i]-1.0);
                p_error_rates[i][0] = 1.0;//(TARGET_RATES[i]/spike_rates[i]-1.0);
            }
        }

        for(int i = 0; i < labels.size(); i++) {
            spike_rates[i] = 0.0;
        }
        

        std::shuffle(indexes.begin(), indexes.end(), rng);
        for(int i = 0; i < labels.size(); i++) {
            std::shuffle(test_data_by_id[i].begin(), test_data_by_id[i].end(), rng);
            std::shuffle(train_data_by_id[i].begin(), train_data_by_id[i].end(), rng);
        }
    
        // TRAINING SECTION
        {
            output_layer->SetRecordData(false);

            output_layer->SetTraining(
                false,
                false,
                true
            );

            network->RandomizeOrder(rng);

            for(sizet k = 0; k < num_train_iteration; k++) {
                // std::cout << "TRAIN " << k << "\r" << std::flush;

                

                int neuron = indexes[k%indexes.size()];
                int example = k/indexes.size();

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = train_data_by_id[labels[neuron]][example].label;

                

                ///////////////////////////////////////////////////////////////
                /// TRAINING RUN

                
                // ORIGINAL

                //if(spike_rates[neuron] <= TARGET_RATES[neuron]) {
                    // w_error_rates[neuron][0] = (1.0 - spike_rates[neuron]/TARGET_RATES[neuron]);
                    // p_error_rates[neuron][0] = (1.0 - spike_rates[neuron]/TARGET_RATES[neuron]);
                // } else {
                //     w_error_rates[neuron][0] = (TARGET_RATES[neuron]/spike_rates[neuron]-1.0);
                //     p_error_rates[neuron][0] = 0.0;//(TARGET_RATES[neuron]/spike_rates[neuron]-1.0);
                // }

                // std::cout << w_error_rates[neuron][0] << " " << p_error_rates[neuron][0] << std::endl;

                network->UpdateLayerErrorValuesSeparate(
                    p_error_rates[neuron], w_error_rates[neuron], output_layer_index
                );

                // network->UpdateLayerErrorValuesSeparate(
                //     p_error_rates[neuron], w_error_rates[neuron], output_layer_index
                // );

                
                input_layer->SetInputs(train_data_by_id[labels[neuron]][example].image);

                time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }

                // vec<int> spikes = output_layer->GetSpikesPerNeuron();
                // // for(int s = 0; s < spikes.size(); s++) {
                // //     // spike_rates[neuron][s] = spikes[s]; // ORIGINAL
                // spike_rates[neuron] = spikes[0];
                // }

                network->Reset();
            }
        }

        // TESTING SECTION
        {

            // Don't train anything
            output_layer->SetTraining(
                false,
                false,
                false
            );

            // Start saving data
            output_layer->SetRecordData(true);

            network->RandomizeOrder(rng);

            network->RebuildDendrites();

            for(sizet k = 0; k < num_test_iteration; k++) {

                int neuron = indexes[k%indexes.size()];
                int example = k/indexes.size();
                unsigned int label = test_data_by_id[labels[neuron]][example].label;

                input_layer->SetInputs(test_data_by_id[labels[neuron]][example].image);

                i64 time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }

                vec<int> spikes = output_layer->GetSpikesPerNeuron();
                // for(int s = 0; s < spikes.size(); s++) {
                //     // spike_rates[neuron][s] = spikes[s]; // ORIGINAL
                spike_rates[neuron] += spikes[0];                

                writer->AddExampleData(std::make_unique<ExampleData>(neuron,label,label_desc[label]));
                network->SaveData(-1);
                network->WriteData(writer);
                network->Reset();
                
            }

        }

        

        // MASK
        {

            output_layer->SetRecordData(false);

            output_layer->SetTraining(
                false,
                true,
                false
            );

            input_layer->AddInputGenerator(ig_mask.get());
            
            network->UpdateLayerErrorValues(
                MASK_RATES, output_layer_index
            );


            i64 time = 1;
            for(; time <= mask_time; time++) {
                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            // Reset the input generator
            ig_mask->Reset();
            network->Reset();
            
            // Remove input generator. Only needed for mask.
            input_layer->AddInputGenerator(nullptr);
        }

    }

    std::cout << "ALL DONE\n" << std::flush;

    network->CleanUpData(writer);
    writer->StopRecording();

}




void RunFMNISTQuintuple(
    Writer * writer,
    Network * network,
    RNG & rng
) {
    std::string TRAIN_LABELS_FILENAME("exp_fmnist/train-labels-idx1-ubyte");
    std::string TRAIN_IMAGES_FILENAME("exp_fmnist/train-images-idx3-ubyte");
    std::string TEST_LABELS_FILENAME("exp_fmnist/t10k-labels-idx1-ubyte");
    std::string TEST_IMAGES_FILENAME("exp_fmnist/t10k-images-idx3-ubyte");

    vec<str> label_desc = {
        "t-shirt/top",
        "trouser",
        "pullover",
        "dress",
        "coat",
        "sandal",
        "shirt",
        "sneaker",
        "bag",
        "ankle boot",
    };

    vec<unsigned> labels = {0,5,1,3,8};
    vec<double> TARGET_RATES = {10.0, 5.0, 7.5, 12.5, 15};

    std::cout << "START: LOADING FMNIST DATA\n";

    MNISTReader train_mnist;
    MNISTReader test_mnist;
    train_mnist.LoadData(TRAIN_LABELS_FILENAME,TRAIN_IMAGES_FILENAME,true);
    test_mnist.LoadData(TEST_LABELS_FILENAME,TEST_IMAGES_FILENAME,true);
    // vec<MNISTData> train_data;
    // vec<MNISTData> test_data;
    umap<int,vec<MNISTData>> test_data_by_id;
    umap<int,vec<MNISTData>> train_data_by_id;
    vec<int> indexes;
    for(int i = 0; i < labels.size(); i++) {
        indexes.push_back(i);
        vec<MNISTData> train_tmp = train_mnist.GetDataWithLabel(labels[i]);
        vec<MNISTData> test_tmp = test_mnist.GetDataWithLabel(labels[i]);
        test_data_by_id.emplace(labels[i],std::move(test_tmp));
        train_data_by_id.emplace(labels[i],std::move(train_tmp));
        // std::copy(train_tmp.begin(), train_tmp.end(), std::back_inserter(train_data));
        // std::copy(test_tmp.begin(), test_tmp.end(), std::back_inserter(test_data));
    }



    std::cout << "DONE: LOADING FMNIST DATA\n";
    
    sizet num_epochs = 1000;
    sizet num_train_iteration = 100; // don't make larger than num examples
    sizet num_test_iteration = 10*labels.size(); // don't make larger than num examples
    i64 time_per_example = 1000;
    i64 time;
    
    i64 output_layer_index = network->GetOutputLayerIndex();
    Layer * input_layer = network->GetLayer(network->GetInputLayerIndex());
    i64 input_layer_size = input_layer->GetLayerSize();
    Layer * output_layer = network->GetLayer(network->GetOutputLayerIndex());

    vec<double> w_rate;
    vec<double> p_rate;
    for(int i = 0; i < labels.size(); i++) {
        w_rate.push_back(0);
        p_rate.push_back(1);
    }

    vec<vec<double>> p_error_rates;
    vec<vec<double>> w_error_rates;
    for(int i = 0; i < labels.size(); i++) {
        vec<double> v1;
        vec<double> v2;
        for(int j = 0; j < output_layer->GetLayerSize(); j++) {
            v1.push_back(0.0);
            v2.push_back(0.0);
        }
        p_error_rates.push_back(v1);
        w_error_rates.push_back(v2);
    }

    // vec<vec<double>> spike_history;
    // vec<double> prates;
    // for(int j = 0; j < labels.size(); j++) {
    //     vec<double> srh;
    //     for(i64 i = 0; i < output_layer->GetLayerSize(); i++) {
    //         srh.push_back(0.0);
    //     }
    //     prates.push_back(1.0);
    //     spike_history.push_back(std::move(srh));
    // }

    uptr<InputGenerator_Poisson> ig_mask = std::make_unique<InputGenerator_Poisson>();
    ig_mask->id = "0";
    for(i64 i = 0; i < input_layer_size; i++) {
        ig_mask->decay.push_back(std::exp(-1.0/10));
        ig_mask->dist = std::uniform_real_distribution<double>(0.0,1.0);
        ig_mask->strength.push_back(500.0);
        ig_mask->signal.push_back(0.0);
        ig_mask->rate.push_back(0.01);
    }

    

    i64 mask_time = 0;
    double MASK_ERROR = -0.1;
    vec<double> MASK_RATES;
    for(unsigned int l = 0; l < labels.size(); l++) {
        MASK_RATES.push_back(MASK_ERROR);
    }
    

    //-------------------------------------------------------------------------
    // Build the rates vector. Default to incorrect rates.
    double CORRECT_RATE = 10.0;
    double INCORRECT_RATE = 1.0;

    

    vec<double> spike_rates;
    vec<double> spike_history;

    for(i64 k = 0; k < labels.size(); k++) {
        spike_rates.push_back(0);
    }
        
    //-------------------------------------------------------------------------
    // Start the run


    network->RebuildDendrites();

    writer->StartRecording();

    output_layer->SetRecordData(false);

    for(sizet e = 0; e < num_epochs; e++) {

        std::cout << e << "\r" << std::flush;


        // Calculate Error
        // for(int i = 0; i < labels.size(); i++) {
        //     for(int j = 0; j < labels.size(); j++) {
        //         double rate = ( spike_rates[i][j]/ (num_test_iteration) );
        //         if(i==j) {
        //             w_error_rates[i][j] = (1.0 - rate/CORRECT_RATE);
        //             p_error_rates[i][j] = (1.0 - rate/CORRECT_RATE);
        //         } else {
        //             w_error_rates[i][j] = (rate >= INCORRECT_RATE) ? (INCORRECT_RATE/rate-1.0) : 1.0-rate;
        //             p_error_rates[i][j] = 0.0;//(1.0 - rate/CORRECT_RATE);
        //             //p_error_rates[i][j] = (rate >= 1) ? (INCORRECT_RATE/rate-1.0) : -rate;
        //         }
        //         // std::cout << i << " " << j << " " << rate << std::endl;
        //     }
        // }

        // //  Reset spike rates
        // for(int i = 0; i < labels.size(); i++) {
        //     for(int j = 0; j < labels.size(); j++) {
        //         spike_rates[i][j]=0;
        //     }
        // }

        for(int i = 0; i < labels.size(); i++) {
            spike_rates[i] /= 10.0;
            // w_error_rates[i][0] = (1.0 - spike_rates[i]/TARGET_RATES[i]);
            // p_error_rates[i][0] = (1.0 - spike_rates[i]/TARGET_RATES[i]);
            if(spike_rates[i] <= TARGET_RATES[i]) {
                w_error_rates[i][0] = (1.0 - spike_rates[i]/TARGET_RATES[i]);
                p_error_rates[i][0] = 1.0;//(1.0 - spike_rates[i]/TARGET_RATES[i]);
            } else {
                w_error_rates[i][0] = (TARGET_RATES[i]/spike_rates[i]-1.0);
                p_error_rates[i][0] = 1.0;//(TARGET_RATES[i]/spike_rates[i]-1.0);
            }
        }

        for(int i = 0; i < labels.size(); i++) {
            spike_rates[i] = 0.0;
        }
        

        std::shuffle(indexes.begin(), indexes.end(), rng);
        for(int i = 0; i < labels.size(); i++) {
            std::shuffle(test_data_by_id[i].begin(), test_data_by_id[i].end(), rng);
            std::shuffle(train_data_by_id[i].begin(), train_data_by_id[i].end(), rng);
        }
    
        // TRAINING SECTION
        {
            output_layer->SetRecordData(false);

            output_layer->SetTraining(
                true,
                true,
                true
            );

            network->RandomizeOrder(rng);

            for(sizet k = 0; k < num_train_iteration; k++) {
                // std::cout << "TRAIN " << k << "\r" << std::flush;

                

                int neuron = indexes[k%indexes.size()];
                int example = k/indexes.size();

                // Get the image
                // uptr<InputGenerator_Poisson> ig = train_mnist.GetDataAsPoissonInputGenerator(train_data[k]);
                unsigned int label = train_data_by_id[labels[neuron]][example].label;

                

                ///////////////////////////////////////////////////////////////
                /// TRAINING RUN

                
                // ORIGINAL

                //if(spike_rates[neuron] <= TARGET_RATES[neuron]) {
                    // w_error_rates[neuron][0] = (1.0 - spike_rates[neuron]/TARGET_RATES[neuron]);
                    // p_error_rates[neuron][0] = (1.0 - spike_rates[neuron]/TARGET_RATES[neuron]);
                // } else {
                //     w_error_rates[neuron][0] = (TARGET_RATES[neuron]/spike_rates[neuron]-1.0);
                //     p_error_rates[neuron][0] = 0.0;//(TARGET_RATES[neuron]/spike_rates[neuron]-1.0);
                // }

                // std::cout << w_error_rates[neuron][0] << " " << p_error_rates[neuron][0] << std::endl;

                network->UpdateLayerErrorValuesSeparate(
                    p_error_rates[neuron], w_error_rates[neuron], output_layer_index
                );

                // network->UpdateLayerErrorValuesSeparate(
                //     p_error_rates[neuron], w_error_rates[neuron], output_layer_index
                // );

                
                input_layer->SetInputs(train_data_by_id[labels[neuron]][example].image);

                time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }

                // vec<int> spikes = output_layer->GetSpikesPerNeuron();
                // // for(int s = 0; s < spikes.size(); s++) {
                // //     // spike_rates[neuron][s] = spikes[s]; // ORIGINAL
                // spike_rates[neuron] = spikes[0];
                // }

                network->Reset();
            }
        }

        // TESTING SECTION
        {

            // Don't train anything
            output_layer->SetTraining(
                false,
                false,
                false
            );

            // Start saving data
            output_layer->SetRecordData(true);

            network->RandomizeOrder(rng);

            network->RebuildDendrites();

            for(sizet k = 0; k < num_test_iteration; k++) {

                int neuron = indexes[k%indexes.size()];
                int example = k/indexes.size();
                unsigned int label = test_data_by_id[labels[neuron]][example].label;

                input_layer->SetInputs(test_data_by_id[labels[neuron]][example].image);

                i64 time = 1;
                for(; time <= time_per_example; time++) {
                    network->Update(
                        time,
                        writer,
                        rng
                    );
                }

                vec<int> spikes = output_layer->GetSpikesPerNeuron();
                // for(int s = 0; s < spikes.size(); s++) {
                //     // spike_rates[neuron][s] = spikes[s]; // ORIGINAL
                spike_rates[neuron] += spikes[0];                

                writer->AddExampleData(std::make_unique<ExampleData>(neuron,label,label_desc[label]));
                network->SaveData(-1);
                network->WriteData(writer);
                network->Reset();
                
            }

        }

        

        // MASK
        {

            output_layer->SetRecordData(false);

            output_layer->SetTraining(
                false,
                true,
                false
            );

            input_layer->AddInputGenerator(ig_mask.get());
            
            network->UpdateLayerErrorValues(
                MASK_RATES, output_layer_index
            );


            i64 time = 1;
            for(; time <= mask_time; time++) {
                network->Update(
                    time,
                    writer,
                    rng
                );
            }

            // Reset the input generator
            ig_mask->Reset();
            network->Reset();
            
            // Remove input generator. Only needed for mask.
            input_layer->AddInputGenerator(nullptr);
        }

    }

    std::cout << "ALL DONE\n" << std::flush;

    network->CleanUpData(writer);
    writer->StopRecording();

}