#pragma once

#include<fstream>
#include<iostream>
#include<cstring>
#include<omp.h>
#include<random>
#include<cassert>

#include"zxlb.hpp"
#include"config.hpp"
#include"sim.hpp"
#include"mnist_reader.hpp"
#include"network_template.hpp"
#include"builder.hpp"
#include"pattern_maker.hpp"
#include"exp_beacon.hpp"
#include"exp_eeg.hpp"
#include"exp_move.hpp"


/******************************************************************************
 * First MNIST test.
 *****************************************************************************/
void RunMNIST(
    Writer * writer,
    Network * network,
    RNG & rng
);
/******************************************************************************
 * Input Position Experiment
 *****************************************************************************/
// 21 inputs, 4 patterns, 0th input shared by all.
void RunInputPosition1(
    Writer * writer,
    Network * network,
    RNG & rng
);

// 25 Inputs, 4 patterns, 0th input shared by all. Each pattern shares an input
// with neighbor
void RunInputPosition2(
    Writer * writer,
    Network * network,
    RNG & rng
);


// 50 inputs 1 pattern. This is to test position compression.
void RunInputPosition3(
    Writer * writer,
    Network * network,
    RNG & rng
);

// 6 inputs 1 pattern. This is to test position compression.
void RunInputPosition4(
    Writer * writer,
    Network * network,
    RNG & rng
);

/******************************************************************************
 * ExpEEG2
 * This experiment will test whether an AD neuron can learn to differentiate
 * between two types of EEGs.
 *****************************************************************************/
void RunEEG2(
    Writer * writer,
    Network * network,
    RNG & rng
);

/******************************************************************************
 * Exp iBeacon
 * 59 input neurons corresponding to the:
 *  20 rows, 26 columns and 13 iBeacons.
 *****************************************************************************/
void RunExpBeacon(
    Writer * writer,
    Network * network,
    RNG & rng
);


/******************************************************************************
 * Exp Radial
 * 1 spike rate to test whether rate shows up in the radial
 * position.
 *****************************************************************************/
void RunRadialPosition0(
    Writer * writer,
    Network * network,
    RNG & rng
);
/******************************************************************************
 * Exp Radial
 * 4 Different spike rates to test whether rate shows up in the radial
 * position.
 *****************************************************************************/
void RunRadialPosition1(
    Writer * writer,
    Network * network,
    RNG & rng
);
/******************************************************************************
 * Exp Radial
 * Similar to the angular test but with varying rates.
 *****************************************************************************/
void RunRadialPosition2(
    Writer * writer,
    Network * network,
    RNG & rng
);



/******************************************************************************
 * TreeBuilding
 * Similar to the angular test but with varying rates.
 *****************************************************************************/
void RunTree0(
    Writer * writer,
    Network * network,
    RNG & rng
);
/******************************************************************************
 * TreeBuilding1
 * 20 inputs 4 patterns all equal
 *****************************************************************************/
void RunTree1(
    Writer * writer,
    Network * network,
    RNG & rng
);
/******************************************************************************
 * TreeBuilding2
 * 20 inputs 4 patterns NOT equal
 *****************************************************************************/
void RunTree2(
    Writer * writer,
    Network * network,
    RNG & rng
);
/******************************************************************************
 * TreeBuilding3
 * 100 inputs 10 patterns Guassian centers.
 *****************************************************************************/
void RunTree3(
    Writer * writer,
    Network * network,
    RNG & rng
);
void RunTree3_TestB(
    Writer * writer,
    Network * network,
    RNG & rng
);












/******************************************************************************
 * 
 *****************************************************************************/
void RunPattern001(
    Writer * writer,
    Network * network,
    PatternMaker * pattern_maker,
    RNG & rng
);

/******************************************************************************
 * Unused.
 *****************************************************************************/
void RunPoisson000(
    Writer * writer,
    Network * network,
    RNG & rng
);
void RunPoisson001(
    Writer * writer,
    Network * network,
    RNG & rng
);
/******************************************************************************
 * Spike Trains 002
 * 2 patterns 55 inputs each. 10 input overlap (45-54).
 * 100hz on, 0hz off.
 * 100hz random mask pattern.
 *****************************************************************************/
void RunPoisson002(
    Writer * writer,
    Network * network,
    RNG & rng
);
/******************************************************************************
 * Spike Trains 003
 * 2 patterns 55 inputs each. 10 input overlap (45-54).
 * 100hz on, 10hz off.
 * 100hz random mask pattern.
 *****************************************************************************/
void RunPoisson003(
    Writer * writer,
    Network * network,
    RNG & rng
);
/******************************************************************************
 * Spike Trains 004
 * 4 patterns 10 inputs each. No overlap
 * 100hz on, 0hz off.
 * 5hz random mask pattern.
 *****************************************************************************/
void RunPoisson004(
    Writer * writer,
    Network * network,
    RNG & rng
);
/******************************************************************************
 * Spike Trains 004_B
 * 4 patterns 10 inputs each. Four overlap (2 next, 2 prev)
 * 100hz on, 0hz off.
 * 5hz random mask pattern.
 *****************************************************************************/
void RunPoisson004_B(
    Writer * writer,
    Network * network,
    RNG & rng
);

/******************************************************************************
 * Spike Trains 004_C
 * 4 patterns 10 inputs each. Four overlap (2 next, 2 prev)
 * 100hz on, 0hz off.
 * 5hz random mask pattern.
 * The shared inputs between patters 1 and 4 and between 2 and 3 are stronger
 * than those between 1 and 2 or 3 and 4.
 *****************************************************************************/
void RunPoisson004_C(
    Writer * writer,
    Network * network,
    RNG & rng
);

/******************************************************************************
 * Spike Trains 004_D
 * 4 patterns 10 inputs each. Four overlap (2 next, 2 prev)
 * 100hz on, 0hz off.
 * 5hz random mask pattern.
 * Test subpatterns within patterns. 
 *****************************************************************************/
void RunPoisson004_D(
    Writer * writer,
    Network * network,
    RNG & rng
);





/******************************************************************************
 * Lines Vert
 * This experiment will test whether an AD neuron can detect horizontal lines
 * versus vertical lines.
 *****************************************************************************/
void RunLinesVert(
    Writer * writer,
    Network * network,
    RNG & rng
);


/******************************************************************************
 * XOR
 * This experiment will test whether an AD neuron can learn a simple XOR.
 *****************************************************************************/
void RunXOR(
    Writer * writer,
    Network * network,
    RNG & rng
);


/******************************************************************************
 * ExpEEG
 * This experiment will test whether an AD neuron can learn to differentiate
 * between two types of EEGs.
 *****************************************************************************/
void RunEEG(
    Writer * writer,
    Network * network,
    RNG & rng
);



/******************************************************************************
 * ExpMove_Hand
 * This experiment will test whether an AD neuron can learn to differentiate
 * between two types of EEGs.
 *****************************************************************************/
void RunMove_Hand_Config(
    Network * network
);
void RunMove_Hand_Config_Reverse(
    Network * network
);
void RunMove_Hand_Random(
    Network * network,
    RNG & rng
);
void RunMove_Hand(
    Writer * writer,
    Network * network,
    RNG & rng
);

/******************************************************************************
 * ExpMove
 * This experiment will test whether an AD neuron can learn to differentiate
 * between two types of EEGs.
 *****************************************************************************/
void RunMove(
    Writer * writer,
    Network * network,
    RNG & rng
);