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
        http://www.movable-type.co.uk/scripts/latlong.html
*/


#pragma once
#include<cmath>
#include"zxlb.hpp"

struct Vec3;

struct VecS {
    double lat;
    double lon;
    double rad;

    VecS();
    VecS(double _rad);
    VecS(double _lat, double _lon, double _rad);
    VecS(const VecS & v) = default;
    VecS(VecS && v) = default;
    VecS& operator=(const VecS & v) = default;
    VecS& operator=(VecS && v) = default;


    str to_string();

    double Lat();
    double Lon();
    double Rad();
    void Lat(double _lat);
    void Lon(double _lon);
    void Rad(double _rad);

    void ChangeRad(double amt);

    void RandomizeLatLong(RNG & rng);

    double Distance(const VecS & v);
    double DistanceStraightLineSqrd(const VecS & v);
    double DistanceStraightLine(const VecS & v);
    double DistanceWithRadius(const VecS & v);
    double HeadingTo(const VecS & v);
    Vec3 VectorTo(const VecS & v);
    void Orbit(double heading, double distance);
    Vec3 ToVec3();
    
};


///////////////////////////////////////////////////////////////////////////////
// TESTS
///////////////////////////////////////////////////////////////////////////////
UTRTN T_VecS_1();
UTRTN T_VecS_2();
UTRTN T_VecS_3();