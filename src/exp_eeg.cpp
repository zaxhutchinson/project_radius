#include"exp_eeg.hpp"



vec<double> EEG::GetReading(sizet index) {
    return readings[index];
}






ExpEEG::ExpEEG() {}

void ExpEEG::LoadData(str fn_train_path, str fn_test_path) {
    std::cout << "LOADING DATA\n";

    std::filesystem::path train_path = fn_train_path;
    std::filesystem::path test_path = fn_test_path;

    std::ifstream ifs;

    // vec<int> as;
    // vec<int> cs;
    // for(int i = 0; i < MAX_READING*2; i++) {
    //     as.push_back(0);
    //     cs.push_back(0);
    // }

    // double min_a = std::numeric_limits<double>::max();
    // double max_a = std::numeric_limits<double>::min();
    // double min_c = std::numeric_limits<double>::max();
    // double max_c = std::numeric_limits<double>::min();
    // double avg_c = 0;
    // double avg_a = 0;
    // int num_over_a = 0;
    // int num_over_c = 0;
    // int total_a = 0;
    // int total_c = 0;

    // double OVER_VAL = -25.0;

    

    for(auto const& dir_entry: std::filesystem::directory_iterator{train_path}) {

        if(dir_entry.is_regular_file()) {
            ifs.open(dir_entry.path().string());
            if(ifs) {

                
                
                EEG eeg;

                /*-------------------------------------------------------------
                    READ HEADER
                -------------------------------------------------------------*/
                str line1, line2, line3, line4;
                std::getline(ifs, line1);
                std::getline(ifs, line2);
                std::getline(ifs, line3);
                std::getline(ifs, line4);

                
                // Get ac from line 1
                eeg.ac = line1[5];
                eeg.id = std::stoi(line1.substr(10,3));
                eeg.trial = std::stoi(dir_entry.path().string().substr(dir_entry.path().string().size()-3));

                // Don't need lines 2 & 3

                std::istringstream iss;
                iss.str(line4);
                vec<str> elements;
                for(str e; std::getline(iss, e, ','); ) {
                    elements.push_back(e);
                }

                eeg.trial_type = elements[0].substr(2);

                // std::cout << eeg.ac << "-" << eeg.id << "-" << eeg.trial
                //             << "-" << eeg.trial_type << " "; 

                /*-------------------------------------------------------------
                    READ DATA
                -------------------------------------------------------------*/

                for(sizet i= 0; i < NUM_READINGS; i++) {
                    vec<double> v;
                    for(sizet k = 0; k < NUM_CHANNELS; k++) {
                        v.push_back(0.0);
                    }
                    eeg.readings.push_back(std::move(v));
                }

                str line5;
                std::getline(ifs, line5); // Consume the first channel header.

                sizet channel_index = 0;
                sizet reading_index = 0;

                vec<double> vals;


                for(str line; std::getline(ifs, line);) {

                    // If new channel...
                    if(line[0]=='#') {
                        channel_index++;
                        reading_index = 0;
                        continue;
                    }

                    vec<str> chan_eles;
                    std::istringstream chan_iss;
                    chan_iss.str(line);
                    for(str e; std::getline(chan_iss, e, ' '); ) {
                        chan_eles.push_back(e);
                    }

                    double reading = std::min(std::max(std::stod(chan_eles[3]),-MIN_READING),MAX_READING);

                    // if(eeg.ac=="a") as[static_cast<int>(reading)+MIN_READING]++;
                    // if(eeg.ac=="c") cs[static_cast<int>(reading)+MIN_READING]++;
                    //double reading = std::stod(chan_eles[3]);
                    reading = (READING_SCALAR * ((reading + MIN_READING) / MINMAX_READING));
                    
                    if(reading > READING_SCALAR || reading < 0) {
                        std::cout << reading << std::endl;
                    }

                    eeg.readings[reading_index][channel_index] = reading;
                    reading_index++;


                    // if(eeg.ac=="a") {
                    //     if(std::stod(chan_eles[3]) < min_a) min_a = std::stod(chan_eles[3]);
                    //     if(std::stod(chan_eles[3]) > max_a) max_a = std::stod(chan_eles[3]);
                    //     if(std::stod(chan_eles[3]) < OVER_VAL) num_over_a++;
                    //     avg_a += reading;
                    //     total_a++;
                    // }
                    // else if(eeg.ac=="c") {
                    //     if(std::stod(chan_eles[3]) < min_c) min_c = std::stod(chan_eles[3]);
                    //     if(std::stod(chan_eles[3]) > max_c) max_c = std::stod(chan_eles[3]);
                    //     if(std::stod(chan_eles[3]) < OVER_VAL) num_over_c++;
                    //     avg_c+=reading;
                    //     total_c++;
                    // }
                    
                
                }
                
                // for(sizet i= 0; i < NUM_READINGS; i++) {
                //     for(sizet k = 0; k < NUM_CHANNELS; k++) {
                //         std::cout << eeg.readings[i][k] << " ";
                //     }
                //     std::cout << std::endl;
                // }
                
                train_eegs.push_back(std::move(eeg));

            }
            ifs.close();
        }

    }

    // std::cout << "TRAINING: \n";
    // std::cout << "MaxC: " << max_c << std::endl;
    // std::cout << "MinC: " << min_c << std::endl;
    // std::cout << "MaxA: " << max_a << std::endl;
    // std::cout << "MinA: " << min_a << std::endl;
    // std::cout << "OvrA: " << num_over_a/static_cast<double>(total_a) << std::endl;
    // std::cout << "OvrC: " << num_over_c/static_cast<double>(total_c) << std::endl;
    // std::cout << "AvgC: " << avg_c / total_c << std::endl;
    // std::cout << "AvgA: " << avg_a / total_a << std::endl;

    // for(int i = 0; i < MAX_READING*2; i++) {
    //     std::cout << i << "  " << as[i] << "  " << cs[i] << std::endl;
    // }

    // min_a = std::numeric_limits<double>::max();
    // max_a = std::numeric_limits<double>::min();
    // min_c = std::numeric_limits<double>::max();
    // max_c = std::numeric_limits<double>::min();
    // num_over_a = 0;
    // num_over_c = 0;
    // total_a = 0;
    // total_c = 0;
    // avg_c = 0;
    // avg_a = 0;
    
    for(auto const& dir_entry: std::filesystem::directory_iterator{test_path}) {

        if(dir_entry.is_regular_file()) {
            ifs.open(dir_entry.path().string());
            if(ifs) {

                
                
                EEG eeg;

                /*-------------------------------------------------------------
                    READ HEADER
                -------------------------------------------------------------*/
                str line1, line2, line3, line4;
                std::getline(ifs, line1);
                std::getline(ifs, line2);
                std::getline(ifs, line3);
                std::getline(ifs, line4);
                
                
                // Get ac from line 1
                eeg.ac = line1[5];
                eeg.id = std::stoi(line1.substr(10,3));
                eeg.trial = std::stoi(dir_entry.path().string().substr(dir_entry.path().string().size()-3));

                // Don't need lines 2 & 3

                std::istringstream iss;
                iss.str(line4);
                vec<str> elements;
                for(str e; std::getline(iss, e, ','); ) {
                    elements.push_back(e);
                }

                eeg.trial_type = elements[0].substr(2);

                // std::cout << eeg.ac << "-" << eeg.id << "-" << eeg.trial
                //             << "-" << eeg.trial_type << std::endl; 

                /*-------------------------------------------------------------
                    READ DATA
                -------------------------------------------------------------*/

                for(sizet i= 0; i < NUM_READINGS; i++) {
                    vec<double> v;
                    for(sizet k = 0; k < NUM_CHANNELS; k++) {
                        v.push_back(0.0);
                    }
                    eeg.readings.push_back(std::move(v));
                }

                str line5;
                std::getline(ifs, line5); // Consume the first channel header.

                sizet channel_index = 0;
                sizet reading_index = 0;

                for(str line; std::getline(ifs, line);) {

                    // If new channel...
                    if(line[0]=='#') {
                        channel_index++;
                        reading_index = 0;
                        continue;
                    }

                    vec<str> chan_eles;
                    std::istringstream chan_iss;
                    chan_iss.str(line);
                    for(str e; std::getline(chan_iss, e, ' '); ) {
                        chan_eles.push_back(e);
                    }

                    double reading = std::min(std::max(std::stod(chan_eles[3]),-MIN_READING),MAX_READING);
                    //double reading = std::stod(chan_eles[3]);
                    reading = (READING_SCALAR * ((reading + MIN_READING) / MINMAX_READING));
                    
                    if(reading > READING_SCALAR || reading < 0) {
                        std::cout << reading << std::endl;
                    }

                    eeg.readings[reading_index][channel_index] = reading;
                    reading_index++;

                    // if(eeg.ac=="a") {
                    //     if(std::stod(chan_eles[3]) < min_a) min_a = std::stod(chan_eles[3]);
                    //     if(std::stod(chan_eles[3]) > max_a) max_a = std::stod(chan_eles[3]);
                    //     if(std::stod(chan_eles[3]) < OVER_VAL) num_over_a++;
                    //     avg_a+=reading;
                    //     total_a++;
                    // }
                    // else if(eeg.ac=="c") {
                    //     if(std::stod(chan_eles[3]) < min_c) min_c = std::stod(chan_eles[3]);
                    //     if(std::stod(chan_eles[3]) > max_c) max_c = std::stod(chan_eles[3]);
                    //     if(std::stod(chan_eles[3]) < OVER_VAL) num_over_c++;
                    //     avg_c+=reading;
                    //     total_c++;
                    // }
                }
                
                test_eegs.push_back(std::move(eeg));

            }
            ifs.close();
        }

    }

    // std::cout << "TESTING: \n";
    // std::cout << "MaxC: " << max_c << std::endl;
    // std::cout << "MinC: " << min_c << std::endl;
    // std::cout << "MaxA: " << max_a << std::endl;
    // std::cout << "MinA: " << min_a << std::endl;
    // std::cout << "OvrA: " << num_over_a/static_cast<double>(total_a) << std::endl;
    // std::cout << "OvrC: " << num_over_c/static_cast<double>(total_c) << std::endl;
    // std::cout << "AvgC: " << avg_c / total_c << std::endl;
    // std::cout << "AvgA: " << avg_a / total_a << std::endl;

    std::cout << "FINISHED LOADING DATA\n";
}


sizet ExpEEG::GetSizeTrainingExamples() {
    return train_eegs.size();
}
sizet ExpEEG::GetSizeTestExamples() {
    return test_eegs.size();
}

EEG * ExpEEG::GetTrainingExample(sizet index) {
    return &(train_eegs[index]);
}
EEG * ExpEEG::GetTestingExample(sizet index) {
    return &(test_eegs[index]);
}