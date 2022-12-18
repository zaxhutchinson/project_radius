#pragma once

#include<mutex>
#include<thread>
#include<fstream>
#include<string>
#include<iostream>
#include<filesystem>

#include<omp.h>

#include"zxlb.hpp"
#include"data.hpp"

struct Writer {
    str write_directory;
    bool run;
    
    mtx mtx_example;
    mtx mtx_synapse;
    mtx mtx_neuron;
    quptr<ExampleData> example_data;
    quptr<SynapseData> synapse_data;
    quptr<NeuronData> neuron_data;

    std::thread t_write;

    Writer(str dir);

    void StartRecording();
    void StopRecording();

    void WriteThread();
    bool HasDataToWrite();

    void AddExampleData(uptr<ExampleData> data);
    void WriteExampleData();

    void AddSynapseData(uptr<SynapseData> data);
    void WriteSynapseData();

    void AddNeuronData(uptr<NeuronData> data);
    void WriteNeuronData();

};