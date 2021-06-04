#include"sim.hpp"

void UpdateNeurons(i64 time, Writer * writer, vec<Neuron> & neurons, double error) {

    for(
        vec<Neuron>::iterator it = neurons.begin();
        it != neurons.end();
        it++
    ) {

        // Call the neuron's update method.
        it->Update(time,writer);

        // Pass the neuron's output to all axonal synapses.
        for(std::size_t i = 0; i < it->axons.size(); i++) {
            neurons[it->axons[i].first].PresynapticSignal(
                time, 
                it->axons[i].second,
                it->just_spiked
            );
        }

        // Pass the neuron's output to all dendritic synapses.
        it->PostsynapticSignal(time, error);

    }
}




void BuildDendrites(vec<Neuron> & neurons) {

    for(
        vec<Neuron>::iterator it = neurons.begin();
        it != neurons.end();
        it++
    ) {
        //-------------------------------------
        // Init lists.
        vec<Synapse> & syns = it->synapses;
        lst<i64> unconnected;
        lst<i64> connected;

        //-------------------------------------
        // Disconnect

        // Disconnect the dendrites.
        it->dendrites.clear();

        // Disconnect all synapses.
        // Complexity: Num Syns
        for(sizet i = 0; i < syns.size(); i++) {
            syns[i].parent = -1;
            syns[i].children.clear();
            unconnected.push_back(i);
        }


        while(!unconnected.empty()) {

            i64 usyn = *(unconnected.begin());
            i64 csyn = -1;
            double min_dist = syns[usyn].location.Rad();

            // Find the unconnected synapse with the smallest radius.
            for(
                lst<i64>::iterator ucit = unconnected.begin();
                ucit != unconnected.end();
                ucit++ 
            ) {

                // Begin by setting the min dist to the syn's radius.
                double local_min_dist = syns[*ucit].location.Rad();
                i64 local_csyn = -1;

                for(
                    lst<i64>::iterator cit = connected.begin();
                    cit != connected.end();
                    cit++
                )
                {
                    double dist_to_conn = syns[*ucit].location.DistanceWithRadius(syns[*cit].location);
                    double dist = 
                        dist_to_conn +
                        zxlb::BF * (
                            syns[*cit].GetDendritePathLength() +
                            dist_to_conn
                        );

                    if(dist < local_min_dist) {
                        local_min_dist = dist;
                        local_csyn = *cit;
                    }

                }

                
                if(local_min_dist < min_dist) {
                    min_dist = local_min_dist;
                    usyn = *ucit;
                    csyn = local_csyn;
                }
            }

            // Connect. If csyn is -1, connect to the soma,
            // else connect to already connected syn.
            // Set the dendritic path length.
            if(csyn == -1) {
                it->dendrites.push_back(usyn);
                syns[usyn].parent = -1;
                syns[usyn].SetDendritePathLength(syns[usyn].location.Rad());
            } else {
                syns[csyn].children.push_back(usyn);
                syns[usyn].parent = csyn;
                syns[usyn].SetDendritePathLength(min_dist);
            }

            // Update connected/unconnected lists.
            connected.push_back(usyn);
            unconnected.erase(
                std::find(
                    unconnected.begin(),
                    unconnected.end(),
                    usyn
                )
            );

        }
    }
}