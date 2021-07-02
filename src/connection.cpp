#include"connection.hpp"

void Connection::Update(bool _just_spiked, double _output) {
    just_spiked = _just_spiked;
    output = _output;
    if(just_spiked) num_spikes++;
    time++;
}
void Connection::SetErrorRate(double rate) {
    target_rate = rate;
}
void Connection::Reset(
    double _target_rate
) {
    output = 0.0;
    time = 1;
    just_spiked = false;
    target_rate = _target_rate;
    num_spikes = 0;
}


double Connection::GetErrorRateRaw() {
    return target_rate;
}
double Connection::GetErrorRate() {
    return target_rate - (static_cast<double>(num_spikes) / (static_cast<double>(time) / 1000.0));
}

double Connection::GetErrorRateNorm() {
    double e = target_rate - (static_cast<double>(num_spikes) / (static_cast<double>(time) / 1000.0));
    return e / (std::abs(e) + target_rate);
}

double Connection::GetErrorRateReLU() {
    double err = GetErrorRateRaw();
    if(err > 0.0) return err;
    else return 0.0;
}