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
    while(run) {

        while(HasDataToWrite()) {
            WriteExampleData();
            WriteSynapseData();
            WriteNeuronData();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    while(HasDataToWrite()) {
        WriteExampleData();
        WriteSynapseData();
        WriteNeuronData();
    }
}

bool Writer::HasDataToWrite() {
    return (!synapse_data.empty() || !neuron_data.empty() || !example_data.empty());
}

void Writer::AddExampleData(uptr<ExampleData> data) {
    mtx_example.lock();
    example_data.push(std::move(data));
    mtx_example.unlock();
}

void Writer::WriteExampleData() {
    if(!example_data.empty()) {
        mtx_example.lock();
        uptr<ExampleData> data = std::move(example_data.front());
        example_data.pop();
        mtx_example.unlock();

        str fname(write_directory+"/EXAMPLES");
        std::ofstream ofs(fname, std::ios::out | std::ios::app);

        ofs << data->iteration << " " << data->example << " " << data->label << "\n";
        ofs.close();
    }
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

        ofs << data->layer_id << " " << data->neuron_id << " " << data->synapse_id << "\n";

        ofs << "TIME ";
        for(std::size_t i = 0; i < data->data_size; i++) {
            ofs << data->time_indexes[i] << " ";
        } ofs << "\n";

        ofs << "LAT ";
        for(std::size_t i = 0; i < data->data_size; i++) {
            ofs << data->locations[i].Lat() << " ";
        } ofs << "\n";

        ofs << "LON ";
        for(std::size_t i = 0; i < data->data_size; i++) {
            ofs << data->locations[i].Lon() << " ";
        } ofs << "\n";

        ofs << "RAD ";
        for(std::size_t i = 0; i < data->data_size; i++) {
            ofs << data->locations[i].Rad() << " ";
        } ofs << "\n";

        ofs << "STR ";
        for(std::size_t i = 0; i < data->data_size; i++) {
            ofs << data->strength[i] << " ";
        } ofs << "\n";

        ofs << "SPKS ";
        for(std::size_t i = 0; i < data->data_size; i++) {
            ofs << data->spikes[i] << " ";
        } ofs << "\n";

        ofs << "IN ";
        for(std::size_t i = 0; i < data->data_size; i++) {
            ofs << data->input[i] << " ";
        } ofs << "\n";

        ofs << "ERR ";
        for(std::size_t i = 0; i < data->data_size; i++) {
            ofs << data->error[i] << " ";
        } ofs << "\n";

        ofs << "PID ";
        for(std::size_t i = 0; i < data->data_size; i++) {
            ofs << data->parent_id[i] << " ";
        } ofs << "\n";
        
        ofs << "CID ";
        for(sizet i = 0; i < data->data_size; i++) {
            ofs << "[";
            for(sizet k = 0; k < data->children_ids[i].size(); k++) {
                ofs << data->children_ids[i][k] << " ";
            }
            ofs << "] ";
         } ofs << "\n";

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

        ofs << data->layer_id << " " << data->neuron_id << "\n";

        ofs << "TIME ";
        for(std::size_t i = 0; i < data->data_size; i++) { 
            ofs << data->time_indexes[i] << " ";
        } ofs << "\n";

        ofs << "LAT ";
        for(std::size_t i = 0; i < data->data_size; i++) { 
            ofs << data->locations[i].Lat() << " ";
        } ofs << "\n";

        ofs << "LON ";
        for(std::size_t i = 0; i < data->data_size; i++) { 
            ofs << data->locations[i].Lon() << " ";
        } ofs << "\n";

        ofs << "RAD ";
        for(std::size_t i = 0; i < data->data_size; i++) { 
            ofs << data->locations[i].Rad() << " ";
        } ofs << "\n";

        ofs << "V ";
        for(std::size_t i = 0; i < data->data_size; i++) { 
            ofs << data->v[i] << " ";
        } ofs << "\n";

        ofs << "U ";
        for(std::size_t i = 0; i < data->data_size; i++) { 
            ofs << data->u[i] << " ";
        } ofs << "\n";

        ofs << "OUT ";
        for(std::size_t i = 0; i < data->data_size; i++) { 
            ofs << data->output[i] << " ";
        } ofs << "\n";

        ofs << "IN ";
        for(std::size_t i = 0; i < data->data_size; i++) { 
            ofs << data->input[i] << " ";
        } ofs << "\n";

        ofs << "ERR ";
        for(std::size_t i = 0; i < data->data_size; i++) { 
            ofs << data->locations[i].Lat() << " ";
        } ofs << "\n";

        ofs << "SPKS ";
        for(std::size_t i = 0; i < data->spike_times.size(); i++) {
            ofs << data->spike_times[i] << " ";
        } ofs << "\n";

        ofs.close();
    }
}