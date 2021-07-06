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

            zxlog::Debug(
                "   Connection: " + lit->first + " to " + std::to_string(layer_ds_id) +
                " with prob " + std::to_string(prob)
            );

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
                        std::uniform_real_distribution<double> radDist(
                            cit->min_radius, cit->max_radius
                        );
                        double radius = radDist(rng);
                        VecS loc(radius);
                        loc.RandomizeLatLong(rng);

                        // Build synapse
                        Synapse synapse(
                            loc,
                            cit->max_strength,
                            strengthDist(rng)
                        );
                        synapse.SetLayerID(layer_ds_id);
                        synapse.SetNeuronID(ds_i);
                        

                        ConnectionAddress ca;
                        ca.pre_layer = layer_us->GetID();
                        ca.pre_neuron = neuron_us->GetID();
                        ca.post_layer = layer_ds->GetID();
                        ca.post_neuron = neuron_ds->GetID();

                        synapse.SetConnectionAddress(ca);

                        neuron_ds->AddSynapse(std::move(synapse));
                    }
                }

            }
        }
    
    }


    return network;
}

