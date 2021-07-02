#pragma once

#include"zxlb.hpp"
#include"vec_s.hpp"

struct ExampleData {
    i64 iteration;
    i64 example;
    str label;
};

struct SynapseData {
    str id;
    i64 synapse_id;
    i64 neuron_id;
    i64 layer_id;
    std::size_t data_size;
    vec<i64> time_indexes;
    vec<double> input;
    vec<int> parent_id;
    vec<vec<int>> children_ids;
    vec<VecS> locations;
    vec<double> strength;
    vec<i64> spikes;
    vec<double> error;
};

struct NeuronData {
    str id;
    i64 neuron_id;
    i64 layer_id;
    std::size_t data_size;
    vec<i64> time_indexes;
    vec<VecS> locations;
    vec<double> v;
    vec<double> u;
    vec<i64> spike_times;
    vec<double> output;
    vec<double> input;
    vec<double> error;
};