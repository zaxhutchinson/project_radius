#pragma once

#include"zxlb.hpp"
#include"neuron.hpp"
#include"writer.hpp"


void UpdateNeurons(i64 time, Writer * writer, vec<Neuron> & neurons, double error);

void BuildDendrites(vec<Neuron> & neurons);