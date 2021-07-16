# IDEAS

This file contains random ideas that crop up during but cannot be tried right away.

### Two-stage training
It might be necessary to run networks through two stages of training. The first will move synapses around but activity will not be subject to the dendritic tree. The idea is, we need a tree before it can be used. Once a certain amount of time expires, we turn the tree on.

Ideally, the spacing of synapses corresponds to the time it takes for a signal to move from one to another, or to the soma. Right now, that isn't the case. Radial position is the temporal proximit of input to output. I doubt any dendrite takes 100 ms to move a signal from its most distant synapse to the soma. Not sure.