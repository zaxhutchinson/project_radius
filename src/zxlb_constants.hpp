#pragma once

#include<cstdint>

namespace zxlb {

    inline constexpr double MAJOR_VERSION {0.0};
    inline constexpr double MINOR_VERSION {0.16};

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


    inline constexpr double PRE_SELF_FORCE_TIME_WINDOW { 10.0 };
    inline constexpr double PRE_OTHER_FORCE_TIME_WINDOW { 10.0 };

    inline constexpr double POST_SYNAPSE_FORCE_TIME_WINDOW { 10.0 }; // UNUSED
    inline constexpr double POST_SOMA_FORCE_TIME_WINDOW { 10.0 };

    // inline constexpr double LEARNING_TIME_WINDOW { 100.0 };
    inline constexpr double LEARNING_WINDOW_SYN_STRENGTH {10.0};

    inline constexpr double MAX_RADIUS {100.0};

    // inline constexpr double RADIUS_TIME_DIFF_SCALAR {1000.0/MAX_RADIUS};

    inline constexpr double PRE_LEARNING_RATE {0.001};
    inline constexpr double POST_LEARNING_RATE {0.001};
    inline constexpr double SYN_STRENGTH_LEARNING_RATE {1000.0}; // Equates to 0.001


    inline constexpr double MAX_TEMPORAL_DIFFERENCE = 100.0;

    /* BF: Balancing Factor
        Used in the calculation of dendritic path lengths. Part of the
        dendrite building algorithm developed by Cuntz2007.

        It represents the effect the total path length to an unconnected
        synapse has on calculating distance for Prim's Algorithm.
    */
    inline constexpr double BF = 0.5;

    /* MAX_COMPARTMENT_LENGTH
        Used to determine where compartments begin and end.
    */
    inline constexpr double MAX_COMPARTMENT_LENGTH = MAX_TEMPORAL_DIFFERENCE;


    inline constexpr double COMPARTMENT_OUT_EXPONENT = 1.0;

    inline constexpr double MIN_COMPARTMENT_SIGNAL_FOR_SOMA = 1.0;

    /* DENDRITE SIGNAL WEIGHT
        Signals moving down the dendrite are clamped to [-1,1].
        This values scales the signal as it enters the soma to boost it
        to a level able to elicit a spike.
    */
    inline constexpr double DENDRITE_SIGNAL_WEIGHT = 100.0;



    inline constexpr double CORRECT_EXPECTED = 100.0;
    inline constexpr double INCORRECT_EXPECTED = 200.0;
    inline constexpr int64_t TASK_DURATION = 1000;

    inline constexpr int64_t EEG_TASK_DURATION = 1024;
    inline constexpr double dEEG_TASK_DURATION = 1024.0;


    // Govern the modified Witch of Agnesi function used to determine
    // synaptic output. A is missing because it is the two distances.
    inline constexpr double WITCH_B = 10.0;
    inline constexpr double WITCH_C = 1.0;


    
};