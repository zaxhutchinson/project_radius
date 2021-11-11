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

    vec<double> as;
    vec<double> cs;

    double min_a = std::numeric_limits<double>::max();
    double max_a = std::numeric_limits<double>::min();
    double min_c = std::numeric_limits<double>::max();
    double max_c = std::numeric_limits<double>::min();

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

                    double reading = READING_SCALAR * ((std::stod(chan_eles[3]) + MIN_READING) / MINMAX_READING);
                    if(reading > READING_SCALAR || reading < 0.0) {
                        std::cout << reading << std::endl;
                    }

                    eeg.readings[reading_index][channel_index] = reading;
                    reading_index++;


                    if(eeg.ac=="a") {
                        if(std::stod(chan_eles[3]) < min_a) min_a = std::stod(chan_eles[3]);
                        if(std::stod(chan_eles[3]) > max_a) max_a = std::stod(chan_eles[3]);
                    }
                    else if(eeg.ac=="c") {
                        if(std::stod(chan_eles[3]) < min_c) min_c = std::stod(chan_eles[3]);
                        if(std::stod(chan_eles[3]) > max_c) max_c = std::stod(chan_eles[3]);
                    }
                
                }
                
                
                train_eegs.push_back(std::move(eeg));

            }
            ifs.close();
        }

    }

    std::cout << "TRAINING: \n";
    std::cout << "MaxC: " << max_c << std::endl;
    std::cout << "MinC: " << min_c << std::endl;
    std::cout << "MaxA: " << max_a << std::endl;
    std::cout << "MinA: " << min_a << std::endl;

    min_a = std::numeric_limits<double>::max();
    max_a = std::numeric_limits<double>::min();
    min_c = std::numeric_limits<double>::max();
    max_c = std::numeric_limits<double>::min();

    
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

                    double reading = READING_SCALAR * ((std::stod(chan_eles[3]) + MIN_READING) / MINMAX_READING);
                    if(reading > READING_SCALAR || reading < 0.) {
                        std::cout << reading << std::endl;
                    }

                    eeg.readings[reading_index][channel_index] = reading;
                    reading_index++;

                    if(eeg.ac=="a") {
                        if(std::stod(chan_eles[3]) < min_a) min_a = std::stod(chan_eles[3]);
                        if(std::stod(chan_eles[3]) > max_a) max_a = std::stod(chan_eles[3]);
                    }
                    else if(eeg.ac=="c") {
                        if(std::stod(chan_eles[3]) < min_c) min_c = std::stod(chan_eles[3]);
                        if(std::stod(chan_eles[3]) > max_c) max_c = std::stod(chan_eles[3]);
                    }
                }
                
                test_eegs.push_back(std::move(eeg));

            }
            ifs.close();
        }

    }

    std::cout << "TRAINING: \n";
    std::cout << "MaxC: " << max_c << std::endl;
    std::cout << "MinC: " << min_c << std::endl;
    std::cout << "MaxA: " << max_a << std::endl;
    std::cout << "MinA: " << min_a << std::endl;

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