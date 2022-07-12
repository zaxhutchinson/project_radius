#include"vecS.hpp"

VecS::VecS()
    :   phi(0.0), theta(0.0), rho(0.0)
{}

VecS::VecS(double _phi, double _theta, double _rho)
    :   phi(_phi), theta(_theta), rho(_rho)
{}

double VecS::Rho() {
    return rho;
}
double VecS::Phi() {
    return phi;
}
double VecS::Theta() {
    return theta;
}
void VecS::Rho(double r) {
    rho = r;
}
void VecS::Phi(double p) {
    phi = p;
}
void VecS::Theta(double t) {
    theta = t;
}

// Vec3 VecS::ToVec3WithoutRho() const {
//     Vec3 v;
//     v.X(std::cos(phi)*std::sin(theta));
//     v.Y(std::sin(phi)*std::sin(theta));
//     v.Z(std::cos(phi));
//     return v;
// }

// Vec3 VecS::ToVec3() const {
//     Vec3 v;
//     v.X(rho*std::cos(phi)*std::sin(theta));
//     v.Y(rho*std::sin(phi)*std::sin(theta));
//     v.Z(rho*std::cos(phi));
//     return v;
// }



// void VecS::InterpolateTo(const VecS & v, double delta) {
//     Vec3 v3_self = ToVec3();
//     Vec3 v3_other = v.ToVec3();

//     Vec3 cross = Vec3::CrossProduct(v3_self, v3_other);
//     Vec3 abs_cross = cross.Abs();
//     Vec3 k = cross / abs_cross;
//     double angle = std::acos(
//         Vec3::DotProduct(v3_self, v3_other) /
//         (v3_self.MagnitudeNormalized() * v3_other.MagnitudeNormalized())
//     );

//     *this = *this * std::cos(delta) + (k*v3_self)*
// }