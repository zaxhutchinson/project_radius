#pragma once

#include"../src/zxlb.hpp"
#include"vecS.hpp";

class Vec3 {
private:
	double x;
	double y;
	double z;
public:

	Vec3();
	Vec3(double _x, double _y);
	Vec3(double _x, double _y, double _z);
	Vec3(const Vec3 & other);

	double X() const;
	double Y() const;
	double Z() const;
	void X(double _x);
	void Y(double _y);
	void Z(double _z);

	str to_string();
	Vec3 operator-(const Vec3 & other);
	Vec3 operator+(const Vec3 & other);
	Vec3 operator*(double v);
	Vec3 operator/(const Vec3 & other);
	bool operator==(const Vec3 & other);
    bool operator!=(const Vec3 & other);
    void operator+=(const Vec3 & other);
    void operator/=(double v);
    void operator*=(double v);

	Vec3 Abs();
	double MagnitudeNormalized();

	void Zero();

	double Length();
	Vec3 Normalize();

	double ToBearing();

	Vec3 VectorTo(Vec3 & dest);

	Vec3 ForceVector(Vec3 & dest, double force);

	// VecS ToVecS();

	// double AngularDistanceBetween(const Vec3 & v1, const Vec3 & v2);
    //-----------------------------------
    // Static functions
    static double Distance(Vec3 & a, Vec3 & b);
    static double Distance2(Vec3 & a, Vec3 & b);

    static Vec3 Average(Vec3 & a, Vec3 & b);

	static Vec3 CrossProduct(const Vec3 & a, const Vec3 & b);
	static double DotProduct(const Vec3 & a, const Vec3 & b);
	
};







///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Test_Vec3_A();
bool Test_Vec3_B(double x, double y, double z);