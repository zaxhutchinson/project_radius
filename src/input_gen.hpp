#pragma once

#include"zxlb.hpp"
#include"neuron.hpp"

struct InputGenerator {
    vec<double> pattern;
    vec<double> seed;
    double GetInput(sizet index, i64 time);
};

struct InputGenerator_Sine : public InputGenerator {
    double GetInput(sizet index, i64 time);
};

struct InputGenerator_Cosine : public InputGenerator {
    double GetInput(sizet index, i64 time);
};