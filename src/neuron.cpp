#include"neuron.hpp"


Neuron::Neuron(
    int _id,
    VecS _loc,
    const NeuronTemplate & _nt
)   :   id(_id),
        location(_loc),
        nt(_nt)
{
    
    vpre = vcur = nt.c;
    upre = ucur = nt.d;
    output = 0.0;
    baseline = 0.0;
    raw_input = 0.0;
    record_data = false;
    record_interval = 1;
    record_interval = 1000;
    time_cur_spike = 0;
    time_pre_spike = 0;
    just_spiked = false;
    ResetWriteData();
}


void Neuron::SetBaseline(double amt) {
    baseline = amt;
}
void Neuron::SetRawInput(double amt) {
    raw_input = amt;
}

void Neuron::PresynapticInput(i64 time, int synapse_id, bool pre_spike) {
    synapses[synapse_id].SetSignal(time,output);
    if(pre_spike) {
        synapses[synapse_id].SetCurSpike(time);
        PresynapticSpike(time, synapse_id);
    }
}

void Neuron::PresynapticSpike(i64 time, int synapse_id) {

    double time_diff_self = static_cast<double>(
        synapses[synapse_id].time_cur_spike - synapses[synapse_id].time_pre_spike
    );

    Vec3 force;
    double partial_force = 0.0;

    for(std::size_t i = 0; i < synapses.size(); i++) {
        if(i != synapse_id) {
            double time_diff_other = static_cast<double>(
                time - synapses[synapse_id].time_cur_spike
            );

            double distance = synapses[synapse_id].location.Distance(synapses[i].location);

            double force_self = (
                (time_diff_self+zxlb::PRE_SELF_FORCE_TIME_WINDOW) /
                zxlb::PRE_SELF_FORCE_TIME_WINDOW *
                std::exp( - time_diff_self / zxlb::PRE_SELF_FORCE_TIME_WINDOW )
            );

            double force_other = (
                (time_diff_other + zxlb::PRE_OTHER_FORCE_TIME_WINDOW) /
                zxlb::PRE_OTHER_FORCE_TIME_WINDOW *
                std::exp( - time_diff_other / zxlb::PRE_OTHER_FORCE_TIME_WINDOW )
            );

            partial_force = (
                (force_self * force_other) *
                std::tanh(distance)
            );

            force += synapses[synapse_id].location.VectorTo(synapses[i].location) * partial_force;
        }
    }

    force /= static_cast<double>(synapses.size()-1);
    
    synapses[synapse_id].location.Orbit(force.ToBearing(), force.Length());

}

void Neuron::PostsynapticInput(i64 time, double error) {

    if(just_spiked) {

        for(std::size_t i = 0; i < synapses.size(); i++) {

            double time_diff_synapse = static_cast<double>(
                synapses[i].time_cur_spike - synapses[i].time_pre_spike
            );

            double time_diff_soma = static_cast<double>(
                time_cur_spike - synapses[i].time_cur_spike
            );

            double distance = time_diff_synapse - synapses[i].location.Rad();

            // double force_synapse = (
            //     (time_diff_synapse + zxlb::POST_SYNAPSE_FORCE_TIME_WINDOW) /
            //     zxlb::POST_SYNAPSE_FORCE_TIME_WINDOW *
            //     std::exp( - time_diff_synapse / zxlb::POST_SYNAPSE_FORCE_TIME_WINDOW)
            // );

            double force_soma = (
                (time_diff_soma + zxlb::POST_SOMA_FORCE_TIME_WINDOW) /
                zxlb::POST_SOMA_FORCE_TIME_WINDOW *
                std::exp( - time_diff_soma / zxlb::POST_SOMA_FORCE_TIME_WINDOW)
            );

            synapses[i].location.Rad(
                force_soma *
                std::tanh(distance)
            );

        }

        for(std::size_t i = 0; i < dendrites.size(); i++) {
            bAP(time, dendrites[i], output, error);
        }

    }


    

}


void Neuron::bAP(i64 time, int synapse_id, double amt, double error) {
    double prepost_time_diff = static_cast<double>(
        time_cur_spike - synapses[synapse_id].time_cur_spike
    );

    double time_delta = (
        ((prepost_time_diff+zxlb::LEARNING_TIME_WINDOW) / zxlb::LEARNING_TIME_WINDOW) *
        std::exp( -prepost_time_diff/zxlb::LEARNING_TIME_WINDOW)
    );

    double str_delta = (
        1.0 - std::abs(synapses[synapse_id].cur_strength) / 
        (synapses[synapse_id].max_strength + std::abs(synapses[synapse_id].cur_strength))
    );

    synapses[synapse_id].cur_strength += (time_delta * str_delta * error);

    double bap_amt = std::tanh(time_delta + amt);

    for(std::size_t i = 0; i < synapses[synapse_id].children.size(); i++) {
        bAP(time, synapses[synapse_id].children[i], bap_amt, error);
    }
}


void Neuron::Update(i64 time, Writer * writer) {
    
    // Calculate input
    double input = baseline + raw_input;
    for(
        vec<int>::iterator it = dendrites.begin();
        it != dendrites.end();
        it++
    ) {
        // Get input from synapses. However that happens.
    }

    // Calculate voltage
    vcur = vpre +
    (
        nt.k * (vpre - nt.vr) * (vpre - nt.vt) -
        upre +
        input
    ) / nt.cap;

    // Calculate recovery
    ucur = upre +
        nt.a *
        (nt.b * (vpre - nt.vr) - upre);

    // Is there a spike?
    if(vcur > nt.vpeak) {
        vpre = vcur = nt.c;
        upre = ucur = ucur + nt.d;
        time_pre_spike = time_cur_spike;
        time_cur_spike = time;
        spike_times_live.push_back(time);
        spike_times_data.push_back(time);
        just_spiked = true;
        // Add Back prop
    
    } else {
        vpre = vcur;
        upre = ucur;
        just_spiked = false;
    }

    output = 0.0;
    double diff = 0.0;
    for(
        lst<i64>::iterator it = spike_times_live.begin();
        it != spike_times_live.end();
    ) {
        diff = time - (*it);
        if(diff > zxlb::NEURON_SPIKE_TIME_WINDOW) {
            it = spike_times_live.erase(it);
        } else {
            output += diff * std::exp( -(diff - 1.0) );
        }
    }

    output = std::tanh(output);

    if(record_data && (time%record_interval)==0) {
        WriteData(time, writer);
    }
}

void Neuron::ResetWriteData() {
    data = std::make_unique<NeuronData>();
    data->id = "NEURON_"+std::to_string(id);
    data->data_size = 0;
}

void Neuron::CleanupData(Writer * writer) {
    if(data->data_size > 0) {
        writer->AddNeuronData(std::move(data));
        ResetWriteData();
    }
}

void Neuron::WriteData(i64 time, Writer * writer) {
    if(data->data_size == record_data_size) {
        writer->AddNeuronData(std::move(data));
        ResetWriteData();
    }
    data->data_size++;
    data->time_indexes.push_back(time);
    data->locations.push_back(location);
    data->v.push_back(vcur);
    data->u.push_back(ucur);
    data->spike_times.push_back(spike_times_data);
    spike_times_data.clear();   // Clear out spike times.
    data->output.push_back(output);
}