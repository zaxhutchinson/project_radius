#pragma once

#include<cmath>
#include<string>

class VecS {
private:
    double rho;
    double phi;
    double theta;
public:
    VecS();
    VecS(double _phi, double _theta, double _rho);
    VecS(const VecS & v)=default;
    VecS(VecS && v)=default;
    VecS& operator=(const VecS & v)=default;
    VecS& operator=(VecS && v)=default;

    double Rho();
    double Phi();
    double Theta();
    void Rho(double r);
    void Phi(double p);
    void Theta(double t);

    // Vec3 ToVec3WithoutRho() const;
    // Vec3 ToVec3() const;

    // void InterpolateTo(const VecS & v, double delta);
    
};