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
            neurons[it->axons[i].first].PresynapticInput(
                time, 
                it->axons[i].second,
                it->just_spiked
            );
        }

        // Pass the neuron's output to all dendritic synapses.
        it->PostsynapticInput(time, error);

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
        lst<sizet> unconnected;
        lst<sizet> connected;
        lst<sizet>::iterator min_it;
        lst<sizet>::iterator to_it;
        lst<sizet>::iterator from_it;
        double min_dist;
        double cur_dist;

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

        //-------------------------------------
        // Connect initial syn

        // Find the synapse with the smallest Radius.
        min_dist = std::numeric_limits<double>::max();
        cur_dist = 0.0;
        min_it = unconnected.begin();
        for(
            lst<sizet>::iterator ucit = unconnected.begin();
            ucit != unconnected.end();
            ucit++
            ) {
            cur_dist = syns[*ucit].location.Rad();
            if(cur_dist < min_dist) {
                min_dist = cur_dist;
                min_it = ucit;
            }
        }

        // Connect the initial min
        connected.push_back(*min_it);    
        unconnected.erase(min_it);
        it->dendrites.push_back(*min_it);


        //-------------------------------------
        // Connect the rest

        double min_dist = 0.0;
        double cur_dist = 0.0;
        sizet from = 0;
        sizet to = 0;

        while(!unconnected.empty()) {

            // Reinit
            min_dist = std::numeric_limits<double>::max();
            to_it = unconnected.begin();

            // Find the unconnected synapse with the smallest radius.
            for(
                lst<sizet>::iterator ucit = unconnected.begin();
                ucit != unconnected.end();
                ucit++ 
            ) {

                cur_dist = syns[*ucit].location.Rad();
                if(cur_dist < min_dist) {
                    to_it = ucit;
                    min_dist = cur_dist;
                }
            }

            // Don't reset min distance because it currently contains
            // the radius of the syn closest to the soma. 
            // If none are closer, this syn will connect to the soma.


            // Find the connected syn closest to the to_it syn.
            from_it = connected.begin();

            for(
                lst<sizet>::iterator cit = connected.begin();
                cit != connected.end();
                cit
            ) {
                cur_dist = syns[*cit].location.DistanceWithRadius(syns[*to_it].location);
                if(cur_dist < min_dist) {
                    from_it = cit;
                    min_dist = cur_dist;
                }
            }


            // Connect. If none were closer than it is to the soma,
            // connect to soma, else connect to already connected syn.
            if(from_it == connected.end()) {
                it->dendrites.push_back(*from_it);
                syns[*to_it].parent = -1;
            } else {
                syns[*from_it].children.push_back(*to_it);
                syns[*to_it].parent = *from_it;
            }

            // Update connected lists.
            connected.push_back(*to_it);
            unconnected.erase(to_it);

        }
    }
}