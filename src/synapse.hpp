#pragma once


#include"zxlb.hpp"
#include"vec_s.hpp"
#include"writer.hpp"
#include"data.hpp"

struct Synapse {

    int id;
    double signal[2];
    double bap[2];
    double max_strength;  
    double cur_strength;
    VecS location;
    double dendrite_path_length;
    double error;
    i64 time_error;
    i64 time_cur_spike;
    i64 time_pre_spike;
    i64 spikes;
    int parent;
    vec<int> children;
    bool record_data;
    std::size_t record_interval;
    std::size_t record_data_size;
    uptr<SynapseData> data;

    Synapse(
        int _id,
        VecS _loc,
        double _max_strength,
        double _cur_strength
    );
    void SetSignal(i64 time, double amt);
    void SetCurSpike(i64 time);

    double GetSignal(i64 time);

    void Update(i64 time, Writer * writer);
    void ResetWriteData();
    void WriteData(i64 time, Writer * writer);
    void CleanupData(Writer * writer);
    
};