#include"input_gen.hpp"


double InputGenerator::GetInput(sizet index, i64 time, RNG & rng) {
    return pattern[index];
}



double InputGenerator_Sine::GetInput(sizet index, i64 time, RNG & rng) {
    return 1.0+std::sin(time/1000.0+seed[index]) * pattern[index];
}

double InputGenerator_Cosine::GetInput(sizet index, i64 time, RNG & rng) {
    return 1.0+std::cos(time/1000.0+seed[index]) * pattern[index];
}

double InputGenerator_Poisson::GetInput(sizet index, i64 time, RNG & rng) {
    if(dist(rng)<rate[index]) {
        signal[index] = strength[index];
    } else {
        signal[index] = signal[index] * decay[index];
    }
    return signal[index];
}