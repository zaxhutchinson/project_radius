#pragma once

#include"zxlb.hpp"
#include"vec3.hpp"
#include"data.hpp"
#include"writer.hpp"
#include"synapse.hpp"
#include"neuron_template.hpp"


struct Neuron {
    i64 id;
    Vec3 location;
    NeuronTemplate nt;
    double vpre;
    double vcur;
    double upre;
    double ucur;
    lst<i64> spike_times;
    vsptr<Synapse> dendrite;
    vsptr<Synapse> axon;
    double output;
    double baseline_input;
    bool record_data;
    i64 record_interval;
    i64 record_data_size;
    uptr<NeuronData> data;

    Neuron(
        i64 _id,
        Vec3 _loc,
        const NeuronTemplate & _nt
    );

    void Update(i64 time, Writer * writer);
    void ResetWriteData();
    void WriteData(i64 time, Writer * writer);
    void CleanupData(Writer * writer);

};