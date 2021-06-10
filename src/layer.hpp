#pragma once

#include"zxlb.hpp"
#include"neuron.hpp"

class Layer {
private:
    i64 id;
    vec<Neuron> neurons;
public:
    Layer() = default;
    Layer(const Layer & l) = delete;
    Layer(Layer && l) = default;
    Layer& operator=(const Layer & l) = delete;
    Layer& operator=(Layer && l) = default;
    //-------------------------------------------------------------------------
    void SetID(i64 _id);
    i64 GetID();

    void AddNeuron(Neuron neuron);
    Neuron * GetNeuron(i64 index);
    void UpdateNeurons(i64 time, Writer * writer);
};