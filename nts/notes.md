**June 11, 2021**
*Version 0.05 work*

* Added logging
* Added the first version of the network builder. Went pretty smooth. Seems to be working. It creates all the template objects. Next I'm going to work on the code that takes the templates and builds an actual network.
* First version that transforms templates into an actual network with layers,
neurons and synapses. So far, so good.


**June 10, 2021**
*Version 0.05 work*

* Layer, Network and Connection Templates added. These will be used to generate networks.
* Example network json file made. Establishes the starting format.

**June 4, 2021**
*Version 0.03 work*

Finished the first version of the algorithm which will rebuild the dendritic
tree. Came out easier than expected.

*Version 0.04 work*

* Worked on the first version of neural input from the dendritic tree. Not an
easy bit of code. Iterative solution used because synapses cannot talk to each
other unlike the previous version (project place). Depth first. Synapses
have a small set of transient values which store upstream signals. This allows
the algorithm to walk down to the leaves and push signals downstream
step-by-step.

* The next step is to work on the network builder. I'd intended to make this a
generic algorithm which uses a JSON template. I can already see that templates
are going to be large.

* I have decided to go with a layered network architecture by default. This
means networks will consist of layers of neurons (rather than an organic
approach which can accommodate more types of structures). However, layers
is a bit of misnomer. They are really groups of neurons which can connect
to other groups of neurons. This was done to make a simpler network JSON
template.

* Cleaned up a few things for 0.04