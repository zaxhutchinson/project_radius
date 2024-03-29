#include"vec3.hpp"

Vec3::Vec3()
    : x(0.0), y(0.0), z(0.0) 
{}
Vec3::Vec3(double _x, double _y)
    : x(_x), y(_y), z(0.0)
{}
Vec3::Vec3(double _x, double _y, double _z)
    : x(_x), y(_y), z(_z) 
{}

Vec3::Vec3(const Vec3 & other) {
    x = other.x;
    y = other.y;
    z = other.z;
}


double Vec3::X() const { return x; }
double Vec3::Y() const { return y; }
double Vec3::Z() const { return z; }
void Vec3::X(double _x) { x = _x; }
void Vec3::Y(double _y) { y = _y; }
void Vec3::Z(double _z) { z = _z; }

str Vec3::to_string() {
    return str(
        std::to_string(x)+","+std::to_string(y)+","+std::to_string(z)
    );
}

Vec3 Vec3::operator-(const Vec3 & other) {
    Vec3 v;
    v.x = other.x - x;
    v.y = other.y - y;
    v.z = other.z - z;
    return v;
}
Vec3 Vec3::operator+(const Vec3 & other) {
    Vec3 v;
    v.x = other.x + x;
    v.y = other.y + y;
    v.z = other.z + z;
    return v;
}
Vec3 Vec3::operator/(const Vec3 & other) {
    Vec3 v;
    v.x = other.x / x;
    v.y = other.y / y;
    v.z = other.z / z;
}
Vec3 Vec3::operator*(double v) {
    return Vec3(x*v, y*v, z*v);
}
bool Vec3::operator==(const Vec3 & other) {
    return x==other.x && y==other.y && z==other.z;
}
bool Vec3::operator!=(const Vec3 & other) {
    return !(*this==other);
}
void Vec3::operator+=(const Vec3 & other) {
    this->x += other.x;
    this->y += other.y;
    this->z += other.z;
}
void Vec3::operator/=(double v) {
    x /= v;
    y /= v;
    z /= v;
}
void Vec3::operator*=(double v) {
	x *= v;
	y *= v;
	z *= v;
}

Vec3 Vec3::Abs() {
    return Vec3(
        std::abs(x), std::abs(y), std::abs(z)
    );
}
double Vec3::MagnitudeNormalized() {
    return std::sqrt(
        x*x + y*y + z*z
    );
}

void Vec3::Zero() {
	x = 0.0; y = 0.0; z = 0.0;
}

double Vec3::Length() {
	return std::sqrt(x*x + y*y + z*z);
}
double Vec3::ToBearing() {
    return atan2(y,x);
}
Vec3 Vec3::Normalize() {
	double len = Length();
	Vec3 normv;
	if(len > 0) {
		normv.x = x/len;
		normv.y = y/len;
		normv.z = z/len;
	}
	return normv;
}
Vec3 Vec3::VectorTo(Vec3 & dest) {
	Vec3 v(dest.x-x,dest.y-y,dest.z-z);
	return v;
}
Vec3 Vec3::ForceVector(Vec3 & dest, double force) {
	Vec3 vec_to = VectorTo(dest).Normalize();
	vec_to *= force;
	return vec_to;
}

// VecS Vec3::ToVecS() {
//     VecS v;
//     v.Rad(std::sqrt(x*x+y*y+z*z));
//     if(v.Rad()==0.0) {
//         v.Lat(0.0);
//         v.Lon(0.0);
//     } else {
//         v.Lat(M_PI_2 - std::acos(z/v.Rad()));
//         v.Lon(std::atan2(y,x));
//     }
//     return v;
// }

// double Vec3::AngularDistanceBetween(const Vec3 & v1, const Vec3 & v2) {
//     Vec3 sv1 = (*this)-v1;
//     Vec3 sv2 = (*this)-v2;
//     double v1_yx = std::atan2(sv1.y,sv1.x);
//     double v1_yz = std::atan2(sv1.y,sv1.z);
//     double v2_yx = std::atan2(sv2.y,sv2.x);
//     double v2_yz = std::atan2(sv2.y,sv2.z);

    
// }

//-------------------------------------------------
// Static functions
double Vec3::Distance(Vec3 & a, Vec3 & b) {
    return  std::sqrt(std::pow(a.X() - b.X(),2.0) +
                      std::pow(a.Y() - b.Y(),2.0) +
                      std::pow(a.Z() - b.Z(),2.0) );
}
double Vec3::Distance2(Vec3 & a, Vec3 & b) {
    return  (std::pow(a.X() - b.X(),2.0) +
            std::pow(a.Y() - b.Y(),2.0) +
            std::pow(a.Z() - b.Z(),2.0) );
}
Vec3 Vec3::Average(Vec3 & a, Vec3 & b) {
    Vec3 avg_v;
    avg_v.x = (a.X()+b.X()) / 2.0;
    avg_v.y = (a.Y()+b.Y()) / 2.0;
    avg_v.z = (a.Z()+b.Z()) / 2.0;
    return avg_v;
}
Vec3 Vec3::CrossProduct(const Vec3 & a, const Vec3 & b) {
    Vec3 v;
    v.X(
        a.Y()*b.Z() - a.Z()*b.Y()
    );
    v.Y(
        a.Z()*b.X() - a.X()*b.Z()
    );
    v.Z(
        a.X()*b.Y() - a.Y()*b.X()
    );
    return v;
}
double Vec3::DotProduct(const Vec3 & a, const Vec3 & b) {
    return a.X()*b.X() + a.Y()*b.Y() + a.Z()*b.Z();
}









///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// bool Test_Vec3_A() {
//     Vec3 v1(1,1,1);
//     Vec3 v2(0,0,0);
//     return v1!=v2;
// }
// bool Test_Vec3_B(double x, double y, double z) {
//     Vec3 v1(x,y,z);
//     Vec3 v2(x,y,z);
//     return Vec3::Distance(v1,v2)==Vec3::Distance(v2,v1);
// }