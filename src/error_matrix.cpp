#include"error_matrix.hpp"

void ErrorMatrix::Tick() {
    total_time++;
}

void ErrorMatrix::ResetClock(i64 starting_time) {
    total_time = starting_time;
}

void ErrorMatrix::SetTargetRateForLayer(vec<double> layer_rates, sizet layer_id) {
    target_spike_rate[layer_id] = std::move(layer_rates);
}

vec<double> & ErrorMatrix::GetCurrerntErrorRateForLayer(sizet layer_id) {
    return current_error_rate[layer_id];

}