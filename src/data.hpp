#pragma once

#include"zxlb.hpp"
#include"vec_s.hpp"

struct SynapseData {
    str id;
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
    std::size_t data_size;
    vec<i64> time_indexes;
    vec<VecS> locations;
    vec<double> v;
    vec<double> u;
    vec<vec<i64>> spike_times;
    vec<double> output;
};