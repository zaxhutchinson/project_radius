#pragma once

#include<iostream>

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
    
    double num_spikes = 0;
    i64 time = 0;
    double target_rate = 0.0;

    umap<i64,double> downstream_error_rates;

    Connection();
    Connection(const Connection & c) = default;
    Connection(Connection && c) = default;
    Connection& operator=(const Connection & c) = default;
    Connection& operator=(Connection && c) = default;

    void SetDownStreamErrorRate(i64 index, double rate);
    double GetDownStreamErrorRate();

    void Update(bool _just_spiked, double _output);
    void SetErrorRate(double rate);
    void Reset(
        double _target_rate=0.0
    );
    // double GetErrorRateRaw();
    // double GetErrorRate();
    double GetErrorRateNorm();
    double GetErrorRateNormReLU();
    // double GetErrorRateReLU();
    // double GetErrorRateReLU_1();
};

using ConnectionMatrix = vec<vec<Connection>>;