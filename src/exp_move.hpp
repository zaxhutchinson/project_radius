#pragma once

#include<iostream>

#include"zxlb.hpp"

enum class MoveType {
    LEFT,
    RIGHT,
    UP,
    DOWN
};

struct ExpMoveInstance {
    MoveType type;
    vec<vec<double>> signals;
};

class ExpMove {

private:
    int field_width;
    int field_height;
    int time_start;
    int time_end;
    double b;
    double c;
    double sigma;
    umap<MoveType, vec<ExpMoveInstance>> instances;

    double Witch(double x, double a);
    double Gauss(double x, double mu);

public:
    ExpMove(
        int _width,
        int _height,
        double _b,
        double _c,
        int _time_start,
        int _time_end,
        vec<MoveType> types
    );
    // ExpMove(
    //     int _width,
    //     int _height,
    //     double _sigma,
    //     int _num_ts,
    //     bool _v,
    //     bool _h
    // );

    vec<ExpMoveInstance> GetAllInstances();
    vec<ExpMoveInstance> & GetInstancesOfType(MoveType type);
};