#pragma once

#include"zxlb.hpp"
#include"layer.hpp"

class Network {
private:
    vec<Layer> layers;
    vec<sizet> input_layers;
    vec<sizet> output_layers;
public:
    Network() = default;
    Network(const Network & n) = default;
    Network(Network && n) = default;
    Network& operator=(const Network & n) = default;
    Network& operator=(Network && n) = default;
    
    //------------------------------------------------------------------------
    /* AddLayer:
        Adds a layer to the network. Sets its id based on its place in the
        vector. Returns the new layer's id.
    */
    i64 AddLayer(Layer layer);
    Layer * GetLayer(i64 index);
};

