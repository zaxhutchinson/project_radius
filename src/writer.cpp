#include"writer.hpp"


Writer::Writer(str dir) : write_directory(dir) {
    run = false;
}

void Writer::StartRecording() {
    run = true;
    while(t_write.joinable()) ;
    t_write = std::thread(&Writer::WriteThread, this);
}

void Writer::StopRecording() {
    run = false;
    if(t_write.joinable()) {
        t_write.join();
    }
}


void Writer::WriteThread() {
    while(run || HasDataToWrite()) {
        WriteSynapseData();


        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

bool Writer::HasDataToWrite() {
    return (!synapse_data.empty());
}


void Writer::AddSynapseData(uptr<SynapseData> data) {
    mtx_synapse.lock();
    synapse_data.push(std::move(data));
    mtx_synapse.unlock();
}

void Writer::WriteSynapseData() {
    if(!synapse_data.empty()) {
        
        mtx_synapse.lock();
        uptr<SynapseData> data = std::move(synapse_data.front());
        synapse_data.pop();
        mtx_synapse.unlock();

        str fname(write_directory+"/"+data->id);
        std::ofstream ofs(fname, std::ios::out | std::ios::app);

        for(std::size_t i = 0; i < data->data_size; i++) {
            ofs << data->time_indexes[i] << ","
                << data->locations[i].Lat() << ","
                << data->locations[i].Lon() << ","
                << data->locations[i].Rad() << ","
                << data->strength[i] << ","
                << data->spikes[i] << ","
                << data->input[i] << ","
                << data->error[i] << ","
                << data->parent_id[i] << ",";
                str children("[");
                for(std::size_t k = 0; k < data->children_ids[i].size(); k++) {
                    children += data->children_ids[i][k] + ",";
                }
                children = children.substr(0,children.size()-1) + "]";
                ofs << children << "\n";
        }
        ofs.close();
    }
}

void Writer::AddNeuronData(uptr<NeuronData> data) {
    mtx_neuron.lock();
    neuron_data.push(std::move(data));
    mtx_neuron.unlock();
}

void Writer::WriteNeuronData() {
    if(!neuron_data.empty()) {
        mtx_neuron.lock();
        uptr<NeuronData> data = std::move(neuron_data.front());
        neuron_data.pop();
        mtx_neuron.unlock();

        str fname(write_directory+"/"+data->id);
        std::ofstream ofs(fname, std::ios::out | std::ios::app);

        for(std::size_t i = 0; i < data->data_size; i++) { 
            ofs << data->time_indexes[i] << ","
                << data->locations[i].Lat() << ","
                << data->locations[i].Lon() << ","
                << data->locations[i].Rad() << ","
                << data->v[i] << ","
                << data->u[i] << ","
                << data->output[i] << ",";
                str spikes("[");
                for(std::size_t k = 0; k < data->spike_times[i].size(); k++) {
                    spikes += data->spike_times[i][k] + ",";
                }
                spikes = spikes.substr(0,spikes.size()-1) + "]";
                ofs << spikes << "\n";
        }
        ofs.close();
    }
}