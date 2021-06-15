#include"neuron.hpp"

Neuron::Neuron()
{
    id = 0;
    location = VecS(0.0, 0.0, 0.0);
    nt = tmps::NeuronTemplate();
    vpre = vcur = nt.c;
    upre = ucur = nt.d;
    baseline = 0.0;
    raw_input = 0.0;
    record_data = false;
    record_interval = 1;
    record_interval = 1000;
    time_cur_spike = 0;
    time_pre_spike = 0;
    just_spiked = false;
    output = 0.0;
    ResetWriteData();
}

Neuron::Neuron(
    VecS _loc,
    const tmps::NeuronTemplate & _nt
)   :   location(_loc),
        nt(_nt)
{
    
    vpre = vcur = nt.c;
    upre = ucur = nt.d;
    baseline = 0.0;
    raw_input = 0.0;
    record_data = false;
    record_interval = 1;
    record_interval = 1000;
    time_cur_spike = 0;
    time_pre_spike = 0;
    just_spiked = false;
    output = 0.0;
    ResetWriteData();
}

void Neuron::Reset() {
    vpre = vcur = nt.c;
    upre = ucur = nt.d;
    raw_input = 0.0;
    time_cur_spike = 0;
    time_pre_spike = 0;
    just_spiked = false;
    output = 0.0;

    for(
        vec<Synapse>::iterator it = synapses.begin();
        it != synapses.end();
        it++
    ) {
        it->Reset();
    }
}

i64 Neuron::GetID() {
    return id;
}
void Neuron::SetID(i64 _id) {
    id = _id;
}

i64 Neuron::AddSynapse(Synapse synapse) {
    // Set the synapse's ID to the index it will
    // occupy in the vector.
    i64 synapse_id = static_cast<i64>(synapses.size());
    synapse.SetID(synapse_id);
    synapses.push_back(std::move(synapse));
    return synapse_id;
}
Synapse * Neuron::GetSynapse(i64 index) {
    return &(synapses[index]);
}

void Neuron::SetBaseline(double amt) {
    baseline = amt;
}
void Neuron::SetRawInput(double amt) {
    raw_input = amt;
}

// void Neuron::PresynapticSignal(i64 time, i64 synapse_id, double signal, bool pre_spike) {
//     synapses[synapse_id].SetSignal(time,signal);
//     if(pre_spike) {
//         synapses[synapse_id].SetCurSpike(time);
//         PresynapticSpike(time, synapse_id);
//     }
// }

void Neuron::PresynapticSpike(i64 time, i64 synapse_id) {

    double time_diff_self = static_cast<double>(
        synapses[synapse_id].time_cur_spike - synapses[synapse_id].time_pre_spike
    );

    Vec3 force;
    double partial_force = 0.0;
    double error = synapses[synapse_id].GetError();

    for(i64 i = 0; i < static_cast<i64>(synapses.size()); i++) {
        if(i != synapse_id) {
            double time_diff_other = static_cast<double>(
                time - synapses[synapse_id].time_cur_spike
            );

            double distance = synapses[synapse_id].location.Distance(synapses[i].location);

            double force_self = (
                (time_diff_self+zxlb::PRE_SELF_FORCE_TIME_WINDOW) /
                zxlb::PRE_SELF_FORCE_TIME_WINDOW *
                std::exp( - time_diff_self / zxlb::PRE_SELF_FORCE_TIME_WINDOW )
            );

            double force_other = (
                (time_diff_other + zxlb::PRE_OTHER_FORCE_TIME_WINDOW) /
                zxlb::PRE_OTHER_FORCE_TIME_WINDOW *
                std::exp( - time_diff_other / zxlb::PRE_OTHER_FORCE_TIME_WINDOW )
            );

            partial_force = (
                (force_self * force_other) *
                std::tanh(distance)
            );

            force += synapses[synapse_id].location.VectorTo(synapses[i].location) * 
                partial_force * 
                error;
        }
    }

    force /= static_cast<double>(synapses.size()-1);
    
    synapses[synapse_id].location.Orbit(force.ToBearing(), force.Length());

}

void Neuron::PostsynapticSignal(i64 time) {

    if(just_spiked) {

        for(std::size_t i = 0; i < synapses.size(); i++) {

            double time_diff_synapse = static_cast<double>(
                synapses[i].time_cur_spike - synapses[i].time_pre_spike
            );

            double time_diff_soma = static_cast<double>(
                time_cur_spike - synapses[i].time_cur_spike
            );

            double distance = time_diff_synapse - synapses[i].location.Rad();

            // double force_synapse = (
            //     (time_diff_synapse + zxlb::POST_SYNAPSE_FORCE_TIME_WINDOW) /
            //     zxlb::POST_SYNAPSE_FORCE_TIME_WINDOW *
            //     std::exp( - time_diff_synapse / zxlb::POST_SYNAPSE_FORCE_TIME_WINDOW)
            // );

            double force = (
                    (time_diff_soma + zxlb::POST_SOMA_FORCE_TIME_WINDOW) /
                    zxlb::POST_SOMA_FORCE_TIME_WINDOW *
                    std::exp( - time_diff_soma / zxlb::POST_SOMA_FORCE_TIME_WINDOW)
                ) * 
                std::tanh(distance) * 
                synapses[i].GetError();

            synapses[i].location.ChangeRad( force );

        }

        for(std::size_t i = 0; i < dendrites.size(); i++) {
            bAP(time, dendrites[i], output);
        }

    }


    

}


void Neuron::bAP(i64 time, i64 synapse_id, double amt) {
    double prepost_time_diff = static_cast<double>(
        time_cur_spike - synapses[synapse_id].time_cur_spike
    );

    double time_delta = (
        ((prepost_time_diff+zxlb::LEARNING_TIME_WINDOW) / zxlb::LEARNING_TIME_WINDOW) *
        std::exp( -prepost_time_diff/zxlb::LEARNING_TIME_WINDOW)
    );

    double str_delta = (
        1.0 - std::abs(synapses[synapse_id].cur_strength) / 
        (synapses[synapse_id].max_strength + std::abs(synapses[synapse_id].cur_strength))
    );

    synapses[synapse_id].cur_strength += (time_delta * str_delta * synapses[synapse_id].GetError());

    double bap_amt = std::tanh(time_delta + amt);

    for(std::size_t i = 0; i < synapses[synapse_id].children.size(); i++) {
        bAP(time, synapses[synapse_id].children[i], bap_amt);
    }
}




double Neuron::GetInput(i64 time) {
    // Calculate input
    double input = baseline + raw_input;

    // Push all the synapses that connect directly
    // to the soma onto the stack.
    stk<i64> synstk;
    for(
        vec<i64>::iterator it = dendrites.begin();
        it != dendrites.end();
        it++
    ) {
        synstk.push(*it);
    }

    i64 synid;
    Synapse * syn;
    vec<i64> * children;

    while(!synstk.empty()) {
        //zxlog::Debug(std::to_string(synstk.size()));

        // Get the top synapse id
        synid = synstk.top();

        syn = GetSynapse(synid);
        children = syn->GetChildren();

        // If the syn has no children, or
        // its children have already been evaluated,
        // process and pop.
        if(children->empty() || syn->GetUpstreamEval()) {

            // If this isn't a synapse that connects directly
            // to the soma, update it's parent's upstream signal.
            if(syn->GetParent() != -1) {
                Synapse * parent = GetSynapse(syn->GetParent());
                parent->SetUpstreamEval(true);
                parent->AddToUpstreamSignal(syn->GetSignalFull(time));
            }
            synstk.pop();
        } else {
            // Reset the upstream eval from the last get input.
            syn->SetUpstreamEval(false);
            for(
                vec<i64>::iterator it = children->begin();
                it != children->end();
                it++
            ) {
                synstk.push(*it);
            }
        }
    }

    // All synapses have been evaluated. Accumulated input
    // is in the synapses that connect directly to the soma.
    // Add this to the mix.
    for(
        vec<i64>::iterator it = dendrites.begin();
        it != dendrites.end();
        it++
    ) {
        input += synapses[*it].GetSignalFull(time);
    }

    return input;
}

void Neuron::Update(i64 time, Writer * writer, i64 layer_id, ConnectionMatrix & cm) {

    // Update all the synapses
    for(sizet i = 0; i < synapses.size(); i++) {
        // Update, and if the input source for this synapse
        // produced a spike, then process it.
        if(synapses[i].Update(time, writer, cm)) {
            PresynapticSpike(time, i);
        }
    }

    // Calculate voltage
    vcur = vpre +
    (
        nt.k * (vpre - nt.vr) * (vpre - nt.vt) -
        upre +
        GetInput(time)
    ) / nt.cap;

    // Calculate recovery
    ucur = upre +
        nt.a *
        (nt.b * (vpre - nt.vr) - upre);

    // Is there a spike?
    if(vcur > nt.vpeak) {
        vpre = vcur = nt.c;
        upre = ucur = ucur + nt.d;
        time_pre_spike = time_cur_spike;
        time_cur_spike = time;
        spike_times_live.push_back(time);
        spike_times_data.push_back(time);
        just_spiked = true;
        // Add Back prop
    
    } else {
        vpre = vcur;
        upre = ucur;
        just_spiked = false;
    }

    output = 0.0;
    double diff = 0.0;
    for(
        lst<i64>::iterator it = spike_times_live.begin();
        it != spike_times_live.end();
    ) {
        diff = time - (*it);
        if(diff > zxlb::NEURON_SPIKE_TIME_WINDOW) {
            it = spike_times_live.erase(it);
        } else {
            output += diff * std::exp( -(diff - 1.0) );
            it++;
        }
    }
    output = std::tanh(output);

    // Update connection matrix.
    cm[layer_id][id].Update(just_spiked, output);

    if(record_data && (time%record_interval)==0) {
        WriteData(time, writer);
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
    spike_times_data.clear();   // Clear out spike times.
    data->output.push_back(output);
}