#pragma once

#include<algorithm>

#include"zxlb.hpp"
#include"network.hpp"
#include"writer.hpp"


// Change update neuron to UPDATE NETWORK method.

void UpdateNetwork(
    i64 time, 
    Writer * writer, 
    Network * network, 
    vec<double> & inputs,
    vec<double> & outputs,
    double error
);

// void UpdateNeurons(i64 time, Writer * writer, vec<Neuron> & neurons, double error);

void BuildDendrites(vec<Neuron> & neurons);