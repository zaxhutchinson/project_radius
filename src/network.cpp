#include"network.hpp"

i64 Network::AddLayer(Layer layer) {
    i64 id = static_cast<i64>(layers.size());
    // Set the layer's id.
    layer.SetID(id);
    // Add to vec
    layers.push_back(std::move(layer));
    if(layer.IsInput()) {
        input_layers.push_back(id);
    }
    if(layer.IsOutput()) {
        output_layers.push_back(id);
    }

    return id;
}

Layer * Network::GetLayer(i64 index) {
    if(index < static_cast<i64>(layers.size())) {
        return &(layers[index]);
    } else {
        return nullptr;
    }
}