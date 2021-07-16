#pragma once

#include<random>
#include<iostream>

#include"zxlb.hpp"
#include"neuron.hpp"

struct InputGenerator {
    vec<double> pattern;
    virtual double GetInput(sizet index, i64 time, RNG & rng);
    virtual void Reset();
};

struct InputGenerator_Sine : public InputGenerator {
    vec<double> seed;
    double GetInput(sizet index, i64 time, RNG & rng) override;
};

struct InputGenerator_Cosine : public InputGenerator {
    vec<double> seed;
    double GetInput(sizet index, i64 time, RNG & rng) override;
};

struct InputGenerator_Poisson : public InputGenerator {
    str id;
    vec<double> rate;
    vec<double> signal;
    vec<double> strength;
    vec<double> decay;
    std::uniform_real_distribution<double> dist;
    double GetInput(sizet index, i64 time, RNG & rng) override;
    void Reset() override;
};