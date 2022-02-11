#include"vec_s.hpp"
#include"vec3.hpp"

VecS::VecS() : lat(0.0), lon(0.0), rad(0.0) {}
VecS::VecS(double _rad) : rad(_rad) {}
VecS::VecS(double _lat, double _lon, double _rad)
    : lat(_lat), lon(_lon), rad(_rad)
{}


str VecS::to_string() {
    return str(
        std::to_string(lat)+","+std::to_string(lon)+","+std::to_string(rad)
    );
}

double VecS::Lat() { return lat; }
double VecS::Lon() { return lon; }
double VecS::Rad() { return rad; }
void VecS::Lat(double _lat) { lat = _lat; }
void VecS::Lon(double _lon) { lon = _lon; }
void VecS::Rad(double _rad) { rad = _rad; }

void VecS::RandomizeLatLong(RNG & rng) {
    std::uniform_real_distribution<double> lonDist(
        M_PI, -M_PI
    );
    std::uniform_real_distribution<double> latDist(
        0.0,1.0
    );
    lat = std::acos(2.0*latDist(rng)-1.0)-M_PI/2.0;
    lon = lonDist(rng);
}

void VecS::ChangeRad(double amt) {
    //amt = (1.0-(std::abs((zxlb::MAX_RADIUS/2.0) - rad) / (zxlb::MAX_RADIUS/2.0))) * amt;

    rad += amt;
    if(rad > zxlb::MAX_RADIUS) rad = zxlb::MAX_RADIUS;
}

double VecS::Distance(const VecS & v) {

    double alon = v.lon-lon;
    double alat = v.lat-lat;

    double a = std::pow( std::sin(alat/2.0), 2.0 ) +
        std::cos(lat) * std::cos(v.lat) *
        std::pow(std::sin(alon/2.0), 2.0);

    // Correct for precision errors.
    if(a>1) a = 1.0; 
    if(a<0) a = 0.0;
    
    return  2.0 * std::atan2(std::sqrt(a), std::sqrt(1-a));
}

double VecS::RadDistance(const VecS & v) {
    return std::abs(v.rad - rad);
}

// double VecS::Distance2(const VecS & v) {
//     // if(alat==0) {
//     //     alon = fmod(alon+2.0*M_PI,M_PI);
//     //     return (alon < M_PI-alon) ? alon : M_PI - alon;
//     // }; 


//     // 1
//     // return acos(
//     //     sin(lat) * sin(v.lat) + cos(lat) * cos(v.lat) * cos(alon)
//     // );

//     // Haversine
//     return 2.0 * std::asin( 
//         std::sqrt (
//             std::pow(std::sin(lat-v.lat/2.0),2.0) +
//             std::cos(lat) * std::cos(v.lat) * std::pow(std::sin(lon-v.lon/2.0),2.0)
//         )
//     );

//     // Vincenty - can't make it work
//     // double top = sqrt(
//     //     pow(
//     //         cos(v.lat) * sin(alon),
//     //         2.0
//     //     ) +
//     //     pow(
//     //         cos(lat) * sin(v.lat) - sin(lat) * cos(v.lat) * cos(alon),
//     //         2.0
//     //     )
//     // );

//     // double bot = sin(lat) * sin(v.lat) + cos(lat) * cos(v.lat) * cos(alon);

//     // return atan(top / bot);
// }

double VecS::DistanceStraightLineSqrd(const VecS & v) {
    Vec3 v3_this(
        rad * std::cos(lat) * std::cos(lon),
        rad * std::cos(lat) * std::sin(lon),
        rad * std::sin(lat)
    );
    Vec3 v3_v(
        v.rad * std::cos(v.lat) * std::cos(v.lon),
        v.rad * std::cos(v.lat) * std::sin(v.lon),
        v.rad * std::sin(v.lat)
    );
    return Vec3::Distance2(v3_this, v3_v);
    
    // return rad*rad +
    //     v.rad*v.rad -
    //     2.0 * rad * v.rad *
    //     (
    //         std::sin(lat)*std::sin(v.lat)*std::cos(lon-v.lon) + 
    //         std::cos(lat)*std::cos(v.lat)
    //     );
}

double VecS::DistanceStraightLine(const VecS & v) {
    Vec3 v3_this(
        rad * std::cos(lat) * std::cos(lon),
        rad * std::cos(lat) * std::sin(lon),
        rad * std::sin(lat)
    );
    Vec3 v3_v(
        v.rad * std::cos(v.lat) * std::cos(v.lon),
        v.rad * std::cos(v.lat) * std::sin(v.lon),
        v.rad * std::sin(v.lat)
    );
    return Vec3::Distance(v3_this, v3_v);
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
    // distance = distance * rad;
    double nlat = asin(
        sin(lat) * cos(distance) + cos(lat) * sin(distance) * cos(heading)
    );
    double nlon = lon + atan2(
        sin(heading) * sin(distance) * cos(lat),
        cos(distance) - sin(lat) * sin(nlat)
    );
    lat = nlat;
    lon = nlon;
    if(lon < -M_PI) lon += MPI2;
    else if(lon > M_PI) lon -= MPI2;
    // if(lat < -MPIOVER2) std::cout << "Lat under pi/2 " << lat << std::endl;
    // else if(lat > MPIOVER2) std::cout << "Lat over pi/2 " << lat << std::endl;
}

Vec3 VecS::VectorTo(const VecS & v) {
    Vec3 vec3;
    double dlon = v.lon - lon;
    double heading = atan2(
        sin(dlon) * cos(v.lat),
        cos(lat) * sin(v.lat) - sin(lat) * cos(v.lat) * cos(dlon)
    );
    
    // vec3.x = std::sin(dlon) * std::cos(v.lat);
    // vec3.y = std::cos(lat) * std::sin(v.lat) - std::sin(lat) * std::cos(v.lat) * std::cos(dlon);
    return Vec3(std::cos(heading), std::sin(heading));
}

Vec3 VecS::ToVec3() {
    return Vec3(
        rad * std::cos(lat) * std::cos(lon),
        rad * std::cos(lat) * std::sin(lon),
        rad * std::sin(lat)
    );
}

///////////////////////////////////////////////////////////////////////////////
// TESTS
///////////////////////////////////////////////////////////////////////////////

UTRTN T_VecS_1() {

    double alat = M_PI/2;
    double alon = M_PI/2;
    double arad = 1.0;
    double blat = M_PI/2;
    double blon = M_PI/2;
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

    double alat = M_PI/2-0.1;
    double alon = -M_PI/2;
    double arad = 1.0;
    double blat = M_PI/2-0.1;
    double blon = M_PI/2;
    double brad = 1.0;

    VecS A(alat, alon, arad);
    VecS B(blat, blon, brad);

    double val = A.HeadingTo(B);
    double dis = A.Distance(B);
    A.Orbit(val,dis);
    // bool cor = abs(val - M_PI) < 0.0000001;
    bool cor = A.Lat()==B.Lat() && A.Lon()==B.Lon();
    str msg = std::to_string(val) + "  " + std::to_string(dis) + "  VAL " + std::to_string(A.Lat()) + "," + std::to_string(A.Lon()) + " COMP " + std::to_string(B.Lat()) + "," + std::to_string(B.Lon());
    UTRTN rtn = std::make_pair(cor,msg);
    return rtn;
}