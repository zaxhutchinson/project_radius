#include"layer.hpp"

Layer::Layer() {
    id = -1;
    name = "ERROR";
    is_input = false;
    is_output = false;
    input_generator = nullptr;
}

void Layer::Reset() {
    for(
        vec<Neuron>::iterator it = neurons.begin();
        it != neurons.end();
        it++
    ) {
        it->Reset();
    }
    input_generator = nullptr;
}
void Layer::SetID(i64 _id) {
    id = _id;
    for(sizet i = 0; i < neurons.size(); i++) {
        neurons[i].SetLayerID(id);
    }
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
void Layer::SetIsInput(bool b) {
    is_input = b;
}
void Layer::SetIsOutput(bool b) {
    is_output = b;
}
i64 Layer::GetLayerSize() {
    return static_cast<i64>(neurons.size());
}

void Layer::AddInputGenerator(InputGenerator * ig) {
    input_generator = ig;
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

void Layer::Update(i64 time, Writer * writer, ConnectionMatrix & cm) {
    if(input_generator != nullptr) {
        #pragma omp parallel for
        for(sizet i = 0; i < neurons.size(); i++) {
            neurons[i].SetRawInput(input_generator->GetInput(i, time));
            neurons[i].Update(time, writer, id, cm);
            neurons[i].PostsynapticSignal(time, cm);
        }
    } else {
        #pragma omp parallel for
        for(sizet i = 0; i < neurons.size(); i++) {
            neurons[i].Update(time, writer, id, cm);
            neurons[i].PostsynapticSignal(time, cm);
        }
    }
}

void Layer::SetInputs(vec<double> & inputs) {
    if(inputs.size() != neurons.size()) {
        zxlog::Error(
            "Layer: Input vector size differs from neuron vector size. Inputs = " +
            std::to_string(inputs.size()) + "Neurons = " + std::to_string(neurons.size())
        );
    } else {
        for(sizet i = 0; i < inputs.size(); i++) {
            neurons[i].SetRawInput(inputs[i]);
        }
    }
}

void Layer::RebuildDendrites() {
    #pragma omp parallel for
    for(sizet i = 0; i < neurons.size(); i++) {
        neurons[i].BuildDendrite();
    }
}

void Layer::CleanUp(Writer * writer) {
    for(sizet i = 0; i < neurons.size(); i++) {
        neurons[i].CleanupData(writer);
    }
}
void Layer::InitWriteData() {
    for(sizet i = 0; i < neurons.size(); i++) {
        neurons[i].InitWriteData();
    }
}
void Layer::SaveData(i64 time, ConnectionMatrix & cm) {
    for(sizet i = 0; i < neurons.size(); i++) {
        neurons[i].SaveData(time, cm);
    }
}
void Layer::WriteData(Writer * writer) {
    for(sizet i = 0; i < neurons.size(); i++) {
        neurons[i].WriteData(writer);
    }
}