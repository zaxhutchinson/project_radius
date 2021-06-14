#include"layer.hpp"


void Layer::SetID(i64 _id) {
    id = _id;
}
i64 Layer::GetID() {
    return id;
}
void Layer::SetName(str _name) {
    name = _name;
}
str Layer::GetName() {
    return name;
}
bool Layer::IsInput() {
    return is_input;
}
bool Layer::IsOutput() {
    return is_output;
}
void Layer::SetInput(bool b) {
    is_input = b;
}
void Layer::SetOutput(bool b) {
    is_output = b;
}
i64 Layer::GetLayerSize() {
    return static_cast<i64>(neurons.size());
}

void Layer::AddNeuron(Neuron neuron) {
    // Set the neuron's id to its index in the list.
    neuron.SetID(static_cast<i64>(neurons.size()));

    neurons.push_back(std::move(neuron));
}

Neuron * Layer::GetNeuron(i64 index) {
    if(index < static_cast<i64>(neurons.size())) {
        return &(neurons[index]);
    } else {
        return nullptr;
    }
}

void Layer::UpdateNeurons(i64 time, Writer * writer) {
    for(sizet i = 0; i < neurons.size(); i++) {
        neurons[i].Update(time, writer);
    }
}