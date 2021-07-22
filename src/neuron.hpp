#pragma once

#include<iostream>
#include<utility>
#include<functional>
#include<omp.h>

#include"zxlb.hpp"
#include"vec3.hpp"
#include"data.hpp"
#include"writer.hpp"
#include"synapse.hpp"
#include"neuron_type.hpp"

struct Neuron {
    i64 id;
    i64 layer_id;
    VecS location;
    tmps::NeuronType nt;
    double vpre;
    double vcur;
    double upre;
    double ucur;
    i64 time_cur_spike;
    i64 time_pre_spike;
    lst<i64> spike_times_live;
    vec<i64> spike_times_data;
    vec<Synapse> synapses;
    vec<sizet> synapse_indexes;
    vec<i64> dendrites;
    double baseline;
    double raw_input;
    bool record_data;
    std::size_t record_interval;
    std::size_t record_data_size;
    uptr<NeuronData> data;
    bool just_spiked;
    double output;
    double input;

    Neuron();
    Neuron(
        VecS _loc,
        const tmps::NeuronType & _nt
    );
    Neuron(const Neuron & n) = delete;
    Neuron(Neuron && n) = default;
    Neuron& operator=(const Neuron & n) = delete;
    Neuron& operator=(Neuron && n) = default;
    //------------------------------------------------------------------------

    //------------------------------------------------------------------------
    void Reset();

    i64 GetID();
    void SetID(i64 _id);
    void SetLayerID(i64 _id);

    i64 AddSynapse(Synapse synapse);
    Synapse * GetSynapse(i64 index);

    void SetBaseline(double amt);
    void SetRawInput(double amt);
    // void PresynapticSignal(i64 time, i64 synapse_id, double signal, bool pre_spike);
    void PresynapticSpike(i64 time, i64 synapse_id, ConnectionMatrix & cm);
    void PostsynapticSignal(i64 time, ConnectionMatrix & cm);
    void bAP(i64 time, i64 synapse_id, double amt);
   
    bool Update(i64 time, Writer * writer, i64 layer_id, ConnectionMatrix & cm);
    void InitWriteData();
    void ResetWriteData();
    void SaveData(i64 time, ConnectionMatrix & cm);
    void WriteData(Writer * writer);
    void CleanupData(Writer * writer);

    void BuildDendrite();
    void BuildDendriteParallel();

    void PrintDendrite();

private:
     /* GetInput:
        Pulls synaptic input from the entire dendritic tree(s).
        Iterative solution used because synapses can't talk
        to each other.
    */
    void GetInput(i64 time);
    void GetInputSimple(i64 time);

};