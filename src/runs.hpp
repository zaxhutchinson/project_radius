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


/******************************************************************************
 * First MNIST test.
 *****************************************************************************/
void RunMNIST(
    Writer * writer,
    Network * network,
    MNISTReader & mnist,
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