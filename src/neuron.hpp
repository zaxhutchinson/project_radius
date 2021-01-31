#pragma once

#include"zxlb.hpp"
#include"vec3.hpp"
#include"data.hpp"
#include"writer.hpp"
#include"synapse.hpp"
#include"neuron_template.hpp"


struct Neuron {
    int id;
    VecS location;
    NeuronTemplate nt;
    double vpre;
    double vcur;
    double upre;
    double ucur;
    bool just_spiked;
    i64 time_cur_spike;
    i64 time_pre_spike;
    lst<i64> spike_times_live;
    vec<i64> spike_times_data;
    vec<Synapse> synapses;
    vec<int> dendrites;
    vec<pair<int,int>> axons;
    double output;
    double baseline;
    double raw_input;
    bool record_data;
    std::size_t record_interval;
    std::size_t record_data_size;
    uptr<NeuronData> data;

    Neuron(
        int _id,
        VecS _loc,
        const NeuronTemplate & _nt
    );

    void SetBaseline(double amt);
    void SetRawInput(double amt);
    void PresynapticInput(i64 time, int synapse_id, bool pre_spike);
    void PresynapticSpike(i64 time, int synapse_id);
    void PostsynapticInput(i64 time, double error);
    void bAP(i64 time, int synapse_id, double amt, double error);

    void Update(i64 time, Writer * writer);
    void ResetWriteData();
    void WriteData(i64 time, Writer * writer);
    void CleanupData(Writer * writer);

};