#pragma once

#include"zxlb.hpp"
#include"vec3.hpp"
#include"data.hpp"
#include"writer.hpp"
#include"synapse.hpp"
#include"axon.hpp"
#include"neuron_template.hpp"


struct Neuron {
    i64 id;
    VecS location;
    tmps::NeuronTemplate nt;
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
    vec<Axon> axons;
    double output;
    double baseline;
    double raw_input;
    bool record_data;
    std::size_t record_interval;
    std::size_t record_data_size;
    uptr<NeuronData> data;

    Neuron();
    Neuron(
        VecS _loc,
        const tmps::NeuronTemplate & _nt
    );
    Neuron(const Neuron & n) = delete;
    Neuron(Neuron && n) = default;
    Neuron& operator=(const Neuron & n) = delete;
    Neuron& operator=(Neuron && n) = default;
    //------------------------------------------------------------------------

    i64 GetID();
    void SetID(i64 _id);

    i64 AddSynapse(Synapse synapse);
    Synapse * GetSynapse(i64 index);
    void AddAxon(Axon axon);

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

private:
     /* GetInput:
        Pulls synaptic input from the entire dendritic tree(s).
        Iterative solution used because synapses can't talk
        to each other.
    */
    double GetInput(i64 time);

};