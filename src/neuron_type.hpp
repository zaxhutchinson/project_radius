#pragma once

#include"zxlb.hpp"

namespace tmps {

    struct NeuronType {
        str name;
        double cap;
        double vr;
        double vt;
        double k;
        double vpeak;
        double a;
        double b;
        double c;
        double d;
    };

    inline const NeuronType NT_REGULAR_SPIKING = {
        "Regular Spiking",
        100.0,
        -60.0,
        -40.0,
        0.7,
        35.0,
        0.03,
        -2.0,
        -50.0,
        100.0,
    };

    inline const NeuronType NT_CHATTERING = {
        "Chattering",
        50.0,
        -60.0,
        -40.0,
        1.5,
        25.0,
        0.03,
        1.0,
        -40.0,
        150.0
    };

    inline const NeuronType NT_MEDIUM_SPINY = {
        "Medium Spiny",
        50.0,
        -80.0,
        -25.0,
        1.0,
        40.0,
        0.01,
        -20.0,
        -55.0,
        150.0
    };

    inline const NeuronType NT_RELAY = {
        "Relay",
        200.0,
        -60.0,
        -50.0,
        1.6,
        35.0,
        0.01,
        0.0,
        -50.0,
        100.0
    };

    inline const NeuronType NT_FAST_SPIKING = {
        "Fast Spiking",
        20.0,
        -55.0,
        -40.0,
        1.0,
        25.0,
        0.15,
        8.0,
        -55.0,
        200.0,
    };

    inline const NeuronType NT_RETICULAR_THALAMIC = {
        "Reticular Thalamic",
        40.0,
        -65.0,
        -45.0,
        0.25,
        0.0,
        0.015,
        0.2,
        -55.0,
        50.0
    };

    inline const NeuronType NT_INTRINSIC_BURSTING = {
        "Intrinsic Bursting",
        150.0,
        -75.0,
        -45.0,
        1.2,
        50.0,
        0.01,
        5.0,
        -56.0,
        130.0
    };

    opt<NeuronType> GetNeuronType(str name);

}