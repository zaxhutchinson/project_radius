#pragma once

#include"json.hpp"
#include<cstdint>
#include<fstream>
#include"zxlb_templates.hpp"

namespace zxlb {

    inline double MAJOR_VERSION {0.0};
    inline double MINOR_VERSION {0.16};

    /* NEURON TYPE: Which type of neuron is this?
        0 = AD neuron
        1 = POINT neuron
    */
    inline int NEURON_TYPE = 0;

    inline double MAX_RADIUS {100.0};

    inline double PRE_LEARNING_RATE {0.001};
    inline double POST_LEARNING_RATE {0.001};
    inline double SYN_STRENGTH_LEARNING_RATE {0.001}; // Equates to 0.001


    inline double MAX_ANG_TEMPORAL_DIFFERENCE = 100.0;
    inline double MAX_RAD_TEMPORAL_DIFFERENCE = 100.0;
    inline double MAX_STR_TEMPORAL_DIFFERENCE = 100.0;
    inline double STR_LEARNING_DECAY_PRE = 11.0;
    inline double STR_LEARNING_DECAY_POST = 10.0;

    /* BF: Balancing Factor
        Used in the calculation of dendritic path lengths. Part of the
        dendrite building algorithm developed by Cuntz2007.

        It represents the effect the total path length to an unconnected
        synapse has on calculating distance for Prim's Algorithm.
    */
    inline double BF = 0.75;

    /* MAX_COMPARTMENT_LENGTH
        Used to determine where compartments begin and end.
    */
    inline double MAX_COMPARTMENT_LENGTH = 1000.0;

    /* DENDRITE SIGNAL WEIGHT
        Signals moving down the dendrite are clamped to [-1,1].
        This values scales the signal as it enters the soma to boost it
        to a level able to elicit a spike.
    */
    inline double DENDRITE_SIGNAL_WEIGHT = 100.0;

    inline bool TRAIN_ANG = false;
    inline bool TRAIN_RAD = false;
    inline bool TRAIN_STR = false;

    inline double CORRECT_EXPECTED = 100.0;
    inline double INCORRECT_EXPECTED = 200.0;

    inline i64 TASK_DURATION = 1000;
    inline i64 NUM_ITERATIONS = 1000;

    // SYNAPSE Witch of Agnesi constants.
    inline double B_SYN_SIG = 10.0;
    inline double C_SYN_SIG = 1.0;

    inline double B_SYN_STR = 4.0;
    inline double C_SYN_STR = 1.0;
    inline double B_SYN_STR_PRE = 4.0;
    inline double C_SYN_STR_PRE = 1.0;
    inline double B_SYN_STR_POST = 4.0;
    inline double C_SYN_STR_POST = 1.0;

    // NEURON Witch of Agnesi constants
    inline double B_NEU_DEN_OUT = 10.0;
    inline double C_NEU_DEN_OUT = 1.0;

    void LoadConstants(str filename);







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
    // inline int64_t NEURON_SPIKE_TIME_WINDOW { 30 };


    // inline double PRE_SELF_FORCE_TIME_WINDOW { 10.0 };
    // inline double PRE_OTHER_FORCE_TIME_WINDOW { 10.0 };

    // inline double POST_SYNAPSE_FORCE_TIME_WINDOW { 10.0 }; // UNUSED
    // inline double POST_SOMA_FORCE_TIME_WINDOW { 10.0 };

    // inline double LEARNING_TIME_WINDOW { 100.0 };
    // inline double LEARNING_WINDOW_SYN_STRENGTH {10.0};

    // inline double RADIUS_TIME_DIFF_SCALAR {1000.0/MAX_RADIUS};


    inline double COMPARTMENT_OUT_EXPONENT = 1.0; // NOT USED

    inline int64_t EEG_TASK_DURATION = 1024;
    inline double dEEG_TASK_DURATION = 1024.0;
    
    // inline double MIN_COMPARTMENT_SIGNAL_FOR_SOMA = 1.0;
};