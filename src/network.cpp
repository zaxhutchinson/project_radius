#include"network.hpp"

void Network::AddLayer(Layer layer, bool is_input, bool is_output) {
    i64 id = static_cast<i64>(layers.size());
    // Set the layer's id.
    layer.SetID(id);
    // Add to vec
    layers.push_back(std::move(layer));
    if(is_input) {
        input_layers.push_back(id);
    }
    if(is_output) {
        output_layers.push_back(id);
    }
}

Layer * Network::GetLayer(i64 index) {
    if(index < static_cast<i64>(layers.size())) {
        return &(layers[index]);
    } else {
        return nullptr;
    }
}