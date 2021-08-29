#pragma once

#include<iostream>
#include<sstream>
#include<random>

#include"zxlb.hpp"

struct ExpBeaconEntry {
    double row[20] = {0.0};
    double col[26] = {0.0};
    vec<double> beacons;
};

struct ExpBeacon {
    static inline double BASE = 200.0;
    static inline double CELL_MULT = 5.0;
    static inline double BEAC_MULT = 2.0;
    /*-----------------------------------------------------------------------
        Data
    ------------------------------------------------------------------------*/
    vec<ExpBeaconEntry> labeled_entries;
    vec<ExpBeaconEntry> unlabeled_entries;
    RNG rng;
    /*-----------------------------------------------------------------------
        Functions
    ------------------------------------------------------------------------*/
    void Init(i64 seed);
    void LoadBeaconEntries();
    void ShuffleLabeledEntries();
    void ShuffleUnlabeledEntries();
    sizet GetSizeLabeledEntries();
    sizet GetSizeUnlabeledEntries();
    ExpBeaconEntry * GetLabeledEntry(sizet index);
    ExpBeaconEntry * GetUnlabeledEntry(sizet index);
    ExpBeaconEntry * GetRandomLabeledEntry();
    ExpBeaconEntry * GetRandomUnlabeledEntry();
};