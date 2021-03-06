**July 15, 2021**
*Version 0.11 work*
* I have been struggling to find the reason synapse locations drift into polar clusters. After much gnashing of teeth and doubting of reality and testing of code I have discovered that too much incorrect input (negative error rate) causes this. I am fairly sure that synapses move away from each other and, depending on their initial distribution over the sphere, congregate in opposing hemispheres. When brought together by positive error rates, this solidifies the poles further. This seems to be also the reason why the clusters, once they come together, rotate in the same direction. This depends on the update order. I'm not 100% on how it works, but testing small numbers of synapses (2) produces the same results.
* Therefore, I have drastically lowered the amount of mask instances in the spike train tests. And lowered their spike rate. I suspect, if the errror is too small, pattern clusters will congregate where they may. Separation of clusters will depend largely on initial location. I need just enough to cause patterns to drift away from each other. 
* The impact of repelling signals on location is different for Spherical geometry than open Cartesian coords. Is this because the angular positions are bounded? A point in 3d coords can simple rocket off out into space to move "away" from all the other points. But a point bound to move over a sphere.
* 0.1 correct to 0.01 incorrect seems to be too low. Currently trying 0.1 to 0.05.
* Turns out that I was moving in the wrong direction. 0.01 seems too high. Lowering to 0.005 improved things a lot. It is likely that my learning rate is too high and should be lowered by a factor or two. The Pois004 4 distinct pattern comes out nicely. All synapses cluster together in separate quardrants (roughly) and connect to the soma along distinct dendrites. In this respect, keeping a certain angular distance ensures this dendritic separation.

**July 14, 2021**
*Version 0.11 work*
* Found a really dumb bug. I've been generating random latitudes (phi) using -PI to PI, but this clusters everything near the poles. This is probably why I'm seeing syns move toward the poles rather than across the equator. Fixed (supposedly) to acos(2.0 * rand(0,1) - 1) from this site: https://www.bogotobogo.com/Algorithms/uniform_distribution_sphere.php. This occurred to me a while back and I'm not sure why I didn't follow up on it.
* The above is wrong, too, for my lat-lon method. It would work if Phi were a measure from the pole down. Must subtract pi/2 to shift the range from -pi/2 to pi/2. 

**July 9-13, 2021**
*Version 0.11 work*
* I've started a write up of the current work. Today I made a small change to the radial movement code. I found that I've been basing synapstic pre-activity on cur_spike-pre_spike. This, I think, I wrong. It would allow for radial movement if a synapse experienced two temporally near inputs a long time ago. Or even just once...and then any time the downstream neuron spikes, it will cause movement. I changed it to time-cur_spike. This should capture how recently the upstream neuron attached to a synapse spiked in relation to the post_synaptic activity.
* With the change above, the time_diff_synapse and time_diff_soma in the postsynapstic function were identical. Rethinking the soma part, I changed it to document the activity of the soma using cur and pre spike times. The more active the neuron is, the more likely a change. Run again. I doubt this will have a huge impact, but we'll see.

**July 7, 2021**
*Version 0.10 work*
* Added a Poisson-based spike train generator and have tested a single neuron exposed to two overlapping patterns. The algorithm(s) nicely separate the two patterns and the overlap inputs move into a space between the two. Pushing so that I have a copy of this that works.

**July 3-6, 2021**
*Version 0.09 work*

* Added an InputGenerator class which allows for (at present) the generation of simple input patterns which vary by time. I hoped to use this to discriminate by temporally varying patterns. For example, sin v. cos input. It's a bit clunky, but I think the structure is on the right path. With some expansion it could be used to present time-varying, predetermined patterns (e.g. spike trains).
* Added some parallelization using OpenMP. Very necessary. The full MNIST with 794 neurons and 7840 synapses just runs too slowly without. With, it is quite tolerable.
* I think the next step is to produce a paper on the synapse clustering. If I can mimick an input spike train to a neuron and then use this to cluster synapses which spike together, that would be something. Then I can do some analysis comparing the spike trains and the location of the synapses and how the dendrite connects them. Thumbs up.

**July 2, 2021**
*Version 0.09 work*

* Found a potential error in the presyn movement calculations. Looks like I was using synapse_id instead of i to get the other_time_diff.
* Implemented the orbital distance-time movement discussed in the previous entry. Synapses of a similar pattern move either into the same space or to an antipodal space. Once the synapses congregate into one of these two locations, they seem to lock orbits. Either they hover or actually sync in opposite orbits. Not sure why. It seems one would move in some direction and then the next would move toward in the oblique direction. But they don't. Depends on starting location. 

**July 2, 2021**
*Version 0.08 work*

* Looks like I didn't push on the 30th. Heh.
* State of things: The algorithm(s) is working well. Synapses of simple patterns (001) move together into a tight group with the current configuration. These are non-overlapping patterns.
    * Radial movement: Should radial movement happen regardless of the error signal or not? If it happens without respect to the error, irrelevant synapses move into orbits similar to the relevant ones. If they use the raw error. ISynapses stay at max radius. I have also experiemented with ReLU error. This has the same effect. ISyns stay at max because there is no overlap. This leaves three raw error options. Basic raw, ReLU and no error for radial movement. I suspect there might be use cases for each one. At first I thought having ISyns mixed among the Rsyns was no good, but if I keep in mind that eventually I would like to incorporate inhibitory signals, mixing the two radially has possibilities.
    * I had a lot of trouble nailing down why, initially, the orbital movement did not cluster synapses. There were two major errors. While I tested orbital movement in abstract successfully, it did not take into account how I would compile the force direction from multiple other synapses. The current solution, now, seems the most obvious. I copy the VecS into a temp variable, and then orbit this temp VecS toward all co-active synapses using the partial force as a distance measure. When this is done, I use the final destination of the temp VecS to get a direction for the actual movement scaled by the learning rate. Without the learning rate, I could just copy the temp back into the actual. As is, it's a one step slerp. Secondly, I had a stupid bug in my dendrite visualization, so even once it was working, the synapses were labeled incorrectly. This threw me off for a few hours.
    * Orbital mechanics are more expensive to compute. I think I, as some point, should investigate approximations. If synapses are moving short distances, so long as the heading is in some ballpark perhaps I can make a trade. Dunno. Another possibility is that something can be precomputed. Not sure what since everything is potentially in motion.
    * Now that things are working, I want to clean up the VecS class. And I would like to incorporate a mechanism to switch between the old 3d version from the proposal and the new orbital one. I think the 3d version might be more intuitive for a clustering paper.
    * Early on in this version I had to rewrite the dendrite building algorithm several times. While the Cuntz2007 algorithm seemed very compatible with this idea, it came with a few things to iron out. Nothing major. For example, synapses that connect directly to the soma use the synapse's radial measurement, whereas syn-syn connections use a straight-line calculation which requires converting the VecS' of each into Vec3s. Stuff like that.
    * The current pattern work isn't using the dendrite to get input. It's using a simple input summation with synaptic strength. The dendrite-based input is going to be difficult to get right. I suspect, I'm missing something crucial. The problem is, when a synapse is buried within a dendrite, (much like the real thing) its signal doesn't reach the soma. If it's signal doesn't reach the soma, it can't gain strength. This is equivalent to the issue where weights go to zero and a network dies. My initial idea is to have pre-synaptic activity increase synaptic strength without regard to the post-synaptic response. Not sure I like it though. This aspect of the algorithm has a tendency to get too complex and I'm fighting that urge (to simulate). I'm looking for a simplification. At the end of the day, I think the synapse movement algorithms, strength change and how the dendrite collects and affects input all have to work in harmony. Hmmm.
    * For Version 0.09. Currently, syn movement generated by pre-syn activity draws syns closer and closer. I think a better approach would be to borrow the idea used in post-syn gen movement. Syns don't move ever toward the position of another syn, they move in the direction of the other syn, but they move toward a point some distance away. This distance is determined by the ISI between the two syns...just like between syn and soma. The trick will be to get the orbital stuff to work with this idea without tripling the computation time. Max window = PI. Moves toward an angular distance based on the time interval. For example, if max window=100. And the avg ISI is 25. Then the syn will move toward a location pi/4 away from the other syn. This has potential consequences in that angular distance might place two synapses on different dendritic trees. Yeah...not sure.


**June 26-30, 2021**
*Version 0.08 work*

* Going to push all this stuff today regardless (30th).
* I am recreating the simple pattern experiments from the paper using the spherical movement rather than straight line; however, I think the distance calculations are still off. The build dendrite algorithm should, at each step, select the closest pair of synapses - one from the unconnected list and one from the connected. However, I am seeing dendrites which curiously zigzag back and forth over a 0.0 axis. This makes me suspect that something is wrong and one axis is using the absolute value. I have suspected this before, but thought I fixed it. Seems not.
* I am also going to make a distance matrix and a connection matrix. The vis_dendrite.py makes fancy pictures of connected dendrites, but it can be hard to see what's what.
* CLARIFICATION: I take back what I said above. I think the dendrite building algo is working. The scale of the different axis threw me off.

**June 25, 2021**
*Version 0.08 work*

* Ironed out a few bugs involving VecS, spherical-to-cartesian and writing to file. All of this impacted how the dendrites were being created. I'm confident that dendrite building is alright.
* The next step is to examine the movement of synapses. I think the strength is changing too slowly and the locations are changing too quickly.

**June 23, 2021**
*Version 0.08 work*

* Reworked the BuildDendrite algorithm. Not sure how I thought the first version was a good one. Apparently, I'd made an unnecessary 3rd inner loop, making an N^3 algorithm when that wasn't necessary at all. It's now something like (N(2N-1))/2. I was a bit surprised when the building took far longer than it had for the original. It is more complicated, but just by a constant factor. Running again and will compare to the output of the previous version.

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