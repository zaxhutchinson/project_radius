#pragma once

#include<omp.h>

#include"zxlb.hpp"
#include"neuron.hpp"
#include"input_gen.hpp"
#include"loader.hpp"

class Layer {
private:
    i64 id;
    str name;
    vec<Neuron> neurons;
    vec<int> spikes_per_neuron;
    vec<sizet> neuron_indexes;
    bool is_input;
    bool is_output;
    InputGenerator * input_generator;
    bool train_rad;
    bool train_ang;
    bool train_str;
public:
    Layer();
    Layer(const Layer & l) = delete;
    Layer(Layer && l);
    Layer& operator=(const Layer & l) = delete;
    Layer& operator=(Layer && l);
    //-------------------------------------------------------------------------
    void LoadPresets(LayData & laydata);
    void Reset(bool purge_data = false);
    void RandomizeNeuronOrder(RNG & rng);
    void SetID(i64 _id);
    i64 GetID();
    void SetName(str _name);
    str GetName();
    bool IsInput();
    bool IsOutput();
    void SetIsInput(bool b);
    void SetIsOutput(bool b);
    i64 GetLayerSize();

    vec<int> & GetSpikesPerNeuron();

    void SetTraining(bool rad, bool ang, bool str);

    void AddInputGenerator(InputGenerator * ig);

    void AddNeuron(Neuron neuron);
    Neuron * GetNeuron(i64 index);
    void Update(i64 time, Writer * writer, ConnectionMatrix & cm, RNG & rng);

    void SetInputs(vec<double> & inputs);

    void InitDendrites();
    void RebuildDendrites();

    void SetRecordData(bool b);
    void CleanUp(Writer * writer);
    void InitWriteData();
    void SaveData(i64 time, ConnectionMatrix & cm);
    void WriteData(Writer * writer);

};