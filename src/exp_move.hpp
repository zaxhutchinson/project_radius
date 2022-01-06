#pragma once

#include<iostream>

#include"zxlb.hpp"

enum class MoveType {
    HORIZONTAL,
    VERTICAL
};

struct ExpMoveInstance {
    MoveType type;
    vec<vec<double>> signals;
};

class ExpMove {

private:
    int field_width;
    int field_height;
    int time_steps;
    double b;
    double c;
    umap<MoveType, vec<ExpMoveInstance>> instances;

    double Witch(double x, double a);

public:
    ExpMove(
        int _width,
        int _height,
        double _b,
        double _c,
        int _num_ts,
        bool _v,
        bool _h
    );

    vec<ExpMoveInstance> GetAllInstances();
    vec<ExpMoveInstance> & GetInstancesOfType(MoveType type);
};