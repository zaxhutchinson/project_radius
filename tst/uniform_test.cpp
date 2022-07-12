#include<iostream>
#include<random>
#include<cmath>

int main() {
    std::random_device rd;
    std::mt19937_64 rng(rd());
    std::uniform_real_distribution<double> dist(M_PI,-M_PI);

    for(int i = 0; i < 10; i++) {
        std::cout << dist(rng) << std::endl;
    }
}