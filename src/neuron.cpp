#include"neuron.hpp"

Neuron::Neuron()
{
    id = 0;
    location = VecS(0.0, 0.0, 0.0);
    nt = tmps::NeuronType();
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
    input = 0.0;
}

Neuron::Neuron(
    VecS _loc,
    const tmps::NeuronType & _nt
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
    input = 0.0;   
}

void Neuron::Reset() {
    vpre = vcur = nt.c;
    upre = ucur = nt.d;
    raw_input = 0.0;
    time_cur_spike = 0;
    time_pre_spike = 0;
    just_spiked = false;
    output = 0.0;
    input = 0.0;

    spike_times_live.clear();

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
void Neuron::SetLayerID(i64 _id) {
    layer_id = _id;
}

i64 Neuron::AddSynapse(Synapse synapse) {
    // Set the synapse's ID to the index it will
    // occupy in the vector.
    i64 synapse_id = static_cast<i64>(synapses.size());
    synapse.SetID(synapse_id);
    synapse.record_data = record_data;
    synapse.record_data_size = record_data_size;
    synapse.record_interval = record_interval;
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

void Neuron::PresynapticSpike(i64 time, i64 synapse_id, ConnectionMatrix & cm) {

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
    force *= zxlb::LEARNING_RATE;

    // Presynaptic activity in any context causes an increase in the
    // synapse's strength. Not sure about this one.
    //synapses[synapse_id].cur_strength += force.Length() * synapses[synapse_id].GetStrengthDelta();
    
    synapses[synapse_id].location.Orbit(force.ToBearing(), force.Length());

}

void Neuron::PostsynapticSignal(i64 time, ConnectionMatrix & cm) {

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
                //cm[layer_id][id].GetErrorRateReLU() *
                zxlb::LEARNING_RATE;

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

    // double str_delta = 1.0 - std::abs(
    //     0.5 - std::abs(synapses[synapse_id].cur_strength) / 
    //     (synapses[synapse_id].max_strength + std::abs(synapses[synapse_id].cur_strength))
    // );

    double str_delta = synapses[synapse_id].GetStrengthDelta();

    double delta = ( zxlb::LEARNING_RATE * time_delta * str_delta * synapses[synapse_id].GetError() * synapses[synapse_id].GetSignal(time));
    
    synapses[synapse_id].cur_strength += delta;//(time_delta * str_delta * synapses[synapse_id].GetError() * amt);

    // if(synapses[synapse_id].cur_strength < 0) std::cout << synapses[synapse_id].cur_strength << std::endl;

    double bap_amt = std::tanh(synapses[synapse_id].GetSignal(time) + amt);

    for(std::size_t i = 0; i < synapses[synapse_id].children.size(); i++) {
        bAP(time, synapses[synapse_id].children[i], bap_amt);
    }
}




void Neuron::GetInput(i64 time) {
    // Calculate input
    input = baseline + raw_input;

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
                double us = syn->GetSignalFull(time);
                parent->AddToUpstreamSignal(us);
                syn->ResetUpstreamSignal();
            }
            
            syn->SetUpstreamEval(false);
            
            synstk.pop();
        } else {

            syn->SetUpstreamEval(true);

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
        double us = synapses[*it].GetSignalFull(time);
        input += us;
        synapses[*it].ResetUpstreamSignal();
    }

    // if(layer_id==1 && id==0) {
    //     std::cout << input << std::endl;
    // }

    // return input;
}

void Neuron::Update(i64 time, Writer * writer, i64 layer_id, ConnectionMatrix & cm) {

    // Update all the synapses
    for(sizet i = 0; i < synapses.size(); i++) {
        // Update, and if the input source for this synapse
        // produced a spike, then process it.
        if(synapses[i].Update(time, writer, cm)) {
            PresynapticSpike(time, i, cm);
        }
    }

    GetInput(time);


    // Calculate voltage
    vcur = vpre +
    (
        nt.k * (vpre - nt.vr) * (vpre - nt.vt) -
        upre +
        input//GetInput(time)
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
    if(output < 0) std::cout << output << std::endl;

    output = std::tanh(output);

    // Update connection matrix.
    cm[layer_id][id].Update(just_spiked, output);

    SaveData(time, cm);

}

void Neuron::InitWriteData() {
    ResetWriteData();
    for(sizet i = 0; i < synapses.size(); i++) {
        synapses[i].ResetWriteData();
    }
}

void Neuron::ResetWriteData() {
    data = std::make_unique<NeuronData>();
    data->id = "NEURON_"+std::to_string(layer_id)+"_"+std::to_string(id);
    data->data_size = 0;
    data->neuron_id = id;
    data->layer_id = layer_id;
}

void Neuron::CleanupData(Writer * writer) {
    if(data->data_size > 0) {
        writer->AddNeuronData(std::move(data));
        ResetWriteData();
    }
    for(sizet i = 0; i < synapses.size(); i++) {
        synapses[i].CleanupData(writer);
    }
}
void Neuron::SaveData(i64 time, ConnectionMatrix & cm) {
    if(record_data && (time%record_interval)==0) {
        data->data_size++;
        data->time_indexes.push_back(time);
        data->locations.push_back(location);
        data->v.push_back(vcur);
        data->u.push_back(ucur);
        for(sizet i = 0; i < spike_times_data.size(); i++) {
            data->spike_times.push_back(spike_times_data[i]);
        }
        spike_times_data.clear();   // Clear out spike times.
        data->output.push_back(output);
        data->input.push_back(input);
        data->error.push_back(cm[layer_id][id].GetErrorRate());
    }
}
void Neuron::WriteData(Writer * writer) {

    if(record_data) {

        writer->AddNeuronData(std::move(data));
        ResetWriteData();

        for(sizet i = 0; i < synapses.size(); i++) {
            synapses[i].WriteData(writer);
        }
    }
}



void Neuron::BuildDendrite() {


    //-------------------------------------
    // Init lists.
    lst<i64> unconnected;
    lst<i64> connected;

    //-------------------------------------
    // Disconnect

    // Disconnect the dendrites.
    dendrites.clear();

    // Disconnect all synapses.
    // Complexity: Num Syns
    for(sizet i = 0; i < synapses.size(); i++) {
        synapses[i].parent = -1;
        synapses[i].children.clear();
        unconnected.push_back(i);
    }

    while(!unconnected.empty()) {

        i64 usyn = *(unconnected.begin());
        i64 csyn = -1;
        double min_dist = synapses[usyn].location.Rad();

        // If the distance is less than 1.0, connect directly
        // to the soma and DO NOT add to the connected this.
        // In this way, unchanged synapses can participate,
        // but they cannot be a conduit for active ones.
        if(min_dist >= zxlb::MAX_RADIUS) {
            unconnected.erase(
                std::find(
                    unconnected.begin(),
                    unconnected.end(),
                    usyn
                )
            );
            dendrites.push_back(usyn);
            synapses[usyn].parent = -1;
            synapses[usyn].SetDendritePathLength(synapses[usyn].location.Rad());
            continue;
        }

        // Find the unconnected synapse with the smallest radius.
        for(
            lst<i64>::iterator ucit = unconnected.begin();
            ucit != unconnected.end();
            ucit++ 
        ) {

            // Begin by setting the min dist to the syn's radius.
            double local_min_dist = synapses[*ucit].location.Rad();
            i64 local_csyn = -1;

            for(
                lst<i64>::iterator cit = connected.begin();
                cit != connected.end();
                cit++
            )
            {
                double dist_to_conn = synapses[*ucit].location.DistanceWithRadius(synapses[*cit].location);
                double dist = 
                    dist_to_conn +
                    zxlb::BF * (
                        synapses[*cit].GetDendritePathLength() +
                        dist_to_conn
                    );

                if(dist < local_min_dist) {
                    local_min_dist = dist;
                    local_csyn = *cit;
                }

            }

            
            if(local_min_dist < min_dist) {
                min_dist = local_min_dist;
                usyn = *ucit;
                csyn = local_csyn;
            }
        }

        // Connect. If csyn is -1, connect to the soma,
        // else connect to already connected syn.
        // Set the dendritic path length.

        if(csyn == -1) {
            
            dendrites.push_back(usyn);
            synapses[usyn].parent = -1;
            synapses[usyn].SetDendritePathLength(synapses[usyn].location.Rad());
        } else {
            synapses[csyn].children.push_back(usyn);
            synapses[usyn].parent = csyn;
            synapses[usyn].SetDendritePathLength(min_dist);
        }

        // Update connected/unconnected lists.
        connected.push_back(usyn);
        unconnected.erase(
            std::find(
                unconnected.begin(),
                unconnected.end(),
                usyn
            )
        );
        // if(connected.size() > 10) {
        //     connected.pop_front();
        // }

    }

}


void Neuron::PrintDendrite() {

    i64 sid;
    vec<i64> open;

    for(sizet i = 0; i < dendrites.size(); i++) {
        open.push_back(dendrites[i]);
    }
    i64 count = 1;
    while(!open.empty()) {

        sid = open.back();
        open.pop_back();
        // for(int i = 0; i < level; i++) {
        //     std::cout << " ";
        // }
        

        Synapse * syn = GetSynapse(sid);

        std::cout << count++ << " " << sid << " " << syn->location.Rad() << std::endl;

        vec<i64> * children = syn->GetChildren();
        // if(!children->empty()) level++;
        // else level--;
        for(sizet i = 0; i < children->size(); i++) {
            open.push_back(children->at(i));
        }

    }

}