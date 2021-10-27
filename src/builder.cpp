#include"builder.hpp"

uptr<Network> BuildNetwork(str network_id, RNG & rng) {

    zxlog::Debug("Building network " + network_id);
    
    // If not such network id, return null
    if(!tmps::NETWORK_TEMPLATES.contains(network_id)) {
        return nullptr;
    }

    uptr<Network> network = std::make_unique<Network>();

    // Get the network template
    tmps::NetworkTemplate nettemp = tmps::NETWORK_TEMPLATES.at(network_id);

    umap<str,i64> layer_lookup;

    // Create the layers.
    for(
        vec<pair<str,tmps::LayerTemplate>>::iterator lit = nettemp.layers.begin();
        lit != nettemp.layers.end();
        lit++
    ) {
        Layer layer;
        layer.SetName(lit->first);

        layer.SetIsInput(lit->second.is_input_layer);
        layer.SetIsOutput(lit->second.is_output_layer);

        // Create and add the neurons
        for(
            vec<tmps::NeuronTemplate>::iterator nit = lit->second.neurons.begin();
            nit != lit->second.neurons.end();
            nit++
        ) {
            for(i64 i = 0; i < nit->number; i++) {
                Neuron neuron(VecS(), nit->type);
                neuron.baseline = nit->baseline;
                neuron.record_data = lit->second.record_data;
                neuron.record_interval = lit->second.record_interval;
                neuron.record_data_size = lit->second.record_data_size;
                neuron.SetID(i);
                layer.AddNeuron(std::move(neuron));
            }
        }

        // Add layer to network and capture the layer's network-assigned id.
        i64 layer_id = network->AddLayer(std::move(layer));

        // Add layer to the lookup table.
        layer_lookup.insert({lit->first, layer_id});
    }


    std::uniform_real_distribution<double> dice(0.0, 1.0);

    zxlog::Debug("Building connections.");
    

    // Iterate through all connection templates.
    for(
        vec<tmps::ConnectionTemplate>::iterator cit = nettemp.connections.begin();
        cit != nettemp.connections.end();
        cit++
    ) {
        // Find the downstream layer.

        i64 layer_to_id = layer_lookup.at(cit->to_layer);
        i64 layer_from_id = layer_lookup.at(cit->from_layer);
        Layer * layer_to = network->GetLayer(layer_to_id);
        Layer * layer_from = network->GetLayer(layer_from_id);

        double prob = cit->prob_making_downstream_connection;

        // zxlog::Debug(
        //     "   Connection: " + lit->first + " to " + std::to_string(layer_ds_id) +
        //     " with prob " + std::to_string(prob)
        // );

        std::uniform_real_distribution<double> strengthDist(
            cit->min_starting_strength, cit->max_starting_strength
        );

        // All upstream neurons
        for(i64 us_i = 0; us_i < layer_from->GetLayerSize(); us_i++) {

            Neuron * neuron_us = layer_from->GetNeuron(us_i);

            // All downstream neurons
            for(i64 ds_i = 0; ds_i < layer_to->GetLayerSize(); ds_i++) {

                for(i64 syn_i = 0; syn_i < cit->number_of_downstream_connections; syn_i++) {
                    
                    Neuron * neuron_ds = layer_to->GetNeuron(ds_i);

                    if(dice(rng) < prob) {
                        

                        // Set location
                        std::uniform_real_distribution<double> radDist(
                            cit->min_radius, cit->max_radius
                        );
                        double radius = radDist(rng);
                        

                        ConnectionAddress ca;
                        ca.pre_layer = layer_from->GetID();
                        ca.pre_neuron = neuron_us->GetID();
                        ca.post_layer = layer_to->GetID();
                        ca.post_neuron = neuron_ds->GetID();


                        VecS loc(radius);
                        loc.RandomizeLatLong(rng);
                        Synapse synapse(
                            loc,
                            cit->max_strength,
                            strengthDist(rng),
                            cit->polarity
                        );
                        synapse.SetLayerID(layer_to_id);
                        synapse.SetNeuronID(ds_i);
                        synapse.SetConnectionAddress(ca);
                        neuron_ds->AddSynapse(std::move(synapse));
                        
                    }
                }
            }
        }
    }



    return network;
}

