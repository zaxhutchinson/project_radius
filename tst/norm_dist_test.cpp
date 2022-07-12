#include<random>
#include<iostream>

int main() {
    std::random_device rd;
    std::mt19937_64 rng(rd());
    std::normal_distribution<double> d(-400.0,-100.0);

    std::cout << d(rng) << std::endl;
}