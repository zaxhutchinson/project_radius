// MNISTReader struct
// code by zax
// v1: May 2020

// This struct reads and stores MNIST data. It is designed to store either
// training or testing images/labels. If you want to store both, you'll need
// two MNISTReader objects.


#pragma once

#include<fstream>
#include<cstdint>
#include<iostream>
#include<unordered_map>
#include<map>
#include<vector>
#include<random>
#include<string>
#include<algorithm>

#include"zxlb.hpp"
#include"log.hpp"

template<typename T, typename U>
using umap = std::unordered_map<T,U>;
template<typename T>
using vec = std::vector<T>;
template<typename T, typename U>
using pair = std::pair<T,U>;

struct MNISTData {
    unsigned label;
    vec<double> image;
};

enum MNISTDIR {
    NONE=0,
    HOR=1,
    VER=2
};

struct MNISTReader {



    // Unnecessary CTOR
    MNISTReader();

    // Loads the labels and images at the given filenames.
    void LoadData(std::string & label_filename, std::string & images_filename);

    // Will return all MNISTData objects that have the label <label>.
    vec<MNISTData> GetDataWithLabel(unsigned label, MNISTDIR trans=MNISTDIR::NONE);

    // Will return all data.
    vec<MNISTData> & GetData();

    // Returns a pointer to the ith MNISTData object. Returns nullptr if
    // your i is bad.
    MNISTData GetDataAt(unsigned i);

    // Returns a vector of MNISTData built by iteration size. Each iteration
    // will contain one example of each label. The order within each iteration
    // is randomized.
    vec<vec<MNISTData>> GetDataAsIteration(vec<unsigned> labels, sizet num_iterations, sizet examples_per_iteration, RNG & rng);

    // Converts the ith image to a b&w ppm image file named: image_[i].ppm
    // If the i is bad, nothing happens
    void ToPPM(unsigned i);
    void ToPPM(MNISTData & md);

    // Randomly shuffles the vector of MNISTData. In case you want to run
    // the input in a different order than it is stored in the file.
    void Shuffle();

    // Will print the meta info associated with the read in data.
    // Also prints a simple verification of sizes.
    void PrintStats();

    // I suspect this was borrowed by me from some StackOverflow post.
    // I have since lost the citations.
    int32_t reverseInt (int32_t i);

    // Normalize values
    void NormalizeImage(double max_value);

    // Flips an image based on the direction.
    //  HOR: rows are inverted.
    //  VER: cols are inverted.
    MNISTData FlipDataAt(unsigned i, MNISTDIR dir);

    int32_t labels_magic_number;
    int32_t labels_size;
    int32_t images_magic_number;
    int32_t images_size;
    int32_t images_rows;
    int32_t images_cols;

    vec<MNISTData> data;
};