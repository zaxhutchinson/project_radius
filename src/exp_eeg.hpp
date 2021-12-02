#pragma once

#include<iostream>
#include<fstream>
#include<filesystem>
#include<sstream>

#include"zxlb.hpp"

inline const sizet NUM_CHANNELS = 64;
inline const sizet NUM_READINGS = 256;

inline const double MAX_READING = 20.0;//453.847;
inline const double MIN_READING = 20.0;//-(-213.491);
inline const double MINMAX_READING = MIN_READING + MAX_READING;

inline const double READING_SCALAR = 0.1;

struct EEG {
    vec<vec<double>> readings;
    str ac;
    str trial_type;
    i32 id;
    i32 trial;

    vec<double> GetReading(sizet index);
};

struct ExpEEG {

    vec<EEG> train_eegs;
    vec<EEG> test_eegs;

    ExpEEG();

    void LoadData(str fn_train, str fn_test);
    sizet GetSizeTrainingExamples();
    sizet GetSizeTestExamples();
    EEG * GetTrainingExample(sizet index);
    EEG * GetTestingExample(sizet index);

};