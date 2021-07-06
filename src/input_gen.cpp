#include"input_gen.hpp"


double InputGenerator::GetInput(sizet index, i64 time) {
    return pattern[index];
}



double InputGenerator_Sine::GetInput(sizet index, i64 time) {
    return 1.0+std::sin(time/1000.0+seed[index]) * pattern[index];
}

double InputGenerator_Cosine::GetInput(sizet index, i64 time) {
    return 1.0+std::cos(time/1000.0+seed[index]) * pattern[index];
}