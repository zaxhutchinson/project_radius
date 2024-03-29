#include"synapse.hpp"

Synapse::Synapse()
    :   id(0),
        max_strength(0.0),
        abs_max_strength(0.0),
        cur_strength(0.0),
        location(VecS(0.0, 0.0, 0.0)),
        polarity(1.0)
{
    parent = -1;
    signal[0] = 0.0;
    signal[1] = 0.0;
    bap = -1;
    error = 0.0;
    time_error = 0;
    time_cur_spike = std::numeric_limits<i64>::min();
    time_pre_spike = std::numeric_limits<i64>::min();
    spikes = 0;
    record_data = false;
    record_interval = 1;
    record_data_size = 1000;
    dendrite_path_length = 0.0;
    straightline_path_length = 0.0;
    compartment = 0;
    compartment_length = 0;
    upstream_signal = 0.0;
    upstream_eval = false;

    // children.reserve(10);

    dist_to_parent = 0.0;

    is_comp_root = false;
    cur_comp_strength = 0.0;

}

Synapse::Synapse(
    VecS _loc,
    double _max_strength,
    double _cur_strength,
    double _polarity
)   :   id(0),
        max_strength(_max_strength),
        abs_max_strength(std::abs(_max_strength)),
        cur_strength(_cur_strength),
        location(_loc),
        polarity(_polarity)
{
    parent = -1;
    signal[0] = 0.0;
    signal[1] = 0.0;
    bap = -1;
    error = 0.0;
    time_error = 0;
    time_cur_spike = std::numeric_limits<i64>::min();
    time_pre_spike = std::numeric_limits<i64>::min();
    spikes = 0;
    record_data = false;
    record_interval = 1;
    record_data_size = 1000;
    dendrite_path_length = 0.0;
    straightline_path_length = 0.0;
    compartment = 0;
    compartment_length = 0;
    upstream_signal = 0.0;
    upstream_eval = false;

    // children.reserve(10);

    dist_to_parent = 0.0;

    is_comp_root = false;
    cur_comp_strength = zxlb::DENDRITE_SIGNAL_WEIGHT;
    max_comp_strength = cur_comp_strength;
    abs_max_comp_strength = std::abs(max_comp_strength);
}

void Synapse::LoadPreset(SynData & syndata) {
    location.Lat(syndata.lat);
    location.Lon(syndata.lon);
    location.Rad(syndata.rad);
    parent = syndata.pid;
    cur_strength = syndata.str;
}

void Synapse::Reset() {
    signal[0] = signal[1] = 0.0;
    bap = -1;
    error = 0.0;
    time_error = 0;
    time_cur_spike = std::numeric_limits<i64>::min();
    time_pre_spike = std::numeric_limits<i64>::min();
    spikes = 0;
    upstream_eval = false;
    upstream_signal =0.0;
    spike_queue = {};
    comp_spike_queue = {};

}

double Synapse::GetError() {
    return error;
}

void Synapse::SetID(i64 _id) {
    id = _id;
}
void Synapse::SetLayerID(i64 _id) {
    layer_id = _id;
}
void Synapse::SetNeuronID(i64 _id) {
    neuron_id = _id;
}

void Synapse::SetConnectionAddress(ConnectionAddress ca) {
    this->ca = ca;
}

void Synapse::SetSignal(i64 time, double amt) {
    signal[time%2] = amt;
}

void Synapse::SetCurSpike(i64 time) {
    spike_queue.push_back(time);
    time_pre_spike = time_cur_spike;
    time_cur_spike = time;
}

i64 Synapse::GetCurSpike() {
    // return spike_queue.back();
    return time_cur_spike;
}

void Synapse::SetDendritePathLength(double path) {
    dendrite_path_length = path;
}

void Synapse::SetStraightLinePathLength(double path) {
    straightline_path_length = path;
}

double Synapse::GetStrength() {
    return (cur_strength*max_strength) / 
        (std::abs(cur_strength) + abs_max_strength);
}
double Synapse::GetStrengthDelta() {
    return abs_max_strength - (std::abs(cur_strength) * abs_max_strength) / (abs_max_strength + std::abs(cur_strength));
}

// void Synapse::ChangeStrengthPre(i64 time) {
    
//     // double time_diff = static_cast<double>(bap - time_cur_spike);
//     // double abs_time_diff = std::abs(time_diff);
//     // if(abs_time_diff<=zxlb::LEARNING_WINDOW_SYN_STRENGTH) {
//     //     double time_delta = (
//     //         (abs_time_diff + zxlb::LEARNING_WINDOW_SYN_STRENGTH) / 
//     //                 zxlb::LEARNING_WINDOW_SYN_STRENGTH
//     //         ) *
//     //         std::exp(-abs_time_diff / zxlb::LEARNING_WINDOW_SYN_STRENGTH);
//     //     if(abs_time_diff != 0.0)
//     //         time_delta *= (time_diff / abs_time_diff);
//     //     double str_delta = time_delta *
//     //         GetError() *
//     //         zxlb::SYN_STRENGTH_LEARNING_RATE;
//     //     cur_strength += GetStrengthDelta() * str_delta;
//     // }

//     if(bap==-1) return;

//     double time_diff = static_cast<double>(GetCurSpike() - bap);
//     double abs_time_diff = std::abs(time_diff);
//     //if(abs_time_diff<=zxlb::LEARNING_WINDOW_SYN_STRENGTH) {
//         double time_delta = (
//             2.0 * (abs_time_diff + zxlb::LEARNING_WINDOW_SYN_STRENGTH) / 
//                     zxlb::LEARNING_WINDOW_SYN_STRENGTH
//             ) *
//             std::exp(-abs_time_diff / zxlb::LEARNING_WINDOW_SYN_STRENGTH) -
//             1.0;
//         // if(abs_time_diff != 0.0)
//         //     time_delta *= (time_diff / abs_time_diff);
//         double str_delta = time_delta *
//             GetError() *
//             zxlb::SYN_STRENGTH_LEARNING_RATE *
//             polarity;
//         cur_strength += GetStrengthDelta() * str_delta;
//     //}
// }

// void Synapse::ChangeStrengthPost(i64 time) {
//     // lw: learning window based on distance from soma
//     // See: Fromke 2005: Spike-timing-dependent synaptic plasticity depends on dendritic location
//     // double lw = zxlb::LEARNING_WINDOW_SYN_STRENGTH; //(location.Rad() / zxlb::MAX_RADIUS) * zxlb::LEARNING_WINDOW_SYN_STRENGTH;
//     // double time_diff = static_cast<double>(bap - time_cur_spike);
//     // double abs_time_diff = std::abs(time_diff);
//     // if(abs_time_diff<=zxlb::LEARNING_WINDOW_SYN_STRENGTH) {
//     //     double time_delta = (
//     //         (abs_time_diff + lw) / 
//     //                 lw
//     //         ) *
//     //         std::exp(-abs_time_diff / lw);
//     //     if(abs_time_diff != 0.0)
//     //         time_delta *= (time_diff / abs_time_diff);
//     //     double str_delta = time_delta *
//     //         GetError() *
//     //         zxlb::SYN_STRENGTH_LEARNING_RATE;
//     //     cur_strength += GetStrengthDelta() * str_delta;
//     // }

//     if(bap==-1) return;

//     //(location.Rad() / zxlb::MAX_RADIUS) * zxlb::LEARNING_WINDOW_SYN_STRENGTH;
//     double time_diff = static_cast<double>(bap - GetCurSpike());
//     double abs_time_diff = std::abs(time_diff);
//     //if(abs_time_diff<=zxlb::LEARNING_WINDOW_SYN_STRENGTH) {
//         double time_delta = (
//             -2.0*(abs_time_diff + zxlb::LEARNING_WINDOW_SYN_STRENGTH) / 
//                     zxlb::LEARNING_WINDOW_SYN_STRENGTH
//             ) *
//             std::exp(-abs_time_diff / zxlb::LEARNING_WINDOW_SYN_STRENGTH) + 1.0;
//         // if(abs_time_diff != 0.0)
//         //     time_delta *= (time_diff / abs_time_diff);
//         double str_delta = time_delta *
//             GetError() *
//             zxlb::SYN_STRENGTH_LEARNING_RATE *
//             polarity;
//         cur_strength += GetStrengthDelta() * str_delta;
//     //}
// }




void Synapse::ChangeStrengthPre(
    i64 pre_spike_time,
    i64 post_spike_time,
    ConnectionMatrix & cm
) {
    if(!pre_spike_time || !post_spike_time) return;

    double delta = zxlb::C_SYN_STR_PRE /
        (
            std::pow(( (pre_spike_time-post_spike_time) - dendrite_path_length ) / zxlb::B_SYN_STR_PRE, 2.0) +
            1.0
        ) - 1.0;
    cur_strength -= delta*error;
}

void Synapse::ChangeStrengthPost(
    i64 pre_spike_time,
    i64 post_spike_time_cur,
    i64 post_spike_time_prev,
    ConnectionMatrix & cm
) {
    if(
        !pre_spike_time ||
        !post_spike_time_prev ||
        !post_spike_time_cur
    ) return;
    
    // Enforces the order of post-pre-post vs pre-post-post.
    if(pre_spike_time >= post_spike_time_prev) {
        double delta_prev = zxlb::C_SYN_STR_POST /
            (
                std::pow(
                    (( pre_spike_time - post_spike_time_prev) - dendrite_path_length) /
                        zxlb::B_SYN_STR_POST,
                    2.0
                ) + 1.0
            );

        double delta_cur = zxlb::C_SYN_STR_POST /
            (
                std::pow(
                    (( post_spike_time_cur - pre_spike_time) - dendrite_path_length) /
                        zxlb::B_SYN_STR_POST,
                    2.0
                ) + 1.0
            );

        cur_strength += delta_cur*delta_prev;
    }

    
}

void Synapse::ChangeStrengthPre_AD(i64 time, i64 cur_neuron_spike, double _error, ConnectionMatrix & cm) {
    this->error = _error;
    double delta = zxlb::C_SYN_STR / 
        (std::pow(((time-cur_neuron_spike) - dendrite_path_length) * zxlb::B_SYN_STR, 2.0) + 1.0);
    delta = delta * _error;// * zxlb::SYN_STRENGTH_LEARNING_RATE;
    cur_strength = cur_strength + delta;
    //cm[ca.pre_layer][ca.pre_neuron].SetDownStreamErrorRate(neuron_id,delta);
}

void Synapse::ChangeStrengthPost_AD(i64 time, double _error, ConnectionMatrix & cm, vec<i64> & comp_spikes) {
    // this->error = _error;
    // double delta = zxlb::C_SYN_STR / 
    //     (std::pow(((time-time_cur_spike) - dendrite_path_length) * zxlb::B_SYN_STR, 2.0) + 1.0);
    // delta = delta * _error;// * zxlb::SYN_STRENGTH_LEARNING_RATE;
    // cur_strength = cur_strength + delta;
    // //cm[ca.pre_layer][ca.pre_neuron].SetDownStreamErrorRate(neuron_id,delta);

    if(is_comp_root) {
        double delta = zxlb::C_SYN_STR / 
            (std::pow(((time-comp_spikes[compartment]) - dendrite_path_length) * zxlb::B_SYN_STR, 2.0) + 1.0);
        delta = delta * error;
        cur_comp_strength += delta;
    }
}


void Synapse::ChangeStrengthCompartment(i64 time, double _error, vec<Synapse> & syns) {
    this->error = _error;

    double delta = 0.0;

    for(lst<i64>::iterator it = comp_spike_queue.begin(); it != comp_spike_queue.end(); ) {
        if(time-*it > zxlb::MAX_STR_TEMPORAL_DIFFERENCE*2.0) {
            it = comp_spike_queue.erase(it);
        } else {
            for(lst<i64>::iterator it2 = spike_queue.begin(); it2 != spike_queue.end(); it2++) {
                delta = zxlb::C_SYN_STR / 
                        (std::pow((std::abs(*it-*it2)) * zxlb::B_SYN_STR, 2.0) + 1.0);
            }
            it++;
        }
        
    }
    delta = delta * _error * zxlb::SYN_STRENGTH_LEARNING_RATE;
    cur_strength = cur_strength + delta;

    for(sizet i = 0; i < children.size(); i++) {
        if(compartment==syns[children[i]].GetCompartment()) {
            syns[children[i]].ChangeStrengthCompartment_Within(time,_error,syns,comp_spike_queue);
        } else {
            syns[children[i]].ChangeStrengthCompartment_Between(time, _error,comp_spike_queue,compartment_length);
        }
    }
}


void Synapse::ChangeStrengthCompartment_Within(i64 time, double _error,vec<Synapse> & syns,lst<i64> & comp_spikes) {

    this->error = _error;

    double delta = 0.0;
    for(lst<i64>::iterator it = comp_spikes.begin(); it != comp_spikes.end(); it++) {
        
        for(lst<i64>::iterator it2 = spike_queue.begin(); it2 != spike_queue.end(); it2++) {
            delta = zxlb::C_SYN_STR / 
                    (std::pow((std::abs(*it-*it2)-compartment_length) * zxlb::B_SYN_STR, 2.0) + 1.0);
        }

    }
    delta = delta * _error * zxlb::SYN_STRENGTH_LEARNING_RATE;
    cur_strength = cur_strength + delta;

    for(sizet i = 0; i < children.size(); i++) {
        if(compartment==syns[children[i]].GetCompartment()) {
            syns[children[i]].ChangeStrengthCompartment_Within(time,_error,syns,comp_spikes);
        } else {
            syns[children[i]].ChangeStrengthCompartment_Between(time, _error,comp_spikes,compartment_length);
        }
    }

}

void Synapse::ChangeStrengthCompartment_Between(i64 time, double _error,lst<i64> & comp_spikes,double compdist) {
    this->error = _error;

    double delta = 0.0;
    for(lst<i64>::iterator it = comp_spikes.begin(); it != comp_spikes.end(); it++) {
        
        for(lst<i64>::iterator it2 = comp_spike_queue.begin(); it2 != comp_spike_queue.end(); it2++) {
            delta = zxlb::C_SYN_STR / 
                    (std::pow((std::abs(*it-*it2)-(compdist+dist_to_parent)) * zxlb::B_SYN_STR, 2.0) + 1.0);
        }

    }
    delta = delta * _error * zxlb::SYN_STRENGTH_LEARNING_RATE;
    cur_comp_strength = cur_comp_strength + delta;
}

void Synapse::ChangeStrengthPre_Simple(i64 time, double _error, ConnectionMatrix & cm) {
    if(bap<0) return;

    double time_diff = static_cast<double>(time - bap);

    if(time_diff <= zxlb::MAX_STR_TEMPORAL_DIFFERENCE) {
        double delta =
            std::exp(-zxlb::STR_LEARNING_DECAY_PRE * time_diff) *
            _error * 
            zxlb::SYN_STRENGTH_LEARNING_RATE;
        
        cur_strength = cur_strength - delta;
    }
}
void Synapse::ChangeStrengthPost_Simple(i64 time, double _error, ConnectionMatrix & cm) {
    if(time_cur_spike<0) return;

    double time_diff = static_cast<double>(time - time_cur_spike);

    if(time_diff <= zxlb::MAX_STR_TEMPORAL_DIFFERENCE) {
        double delta =
            std::exp(-zxlb::STR_LEARNING_DECAY_POST * time_diff) *
            _error * 
            zxlb::SYN_STRENGTH_LEARNING_RATE;

        cur_strength = cur_strength + delta;
    }
}

void Synapse::SetBAP(i64 time) {
    bap = time;
}

double Synapse::psi(double sig) {
    return (sig >= 0.0) ? sig : 0.0;
}

double Synapse::GetInput(i64 time, vec<Synapse> & syns, i64 parent_id, double compdist) {
    
    double sig = GetSignal_Within(time,compdist);

    for(sizet i = 0; i < children.size(); i++) {

        // If the next child is in our compartment, use the compdist
        // Else reset compdist to the distance to the next synapse.
        if(compartment==syns[children[i]].GetCompartment()) {
            // sig += syns[children[i]].GetInput(time,syns,parent,compdist);
            sig += syns[children[i]].GetInput(time,syns,parent,compdist+syns[i].dist_to_parent);
        } else {
            // sig += psi(syns[children[i]].GetInput(time,syns,parent,syns[i].dist_to_parent));
            sig += psi(syns[children[i]].GetInput(time,syns,parent,0.0));
        }
    }

    return sig;
}



double Synapse::GetInput_Within(
    i64 time, 
    vec<Synapse> & syns,
    vec<i64> & comp_spikes,
    vec<i64> & syns_per_comp,
    vec<pair<double,double>> & comp_izh,
    tmps::NeuronType & nt,
    i64 parent_id, 
    double compdist,
    double error,
    bool train_str
) {

        double sig = GetSignal_Within(time,compdist+dist_to_parent);

        for(sizet i = 0; i < children.size(); i++) {

            if(compartment==syns[children[i]].GetCompartment()) {
                sig += syns[children[i]].GetInput_Within(
                    time, syns, comp_spikes,syns_per_comp,comp_izh,nt,parent,
                    compdist+dist_to_parent,error,train_str
                );
            } else {
                sig += syns[children[i]].GetInput_Between(
                    time, syns, comp_spikes,syns_per_comp,comp_izh,nt,parent,
                    0.0,error,train_str
                );
            }
        }

        return sig;

}

double Synapse::GetInput_Between(
    i64 time, 
    vec<Synapse> & syns,
    vec<i64> & comp_spikes,
    vec<i64> & syns_per_comp,
    vec<pair<double,double>> & comp_izh,
    tmps::NeuronType & nt,
    i64 parent_id, 
    double compdist,
    double error,
    bool train_str
) {
    double sig = GetInput_Within(
        time,
        syns,
        comp_spikes,
        syns_per_comp,
        comp_izh,
        nt,
        parent,
        0.0,
        error,
        train_str
    );

    // sig /= syns_per_comp[compartment];

    double vpre = comp_izh[compartment].first;
    double upre = comp_izh[compartment].second;

    comp_izh[compartment].first = vpre +
    (
        nt.k * (vpre - nt.vr) * (vpre - nt.vt) -
        upre +
        sig + 50.0
    ) / nt.cap;

    comp_izh[compartment].second = upre +
        nt.a *
        (nt.b * (vpre - nt.vr) - upre);

    if(comp_izh[compartment].first > nt.vpeak) {
        comp_izh[compartment].first = nt.c;
        comp_izh[compartment].second += nt.d;
        comp_spikes[compartment]=time;
        comp_spike_queue.push_back(time);
        while(!comp_spike_queue.empty() && time-comp_spike_queue.front() > zxlb::MAX_STR_TEMPORAL_DIFFERENCE) {
            comp_spike_queue.pop_front();
        }
        if(train_str) ChangeStrengthCompartment(time, error, syns);
    }

    return GetSignal_Between(time, comp_spikes[compartment],dist_to_parent,syns_per_comp[compartment]);
}





// The GOOD ONE
double Synapse::GetSignal_Within(i64 time, double compdist) {
    double sig = 0.0;
    i64 diff_time = 0;
    for(
        lst<i64>::iterator it = spike_queue.begin();
        it != spike_queue.end();
    ) {
        diff_time = time - *it;
        if(diff_time > zxlb::MAX_RAD_TEMPORAL_DIFFERENCE*2.0) {
            it = spike_queue.erase(it);
        } else {
            // if(GetCurSpike()<0) return 0.0;
            // else {
                sig+= 1.0 /
                    (std::pow(((diff_time)-compdist)*zxlb::B_SYN_SIG_SELF, 2.0) + 1.0);
            // }
            it++;
        }
    }
    return sig * GetStrength();
}


double Synapse::GetSignal_Between(i64 time, i64 comp_spike_time, double compdist, i64 syns_in_comp) {
    double sig = 0.0;
    i64 diff_time = 0;
    double s = (cur_comp_strength*max_comp_strength) / 
            (std::abs(cur_comp_strength)+abs_max_comp_strength);

    for(
        lst<i64>::iterator it = comp_spike_queue.begin();
        it != comp_spike_queue.end();
    ) {
        diff_time = time - *it;
        if(diff_time > zxlb::MAX_RAD_TEMPORAL_DIFFERENCE*2.0) {
            it = comp_spike_queue.erase(it);
        } else {

            sig+= 1.0 /
                (std::pow(((diff_time)-compdist)*zxlb::B_NEU_DEN_OUT, 2.0) + 1.0);
            
            it++;
            
        }
    }
    
    return sig * s;//(s*syns_in_comp);
}

double Synapse::GetSignal_Simple(i64 time) {
    return signal[(time+1)%2];
}
double Synapse::GetSignal_In(i64 time) {

    return signal[(time+1)%2] *
        GetStrength() +
        upstream_signal;

}
double Synapse::GetSignal_Out(i64 time) {

    double s = signal[(time+1)%2] *
        GetStrength() +
        upstream_signal;

    // Compartmental transfer
    // if(s > zxlb::MIN_COMPARTMENT_SIGNAL_FOR_SOMA) {
    //     s -= zxlb::MIN_COMPARTMENT_SIGNAL_FOR_SOMA;
    s = 2.0 / (1.0 + std::exp(-s*zxlb::COMPARTMENT_OUT_EXPONENT)) - 1.0;
    // }
    
    return s;
}

double Synapse::GetSignalWitch_Self(i64 time) {
    if(GetCurSpike()<0) return 0.0;
    else return GetStrength() * zxlb::C_SYN_SIG_SELF /
            (std::pow((time-GetCurSpike())/zxlb::B_SYN_SIG_SELF, 2.0) + 1.0);
}

double Synapse::GetSignalWitchMod(i64 time, double dist, double spike_time_diff) {
    
    return zxlb::C_SYN_SIG_MOD /
        (std::pow(( spike_time_diff - dist) / zxlb::B_SYN_SIG_MOD, 2.0) + 1.0);

}
// double Synapse::GetSignalWitchMod_Out(i64 time, double dist, double spike_time_diff) {
//     double s = GetSignalWitch_In(time,rad_dist,ang_dist,spike_time_diff);
//     return 2.0 / (1.0 + std::exp(-s*zxlb::COMPARTMENT_OUT_EXPONENT)) - 1.0;
// }

double Synapse::GetDendritePathLength() {
    return dendrite_path_length;
}


void Synapse::SetCompartment(i64 comp_id) {
    compartment = comp_id;
}
i64 Synapse::GetCompartment() {
    return compartment;
}
void Synapse::SetCompartmentLength(double len) {
    compartment_length = len;
}
double Synapse::GetCompartmentLength() {
    return compartment_length;
}



//------------------------------------------------------------------------
    // Dendrite connections
vec<i64> * Synapse::GetChildren() {
    return &children;
}
i64 Synapse::GetParent() {
    return parent;
}

//------------------------------------------------------------------------
    // Upstream signal
double Synapse::GetUpstreamSignal() {
    return upstream_signal;
}
void Synapse::SetUpstreamSignal(double sig) {
    upstream_signal = sig;
}
void Synapse::ResetUpstreamSignal() {
    upstream_signal = 0.0;
}
void Synapse::AddToUpstreamSignal(double sig) {
    upstream_signal += sig;
}
bool Synapse::GetUpstreamEval() {
    return upstream_eval;
}
void Synapse::SetUpstreamEval(bool b) {
    upstream_eval = b;
}


vec<DenSig> Synapse::GetDendriticSignals() {
    return densigs;
}
void Synapse::AddDendriticSignal(DenSig sig) {
    densigs.push_back(sig);
}
void Synapse::AddDendriticSignals(vec<DenSig> sigs) {
    densigs = std::move(sigs);
    // densigs.insert(densigs.end(), sigs.begin(), sigs.end());
}
void Synapse::ResetDendriticSignals() {
    densigs.clear();
}
int Synapse::GetCompartmentSize() {
    return compartment_size;
}
void Synapse::AddToCompartmentSize(int amt) {
    compartment_size += amt;
}
void Synapse::ResetCompartmentSize() {
    compartment_size = 0;
}



bool Synapse::Update(i64 time, Writer * writer, ConnectionMatrix & cm) {
    

    // SetSignal(time, cm[ca.pre_layer][ca.pre_neuron].output);

    //error = cm[ca.post_layer][ca.post_neuron].GetErrorRateRaw();

    //SaveData(time);

    if(cm[ca.pre_layer][ca.pre_neuron].just_spiked) {
        SetCurSpike(time);
        spikes++;
        return true;
    } else {
        return false;
    }
}

void Synapse::ResetWriteData() {
    data = std::make_unique<SynapseData>();
    data->id = "SYNAPSE_"+std::to_string(layer_id)+"_"+std::to_string(neuron_id)+"_"+std::to_string(id);
    data->data_size = 0;
    data->synapse_id = id;
    data->neuron_id = neuron_id;
    data->layer_id = layer_id;
}

void Synapse::CleanupData(Writer * writer) {
    if(data->data_size > 0) {
        writer->AddSynapseData(std::move(data));
        ResetWriteData();
    }
}

void Synapse::SaveData(i64 time) {
    //if(record_data && time%record_interval==0) {
        data->data_size++;
        data->input.push_back(signal[time%2]);
        data->strength.push_back(cur_strength);
        data->error.push_back(error);
        data->locations.push_back(location);
        data->spikes.push_back(spikes);
        spikes = 0;     // RESET SPIKES.
        data->time_indexes.push_back(time);
        data->parent_id.push_back(parent);
        vec<int> c_ids;
        for(std::size_t i = 0; i < children.size(); i++) {
            c_ids.push_back(children[i]);
        }
        data->children_ids.push_back(c_ids);
        data->compartment_ids.push_back(compartment);
        data->comp_strength.push_back(cur_comp_strength);
    //}
}

void Synapse::WriteData(Writer * writer) {

    if(record_data) {
        writer->AddSynapseData(std::move(data));
        ResetWriteData();
    }

}

