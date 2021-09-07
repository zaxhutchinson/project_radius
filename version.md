**Version 0.14**
- Paper version
- Hard cut here before further development

**Version 0.13**
- Fixes a bug in the PreSynaptic Update. (See notes for July 29 2021).
- Adds the Beacon experiment

**Version 0.12**
- No significant core changes. Work on 0.12 focused on longer runs of more complex patterns.

**Version 0.11**
- Changes Radial movement algorithm to use time-cur_spike as the basis for synaptic activity rather than cur_spike-pre_spike.
- Fixed a bug in the random distribution of synpases over a sphere.
- Moved all simulation run functions into run.hpp/cpp.
- Fixed a bug in the InputGenerator. Signal values were not being reset from previous runs.

**Version 0.10**
- Adds an InputGenerator that can produce spike trains using a Poisson distribution.

**Version 0.09**
- Parallelized (OpenMP) some of the more labor intensive updates. Created a parallel version of the BuildDendrites function.
- Changed the Python visualization so the Output class will only load syns and neurons that are asked for.
- Adds an InputGenerator class which can be used to generate input to a layer at each time step. Rather than the PatternMaker or MNIST which present only one signal for an entire example.

**Version 0.08**
- Lots of changes. Synaptic clustering using spherical geo/trig is working.

**Version 0.07**
- Visualization and debugging.

**Version 0.06**
- Worked out many of the issues. Committing for posterity.

**Version 0.05**
- Begin work on the JSON network builder.
- Network builder, v1, complete.
- Adds logging capabilities from another project. Reworked how the logger functions.
- First working version. There are issues, but it works.

**Version 0.04**
- Neuron input via dendrite and synapses. Iterative solution - untested.
- Higher-level arch: Network -> Layer -> Neuron -> Synapse.
- Axon struct added.

**Version 0.03**
- Function name changes.
- Starting put it all together.
- MNIST reader copied over.
- Network struct.
- Network building based on Cuntz2007 - untested.

**Version 0.02**
- Redesign of neuron and synapse around a more data-centered focus rather than an OOP design. Shifting away from pointers.

**Version 0.01**
- First version of the VecS class which uses spherical geometry to track and move synapses.
- Might also be the first version to use the Tester.hpp file. My own brand of janky unit testing.

**Version 0.00**
- Began adaption from ver3 from the Artificial Dendrites paper and the start of the next step forward.
- "Everything should be easier than it is." Anna Turing.