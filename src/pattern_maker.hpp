#pragma once

#include<cassert>
#include<algorithm>
#include<iostream>

#include"zxlb.hpp"

struct PatternTemplate {
    str label;
    sizet size;
    sizet amount;
    sizet amount_per_iteration;
    vec<double> mus;
    vec<double> stdevs;
};


struct Pattern {
    vec<double> data;
    str label;
};


class PatternMaker {
private:
    sizet pattern_size;
    umap<str, vec<Pattern>> patterns;
    umap<str, sizet> patterns_per_iteration;
public:
    PatternMaker(sizet size);
    void AddPatterns(vec<PatternTemplate> & templates, RNG & rng);
    vec<Pattern> & GetAllWithLabel(str label);
    vec<Pattern*> GetRandomOneOfEachOrdered(RNG & rng);
    vec<Pattern*> GetRandomOneOfEachRandomized(RNG & rng);
    vec<str> GetAllLabels();
    sizet GetIterationSize();
    vec<vec<Pattern>> GetDataAsIteration(
        vec<str> labels,
        sizet num_iterations,
        RNG & rng
    );
};

PatternMaker BuildPattern001(RNG & rng);
PatternMaker BuildPattern002(RNG & rng);