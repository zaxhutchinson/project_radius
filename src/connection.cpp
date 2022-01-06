#include"connection.hpp"

Connection::Connection() {}


void Connection::SetDownStreamErrorRate(i64 index, double rate) {
    downstream_error_rates.insert_or_assign(index,rate);
}

double Connection::GetDownStreamErrorRate() {
    double rate = 0.0;
    for(
        umap<i64,double>::iterator it = downstream_error_rates.begin();
        it != downstream_error_rates.end();
        it++
    ) {
        rate += it->second;
    }
    if(downstream_error_rates.size() > 0)
        return rate / downstream_error_rates.size();
    else
        return rate;
}

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
    downstream_error_rates.clear();
    downstream_error_rates.emplace(-1,_target_rate);
}


// double Connection::GetErrorRateRaw() {
//     return target_rate;
// }
// double Connection::GetErrorRate() {
//     return target_rate - (static_cast<double>(num_spikes) / (static_cast<double>(time) / 1000.0));
// }

double Connection::GetErrorRateNorm() {
    double rate = GetDownStreamErrorRate();
    if(rate != 0.0)
        return rate / std::abs(rate);
    else
        return rate;
    // double e = target_rate - (static_cast<double>(num_spikes) / (static_cast<double>(time) / 1000.0));
    // return e / (std::abs(e) + target_rate);
}
double Connection::GetErrorRateNormReLU() {
    return std::max(0.0,GetErrorRateNorm());
    // double e = target_rate - (static_cast<double>(num_spikes) / (static_cast<double>(time) / 1000.0));
    // return e / (std::abs(e) + target_rate);
}

// double Connection::GetErrorRateReLU() {
//     double err = GetErrorRateRaw();
//     if(err > 0.0) return err;
//     else return 0.0;
// }

// double Connection::GetErrorRateReLU_1() {
//     double err = GetErrorRateRaw();
//     if(err > 0.0) return err;
//     else return 1.0;
// }