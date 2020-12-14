#pragma once


#include"zxlb.hpp"
#include"vec3.hpp"
#include"writer.hpp"
#include"data.hpp"

struct Synapse {

    i64 id;
    double input[2];
    double max_strength;  
    double cur_strength;
    Vec3 location;
    double error;
    i64 time_error;
    i64 time_cur_spike;
    i64 time_pre_spike;
    i64 spikes;
    Synapse * parent;
    vec<Synapse*> children;
    bool record_data;
    i64 record_interval;
    i64 record_data_size;
    uptr<SynapseData> data;

    Synapse(
        i64 _id,
        Vec3 _loc,
        double _max_strength,
        double _cur_strength
    );

    void Update(i64 time, Writer * writer);
    void ResetWriteData();
    void WriteData(i64 time, Writer * writer);
    void CleanupData(Writer * writer);
};