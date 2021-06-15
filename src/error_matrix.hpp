#pragma once

#include"zxlb.hpp"

struct ErrorMatrix {
    vec<vec<i64>> spike_count_matrix;
    vec<vec<double>> target_spike_rate;
    vec<vec<double>> current_error_rate;
    i64 total_time;

    void Tick();
    void ResetClock(i64 starting_time=0);
    void UpdateNeuronSpikeCount(sizet layer_id, sizet neuron_id, bool just_spiked);
    void SetTargetRateForLayer(vec<double> layer_rates, sizet layer_id);
    vec<double> & GetCurrerntErrorRateForLayer(sizet layer_id);
};