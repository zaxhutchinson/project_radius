#include"vec_s.hpp"

VecS::VecS() : lat(0.0), lon(0.0), rad(0.0) {}
VecS::VecS(double _rad) : rad(_rad) {}
VecS::VecS(double _lat, double _lon, double _rad)
    : lat(_lat), lon(_lon), rad(_rad)
{}

double VecS::Lat() { return lat; }
double VecS::Lon() { return lon; }
double VecS::Rad() { return rad; }
void VecS::Lat(double _lat) { lat = _lat; }
void VecS::Lon(double _lon) { lon = _lon; }
void VecS::Rad(double _rad) { rad = _rad; }

void VecS::RandomizeLatLong(RNG & rng) {
    std::uniform_real_distribution<double> dist(
        M_PI, -M_PI
    );
    lat = dist(rng);
    lon = dist(rng);
}

double VecS::Distance(const VecS & v) {

    double alon = v.lon-lon;
    double alat = v.lat-lat;

    // if(alat==0) {
    //     alon = fmod(alon+2.0*M_PI,M_PI);
    //     return (alon < M_PI-alon) ? alon : M_PI - alon;
    // }; 

    double a = pow( sin(alat/2.0), 2.0 ) +
        cos(lat) * cos(v.lat) *
        pow(sin(alon/2.0), 2.0);
    return  2.0 * atan2(sqrt(a), sqrt(1-a));
    
    
    // 1
    // return acos(
    //     sin(lat) * sin(v.lat) + cos(lat) * cos(v.lat) * cos(alon)
    // );

    // Haversine
    // return 2.0 * asin( 
    //     sqrt (
    //         pow(sin(alat/2.0),2.0) +
    //         cos(lat) * cos(v.lat) * pow(sin(alon/2.0),2.0)
    //     )
    // );

    // Vincenty - can't make it work
    // double top = sqrt(
    //     pow(
    //         cos(v.lat) * sin(alon),
    //         2.0
    //     ) +
    //     pow(
    //         cos(lat) * sin(v.lat) - sin(lat) * cos(v.lat) * cos(alon),
    //         2.0
    //     )
    // );

    // double bot = sin(lat) * sin(v.lat) + cos(lat) * cos(v.lat) * cos(alon);

    // return atan(top / bot);
}

/* Distance is computed by first taking the differences in the radius.
    Then the arc length. In visual terms, this vec moves to the orbit of
    the other vec, and then moves around.
*/
double VecS::DistanceWithRadius(const VecS & v) {
    return std::abs(v.rad-rad) + (v.rad * Distance(v));
}

double VecS::HeadingTo(const VecS & v) {
    double dlon = v.lon - lon;
    return atan2(
        sin(dlon) * cos(v.lat),
        cos(lat) * sin(v.lat) - sin(lat) * cos(v.lat) * cos(dlon)
    );
}

void VecS::Orbit(double heading, double distance) {
    double delta = distance / rad;
    double nlat = asin(
        sin(lat) * cos(delta) + cos(lat) * sin(delta) * cos(heading)
    );
    double nlon = lon + atan2(
        sin(heading) * sin(delta) * cos(lat),
        cos(delta) - sin(lat) * sin(nlat)
    );
    lat = nlat;
    lon = nlon;
}

Vec3 VecS::VectorTo(const VecS & v) {
    Vec3 vec3;
    double dlon = v.lon - lon;
    vec3.x = sin(dlon) * cos(v.lat);
    vec3.y = cos(lat) * sin(v.lat) - sin(lat) * cos(v.lat) * cos(dlon);
    return vec3;
}

Vec3 VecS::ToVec3() {
    return Vec3(
        rad * sin(lat) * cos(lon),
        rad * sin(lat) * sin(lon),
        rad * cos(lat)
    );
}

///////////////////////////////////////////////////////////////////////////////
// TESTS
///////////////////////////////////////////////////////////////////////////////

UTRTN T_VecS_1() {

    double alat = M_PI/4;
    double alon = M_PI/4;
    double arad = 1.0;
    double blat = -M_PI/4;
    double blon = -M_PI/4;
    double brad = 1.0;

    VecS A(alat, alon, arad);
    VecS B(blat, blon, brad);

    double val = A.Distance(B);
    // bool cor = abs(val - M_PI) < 0.0000001;
    bool cor = val == M_PI/2;
    str msg = "VAL " + std::to_string(val) + " COMP " + std::to_string(M_PI);
    UTRTN rtn = std::make_pair(cor,msg);
    return rtn;
}


UTRTN T_VecS_2() {

    double alat = -M_PI/4;
    double alon = -M_PI/4;
    double arad = 1.0;
    double blat = M_PI/4;
    double blon = M_PI/4;
    double brad = 1.0;

    VecS A(alat, alon, arad);
    VecS B(blat, blon, brad);

    double val = A.HeadingTo(B);
    // bool cor = abs(val - M_PI) < 0.0000001;
    bool cor = val == M_PI;
    str msg = "VAL " + std::to_string(val) + " COMP " + std::to_string(M_PI);
    UTRTN rtn = std::make_pair(cor,msg);
    return rtn;
}

UTRTN T_VecS_3() {

    double alat = M_PI/2-0.001;
    double alon = -M_PI/4;
    double arad = 1.0;
    double blat = -M_PI/2+0.001;
    double blon = M_PI/4;
    double brad = 1.0;

    VecS A(alat, alon, arad);
    VecS B(blat, blon, brad);

    double val = A.HeadingTo(B);
    double dis = A.Distance(B);
    A.Orbit(val,dis);
    // bool cor = abs(val - M_PI) < 0.0000001;
    bool cor = A.Lat()==B.Lat() && A.Lon()==B.Lon();
    str msg = "VAL " + std::to_string(A.Lat()) + "," + std::to_string(A.Lon()) + " COMP " + std::to_string(B.Lat()) + "," + std::to_string(B.Lon());
    UTRTN rtn = std::make_pair(cor,msg);
    return rtn;
}