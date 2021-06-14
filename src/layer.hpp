#pragma once

#include"zxlb.hpp"
#include"neuron.hpp"

class Layer {
private:
    i64 id;
    str name;
    vec<Neuron> neurons;
    bool is_input;
    bool is_output;
public:
    Layer() = default;
    Layer(const Layer & l) = delete;
    Layer(Layer && l) = default;
    Layer& operator=(const Layer & l) = delete;
    Layer& operator=(Layer && l) = default;
    //-------------------------------------------------------------------------
    void SetID(i64 _id);
    i64 GetID();
    void SetName(str _name);
    str GetName();
    bool IsInput();
    bool IsOutput();
    void SetInput(bool b);
    void SetOutput(bool b);
    i64 GetLayerSize();

    void AddNeuron(Neuron neuron);
    Neuron * GetNeuron(i64 index);
    void UpdateNeurons(i64 time, Writer * writer);
};