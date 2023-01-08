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
    double pos_target_rate = 0.0;
    double weight_target_rate = 0.0;

    umap<i64,double> pos_downstream_error_rates;
    umap<i64,double> weight_downstream_error_rates;

    Connection();
    Connection(const Connection & c) = default;
    Connection(Connection && c) = default;
    Connection& operator=(const Connection & c) = default;
    Connection& operator=(Connection && c) = default;

    void SetPosDownStreamErrorRate(i64 index, double rate);
    double GetPosDownStreamErrorRate();
    void SetWeightDownStreamErrorRate(i64 index, double rate);
    double GetWeightDownStreamErrorRate();

    double GetPosTargetErrorRate();
    double GetWeightTargetErrorRate();

    void Update(bool _just_spiked, double _output);
  
    void Reset(
        double _target_rate=0.0
    );
    void Reset(
        double _pos_target_rate,
        double _weight_target_rate
    );
    // double GetErrorRateRaw();
    // double GetErrorRate();
    double GetPosErrorRateNorm();
    double GetPosErrorRateNormReLU();
    double GetWeightErrorRateNorm();
    double GetWeightErrorRateNormReLU();
    // double GetErrorRateReLU();
    // double GetErrorRateReLU_1();
};

using ConnectionMatrix = vec<vec<Connection>>;