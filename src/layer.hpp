#pragma once

#include"zxlb.hpp"
#include"log.hpp"
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
    void Reset();
    void SetID(i64 _id);
    i64 GetID();
    void SetName(str _name);
    str GetName();
    bool IsInput();
    bool IsOutput();
    void SetIsInput(bool b);
    void SetIsOutput(bool b);
    i64 GetLayerSize();

    void AddNeuron(Neuron neuron);
    Neuron * GetNeuron(i64 index);
    void Update(i64 time, Writer * writer, ConnectionMatrix & cm);

    void SetInputs(vec<double> & inputs);

    void RebuildDendrites();

    void CleanUp(Writer * writer);
    void InitWriteData();
    void WriteData(Writer * writer);

};