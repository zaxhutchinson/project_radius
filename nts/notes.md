**GENERAL**
* Randomizing neuron order within a layer removes the orbiting of synapses. They no longer chase each otehr.
* It is very important to scale input into the network so that the min and max spike rates mesh with the learning rate window. If they are dialed too high, the all activity will be within the windows and cause clustering or strength increase. 0-100 or 0-200 seems to be OK. This also depends on how many inputs one has. In the eeg_ac experiment, the raw values have too large a swing.

**Vert/Hor Lines Experiment**
* Problematic. On the one hand, the neuron had to learn that horizontal lines are good and vertical ones are not. I can see this working if all synapses of a pattern move into one dendrite. This way the critical mass from one dendrite will drive the neuron. And when active inputs are spread across multiple dendrites (vert non-patterns), they cannot move the soma. But what happens is that most synapses cluster into separate dendrites. But not all do. And some get a late start...probably for initial value reasons. So they become the big negative allowing the system to learn. The result is that the neuron learns all the patterns but one, which has a strong negative weight. Not sure how to fix this one or if it was just a bad experiment idea.

**Version 0.15 work**
* The next step is to turn on radial and connectivity.
* All aspects of the algorithm have been given an initial version. Radius and Angular Distance (spread) are unchanged from 0.14. The weight training underwent an overhaul but it remains, more or less, Hebbian. And compartments saw an initial implementation. Turned out to be pretty easy to embed into the BuildDendrite() method.
* Compartments: This is a simplified version of the initial concept from the proposal. In that version compartments depended on a weighted distance. Tight clusters would tend to weigh the boundaries of compartments. This simpler version uses unweighted path lengths and a hard cut off max length. All connections from the soma are separate dendrites. The initial max length is 1/3 of the starting radius.
* Starting with POIS004_B used in the paper...but with all things turned on.
* From a 10,000 iteration run, the clustering looks much the same as the one from the paper. Clustering appears to be working. However, the strength changes are concerning. All synapses start at 100. The syns which are unique to the four patterns drop slightly at first and then rise slowly back to ~95 after 10,000 its. However, the shared synapses drop steadily. Finished under -1000. Furthermore, these syns are the initial connection points for the dendrite so input from the positive syns move through these strong negatives.
* However, the output neuron still spikes at 25-30 hz although the shared syn strengths drop negative. Need to double check, but I believe this is because some of the positive stren syns are not connected through the shared.
* Radius for the shared syns is shorter than the unique. Why is this? They all spike at the same rate. The only diff is that they appear twice as often. Is that enough? They should still coincide with a post-syn reponse with the same frequency.
* Perhaps results would be easier to interpret if I used a precise spike pattern.
* BREAKTHROUGH: I've discovered that certain pattern recognition abilities depend strongly on the magnitude of input strength to the input neurons. For a while I've had the input at 1000. Neurons spike strongly and regularly to even the sparsest spike. In the lines_vert experiment, this means that inputs distributed across compartments, in spite of the squashing, can muster strong input. I believe this is a form of saturation as compartmental transfer function output is near 1, no matter if there's one active synapse in a compartment or five. Reducing the input to 500 and then ultimately to 400 has increased accuracy dramatically. From ~52% to stretches of 100%.


**Aug 15,2021**
*Version 0.14 work*
Using all:
    0: 1.0
    1: 

**Aug 12,2021**
*Version 0.14 work*
* MNIST work. Exp: run of 20 iterations of 100 examples each. at 5hz not enough mask effect. Beef it up to 10 hz.

**Aug 10,2021**
*Version 0.14 work*
* Been lax in updating this. Working on the Beacon experiment. It is placing synapses in the right place. The key was to remove the mask.
* Ran 50, 100 ,200 and 500. The max normalized difference between the synaptic angular distance and the beacon distances decreases from 50 to 200. However, with 500, it goes up again. I'm not sure if this is an outlier.
* Overall, the results are somewhat interesting. The synapse locations are definitely a product of the signals. The syn locations do not exactly match the beacon locations because the signals received are not always what one would expect. For example, the algorithm repeatedly places b13 surrounded by a semicircle of b10,11 and 12. This is because 13 is active with 10 and 11. There are even instances where 10 and 13 are active but 12 is not. So there is a pull toward 10 which causes 12 and especially 13 to be close to 9. When according to the map, it shouldn't be. I suspect this would work to reproduce the layout of beacons with more examples. The dataset isn't superb since there are 4 or 5 times the number of examples containing signals from some beacons than others e.g. 2 v 13.
* I tried to limit the beacon entries to those that only had two signals as a way to refine the locations. I did not get a change to run enough examples. 50 iterations isn't enough since this reduces the number of examples per iteration.

**July 29, 2021**
*Version 0.13 work*
* Fixed a very big, stupid error in the code. This was the reason that the random version wasn't working. And I suspect it has caused the unreliability and looseness of clustering. In the Presyn Update, the direction of movement for the copied VecS wasn't using the heading of the copy to the dest VecS, but the heading from the original location to the dest VecS. This means, after the first movement, all other headings were off...perhaps by a lot. Not sure how I missed that one. I guess when coding I was considering only the i=0 case, when the copied VecS==original VecS.
* I have also discovered the cause of orbiting patterns. For example, in 002, if they share the first 10 inputs, the patterns will orbit. I'm not sure exactly why, but I'm guessing these shared inputs pull the patterns around. The shared move closer to one and then the other moves closer to the shared and then the mask pushes them apart. Something in this pull and push causes a rather perfect orbit. To understand this, I might have to track the centroid of each pattern and how it moves. I didn't expect to see orbits since the order that patterns are shown is random and synapse update is randomized. I would think that they'd wobble about.
* So, I've looked at the orbits in detail. It looks like the orbit is caused by the fact that the shared inputs are by chance closer to one of the patterns than the other. I may be able to minimize the orbit by lowering the mask strength. Hmmm.

**July 27, 2021**
*Version 0.13 work*
* Added the ability to randomize the update order or neurons within layers and synapses within neurons. This is accomplished by keeping a vec of indices within each layer and neuron. For the first version of this, the index vector is shuffled after each iteration.
* The first runs using randomization screwed things up. I've glanced back through the code and I suspect I missed something. So, for now, I've disabled it.

**July 22-23, 2021**
*Version 0.13 work*
* Started adding support the iBeacon experiment.

**July 20, 2021**
*Version 0.12 work*
* Working on the meta-pattern run: 004C. Depends on the input and the complexity. Haven't tried a lot of stuff yet. Concerned about the orbiting. I'm not sure why it happens. I have a few theories, but it's been there since the beginning. I would like see a stabilization of synpase locations, but they fall into these orbits. The relative positions within a pattern stabilize, but the dendrite building depends on the relationship of patterns themselves.
* To this end, I have experimented with no masks, but that doesn't help. I'm not considering that it might be the update order. It's invariant. I am now mixing the synapses associated with each pattern so that a single pattern won't update before all the rest. If this mitigates it, (or doesn't help at all), I'm going to try scrambling the synapse update order each iteration (run?).
* Changing the update order across patterns did the trick for 004C. This leads me to believe it should randomize update order for each iteration.


**July 16-17, 2021**
*Version 0.12 work*
* Ran a version of POIS004_B (C) which sends a 10hz input to off-synapses for each pattern. This did not impact the tendency of patterns to cluster. Pretty much identical outcome.

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