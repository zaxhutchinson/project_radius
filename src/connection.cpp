#include"connection.hpp"

Connection::Connection() {}


void Connection::SetPosDownStreamErrorRate(i64 index, double rate) {
    pos_downstream_error_rates.insert_or_assign(index,rate);
}

double Connection::GetPosDownStreamErrorRate() {
    double rate = 0.0;
    for(
        umap<i64,double>::iterator it = pos_downstream_error_rates.begin();
        it != pos_downstream_error_rates.end();
        it++
    ) {
        rate += it->second;
    }
    if(pos_downstream_error_rates.size() > 0)
        return rate / pos_downstream_error_rates.size();
    else
        return rate;
}

void Connection::SetWeightDownStreamErrorRate(i64 index, double rate) {
    weight_downstream_error_rates.insert_or_assign(index,rate);
}

double Connection::GetWeightDownStreamErrorRate() {
    double rate = 0.0;
    for(
        umap<i64,double>::iterator it = weight_downstream_error_rates.begin();
        it != weight_downstream_error_rates.end();
        it++
    ) {
        rate += it->second;
    }
    if(weight_downstream_error_rates.size() > 0)
        return rate / weight_downstream_error_rates.size();
    else
        return rate;
}

double Connection::GetPosTargetErrorRate() {
    return pos_target_rate;
}
double Connection::GetWeightTargetErrorRate() {
    return weight_target_rate;
}

void Connection::Update(bool _just_spiked, double _output) {
    just_spiked = _just_spiked;
    output = _output;
    if(just_spiked) num_spikes++;
    time++;
}

void Connection::Reset(
    double _target_rate
) {
    output = 0.0;
    time = 1;
    just_spiked = false;
    pos_target_rate = _target_rate;
    weight_target_rate = _target_rate;
    num_spikes = 0;
    pos_downstream_error_rates.clear();
    pos_downstream_error_rates.emplace(-1,_target_rate);
    weight_downstream_error_rates.clear();
    weight_downstream_error_rates.emplace(-1,_target_rate);
}
void Connection::Reset(
    double _pos_target_rate,
    double _weight_target_rate
) {
    output = 0.0;
    time = 1;
    just_spiked = false;
    pos_target_rate = _pos_target_rate;
    weight_target_rate = _weight_target_rate;
    num_spikes = 0;
    pos_downstream_error_rates.clear();
    pos_downstream_error_rates.emplace(-1,_pos_target_rate);
    weight_downstream_error_rates.clear();
    weight_downstream_error_rates.emplace(-1,_weight_target_rate);
}

// double Connection::GetErrorRateRaw() {
//     return target_rate;
// }
// double Connection::GetErrorRate() {
//     return target_rate - (static_cast<double>(num_spikes) / (static_cast<double>(time) / 1000.0));
// }

double Connection::GetPosErrorRateNorm() {
    double rate = GetPosDownStreamErrorRate();
    if(rate != 0.0)
        return rate / std::abs(rate);
    else
        return rate;
    // double e = target_rate - (static_cast<double>(num_spikes) / (static_cast<double>(time) / 1000.0));
    // return e / (std::abs(e) + target_rate);
}
double Connection::GetPosErrorRateNormReLU() {
    return std::max(0.0,GetPosErrorRateNorm());
    // double e = target_rate - (static_cast<double>(num_spikes) / (static_cast<double>(time) / 1000.0));
    // return e / (std::abs(e) + target_rate);
}
double Connection::GetWeightErrorRateNorm() {
    double rate = GetWeightDownStreamErrorRate();
    if(rate != 0.0)
        return rate / std::abs(rate);
    else
        return rate;
    // double e = target_rate - (static_cast<double>(num_spikes) / (static_cast<double>(time) / 1000.0));
    // return e / (std::abs(e) + target_rate);
}
double Connection::GetWeightErrorRateNormReLU() {
    return std::max(0.0,GetWeightErrorRateNorm());
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