#include"synapse.hpp"

Synapse::Synapse()
    :   id(0),
        max_strength(0.0),
        abs_max_strength(0.0),
        cur_strength(0.0),
        location(VecS(0.0, 0.0, 0.0))
{
    parent = -1;
    signal[0] = 0.0;
    signal[1] = 0.0;
    bap[0] = 0.0;
    bap[1] = 0.0;
    error = 0.0;
    time_error = 0;
    time_cur_spike = 0;
    time_pre_spike = 0;
    record_data = false;
    record_interval = 1;
    record_data_size = 1000;
    dendrite_path_length = 0.0;

    upstream_signal = 0.0;
    upstream_eval = false;

    ResetWriteData();
}

Synapse::Synapse(
    VecS _loc,
    double _max_strength,
    double _cur_strength
)   :   id(0),
        max_strength(_max_strength),
        abs_max_strength(std::abs(_max_strength)),
        cur_strength(_cur_strength),
        location(_loc)
{
    parent = -1;
    signal[0] = 0.0;
    signal[1] = 0.0;
    bap[0] = 0.0;
    bap[1] = 0.0;
    error = 0.0;
    time_error = 0;
    time_cur_spike = 0;
    time_pre_spike = 0;
    record_data = false;
    record_interval = 1;
    record_data_size = 1000;
    dendrite_path_length = 0.0;

    upstream_signal = 0.0;
    upstream_eval = false;

    ResetWriteData();
}

void Synapse::Reset() {
    signal[0] = signal[1] = 0.0;
    bap[0] = bap[1] = 0.0;
    error = 0.0;
    time_error = 0;
    time_cur_spike = 0;
    time_pre_spike = 0;
}

double Synapse::GetError() {
    return error;
}

void Synapse::SetID(i64 _id) {
    id = _id;
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
    return (cur_strength * max_strength) / 
        (std::abs(cur_strength) + abs_max_strength);
}

double Synapse::GetSignal(i64 time) {
    return signal[(time+1)%2];
}

double Synapse::GetSignalWithStrength(i64 time) {
    // Needs more work. Prev version modified the signal by the strength.
    return (
        signal[(time+1)%2] *
        (
            (cur_strength * max_strength) / 
            (std::abs(cur_strength) + abs_max_strength)
        )
    );
}
double Synapse::GetSignalFull(i64 time) {
    return (
        (signal[(time+1)%2] + upstream_signal) *
        (
            (cur_strength * max_strength) / 
            (std::abs(cur_strength) + abs_max_strength)
        )
    );
}



double Synapse::GetDendritePathLength() {
    return dendrite_path_length;
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

    error = cm[ca.post_layer][ca.post_neuron].GetErrorRate();

    if(record_data && time%record_interval==0) {
        WriteData(time,writer);
    }

    if(cm[ca.pre_layer][ca.pre_neuron].just_spiked) {
        SetCurSpike(time);
        return true;
    } else {
        return false;
    }
}

void Synapse::ResetWriteData() {
    data = std::make_unique<SynapseData>();
    data->id = "SYNAPSE_"+std::to_string(id);
    data->data_size = 0;
}

void Synapse::CleanupData(Writer * writer) {
    if(data->data_size > 0) {
        writer->AddSynapseData(std::move(data));
        ResetWriteData();
    }
}

void Synapse::WriteData(i64 time, Writer * writer) {
    if(data->data_size==record_data_size) {
        writer->AddSynapseData(std::move(data));
        ResetWriteData();
    }

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
}

