#pragma once

#include<algorithm>

#include"zxlb.hpp"
#include"network.hpp"
#include"writer.hpp"


void UpdateNeurons(i64 time, Writer * writer, vec<Neuron> & neurons, double error);

void BuildDendrites(vec<Neuron> & neurons);