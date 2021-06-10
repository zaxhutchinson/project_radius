#pragma once

#include"zxlb.hpp"
#include"layer.hpp"

class Network {
private:
    vec<Layer> layers;
    vec<sizet> input_layers;
    vec<sizet> output_layers;
public:
    Network();
    Network(const Network & n) = default;
    Network(Network && n) = default;
    Network& operator=(const Network & n) = default;
    Network& operator=(Network && n) = default;
    
    //------------------------------------------------------------------------
    void AddLayer(Layer layer, bool is_input, bool is_output);
    Layer * GetLayer(i64 index);
};

