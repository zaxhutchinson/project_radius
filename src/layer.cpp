#include"layer.hpp"

Layer::Layer() {
    id = -1;
    name = "ERROR";
    is_input = false;
    is_output = false;
    input_generator = nullptr;
    train_ang = false;
    train_rad = false;
    train_str = false;
}

Layer::Layer(Layer && l) {
    if(&l != this) {
        id = l.id;
        name = std::move(l.name);
        neurons = std::move(l.neurons);
        neuron_indexes = std::move(l.neuron_indexes);
        is_input = l.is_input;
        is_output = l.is_output;
        input_generator = l.input_generator;
        train_rad = l.train_rad;
        train_ang = l.train_ang;
        train_str = l.train_str;
    }
}

Layer& Layer::operator=(Layer && l) {
    if(&l != this) {
        id = l.id;
        name = std::move(l.name);
        neurons = std::move(l.neurons);
        neuron_indexes = std::move(l.neuron_indexes);
        is_input = l.is_input;
        is_output = l.is_output;
        input_generator = l.input_generator;
        train_rad = l.train_rad;
        train_ang = l.train_ang;
        train_str = l.train_str;
    }
    return *this;
}

void Layer::LoadPresets(LayData & laydata) {

    for(
        umap<i64,NeuData>::iterator it = laydata.neudata.begin();
        it != laydata.neudata.end();
        it++
    ) {
        neurons[it->first].LoadPresets(it->second);
    }

}

void Layer::Reset(bool purge_data) {
    for(
        vec<Neuron>::iterator it = neurons.begin();
        it != neurons.end();
        it++
    ) {
        it->Reset(purge_data);
    }
    //input_generator = nullptr;
}
void Layer::RandomizeNeuronOrder(RNG & rng) {
    std::shuffle(
        neuron_indexes.begin(),
        neuron_indexes.end(),
        rng
    );
    for(
        vec<Neuron>::iterator it = neurons.begin();
        it != neurons.end();
        it++
    ) {
        it->RandomizeSynapseOrder(rng);
    }
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

void Layer::SetTraining(bool rad, bool ang, bool str) {
    train_rad=rad;
    train_ang=ang;
    train_str=str;
}

void Layer::AddInputGenerator(InputGenerator * ig) {
    // std::cout << dynamic_cast<InputGenerator_Poisson*>(ig)->id << std::endl;
    input_generator = ig;
}

void Layer::AddNeuron(Neuron neuron) {
    // Set the neuron's id to its index in the list.
    neuron.SetID(static_cast<i64>(neurons.size()));
    neuron_indexes.push_back(neurons.size());
    neurons.push_back(std::move(neuron));
}

Neuron * Layer::GetNeuron(i64 index) {
    if(index < static_cast<i64>(neurons.size())) {
        return &(neurons[index]);
    } else {
        return nullptr;
    }
}

void Layer::Update(i64 time, Writer * writer, ConnectionMatrix & cm, RNG & rng) {
    sizet i=0;
    if(input_generator != nullptr) {
        //#pragma omp parallel for
        for(sizet j = 0; j < neuron_indexes.size(); j++) {
            i = neuron_indexes[j];
            neurons[i].SetRawInput(input_generator->GetInput(i, time, rng));
            if(neurons[i].Update(time, writer, id, cm, train_str, train_ang)) {
                if(train_rad) neurons[i].PostsynapticSignal(time, cm);
                neurons[i].bAP(time,1.0, train_str, cm);
            }
        }
    } else {
        // #pragma omp parallel for
        for(sizet j = 0; j < neuron_indexes.size(); j++) {
            i = neuron_indexes[j];
            if(neurons[i].Update(time, writer, id, cm, train_str, train_ang)) {
                if(train_rad) neurons[i].PostsynapticSignal(time, cm);
                neurons[i].bAP(time,1.0, train_str, cm);
            }
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
void Layer::InitDendrites() {
    #pragma omp parallel for
    for(sizet i = 0; i < neurons.size(); i++) {
        neurons[i].InitDendrites();
    }
}
void Layer::RebuildDendrites() {
    #pragma omp parallel for
    for(sizet i = 0; i < neurons.size(); i++) {
        neurons[i].BuildDendrite4();
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