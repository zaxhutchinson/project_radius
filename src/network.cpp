#include"network.hpp"


void Network::LoadPresets(NetData & netdata) {

    for(
        umap<i64,LayData>::iterator it = netdata.laydata.begin();
        it != netdata.laydata.end(); it++
    ) {
        layers[it->first].LoadPresets(it->second);
    }

}




void Network::Reset(bool purge_data) {
    // Reset the layers
    for(
        vec<Layer>::iterator it = layers.begin();
        it != layers.end();
        it++
    ) {
        it->Reset(purge_data);
    }

    // Reset the connection matrix.
    for(
        vec<vec<Connection>>::iterator it1 = connection_matrix.begin();
        it1 != connection_matrix.end();
        it1++
    ) {
        for(
            vec<Connection>::iterator it2 = it1->begin();
            it2 != it1->end();
            it2++
        ) {
            it2->just_spiked = false;
            it2->output = 0.0;
            it2->time = 0;
            it2->num_spikes = 0;
            it2->pos_target_rate = 0.0;
            it2->weight_target_rate = 0.0;
        }
    }
}
void Network::RandomizeOrder(RNG & rng) {
    for(
        vec<Layer>::iterator it = layers.begin();
        it != layers.end();
        it++
    ) {
        it->RandomizeNeuronOrder(rng);
    }
}

i64 Network::AddLayer(Layer layer) {
    i64 id = static_cast<i64>(layers.size());
    // Set the layer's id.
    layer.SetID(id);

    if(layer.IsInput()) {
        input_layer=id;
    }
    if(layer.IsOutput()) {
        output_layer=id;
    }

    // Add a layer to the connection matrix.
    vec<Connection> conns;
    for(i64 i = 0; i < layer.GetLayerSize(); i++) {
        conns.push_back(Connection());
    }
    connection_matrix.push_back(std::move(conns));

    // Add to vec
    layers.push_back(std::move(layer));
    
    zxlog::Debug(
        "Adding layer " + std::to_string(id)
    );

    return id;
}

Layer * Network::GetLayer(i64 index) {
    if(index < static_cast<i64>(layers.size())) {
        return &(layers[index]);
    } else {
        return nullptr;
    }
}

i64 Network::GetOutputLayerIndex() {
    return output_layer;
}
i64 Network::GetInputLayerIndex() {
    return input_layer;
}

void Network::SetInputs(vec<double> & inputs) {

    // Set the raw input of all neurons in input layers.

    Layer * layer = GetLayer(input_layer);
    layer->SetInputs(inputs);

}

void Network::SetOutputs(vec<Connection*> & outputs) {
    for(
        vec<Connection>::iterator it = connection_matrix[output_layer].begin();
        it != connection_matrix[output_layer].end();
        it++
    ) {
        outputs.push_back(&(*it));
    }
}

void Network::Update(i64 time, Writer * writer, RNG & rng) {

    for(sizet i = 0; i < layers.size(); i++) {
        layers[i].Update(time, writer, connection_matrix, rng);
    }

}

void Network::UpdateLayerErrorValues(
    vec<double> & rates,
    i64 layer_id
) {
    for(i64 i = 0; i < layers[layer_id].GetLayerSize(); i++) {
        connection_matrix[layer_id][i].Reset(
            rates[i]
        );
    }
}
void Network::UpdateLayerErrorValuesSeparate(
    vec<double> & pos_rates,
    vec<double> & weight_rates,
    i64 layer_id
) {
    for(i64 i = 0; i < layers[layer_id].GetLayerSize(); i++) {
        connection_matrix[layer_id][i].Reset(
            pos_rates[i],
            weight_rates[i]
        );
    }
}

vec<double> Network::GetPosErrorRates(i64 layer_id) {
    vec<double> erates;
    for(sizet i = 0; i < connection_matrix[layer_id].size(); i++) {
        erates.push_back(connection_matrix[layer_id][i].GetPosDownStreamErrorRate());
    }
    return erates;
}
vec<double> Network::GetWeightErrorRates(i64 layer_id) {
    vec<double> erates;
    for(sizet i = 0; i < connection_matrix[layer_id].size(); i++) {
        erates.push_back(connection_matrix[layer_id][i].GetWeightDownStreamErrorRate());
    }
    return erates;
}

void Network::InitWriteData() {
    for(sizet i = 0; i < layers.size(); i++) {
        layers[i].InitWriteData();
    }
}

void Network::InitDendrites() {
    for(sizet i = 0; i < layers.size(); i++) {
        layers[i].InitDendrites();
    }
}
void Network::RebuildDendrites() {
    for(sizet i = 0; i < layers.size(); i++) {
        layers[i].RebuildDendrites();
    }
}


void Network::CleanUpData(Writer * writer) {
    for(sizet i = 0; i < layers.size(); i++) {
        layers[i].CleanUp(writer);
    }
}
void Network::SaveData(i64 time) {
    for(sizet i = 0; i < layers.size(); i++) {
        layers[i].SaveData(time, connection_matrix);
    }
}

void Network::WriteData(Writer * writer) {
    for(sizet i = 0; i < layers.size(); i++) {
        layers[i].WriteData(writer);
    }
}