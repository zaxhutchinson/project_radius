#pragma once

#include<cstdint>

namespace zxlb {

    inline constexpr double MAJOR_VERSION {0.0};
    inline constexpr double MINOR_VERSION {0.10};

    /* NEURON_SPIKE_TIME_WINDOW
        Determines how long spikes are saved and continue to affect neuronal
        output. Since the alpha function used to calculate output drops to near
        zero by 10 ms, it doesn't make much sense to extend this value much
        beyond.

        A neuron spiking frequently enough to have two live spikes would need
        to be spiking at > 100 hz. Therefore, a possible optimization is to
        remove the saving of spike time altogether, and just keep the last
        one. At least for neurons. Plateau potentials will warrant saving spikes
        ...maybe.
    */
    inline constexpr int64_t NEURON_SPIKE_TIME_WINDOW { 30 };


    inline constexpr double PRE_SELF_FORCE_TIME_WINDOW { 30.0 };
    inline constexpr double PRE_OTHER_FORCE_TIME_WINDOW { 30.0 };

    inline constexpr double POST_SYNAPSE_FORCE_TIME_WINDOW { 30.0 }; // UNUSED
    inline constexpr double POST_SOMA_FORCE_TIME_WINDOW { 30.0 };

    inline constexpr double LEARNING_TIME_WINDOW { 30.0 };

    inline constexpr double MAX_RADIUS {100.0};

    inline constexpr double LEARNING_RATE {0.01};

    /* BF: Balancing Factor
        Used in the calculation of dendritic path lengths. Part of the
        dendrite building algorithm developed by Cuntz2007.

        It represents the effect the total path length to an unconnected
        synapse has on calculating distance for Prim's Algorithm.
    */
    inline constexpr double BF = 0.5;
};