#include"synapse.hpp"


Synapse::Synapse(
    int _id,
    VecS _loc,
    double _max_strength,
    double _cur_strength
)   :   id(_id),
        max_strength(_max_strength),
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

    ResetWriteData();
}

void Synapse::SetSignal(i64 time, double amt) {
    signal[time%2]=amt;

}

void Synapse::SetCurSpike(i64 time) {
    time_pre_spike = time_cur_spike;
    time_cur_spike = time;
}


double Synapse::GetSignal(i64 time) {
    // Needs more work. Prev version modified the signal by the strength.
    return signal[(time+1)%2];
}





void Synapse::Update(i64 time, Writer * writer) {
    if(record_data && time%record_interval==0) {
        WriteData(time,writer);
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

