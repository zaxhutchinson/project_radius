#pragma once

#include<fstream>

#include<json.hpp>

#include"zxlb.hpp"
#include"log.hpp"
#include"neuron_template.hpp"

namespace tmps {

    struct ConnectionTemplate {
        str downstream_layer_id;
        i64 number_of_downstream_connections;
        double prob_making_downstream_connection;
        double min_starting_strength;
        double max_starting_strength;
        double max_strength;
        double radius;
    };

    struct LayerTemplate {
        str layer_template_id;
        vec<pair<NeuronTemplate, i64>> neurons;
        bool is_input_layer;
        bool is_output_layer;
        vec<ConnectionTemplate> connections;
    };

    struct NetworkTemplate {
        str network_template_id;
        vec<pair<str, LayerTemplate>> layers;
    };


    void ReadNetworkTemplate(str filename);


    inline umap<str, NetworkTemplate> NETWORK_TEMPLATES;

}
