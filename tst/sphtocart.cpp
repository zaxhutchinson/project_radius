#include<cmath>
#include<iostream>

int main() {

    double rad1 = 100.0;
    double lat1 = M_PI/2.0-0.01;
    double lon1 = M_PI/2.0;

    double rad2 = 100.0;
    double lat2 = M_PI/2.0-0.01;
    double lon2 = -M_PI/2.0;

    double x1 = rad1 * std::cos(lat1) * std::cos(lon1);
    double y1 = rad1 * std::cos(lat1) * std::sin(lon1);
    double z1 = rad1 * std::sin(lat1);

    double x2 = rad2 * std::cos(lat2) * std::cos(lon2);
    double y2 = rad2 * std::cos(lat2) * std::sin(lon2);
    double z2 = rad2 * std::sin(lat2);

    double dist = std::sqrt(
        std::pow((x2-x1), 2.0) +
        std::pow((y2-y1), 2.0) +
        std::pow((z2-z1), 2.0)
    );

    std::cout << "1 " << x1 << "," << y1 << "," << z1 << std::endl;
    std::cout << "2 " << x2 << "," << y2 << "," << z2 << std::endl;
    std::cout << "D " << dist << std::endl;

    return 0;
}