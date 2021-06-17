#include"sim.hpp"



void UpdateNetwork(
    i64 time, 
    Writer * writer, 
    Network * network, 
    vec<double> & inputs,
    vec<double> & outputs,
    double error
) {

    // Set the raw input for neurons in the input layer.
    // network->SetInputs(inputs);

    // // Update the network
    // network->Update(time, writer);

    // // Update the output list.
    // network->SetOutputs(outputs);
}



// void UpdateNeurons(i64 time, Writer * writer, vec<Neuron> & neurons, double error) {

//     for(
//         vec<Neuron>::iterator it = neurons.begin();
//         it != neurons.end();
//         it++
//     ) {

//         // Call the neuron's update method.
//         it->Update(time,writer);

        
//         //Pass the neuron's output to all axonal synapses.
//         // for(std::size_t i = 0; i < it->axons.size(); i++) {
//         //     neurons[it->axons[i].first].PresynapticSignal(
//         //         time, 
//         //         it->axons[i].second,
//         //         it->just_spiked
//         //     );
//         // }

//         // Pass the neuron's output to all dendritic synapses.
//         it->PostsynapticSignal(time, error);

//     }
// }




