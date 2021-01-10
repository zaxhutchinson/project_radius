#pragma once

#include"zxlb.hpp"
#include"vec3.hpp"
#include"data.hpp"
#include"writer.hpp"
#include"synapse.hpp"
#include"neuron_template.hpp"


struct Neuron {
    i64 id;
    VecS location;
    NeuronTemplate nt;
    double vpre;
    double vcur;
    double upre;
    double ucur;
    lst<i64> spike_times_live;
    vec<i64> spike_times_data;
    vsptr<Synapse> dendrite;
    vsptr<Synapse> axon;
    double output;
    double baseline_input;
    bool record_data;
    std::size_t record_interval;
    std::size_t record_data_size;
    uptr<NeuronData> data;

    Neuron(
        i64 _id,
        VecS _loc,
        const NeuronTemplate & _nt
    );

    void Update(i64 time, Writer * writer);
    void ResetWriteData();
    void WriteData(i64 time, Writer * writer);
    void CleanupData(Writer * writer);

};