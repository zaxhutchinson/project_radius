#pragma once

#include"zxlb.hpp"

struct ConnectionAddress {
    i64 pre_layer;
    i64 post_layer;
    i64 pre_neuron;
    i64 post_neuron;
};

struct Connection {
    bool just_spiked = false;
    double output = 0.0;
    
    i64 num_spikes = 0;
    i64 time = 0;
    double target_rate = 0.0;

    void Update(bool _just_spiked, double _output);
    void SetErrorRate(double rate);
    void Reset(
        double _target_rate=0.0, 
        i64 _time=0
    );
    double GetErrorRate();
};

using ConnectionMatrix = vec<vec<Connection>>;