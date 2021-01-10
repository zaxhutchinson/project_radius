#include"neuron.hpp"


Neuron::Neuron(
    i64 _id,
    VecS _loc,
    const NeuronTemplate & _nt
)   :   id(_id),
        location(_loc),
        nt(_nt)
{
    
    vpre = vcur = nt.c;
    upre = ucur = nt.d;
    output = 0.0;
    baseline_input = 0.0;
    record_data = false;
    record_interval = 1;
    record_interval = 1000;
    ResetWriteData();
}

void Neuron::Update(i64 time, Writer * writer) {
    
    // Calculate input
    double input = baseline_input;
    for(
        vsptr<Synapse>::iterator it = dendrite.begin();
        it != dendrite.end();
        it++
    ) {
        // Get input from synapses. However that happens.
    }

    // Calculate voltage
    vcur = vpre +
    (
        nt.k * (vpre - nt.vr) * (vpre - nt.vt) -
        upre +
        input
    ) / nt.cap;

    // Calculate recovery
    ucur = upre +
        nt.a *
        (nt.b * (vpre - nt.vr) - upre);

    // Is there a spike?
    if(vcur > nt.vpeak) {
        vpre = vcur = nt.c;
        upre = ucur = ucur + nt.d;
        spike_times_live.push_back(time);
        spike_times_data.push_back(time);

        // Add Back prop
    
    } else {
        vpre = vcur;
        upre = ucur;
    }   
}

void Neuron::ResetWriteData() {
    data = std::make_unique<NeuronData>();
    data->id = "NEURON_"+std::to_string(id);
    data->data_size = 0;
}

void Neuron::CleanupData(Writer * writer) {
    if(data->data_size > 0) {
        writer->AddNeuronData(std::move(data));
        ResetWriteData();
    }
}

void Neuron::WriteData(i64 time, Writer * writer) {
    if(data->data_size == record_data_size) {
        writer->AddNeuronData(std::move(data));
        ResetWriteData();
    }
    data->data_size++;
    data->time_indexes.push_back(time);
    data->locations.push_back(location);
    data->v.push_back(vcur);
    data->u.push_back(ucur);
    data->spike_times.push_back(spike_times_data);
    data->output.push_back(output);
}