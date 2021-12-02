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
    input_method = InputMethod::Full;
    SetInputMethod(input_method);
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
    input_method = InputMethod::Full;
    SetInputMethod(input_method);
}

Neuron::Neuron(Neuron && n) {
    if(&n != this) {
        id = n.id;
        layer_id = n.layer_id;
        location = std::move(n.location);
        nt = n.nt;
        vpre = n.vpre;
        vcur = n.vcur;
        upre = n.upre;
        ucur = n.ucur;
        time_cur_spike = n.time_cur_spike;
        time_pre_spike = n.time_pre_spike;
        spike_times_live = std::move(n.spike_times_live);
        spike_times_data = std::move(n.spike_times_data);
        synapses = std::move(n.synapses);
        synapse_indexes = std::move(n.synapse_indexes);
        dendrites = std::move(n.dendrites);
        baseline = n.baseline;
        raw_input = n.raw_input;
        record_data = n.record_data;
        record_interval = n.record_interval;
        record_data_size = n.record_data_size;
        data = std::move(n.data);
        just_spiked = n.just_spiked;
        output = n.output;
        input = n.input;
        SetInputMethod(n.input_method);
    }
}

Neuron& Neuron::operator=(Neuron && n) {
    if(&n != this) {
        id = n.id;
        layer_id = n.layer_id;
        location = std::move(n.location);
        nt = n.nt;
        vpre = n.vpre;
        vcur = n.vcur;
        upre = n.upre;
        ucur = n.ucur;
        time_cur_spike = n.time_cur_spike;
        time_pre_spike = n.time_pre_spike;
        spike_times_live = std::move(n.spike_times_live);
        spike_times_data = std::move(n.spike_times_data);
        synapses = std::move(n.synapses);
        synapse_indexes = std::move(n.synapse_indexes);
        dendrites = std::move(n.dendrites);
        baseline = n.baseline;
        raw_input = n.raw_input;
        record_data = n.record_data;
        record_interval = n.record_interval;
        record_data_size = n.record_data_size;
        data = std::move(n.data);
        just_spiked = n.just_spiked;
        output = n.output;
        input = n.input;
        SetInputMethod(n.input_method);
    }
    return *this;
}

void Neuron::LoadPresets(NeuData & neudata) {

    for(
        umap<i64,SynData>::iterator it = neudata.syndata.begin();
        it != neudata.syndata.end();
        it++
    ) {
        synapses[it->first].LoadPreset(it->second);
    }

}

void Neuron::Reset(bool purge_data) {
    vpre = vcur = nt.c;
    upre = ucur = nt.d;
    raw_input = 0.0;
    time_cur_spike = 0;
    time_pre_spike = 0;
    just_spiked = false;
    output = 0.0;
    input = 0.0;

    spike_times_live.clear();

    if(purge_data) {
        spike_times_data.clear();
    }

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
    synapse.compartment = synapse_id;
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


    // ORIGINAL KEEP: This scales the force of movement by the spike rate of
    //  the synapse's neuron.
    // double time_diff_self = static_cast<double>(
    //     synapses[synapse_id].time_cur_spike - synapses[synapse_id].time_pre_spike
    // );
    // double force_self = (
    //     (time_diff_self+zxlb::PRE_SELF_FORCE_TIME_WINDOW) /
    //     zxlb::PRE_SELF_FORCE_TIME_WINDOW *
    //     std::exp( - time_diff_self / zxlb::PRE_SELF_FORCE_TIME_WINDOW )
    // ) * error * synapses[synapse_id].polarity;

    double force_self = error * synapses[synapse_id].polarity;

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
            if(time_diff_other > zxlb::MAX_TEMPORAL_DIFFERENCE) {
                continue;
            }

            // What percent of the orbit are we aiming for?
            double target_angular_distance = M_PI * (time_diff_other / zxlb::MAX_TEMPORAL_DIFFERENCE);//zxlb::PRE_OTHER_FORCE_TIME_WINDOW);
            // double cur_distance = synapses[synapse_id].location.Distance(synapses[other_id].location);
            double cur_distance = direction.Distance(synapses[other_id].location);
            double angular_delta = cur_distance-target_angular_distance;


            // double force_other = (
            //     (time_diff_other + zxlb::PRE_OTHER_FORCE_TIME_WINDOW) /
            //     zxlb::PRE_OTHER_FORCE_TIME_WINDOW *
            //     std::exp( - time_diff_other / zxlb::PRE_OTHER_FORCE_TIME_WINDOW )
            // );

            // partial_force = (
            //     (force_self * force_other)
            // ) * error;

            partial_force = force_self * 
                (1.0 / (std::pow(angular_delta,2.0)+1.0)) * angular_delta *
                synapses[other_id].polarity;

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
        synapses[synapse_id].location.Distance(direction) * zxlb::PRE_LEARNING_RATE
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

        if(time_diff > zxlb::MAX_TEMPORAL_DIFFERENCE) {
            continue;
        }

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

        double target_radial_distance = zxlb::MAX_RADIUS * (time_diff / zxlb::MAX_TEMPORAL_DIFFERENCE);
        double distance = target_radial_distance - synapses[synapse_indexes[i]].location.Rad();
        
        // GOOD ORIGINAL: Scales the rate of change by the time_diff using an
        //  alpha function.
        // double force = (
        //         (time_diff + zxlb::POST_SOMA_FORCE_TIME_WINDOW) /
        //         zxlb::POST_SOMA_FORCE_TIME_WINDOW *
        //         std::exp( - time_diff / zxlb::POST_SOMA_FORCE_TIME_WINDOW)
        //     ) * 
        //     std::tanh(distance) * 
        //     cm[layer_id][id].GetErrorRateReLU() *
        //     zxlb::POST_LEARNING_RATE;

        // Removes alpha function from the one above. All change is equal.
        double force =  
            (1.0 / (std::pow(distance,2.0)+1.0)) * distance *
            cm[layer_id][id].GetErrorRateReLU() *
            zxlb::POST_LEARNING_RATE;

        synapses[synapse_indexes[i]].location.ChangeRad( force );

        // std::setprecision(10);
        // std::cout << distance << " "<< force << std::endl;

    }

}


void Neuron::bAP(i64 time, double signal, bool train_str) {

    stk<i64> synstk;
    for(
        vec<i64>::iterator it = dendrites.begin();
        it != dendrites.end();
        it++
    ) {
        synstk.push(*it);
    }

    Synapse * syn;
    vec<i64> * children;
    while(!synstk.empty()) {
        syn = GetSynapse(synstk.top());
        synstk.pop();
        syn->SetBAP(time);
        if(train_str) syn->ChangeStrengthPost(time);
        children = syn->GetChildren();
        for(sizet i = 0; i < children->size(); i++) {
            synstk.push(children->at(i));
        }
    }
}

void Neuron::GetInput(i64 time) {
    input = baseline + raw_input;
    stk<i64> synstk;
    for(
        vec<i64>::iterator it = dendrites.begin();
        it != dendrites.end();
        it++
    ) {
        synstk.push(*it);
    }

    Synapse * syn;
    Synapse * parent;
    vec<i64> * children;
    i64 pid;
    double temp_input = 0.0;
    while(!synstk.empty()) {
        syn = GetSynapse(synstk.top());
        children = syn->GetChildren();
        if(children->empty() || syn->GetUpstreamEval()) {
            pid = syn->GetParent();
            // If soma is parent...
            if(pid==-1) {
                // This version allows negative input into the soma
                // input += syn->GetSignal_Out(time) * zxlb::DENDRITE_SIGNAL_WEIGHT;

                // This version limits somatic input to only positive.
                temp_input = syn->GetSignal_Out(time) * zxlb::DENDRITE_SIGNAL_WEIGHT;
                if(temp_input > 0.0) input += temp_input;
            } 
            // Add this syn's signal to its parent's upstream signal.
            else {
                parent = GetSynapse(pid);
                // Same or diff compartment?
                if(parent->GetCompartment() == syn->GetCompartment()) {
                    parent->AddToUpstreamSignal(syn->GetSignal_In(time)); 
                } else {
                    parent->AddToUpstreamSignal(syn->GetSignal_Out(time));
                }
            }
            syn->ResetUpstreamSignal();
            syn->SetUpstreamEval(false);
            synstk.pop();
        } else {
            syn->SetUpstreamEval(true);
            for(sizet i = 0; i < children->size(); i++) {
                synstk.push(children->at(i));
            }
        }
    }
}

void Neuron::GetInputWitch(i64 time) {
    input = baseline + raw_input;
    stk<i64> synstk;
    for(
        vec<i64>::iterator it = dendrites.begin();
        it != dendrites.end();
        it++
    ) {
        synstk.push(*it);
    }

    Synapse * syn;
    Synapse * parent;
    vec<i64> * children;
    i64 pid;
    double temp_input = 0.0;
    double ang_dist = 0.0;
    double rad_dist = 0.0;
    double dist = 0.0;
    double time_diff = 0.0;
    while(!synstk.empty()) {
        syn = GetSynapse(synstk.top());
        children = syn->GetChildren();
        if(children->empty() || syn->GetUpstreamEval()) {
            pid = syn->GetParent();
            // If soma is parent...
            if(pid==-1) {
                ang_dist = 0.0; // Reset, but don't use. Irrelevant for soma.
                rad_dist = location.RadDistance(syn->location);
                time_diff = time - syn->GetCurSpike();
                // Convert dendritic field dists to temporal dists.
                rad_dist = (zxlb::MAX_TEMPORAL_DIFFERENCE * rad_dist) / zxlb::MAX_RADIUS;

                // Witch out: For the soma, we pass in time_diff for the
                // ang_dist as well so that its calculation is 1 and
                // does not affect the overall calculation. 
                // REASON: Angular distance makes no sense for the soma 
                // and a synapse.
                // temp_input = syn->GetSignalWitch_Out(
                //     time, rad_dist, time_diff, time_diff
                // );

                // This version allows negative input into the soma
                // input += syn->GetSignal_Out(time) * zxlb::DENDRITE_SIGNAL_WEIGHT;

                // This version limits somatic input to only positive.
                temp_input = temp_input * zxlb::DENDRITE_SIGNAL_WEIGHT;
                if(temp_input > 0.0) input += temp_input;
            } 
            // Add this syn's signal to its parent's upstream signal.
            else {
                parent = GetSynapse(pid);
                ang_dist = parent->location.Distance(syn->location);
                rad_dist = parent->location.RadDistance(syn->location);
                time_diff = parent->GetCurSpike() - syn->GetCurSpike();
                // Convert dendritic field dists to temporal dists.
                ang_dist = (zxlb::MAX_TEMPORAL_DIFFERENCE * ang_dist) / M_PI;
                rad_dist = (zxlb::MAX_TEMPORAL_DIFFERENCE * rad_dist) / zxlb::MAX_RADIUS;
                dist = std::sqrt(ang_dist*ang_dist + rad_dist*rad_dist);

                // Same or diff compartment?
                // if(parent->GetCompartment() == syn->GetCompartment()) {
                //     parent->AddToUpstreamSignal(syn->GetSignalWitch_In(time, rad_dist, ang_dist, time_diff)); 
                // } else {
                //     parent->AddToUpstreamSignal(syn->GetSignalWitch_Out(time, rad_dist, ang_dist, time_diff));
                // }
            }
            syn->ResetDendriticSignals();
            syn->SetUpstreamEval(false);
            synstk.pop();
        } else {
            syn->SetUpstreamEval(true);
            for(sizet i = 0; i < children->size(); i++) {
                synstk.push(children->at(i));
            }
        }
    }
}

void Neuron::GetInputWitch2(i64 time) {
    input = baseline + raw_input;
    stk<i64> synstk;
    for(
        vec<i64>::iterator it = dendrites.begin();
        it != dendrites.end();
        it++
    ) {
        synstk.push(*it);
    }


    Synapse * syn;
    vec<i64> * children;
    Synapse * parent;
    i64 pid;
    vec<DenSig> densigs;
    double ang_dist = 0.0;
    double rad_dist = 0.0;
    double time_diff = 0.0;
    double sig = 0.0;
    double dist = 0.0;
    double temp_input = 0.0;
    while(!synstk.empty()) {
        syn = GetSynapse(synstk.top());
        children = syn->GetChildren();
        if(children->empty() || syn->GetUpstreamEval()) {
            pid = syn->GetParent();
            densigs = syn->GetDendriticSignals();

            // Parent is soma.
            if(pid==-1) {
                ang_dist = 0.0;
                rad_dist = syn->location.Rad();
                rad_dist = (zxlb::MAX_TEMPORAL_DIFFERENCE * rad_dist) / zxlb::MAX_RADIUS;
                time_diff = time - syn->GetCurSpike();
                dist = rad_dist;
                sig = syn->GetSignalWitch_Self(time);

                for(sizet i = 0; i < densigs.size(); i++) {
                    sig += densigs[i].sig * 
                        syn->GetSignalWitchMod(time,dist,densigs[i].Ts) *
                        zxlb::DENDRITE_SIGNAL_WEIGHT;
                }
                if(sig > 0) input += sig;
            } 
            // Parent is a synapse.
            else {
                parent = GetSynapse(pid);
                ang_dist = parent->location.Distance(syn->location);
                rad_dist = parent->location.RadDistance(syn->location);
                time_diff = time - syn->GetCurSpike();
                ang_dist = (zxlb::MAX_TEMPORAL_DIFFERENCE * ang_dist) / M_PI;
                rad_dist = (zxlb::MAX_TEMPORAL_DIFFERENCE * rad_dist) / zxlb::MAX_RADIUS;
                dist = std::sqrt(ang_dist*ang_dist + rad_dist*rad_dist); // Simplified from the spherical triangle pythag.
                sig = syn->GetSignalWitch_Self(time);

                if(parent->GetCompartment()==syn->GetCompartment()) {
                    for(sizet i = 0; i < densigs.size(); i++) {
                        densigs[i].sig *= syn->GetSignalWitchMod(time,densigs[i].D,densigs[i].Ts);
                        densigs[i].D += dist; // This must be updated after the sig.
                    }

                    densigs.emplace_back(DenSig(dist,time_diff,sig));

                    parent->AddDendriticSignals(std::move(densigs));
                } else {
                    double upstream_sigs = sig;
                    for(sizet i = 0; i < densigs.size(); i++) {
                        upstream_sigs += densigs[i].sig * syn->GetSignalWitchMod(time,densigs[i].D,densigs[i].Ts);
                    }

                    // densigs->size() should be abs in the demoninator;
                    // however, it cannot be < 0, so save an op.
                    upstream_sigs = (upstream_sigs*(densigs.size()+1)) /
                        (std::abs(upstream_sigs) + densigs.size() + 1);

                    DenSig densig(dist,time_diff,upstream_sigs);

                    parent->AddDendriticSignal(densig);
                }
                
            }
            
            syn->SetUpstreamEval(false);
            syn->ResetDendriticSignals();
            synstk.pop();

        } else {
            syn->SetUpstreamEval(true);
            for(sizet i = 0; i < children->size(); i++) {
                synstk.push(children->at(i));
            }
        }
    }
}

void Neuron::GetInputWitch3(i64 time) {
    input = baseline + raw_input;
    stk<i64> synstk;
    for(
        vec<i64>::iterator it = dendrites.begin();
        it != dendrites.end();
        it++
    ) {
        synstk.push(*it);
    }


    Synapse * syn;
    vec<i64> * children;
    Synapse * parent;
    i64 pid;
    vec<DenSig> densigs;
    vec<vec<DenSig>> den_terms;
    double ang_dist = 0.0;
    double rad_dist = 0.0;
    double time_diff = 0.0;
    double sig = 0.0;
    double dist = 0.0;
    double temp_input = 0.0;
    while(!synstk.empty()) {
        syn = GetSynapse(synstk.top());
        children = syn->GetChildren();
        if(children->empty() || syn->GetUpstreamEval()) {
            pid = syn->GetParent();
            densigs = syn->GetDendriticSignals();

            // Parent is soma.
            if(pid==-1) {
                // ang_dist = 0.0;
                // rad_dist = syn->location.Rad();
                // rad_dist = (zxlb::MAX_TEMPORAL_DIFFERENCE * rad_dist) / zxlb::MAX_RADIUS;
                time_diff = time - syn->GetCurSpike();
                dist = syn->dist_to_parent;
                sig = syn->GetSignalWitch_Self(time);

                for(sizet i = 0; i < densigs.size(); i++) {
                    densigs[i].sig *= 1.0+sig*syn->GetSignalWitchMod(time,densigs[i].D,densigs[i].Ts);
                    densigs[i].D += dist;
                    // sig += densigs[i].sig * 
                    //     syn->GetSignalWitchMod(time,dist,densigs[i].Ts) *
                    //     zxlb::DENDRITE_SIGNAL_WEIGHT;
                }
                densigs.emplace_back(DenSig(dist,time_diff,sig));
                den_terms.push_back(std::move(densigs));
                // if(sig > 0) input += sig;
                
            } 
            // Parent is a synapse.
            else {
                parent = GetSynapse(pid);
                // ang_dist = parent->location.Distance(syn->location);
                // rad_dist = parent->location.RadDistance(syn->location);
                
                // ang_dist = (zxlb::MAX_TEMPORAL_DIFFERENCE * ang_dist) / M_PI;
                // rad_dist = (zxlb::MAX_TEMPORAL_DIFFERENCE * rad_dist) / zxlb::MAX_RADIUS;
                time_diff = time - syn->GetCurSpike();
                dist = syn->dist_to_parent; //std::sqrt(ang_dist*ang_dist + rad_dist*rad_dist); // Simplified from the spherical triangle pythag.
                sig = syn->GetSignalWitch_Self(time);

                if(parent->GetCompartment()==syn->GetCompartment()) {
                    for(sizet i = 0; i < densigs.size(); i++) {
                        densigs[i].sig *= 1.0 + sig * syn->GetSignalWitchMod(time,densigs[i].D,densigs[i].Ts);
                        densigs[i].D += dist; // This must be updated after the sig.
                    }

                    densigs.emplace_back(DenSig(dist,time_diff,sig));

                    parent->AddDendriticSignals(std::move(densigs));
                } else {
                    double upstream_sigs = sig;
                    for(sizet i = 0; i < densigs.size(); i++) {
                        upstream_sigs += densigs[i].sig * (1.0+sig*syn->GetSignalWitchMod(time,densigs[i].D,densigs[i].Ts));
                    }

                    // densigs->size() should be abs in the demoninator;
                    // however, it cannot be < 0, so save an op.
                    upstream_sigs = (upstream_sigs*(densigs.size()+1)) /
                        (std::abs(upstream_sigs) + densigs.size() + 1);

                    DenSig densig(dist,time_diff,upstream_sigs);

                    parent->AddDendriticSignal(densig);
                }
                
            }
            
            syn->SetUpstreamEval(false);
            syn->ResetDendriticSignals();
            synstk.pop();

        } else {
            syn->SetUpstreamEval(true);
            for(sizet i = 0; i < children->size(); i++) {
                synstk.push(children->at(i));
            }
        }
    }
    
    for(sizet i = 0; i < den_terms.size(); i++) {
        
        double sig = 0.0;
        double num = den_terms[i].size();
        for(sizet k = 0; k < num; k++) {
            sig += den_terms[i][k].sig * 
                (zxlb::WITCH_C / (std::pow((den_terms[i][k].Ts-den_terms[i][k].D)/zxlb::WITCH_B,2.0) + 1.0) + 1.0) ;
        }
        sig = (sig*num) / (std::abs(sig) + num);
        if(sig > 0) input += sig * zxlb::DENDRITE_SIGNAL_WEIGHT;
    }
    
    
}

void Neuron::GetInput2(i64 time) {
    input = baseline + raw_input;
    que<i64> synque;
    for(
        vec<Synapse>::iterator it = synapses.begin();
        it != synapses.end();
        it++
    ) {
        if(it->children.empty()) {
            synque.push(it->id);
        }
    }

    Synapse * syn;
    Synapse * parent;
    i64 pid;
    double temp_input = 0.0;
    while(!synque.empty()) {
        syn = GetSynapse(synque.front());
        synque.pop();
        pid = syn->GetParent();
        if(pid==-1) {
            temp_input = syn->GetSignal_Out(time) * zxlb::DENDRITE_SIGNAL_WEIGHT;
            if(temp_input > 0.0) input += temp_input;
        } else {
            parent = GetSynapse(pid);
            if(parent->GetCompartment() == syn->GetCompartment()) {
                parent->AddToUpstreamSignal(syn->GetSignal_In(time)); 
            } else {
                parent->AddToUpstreamSignal(syn->GetSignal_Out(time));
            }
            synque.push(parent->id);
        }
        syn->ResetUpstreamSignal();
    }
}


void Neuron::GetInput_Old(i64 time) {
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
                double us = syn->GetSignal_Out(time);
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
        input += synapses[*it].GetSignal_Out(time);
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
        input += it->GetSignal_Out(time);
    }
    input *=  zxlb::DENDRITE_SIGNAL_WEIGHT;
}

bool Neuron::Update(i64 time, Writer * writer, i64 layer_id, ConnectionMatrix & cm, bool train_str, bool train_ang) {

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
        // Change strength
        if(train_str) synapses[syns_with_pre_spikes[i]].ChangeStrengthPre(time);
        if(train_ang) PresynapticSpike(time, syns_with_pre_spikes[i], cm);
    }

    // get_input(time);
    GetInputWitch3(time);

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
        time_pre_spike = time_cur_spike;
        time_cur_spike = time;
        spike_times_live.push_back(time);
        spike_times_data.push_back(time);
        just_spiked = true;
    
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
    data = nullptr;
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

void Neuron::InitDendrites() {
    dendrites.clear();
    for(sizet i = 0; i < synapses.size(); i++) {
        synapses[i].parent = -1;
        synapses[i].children.clear();
        synapses[i].dendrite_path_length = 0.0;
        synapses[i].compartment = static_cast<i64>(i);
        synapses[i].compartment_length = 0;
        dendrites.push_back(i);
    }
}

void Neuron::BuildDendrite() {


    //-------------------------------------
    // Init lists.
    lst<i64> unconnected;
    lst<i64> connected = {-1};
    i64 comp_id = 0;
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
        synapses[i].compartment = 0;
        synapses[i].compartment_length = 0;
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
            synapses[*min_uit].SetCompartmentLength(min_path_len);
            // Since it is connected to soma, new compartment.
            synapses[*min_uit].SetCompartment(comp_id++);
        } else {
            synapses[*min_cit].children.push_back(*min_uit);
            synapses[*min_uit].parent = *min_cit;
            double min_path_len = synapses[*min_cit].dendrite_path_length +
                synapses[*min_cit].location.DistanceStraightLine(synapses[*min_uit].location);
            synapses[*min_uit].SetDendritePathLength(min_path_len);

            // Parent is another syn...
            double cur_comp_length = synapses[*min_cit].GetCompartmentLength();
            double add_comp_length = synapses[*min_cit].location.DistanceStraightLine(synapses[*min_uit].location);
            if(cur_comp_length+add_comp_length <= zxlb::MAX_COMPARTMENT_LENGTH) {
                synapses[*min_uit].SetCompartmentLength(cur_comp_length+add_comp_length);
                synapses[*min_uit].SetCompartment(synapses[*min_cit].GetCompartment());
            } else {
                synapses[*min_uit].SetCompartment(comp_id++);
                synapses[*min_uit].SetCompartmentLength(0.0);
            }
        }

        // Update connected/unconnected lists.
        connected.push_back(*min_uit);

        unconnected.erase(min_uit);


    }

}

void Neuron::BuildDendrite2() {


    //-------------------------------------
    // Init lists.
    lst<i64> unconnected;
    lst<i64> connected = {-1};
    i64 comp_id = 0;
    double ang_dist = 0.0;
    double rad_dist = 0.0;
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
        synapses[i].compartment = 0;
        synapses[i].compartment_length = 0;
        synapses[i].dist_to_parent = 0;
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
                    uc_dist = (zxlb::MAX_TEMPORAL_DIFFERENCE * uc_dist) / zxlb::MAX_RADIUS;

                } else {
                    ang_dist = synapses[*cit].location.Distance(synapses[*uit].location);
                    rad_dist = synapses[*cit].location.RadDistance(synapses[*uit].location);
                    ang_dist = (zxlb::MAX_TEMPORAL_DIFFERENCE * ang_dist) / M_PI;
                    rad_dist = (zxlb::MAX_TEMPORAL_DIFFERENCE * rad_dist) / zxlb::MAX_RADIUS;
                    uc_dist = std::sqrt(ang_dist*ang_dist + rad_dist*rad_dist);
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
            min_path_len = (zxlb::MAX_TEMPORAL_DIFFERENCE * min_path_len) / zxlb::MAX_RADIUS;
            synapses[*min_uit].dist_to_parent = min_path_len;
            synapses[*min_uit].SetDendritePathLength(min_path_len);
            synapses[*min_uit].SetCompartmentLength(min_path_len);
            // Since it is connected to soma, new compartment.
            //std::cout << "SOMA " << min_path_len << std::endl;
            synapses[*min_uit].SetCompartment(comp_id++);
        } else {
            synapses[*min_cit].children.push_back(*min_uit);
            synapses[*min_uit].parent = *min_cit;
            ang_dist = synapses[*min_cit].location.Distance(synapses[*min_uit].location);
            rad_dist = synapses[*min_cit].location.RadDistance(synapses[*min_uit].location);
            ang_dist = (zxlb::MAX_TEMPORAL_DIFFERENCE * ang_dist) / M_PI;
            rad_dist = (zxlb::MAX_TEMPORAL_DIFFERENCE * rad_dist) / zxlb::MAX_RADIUS;
            double min_path_len = std::sqrt(ang_dist*ang_dist + rad_dist*rad_dist);
            synapses[*min_uit].dist_to_parent = min_path_len;
            //std::cout << "SYNSYN " << min_path_len << std::endl;
            synapses[*min_uit].SetDendritePathLength(min_path_len+synapses[*min_cit].dendrite_path_length);

            // Parent is another syn...
            double cur_comp_length = synapses[*min_cit].GetCompartmentLength();
            double add_comp_length = min_path_len;
            
            if(cur_comp_length+add_comp_length <= zxlb::MAX_COMPARTMENT_LENGTH) {
                synapses[*min_uit].SetCompartmentLength(cur_comp_length+add_comp_length);
                synapses[*min_uit].SetCompartment(synapses[*min_cit].GetCompartment());
            } else {
                synapses[*min_uit].SetCompartment(comp_id++);
                synapses[*min_uit].SetCompartmentLength(0.0);
            }
        }

        // Update connected/unconnected lists.
        connected.push_back(*min_uit);

        unconnected.erase(min_uit);


    }

}

void Neuron::BuildCompartments() {
    umap<i64,double> comp_lengths;
    i64 compid = 0;
    stk<i64> synstk;
    for(
        vec<i64>::iterator it = dendrites.begin();
        it != dendrites.end();
        it++
    ) {
        synapses[*it].compartment=compid;
        comp_lengths.insert({
            compid++,
            synapses[*it].location.DistanceStraightLine(VecS())
        });
        synstk.push(*it);
    }

    Synapse * syn;
    vec<i64> * children;
    Synapse * child;
    double cur_dist;
    double ext_dist;
    while(!synstk.empty()) {
        syn = GetSynapse(synstk.top());
        synstk.pop();
        children = syn->GetChildren();
        cur_dist = comp_lengths.at(syn->compartment);
        for(sizet i = 0; i < children->size(); i++) {
            child = GetSynapse((*children)[i]);
            ext_dist = child->location.DistanceStraightLine(syn->location);
            // If still in same compartment...
            if(ext_dist+cur_dist <= zxlb::MAX_COMPARTMENT_LENGTH) {
                comp_lengths.at(syn->compartment) = ext_dist+cur_dist;
            }
        }
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

void Neuron::SetInputMethod(InputMethod im) {
    switch(im) {
        case InputMethod::Full:
            get_input = std::bind( &Neuron::GetInput,this, std::placeholders::_1);
            break;
        case InputMethod::Simple:
            get_input = std::bind( &Neuron::GetInputSimple, this, std::placeholders::_1);
            break;
        case InputMethod::Witch:
            get_input = std::bind( &Neuron::GetInputWitch, this, std::placeholders::_1);
            break;
        default:
            get_input = std::bind( &Neuron::GetInput,this, std::placeholders::_1);  
            break;
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

