
#include"network_template.hpp"


namespace tmps {

    using namespace nlohmann;

    void ReadNetworkTemplate(str filename) {
        std::ifstream ifs("tpl/"+filename);
        nlohmann::json j;
        ifs >> j;
        
        // Iterate through all the networks in the JSON file
        for(json::iterator nit = j.begin(); nit != j.end(); nit++) {

            NetworkTemplate network_template;
            network_template.network_template_id = nit.key();

            zxlog::Debug(
                "NetworkTemplate " + network_template.network_template_id + " loading."
            );

            json layers = nit.value().at("layers");

            // Iterate through this network's layers.
            for(json::iterator lit = layers.begin(); lit != layers.end(); lit++) {

                LayerTemplate layer_template;
                layer_template.layer_template_id = lit.key();

                zxlog::Debug("LayerTemplate " + layer_template.layer_template_id + " loading.");

                if(lit->contains("is_input_layer")) {
                    layer_template.is_input_layer = lit->at("is_input_layer");
                } else layer_template.is_input_layer = false;
                
                if(lit->contains("is_output_layer")) {
                    layer_template.is_output_layer = lit->at("is_output_layer");
                } else layer_template.is_output_layer = false;
                
                if(lit->contains("record_data")) {
                    layer_template.record_data = lit->at("record_data");
                } else layer_template.record_data = false;

                if(lit->contains("record_interval")) {
                    layer_template.record_interval = lit->at("record_interval");
                } else layer_template.record_interval = 1;

                if(lit->contains("record_data_size")) {
                    layer_template.record_data_size = lit->at("record_data_size");
                } else layer_template.record_data_size = 1000;

                if(lit->contains("neuron_templates")) {
                    json neuron_temps = lit->at("neuron_templates");
                    for(
                        json::iterator neuronit = neuron_temps.begin();
                        neuronit != neuron_temps.end();
                        neuronit++
                    ) {
                        NeuronTemplate neuron_template;
                        str neuron_type = neuronit->at("type");
                        opt<NeuronType> nt = GetNeuronType(neuron_type);
                        if(nt) {
                            neuron_template.type = *nt;
                        }
                        neuron_template.number = neuronit->at("number");
                        neuron_template.baseline = neuronit->at("baseline");

                        layer_template.neurons.push_back(neuron_template);
                    }
                } else {
                    zxlog::Error(
                        "LAYER TEMPLATE " + 
                        layer_template.layer_template_id +
                        " has zero neuron templates."
                    );
                }

                if(!layer_template.is_output_layer) {
                    zxlog::Warning(
                        "LAYER TEMPLATE "+
                        layer_template.layer_template_id +
                        " has no connections."
                    );
                }

                network_template.layers.push_back(
                    std::make_pair(lit.key(), layer_template)
                );

            }


            json conns = nit->at("connections");

            for(
                json::iterator cit = conns.begin();
                cit != conns.end();
                cit++
            ) {
                ConnectionTemplate connection_template;
                
                if(cit->contains("from_layer")) {
                    connection_template.from_layer =
                        cit->at("from_layer");
                }
                if(cit->contains("to_layer")) {
                    connection_template.to_layer =
                        cit->at("to_layer");
                }
                if(cit->contains("number_of_downstream_connections")) {
                    connection_template.number_of_downstream_connections =
                        cit->at("number_of_downstream_connections");
                }
                if(cit->contains("prob_of_making_downstream_connection")) {
                    connection_template.prob_making_downstream_connection =
                        cit->at("prob_of_making_downstream_connection");
                }
                if(cit->contains("min_starting_strength")) {
                    connection_template.min_starting_strength =
                        cit->at("min_starting_strength");
                }
                if(cit->contains("max_starting_strength")) {
                    connection_template.max_starting_strength =
                        cit->at("max_starting_strength");
                }
                if(cit->contains("max_strength")) {
                    connection_template.max_strength =
                        cit->at("max_strength");
                }
                if(cit->contains("min_radius")) {
                    connection_template.min_radius =
                        cit->at("min_radius");
                }
                if(cit->contains("max_radius")) {
                    connection_template.max_radius =
                        cit->at("max_radius");
                }
                if(cit->contains("polarity")) {
                    connection_template.polarity = 
                        cit->at("polarity");
                }

                network_template.connections.push_back(connection_template);
            }



            NETWORK_TEMPLATES.emplace(network_template.network_template_id, network_template);
        }

        

    }

}