#pragma once

#include<filesystem>
#include<iostream>

#include"zxlb.hpp"

const str PATH = "dat/output/";
const i64 LAYERID = 1;
const vec<i64> NIDS {
    0,1
};

struct SynData {
    i64 id;
    double lat;
    double lon;
    double rad;
    double str;
    i64 pid;

    std::string to_string();
};

struct NeuData {
    i64 id;
    umap<i64,SynData> syndata;

    std::string to_string();
};

struct LayData {
    i64 id;
    umap<i64,NeuData> neudata;

    std::string to_string();
};

struct NetData {
    umap<i64,LayData> laydata;

    std::string to_string();
};

vec<str> SplitString(str line, char delim);

NetData LoadNetworkOutput(std::string path);