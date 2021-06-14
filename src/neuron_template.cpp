#include"neuron_template.hpp"

namespace tmps {

    opt<NeuronTemplate> GetNeuronTemplate(str name) {
        opt<NeuronTemplate> nt;
        
        if(name=="Regular Spiking") {
            nt = NT_REGULAR_SPIKING;
        } else if(name == "Chattering") {
            nt = NT_CHATTERING;
        } else if(name == "Medium Spiny") {
            nt = NT_MEDIUM_SPINY;
        } else if(name == "Relay") {
            nt = NT_RELAY;
        } else if(name == "Fast Spiking") {
            nt = NT_FAST_SPIKING;
        } else if(name == "Reticular Thalamic") {
            nt = NT_RETICULAR_THALAMIC;
        } else if(name == "Intrinsic Bursting") {
            nt = NT_INTRINSIC_BURSTING;
        }

        return nt;
    }

}