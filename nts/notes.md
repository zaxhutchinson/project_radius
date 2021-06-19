**June 19, 2021**
*Version 0.07 work*

* Reworked data storage format to be example-based.
* Began work on dendrite visualization; however, this requires some background work on common data structures to store model output. Mundane stuff.
* Added output of meta data (iteration, example and label).
* Dendrite visualization v1 is done and this has helped spot a few bugs. I have been having trouble with the output layer going silent once the synapses start to move and the dendrite is built. When the network is instantiated, synapses are at MAX RADIUS, which is currently 100. At max radius, synapses connect directly to the soma. Which means their input is not subject to the nonlinearities of the dendritic tree. Now that I can see the dendritic connections, it looks like the dendrite building algorithm has a logical error. I'm seeing soma->synapse connections being made with synapses which are far from the soma; whereas synapses close to the soma form long chains. I will revisit this algorithm tomorrow.

**June 17, 2021**
*Version 0.06 work*

* Spent a lot of time debugging logical errors and bad parameter values. Since the project was started back in spring and only now brought to a working state, there's a clash between what I thought I was going to do and what I ended up doing. However, things are coming together. I made the first run in which the output and behavior looks ok. I will try a really long run tonight or tomorrow. After 10 iterations, the loss values indicate that spiking behavior isn't saturating to max or 0.
* An idea occurred to me to give the Presynatic() function the ability to boost synaptic strength no matter what. There might be several reasons for doing this. 1) It will prevent neurons from going completely dead forever. I have seen this used elsewhere (not sure the project). The other project to use this (IIRC) boosted only when the value was below some threshold. 2) It might be a way to avoid local minimums; however, this can be done in other ways.
* I've tacked on several things to various classes/functions within the project. I've overdue to spend a day cleaning up the code and thinking through these hacks.

**June 16, 2021**
*Version 0.06 work*

* The program is running, but it is slow. I've had to limit the number of connected synapses to which unconnected ones connect to 10, else the n^3 algorithm takes forever (of course it does).
* Output doesn't look good. Seems like the output neurons are all 0. A quick check shows that the input to the output neurons goes to infinity during the presentation of the first image. That seems to indicate something's not getting reset between each call to the neuron's GetInput().

**June 15, 2021**
*Version 0.05 work*

* Worked on RunMNIST() code. Looking back at the way I loaded MNIST data in Project Place, I've decided to offload the carving up of the data into the MNISTReader itself. Seems to be working.
* Added a reset to the whole network (layers, neurons, synapses, connections). I am hoping that a hard reset to the network will work better than an ITI. Previous experiments showed that even with a 1 sec ITI, network errors favored the previous item category by a few percent. A possible downside is that the network will be "cold" for each example. By cold, I mean that it takes several 10s of milliseconds before the neuron variables charge up to an operating state. Again, not sure this is a downside.
* Going to call this version 0.05.
* The sim is working, but there are some problems. Input is way too high, but I think that has to do with the way input is funneled down through the dendrite. I'm seeing error rates of -450, which means it is spiking at a rate of +450 hz. And I don't have baseline input in for the input neurons.
* Also, there might be a problem with the Writer. Need to debug that. It's not throwing errors, but nothing is being written.


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