/*
    Spherical Vector Class
    zax - First version 1-9-2021

    Models a location on the surface of a sphere using latitude, longitude,
    and radius. Provides some functionality for measuring distance between
    two vectors, heading from one vector to another and moving vectors.

    This is designed primarily for use in the Radius Project to track the
    location of synpases.

    Distance, HeadingTo and Move were programmed based on code from the
    following sources:
        https://stackoverflow.com/a/26711070
        https://en.wikipedia.org/wiki/Great-circle_distance
*/


#pragma once
#include<cmath>
#include"zxlb.hpp"

struct VecS {
    double lat;
    double lon;
    double rad;

    VecS();
    VecS(double _lat, double _lon, double _rad);
    VecS(const VecS & v);

    double Lat();
    double Lon();
    double Rad();
    void Lat(double _lat);
    void Lon(double _lon);
    void Rad(double _rad);


    double Distance(const VecS & v);
    double HeadingTo(const VecS & v);
    void Move(double heading, double distance);
};


///////////////////////////////////////////////////////////////////////////////
// TESTS
///////////////////////////////////////////////////////////////////////////////
UTRTN T_VecS_1();
UTRTN T_VecS_2();
UTRTN T_VecS_3();