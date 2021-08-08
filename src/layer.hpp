#pragma once

#include<omp.h>

#include"zxlb.hpp"
#include"neuron.hpp"
#include"input_gen.hpp"

class Layer {
private:
    i64 id;
    str name;
    vec<Neuron> neurons;
    vec<sizet> neuron_indexes;
    bool is_input;
    bool is_output;
    InputGenerator * input_generator;
public:
    Layer();
    Layer(const Layer & l) = delete;
    Layer(Layer && l) = default;
    Layer& operator=(const Layer & l) = delete;
    Layer& operator=(Layer && l) = default;
    //-------------------------------------------------------------------------
    void Reset();
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

    void AddInputGenerator(InputGenerator * ig);

    void AddNeuron(Neuron neuron);
    Neuron * GetNeuron(i64 index);
    void Update(i64 time, Writer * writer, ConnectionMatrix & cm, RNG & rng);

    void SetInputs(vec<double> & inputs);

    void RebuildDendrites();

    void CleanUp(Writer * writer);
    void InitWriteData();
    void SaveData(i64 time, ConnectionMatrix & cm);
    void WriteData(Writer * writer);

};