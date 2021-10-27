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
    time_cur_spike = 0;
    time_pre_spike = 0;
    spikes = 0;
    record_data = false;
    record_interval = 1;
    record_data_size = 1000;
    dendrite_path_length = 0.0;
    compartment = 0;
    compartment_length = 0;
    upstream_signal = 0.0;
    upstream_eval = false;

    children.reserve(10);


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
    time_cur_spike = 0;
    time_pre_spike = 0;
    spikes = 0;
    record_data = false;
    record_interval = 1;
    record_data_size = 1000;
    dendrite_path_length = 0.0;
    compartment = 0;
    compartment_length = 0;
    upstream_signal = 0.0;
    upstream_eval = false;

    children.reserve(10);
}

void Synapse::Reset() {
    signal[0] = signal[1] = 0.0;
    bap = -1;
    error = 0.0;
    time_error = 0;
    time_cur_spike = 0;
    time_pre_spike = 0;
    spikes = 0;
    upstream_eval = false;
    upstream_signal =0.0;
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
    time_pre_spike = time_cur_spike;
    time_cur_spike = time;
}

void Synapse::SetDendritePathLength(double path) {
    dendrite_path_length = path;
}

double Synapse::GetStrength() {
    return cur_strength / 
        (std::abs(cur_strength) + abs_max_strength);
}
double Synapse::GetStrengthDelta() {
    return abs_max_strength - (std::abs(cur_strength) * abs_max_strength) / (abs_max_strength + std::abs(cur_strength));
}

void Synapse::ChangeStrengthPre(i64 time) {
    
    // double time_diff = static_cast<double>(bap - time_cur_spike);
    // double abs_time_diff = std::abs(time_diff);
    // if(abs_time_diff<=zxlb::LEARNING_WINDOW_SYN_STRENGTH) {
    //     double time_delta = (
    //         (abs_time_diff + zxlb::LEARNING_WINDOW_SYN_STRENGTH) / 
    //                 zxlb::LEARNING_WINDOW_SYN_STRENGTH
    //         ) *
    //         std::exp(-abs_time_diff / zxlb::LEARNING_WINDOW_SYN_STRENGTH);
    //     if(abs_time_diff != 0.0)
    //         time_delta *= (time_diff / abs_time_diff);
    //     double str_delta = time_delta *
    //         GetError() *
    //         zxlb::SYN_STRENGTH_LEARNING_RATE;
    //     cur_strength += GetStrengthDelta() * str_delta;
    // }

    if(bap==-1) return;

    double time_diff = static_cast<double>(time_cur_spike - bap);
    double abs_time_diff = std::abs(time_diff);
    //if(abs_time_diff<=zxlb::LEARNING_WINDOW_SYN_STRENGTH) {
        double time_delta = (
            2.0 * (abs_time_diff + zxlb::LEARNING_WINDOW_SYN_STRENGTH) / 
                    zxlb::LEARNING_WINDOW_SYN_STRENGTH
            ) *
            std::exp(-abs_time_diff / zxlb::LEARNING_WINDOW_SYN_STRENGTH) -
            1.0;
        // if(abs_time_diff != 0.0)
        //     time_delta *= (time_diff / abs_time_diff);
        double str_delta = time_delta *
            GetError() *
            zxlb::SYN_STRENGTH_LEARNING_RATE *
            polarity;
        cur_strength += GetStrengthDelta() * str_delta;
    //}
}

void Synapse::ChangeStrengthPost(i64 time) {
    // lw: learning window based on distance from soma
    // See: Fromke 2005: Spike-timing-dependent synaptic plasticity depends on dendritic location
    // double lw = zxlb::LEARNING_WINDOW_SYN_STRENGTH; //(location.Rad() / zxlb::MAX_RADIUS) * zxlb::LEARNING_WINDOW_SYN_STRENGTH;
    // double time_diff = static_cast<double>(bap - time_cur_spike);
    // double abs_time_diff = std::abs(time_diff);
    // if(abs_time_diff<=zxlb::LEARNING_WINDOW_SYN_STRENGTH) {
    //     double time_delta = (
    //         (abs_time_diff + lw) / 
    //                 lw
    //         ) *
    //         std::exp(-abs_time_diff / lw);
    //     if(abs_time_diff != 0.0)
    //         time_delta *= (time_diff / abs_time_diff);
    //     double str_delta = time_delta *
    //         GetError() *
    //         zxlb::SYN_STRENGTH_LEARNING_RATE;
    //     cur_strength += GetStrengthDelta() * str_delta;
    // }

    if(bap==-1) return;

    //(location.Rad() / zxlb::MAX_RADIUS) * zxlb::LEARNING_WINDOW_SYN_STRENGTH;
    double time_diff = static_cast<double>(bap - time_cur_spike);
    double abs_time_diff = std::abs(time_diff);
    //if(abs_time_diff<=zxlb::LEARNING_WINDOW_SYN_STRENGTH) {
        double time_delta = (
            -2.0*(abs_time_diff + zxlb::LEARNING_WINDOW_SYN_STRENGTH) / 
                    zxlb::LEARNING_WINDOW_SYN_STRENGTH
            ) *
            std::exp(-abs_time_diff / zxlb::LEARNING_WINDOW_SYN_STRENGTH) + 1.0;
        // if(abs_time_diff != 0.0)
        //     time_delta *= (time_diff / abs_time_diff);
        double str_delta = time_delta *
            GetError() *
            zxlb::SYN_STRENGTH_LEARNING_RATE *
            polarity;
        cur_strength += GetStrengthDelta() * str_delta;
    //}
}

void Synapse::SetBAP(i64 time) {
    bap = time;
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
    s = 2.0 / (1.0 + std::exp(-s*zxlb::COMPARTMENT_OUT_EXPONENT)) - 1.0;
    
    return s;
}

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

bool Synapse::Update(i64 time, Writer * writer, ConnectionMatrix & cm) {
    

    SetSignal(time, cm[ca.pre_layer][ca.pre_neuron].output);

    error = cm[ca.post_layer][ca.post_neuron].GetErrorRateRaw();

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
    //}
}

void Synapse::WriteData(Writer * writer) {

    if(record_data) {
        writer->AddSynapseData(std::move(data));
        ResetWriteData();
    }

}

