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
    bool just_spiked;
    i64 time_cur_spike;
    i64 time_pre_spike;
    lst<i64> spike_times_live;
    vec<i64> spike_times_data;
    vec<Synapse> synapses;
    vec<i64> dendrites;
    vec<pair<i64,i64>> axons;
    double output;
    double baseline;
    double raw_input;
    bool record_data;
    std::size_t record_interval;
    std::size_t record_data_size;
    uptr<NeuronData> data;

    Neuron(
        i64 _id,
        VecS _loc,
        const NeuronTemplate & _nt
    );

    void SetBaseline(double amt);
    void SetRawInput(double amt);
    void PresynapticSignal(i64 time, i64 synapse_id, bool pre_spike);
    void PresynapticSpike(i64 time, i64 synapse_id);
    void PostsynapticSignal(i64 time, double error);
    void bAP(i64 time, i64 synapse_id, double amt, double error);

    void Update(i64 time, Writer * writer);
    void ResetWriteData();
    void WriteData(i64 time, Writer * writer);
    void CleanupData(Writer * writer);

};