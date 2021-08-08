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

    // std::shuffle(synapse_indexes.begin(), synapse_indexes.end(), rng);

    for(
        vec<Synapse>::iterator it = synapses.begin();
        it != synapses.end();
        it++
    ) {
        it->Reset();
    }

    
}
void Neuron::RandomizeSynapseOrder(RNG & rng) {
    std::shuffle(
        synapse_indexes.begin(),
        synapse_indexes.end(),
        rng
    );
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
    synapse_indexes.push_back(synapses.size());
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

    VecS direction = synapses[synapse_id].location;
    double partial_force = 0.0;
    double error = synapses[synapse_id].GetError();

    double time_diff_self = static_cast<double>(
        synapses[synapse_id].time_cur_spike - synapses[synapse_id].time_pre_spike
    );

    double force_self = (
        (time_diff_self+zxlb::PRE_SELF_FORCE_TIME_WINDOW) /
        zxlb::PRE_SELF_FORCE_TIME_WINDOW *
        std::exp( - time_diff_self / zxlb::PRE_SELF_FORCE_TIME_WINDOW )
    ) * error;

    i64 other_id = synapse_id;
    for(i64 i = 0; i < static_cast<i64>(synapse_indexes.size()); i++) {
        other_id = synapse_indexes[i];
        if(other_id != synapse_id) {
            double time_diff_other = static_cast<double>(
                time - synapses[other_id].time_cur_spike
            );

            // If the difference between cur time (this presyn spike time) and
            // the prev spike time of the other syn is greater than the
            // window, skip
            if(time_diff_other > zxlb::PRE_OTHER_FORCE_TIME_WINDOW) {
                continue;
            }

            // What percent of the orbit are we aiming for?
            double target_angular_distance = M_PI * (time_diff_other / zxlb::PRE_OTHER_FORCE_TIME_WINDOW);
            double cur_distance = synapses[synapse_id].location.Distance(synapses[other_id].location);
            double angular_delta = cur_distance-target_angular_distance;


            // double force_other = (
            //     (time_diff_other + zxlb::PRE_OTHER_FORCE_TIME_WINDOW) /
            //     zxlb::PRE_OTHER_FORCE_TIME_WINDOW *
            //     std::exp( - time_diff_other / zxlb::PRE_OTHER_FORCE_TIME_WINDOW )
            // );

            // partial_force = (
            //     (force_self * force_other)
            // ) * error;

            partial_force = force_self * angular_delta;

            // VecS temp_direction(direction);

            //std::cout << synapse_id << "  "<< synapses[synapse_id].location.HeadingTo(synapses[i].location) << "  " << partial_force << std::endl;
            // direction.Orbit(synapses[synapse_id].location.HeadingTo(synapses[other_id].location), partial_force);
            direction.Orbit(direction.HeadingTo(synapses[other_id].location), partial_force);
            
            // if(std::isnan(direction.Lat()) || std::isnan(temp_direction.Lat()) || std::isnan(cur_distance)) {
            // // if(angular_delta > M_PI || angular_delta < -M_PI) {
            //     std::cout.precision(30);
                // std::cout << "NAN DETECTED:\n"
                //     << "   IDS:                 " << synapse_id << "  " << i << std::endl 
                //     << "   FROM:                " << synapses[synapse_id].location.to_string() << std::endl
                //     << "   FROM:                " << synapses[synapse_id].location.Lat() << "," << synapses[synapse_id].location.Lon() << "," << synapses[synapse_id].location.Rad() << std::endl
                //     << "   TO:                  " << synapses[i].location.to_string() << std::endl
                //     << "   TO:                  " << synapses[i].location.Lat() << "," << synapses[i].location.Lon() << "," << synapses[i].location.Rad() << std::endl
                //     << "   RECALC DIST:         " << synapses[synapse_id].location.Distance(synapses[i].location) << std::endl
                //     << "   OLD DIRECTION:       " << temp_direction.to_string() << std::endl
                //     << "   NEW DIRECTION:       " << direction.to_string() << std::endl
                //     << "   TARGET_ANGULAR_DIST: " << target_angular_distance << std::endl
                //     << "   CUR_DIST:            " << cur_distance << std::endl
                //     << "   ANGULAR_DELTA:       " << angular_delta << std::endl
                //     << "   PARTIAL FORCE:       " << partial_force << std::endl
                //     << "   RADIUS:              " << direction.Rad() << std::endl;
            // }
        }
    }
    synapses[synapse_id].location.Orbit(
        synapses[synapse_id].location.HeadingTo(direction),
        synapses[synapse_id].location.Distance(direction) * zxlb::LEARNING_RATE
    );
    // if(synapse_id==0)
    // std::cout << synapses[synapse_id].location.HeadingTo(direction) << "  "
    //     << synapses[synapse_id].location.Distance(direction) << "  "
    //     << synapses[synapse_id].location.to_string() << std::endl;


    // force /= static_cast<double>(synapses.size()-1);
    // force *= zxlb::LEARNING_RATE * error;
    // VecS force_vecS = force.ToVecS();
    // synapses[synapse_id].location.Orbit(
    //     //synapses[synapse_id].location.HeadingTo(force_vecS), 
    //     force.ToBearing(),
    //     force.Length()
    // );

    // Presynaptic activity in any context causes an increase in the
    // synapse's strength. Not sure about this one.
    //synapses[synapse_id].cur_strength += force.Length() * synapses[synapse_id].GetStrengthDelta();



}

void Neuron::PostsynapticSignal(i64 time, ConnectionMatrix & cm) {

    

    #pragma omp parallel for
    for(std::size_t i = 0; i < synapse_indexes.size(); i++) {

        // double time_diff_synapse = static_cast<double>(
        //     synapses[i].time_cur_spike - synapses[i].time_pre_spike
        // );
        double time_diff = static_cast<double>(
            time_cur_spike - synapses[synapse_indexes[i]].time_cur_spike
        );


        // THIS IS THE WORKING VERSION of time_diff_soma
        // double time_diff_soma = static_cast<double>(
        //     time_cur_spike - synapses[i].time_cur_spike
        // );


        // THIS IS THE NOT WORKING VERSION
        // THIS seems to me to be the correct way to calculate the
        //  time_diff_soma; however, it destroys the clustering
        //  of synapses around a pattern. I'm not sure why.
        //  I thought it might have caused the learning rate to be too large
        //  small, but I'm not sure.
        // NOTE: I have tried to increase the learning rate, but unless it
        //  is very large, the problem persists.
        // double time_diff_soma = static_cast<double>(
        //     time_cur_spike - time_pre_spike
        // );

        double distance = time_diff - synapses[synapse_indexes[i]].location.Rad();
        

        double force = (
                (time_diff + zxlb::POST_SOMA_FORCE_TIME_WINDOW) /
                zxlb::POST_SOMA_FORCE_TIME_WINDOW *
                std::exp( - time_diff / zxlb::POST_SOMA_FORCE_TIME_WINDOW)
            ) * 
            std::tanh(distance) * 
            cm[layer_id][id].GetErrorRateReLU() *
            zxlb::LEARNING_RATE;

        synapses[synapse_indexes[i]].location.ChangeRad( force );

    }

    // UNCOMMENT TO ADD STRENGTH CHANGES
    // for(std::size_t i = 0; i < dendrites.size(); i++) {
    //     bAP(time, dendrites[i], output);
    // }




    

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

    double delta = ( str_delta * time_delta * synapses[synapse_id].GetError() * synapses[synapse_id].GetSignal(time)); // str_delta zxlb::LEARNING_RATE * 
    
    synapses[synapse_id].cur_strength += delta;//(time_delta * str_delta * synapses[synapse_id].GetError() * amt);

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

void Neuron::GetInputSimple(i64 time) {
    input = baseline + raw_input;
    for(
        vec<Synapse>::iterator it = synapses.begin();
        it != synapses.end();
        it++
    ) {
        input += it->GetSignalWithStrength(time);
    }
}

bool Neuron::Update(i64 time, Writer * writer, i64 layer_id, ConnectionMatrix & cm) {

    // Update all the synapses
    vec<sizet> syns_with_pre_spikes;
    // #pragma omp parallel for
    for(sizet i = 0; i < synapse_indexes.size(); i++) {
        // Update, and if the input source for this synapse
        // produced a spike, then save index.
        if(synapses[synapse_indexes[i]].Update(time, writer, cm)) {
            syns_with_pre_spikes.push_back(synapse_indexes[i]);
        }
    }

    // Prespikes must be processed after all updates are finished
    // to insure that all syns have the latest pre-spike times. Otherwise
    // syns updated earlier in the vector won't see current prespikes of those
    // further down the vector.
    for(sizet i = 0; i < syns_with_pre_spikes.size(); i++) {
        PresynapticSpike(time, syns_with_pre_spikes[i], cm);
    }

    //GetInput(time);
    GetInputSimple(time);

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
    

    output = std::tanh(output);

    // Update connection matrix.
    cm[layer_id][id].Update(just_spiked, output);

    //SaveData(time, cm);
    return just_spiked;
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
    //if(record_data && (time%record_interval)==0) {
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
    //}

    for(sizet i = 0; i < synapses.size(); i++) {
        synapses[i].SaveData(time);
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
    lst<i64> connected = {-1};

    //-------------------------------------
    // Disconnect

    // Disconnect the dendrites.
    dendrites.clear();

    // Disconnect all synapses.
    // Complexity: Num Syns
    for(sizet i = 0; i < synapses.size(); i++) {
        synapses[i].parent = -1;
        synapses[i].children.clear();
        synapses[i].dendrite_path_length = 0.0;
        unconnected.push_back(i);
    }
    // std::cout << "NEWBUILD\n";
    while(!unconnected.empty()) {

        lst<i64>::iterator min_uit = unconnected.begin();
        lst<i64>::iterator min_cit = connected.begin();
        double min_dist = std::numeric_limits<double>::max();

        for(
            lst<i64>::iterator uit = unconnected.begin();
            uit != unconnected.end();
            uit++ 
        ) {

            for(
                lst<i64>::iterator cit = connected.begin();
                cit != connected.end();
                cit++
            ) {

                double uc_dist = 0.0;

                if(*cit==-1) {
                    uc_dist = synapses[*uit].location.Rad();
                    //uc_dist += zxlb::BF * uc_dist;

                } else {
                    uc_dist = synapses[*cit].location.DistanceStraightLine(synapses[*uit].location);
                    uc_dist += zxlb::BF * synapses[*cit].dendrite_path_length;
                }

                if(uc_dist < min_dist) {
                    min_cit = cit;
                    min_uit = uit;
                    min_dist = uc_dist;
                }

            }

        }

        if((*min_cit)==-1) {
            dendrites.push_back(*min_uit);
            synapses[*min_uit].parent = -1;
            double min_path_len = synapses[*min_uit].location.Rad();
            synapses[*min_uit].SetDendritePathLength(min_path_len);
        } else {
            synapses[*min_cit].children.push_back(*min_uit);
            synapses[*min_uit].parent = *min_cit;
            double min_path_len = synapses[*min_cit].dendrite_path_length +
                synapses[*min_cit].location.DistanceStraightLine(synapses[*min_uit].location);
            synapses[*min_uit].SetDendritePathLength(min_path_len);
        }

        // Update connected/unconnected lists.
        connected.push_back(*min_uit);

        unconnected.erase(min_uit);


    }

}

void Neuron::BuildDendriteParallel() {


    //-------------------------------------
    // Init lists.
    vec<i64> unconnected;
    vec<i64> connected = {-1};

    //-------------------------------------
    // Disconnect

    // Disconnect the dendrites.
    dendrites.clear();

    // Disconnect all synapses.
    // Complexity: Num Syns
    for(sizet i = 0; i < synapses.size(); i++) {
        synapses[i].parent = -1;
        synapses[i].children.clear();
        synapses[i].dendrite_path_length = 0.0;
        unconnected.push_back(i);
    }
    // std::cout << "NEWBUILD\n";
    while(!unconnected.empty()) {

        i64 min_uit = 0;
        i64 min_cit = 0;
        // vec<i64>::iterator min_uit = unconnected.begin();
        // vec<i64>::iterator min_cit = connected.begin();
        double min_dist = std::numeric_limits<double>::max();

        #pragma omp parallel for
        for(
            sizet uit = 0;
            uit < unconnected.size();
            uit++
        ) {

            for(
                sizet cit = 0;
                cit < connected.size();
                cit++
            ) {

                double uc_dist = 0.0;

                if(connected[cit]==-1) {
                    uc_dist = synapses[uit].location.Rad();
                    //uc_dist += zxlb::BF * uc_dist;

                } else {
                    uc_dist = synapses[cit].location.DistanceStraightLine(synapses[uit].location);
                    uc_dist += zxlb::BF * synapses[cit].dendrite_path_length;
                }

                if(uc_dist < min_dist) {
                    #pragma omp critical 
                    {
                        min_cit = cit;
                        min_uit = uit;
                        min_dist = uc_dist;
                    }
                }

            }

        }

        if(min_cit==-1) {
            dendrites.push_back(min_uit);
            synapses[min_uit].parent = -1;
            double min_path_len = synapses[min_uit].location.Rad();
            synapses[min_uit].SetDendritePathLength(min_path_len);
        } else {
            synapses[min_cit].children.push_back(min_uit);
            synapses[min_uit].parent = min_cit;
            double min_path_len = synapses[min_cit].dendrite_path_length +
                synapses[min_cit].location.DistanceStraightLine(synapses[min_uit].location);
            synapses[min_uit].SetDendritePathLength(min_path_len);
        }

        // Update connected/unconnected lists.
        connected.push_back(min_uit);

        unconnected.erase(unconnected.begin()+min_uit);


    }

}


void Neuron::PrintDendrite() {

    i64 sid;
    vec<i64> open;

    for(sizet i = 0; i < dendrites.size(); i++) {
        open.push_back(dendrites[i]);
    }
    while(!open.empty()) {

        sid = open.back();
        open.pop_back();
        // for(int i = 0; i < level; i++) {
        //     std::cout << " ";
        // }
        

        Synapse * syn = GetSynapse(sid);

        

        vec<i64> * children = syn->GetChildren();
        // if(!children->empty()) level++;
        // else level--;
        for(sizet i = 0; i < children->size(); i++) {
            open.push_back(children->at(i));
        }

    }

}




// OLD VERSION
// void Neuron::BuildDendrite() {


//     //-------------------------------------
//     // Init lists.
//     lst<i64> unconnected;
//     lst<i64> connected;

//     //-------------------------------------
//     // Disconnect

//     // Disconnect the dendrites.
//     dendrites.clear();

//     // Disconnect all synapses.
//     // Complexity: Num Syns
//     for(sizet i = 0; i < synapses.size(); i++) {
//         synapses[i].parent = -1;
//         synapses[i].children.clear();
//         unconnected.push_back(i);
//     }

//     while(!unconnected.empty()) {

//         i64 usyn = *(unconnected.begin());
//         i64 csyn = -1;
//         double min_dist = synapses[usyn].location.Rad();

//         // If the synapse is at MAX_RADIUS, connect directly
//         // to the soma and don't add to connected list.
//         // In this way, unchanged synapses can participate,
//         // but they cannot be a conduit for active ones.
//         if(min_dist >= zxlb::MAX_RADIUS) {
//             unconnected.erase(
//                 std::find(
//                     unconnected.begin(),
//                     unconnected.end(),
//                     usyn
//                 )
//             );
//             dendrites.push_back(usyn);
//             synapses[usyn].parent = -1;
//             synapses[usyn].SetDendritePathLength(synapses[usyn].location.Rad());
//             continue;
//         }

//         // Find the unconnected synapse with the smallest radius.
//         for(
//             lst<i64>::iterator ucit = unconnected.begin();
//             ucit != unconnected.end();
//             ucit++ 
//         ) {

//             // Begin by setting the min dist to the syn's radius.
//             double local_min_dist = synapses[*ucit].location.Rad();
//             i64 local_csyn = -1;

//             for(
//                 lst<i64>::iterator cit = connected.begin();
//                 cit != connected.end();
//                 cit++
//             )
//             {
//                 double dist_to_conn = synapses[*ucit].location.DistanceWithRadius(synapses[*cit].location);
//                 double dist = 
//                     dist_to_conn +
//                     zxlb::BF * (
//                         synapses[*cit].GetDendritePathLength() +
//                         dist_to_conn
//                     );

//                 if(dist < local_min_dist) {
//                     local_min_dist = dist;
//                     local_csyn = *cit;
//                 }

//             }

            
//             if(local_min_dist < min_dist) {
//                 min_dist = local_min_dist;
//                 usyn = *ucit;
//                 csyn = local_csyn;
//             }
//         }

//         // Connect. If csyn is -1, connect to the soma,
//         // else connect to already connected syn.
//         // Set the dendritic path length.

//         if(csyn == -1) {
            
//             dendrites.push_back(usyn);
//             synapses[usyn].parent = -1;
//             synapses[usyn].SetDendritePathLength(synapses[usyn].location.Rad());
//         } else {
//             synapses[csyn].children.push_back(usyn);
//             synapses[usyn].parent = csyn;
//             synapses[usyn].SetDendritePathLength(min_dist);
//         }

//         // Update connected/unconnected lists.
//         connected.push_back(usyn);
//         unconnected.erase(
//             std::find(
//                 unconnected.begin(),
//                 unconnected.end(),
//                 usyn
//             )
//         );
//         // if(connected.size() > 10) {
//         //     connected.pop_front();
//         // }

//     }

// }

