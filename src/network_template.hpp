#pragma once

#include<fstream>
#include<iostream>

#include<json.hpp>

#include"zxlb.hpp"
#include"log.hpp"
#include"neuron_type.hpp"

namespace tmps {

    struct ConnectionTemplate {
        str downstream_layer_id;
        i64 number_of_downstream_connections;
        double prob_making_downstream_connection;
        double min_starting_strength;
        double max_starting_strength;
        double max_strength;
        double min_radius;
        double max_radius;
    };

    struct NeuronTemplate {
        NeuronType type;
        i64 number;
        double baseline;
    };

    struct LayerTemplate {
        str layer_template_id;
        vec<NeuronTemplate> neurons;
        bool is_input_layer;
        bool is_output_layer;
        vec<ConnectionTemplate> connections;
        bool record_data;
        sizet record_interval;
        sizet record_data_size;
    };

    struct NetworkTemplate {
        str network_template_id;
        vec<pair<str, LayerTemplate>> layers;
    };


    void ReadNetworkTemplate(str filename);


    inline umap<str, NetworkTemplate> NETWORK_TEMPLATES;

}
