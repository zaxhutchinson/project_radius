#include"builder.hpp"

uptr<Network> BuildNetwork(str network_id, RNG & rng) {
    
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

        

        layer.SetInput(lit->second.is_input_layer);
        layer.SetOutput(lit->second.is_output_layer);

        // Create and add the neurons
        for(
            vec<pair<tmps::NeuronTemplate, i64>>::iterator nit = lit->second.neurons.begin();
            nit != lit->second.neurons.end();
            nit++
        ) {
            for(i64 i = 0; i < nit->second; i++) {
                Neuron neuron(VecS(), nit->first);
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

    // Iterate back through all the layer templates so we can connect things.
    for(
        vec<pair<str,tmps::LayerTemplate>>::iterator lit = nettemp.layers.begin();
        lit != nettemp.layers.end();
        lit++
    ) {
    
        Layer * layer_us = network->GetLayer(layer_lookup.at(lit->first));

        // Iterate through all connection templates.
        for(
            vec<tmps::ConnectionTemplate>::iterator cit = lit->second.connections.begin();
            cit != lit->second.connections.end();
            cit++
        ) {
            // Find the downstream layer.
            i64 layer_ds_id = layer_lookup.at(cit->downstream_layer_id);
            Layer * layer_ds = network->GetLayer(layer_ds_id);

            double prob = cit->prob_making_downstream_connection;

            std::uniform_real_distribution<double> strengthDist(
                cit->min_starting_strength, cit->max_starting_strength
            );

            // All upstream neurons
            for(i64 us_i = 0; us_i < layer_us->GetLayerSize(); us_i++) {

                Neuron * neuron_us = layer_us->GetNeuron(us_i);

                // All downstream neurons
                for(i64 ds_i = 0; ds_i < layer_ds->GetLayerSize(); ds_i++) {

                    if(dice(rng) < prob) {
                        Neuron * neuron_ds = layer_ds->GetNeuron(ds_i);

                        // Set location
                        double radius = cit->radius;
                        VecS loc(radius);
                        loc.RandomizeLatLong(rng);

                        // Build synapse
                        Synapse synapse(
                            loc,
                            cit->max_strength,
                            strengthDist(rng)
                        );

                        i64 synapse_id = neuron_ds->AddSynapse(std::move(synapse));

                        Axon axon;
                        axon.layer_index = layer_ds->GetID();
                        axon.neuron_index = neuron_ds->GetID();
                        axon.synapse_index = synapse_id;

                        neuron_us->AddAxon(axon);
                    }
                }

            }
        }
    
    }


    return network;
}