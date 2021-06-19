import os

import location

class Example:
    def __init__(
        self,
        iteration,
        example,
        info
    ):
        self.iteration = iteration
        self.example = example
        self.info = info

class Synapse:
    def __init__(
        self, 
        synapse_id,
        neuron_id,
        layer_id
    ):
        self.synapse_id = synapse_id
        self.neuron_id = neuron_id
        self.layer_id = layer_id
        self.times = []
        self.lats = []
        self.lons = []
        self.rads = []
        self.strengths = []
        self.spikes = []
        self.inputs = []
        self.errors = []
        self.parents = []
        self.children = []

    def AddTimes(self, times):
        self.times.append(times)
    def AddLats(self, lats):
        self.lats.append(lats)
    def AddLons(self, lons):
        self.lons.append(lons)
    def AddRads(self, rads):
        self.rads.append(rads)
    def AddStrengths(self, strs):
        self.strengths.append(strs)
    def AddSpikes(self, spks):
        self.spikes.append(spks)
    def AddInputs(self, inputs):
        self.inputs.append(inputs)
    def AddErrors(self, errors):
        self.errors.append(errors)
    def AddParents(self, pids):
        self.parents.append(pids)
    def AddChildren(self, cids):
        self.children.append(cids)

class Neuron:
    def __init__(
        self,
        neuron_id,
        layer_id
    ):
        self.neuron_id = neuron_id
        self.layer_id = layer_id
        self.times = []
        self.lats = []
        self.lons = []
        self.rads = []
        self.vs = []
        self.us = []
        self.outputs = []
        self.inputs = []
        self.spikes = []
        self.errors = []

    def AddTimes(self, times):
        self.times.append(times)
    def AddLats(self, lats):
        self.lats.append(lats)
    def AddLons(self, lons):
        self.lons.append(lons)
    def AddRads(self, rads):
        self.rads.append(rads)
    def AddVs(self, vs):
        self.vs.append(vs)
    def AddUs(self, us):
        self.us.append(us)
    def AddOutputs(self, outputs):
        self.outputs.append(outputs)
    def AddInputs(self, inputs):
        self.inputs.append(inputs)
    def AddSpikes(self, spikes):
        self.spikes.append(spikes)
    def AddErrors(self, errors):
        self.errors.append(errors)

class Output:
    def __init__(self, path):
        self.path = path
        self.example_filenames = []
        self.neuron_filenames = []
        self.synapse_filenames = []
        self.examples = []
        self.neurons = {}
        self.synapses = {}

        # Load all the filenames in the output directory
        for filename in os.scandir(path):
            if filename.is_file():
                fn = filename.name
                if fn[0]=='S':
                    self.synapse_filenames.append(fn)
                elif fn[0]=='N':
                    self.neuron_filenames.append(fn)
                elif fn[0]=='E':
                    self.example_filenames.append(fn)

        # Load all the example data
        for efn in self.example_filenames:
            with open(self.path+efn, 'r') as f:
                line = f.readline().strip().split()
                iteration = int(line[0])
                example = int(line[1])
                info = line[2]
                self.examples.append(Example(iteration, example, info))

        # Load all neuron data
        for nfn in self.neuron_filenames:

            with open(self.path+nfn, 'r') as f:

                ids = f.readline()

                id_data = ids.strip().split()
                layer_id = int(id_data[0])
                neuron_id = int(id_data[1])
                
                neuron = Neuron(neuron_id, layer_id)

                while ids:
                    
                    time_data = f.readline().strip().split()[1:]
                    lat_data = f.readline().strip().split()[1:]
                    lon_data = f.readline().strip().split()[1:]
                    rad_data = f.readline().strip().split()[1:]
                    v_data = f.readline().strip().split()[1:]
                    u_data = f.readline().strip().split()[1:]
                    out_data = f.readline().strip().split()[1:]
                    in_data = f.readline().strip().split()[1:]
                    err_data = f.readline().strip().split()[1:]
                    spks_data = f.readline().strip().split()[1:]

                    for i in range(len(time_data)):
                        time_data[i] = int(time_data[i])
                        lat_data[i] = float(lat_data[i])
                        lon_data[i] = float(lon_data[i])
                        rad_data[i] = float(rad_data[i])
                        v_data[i] = float(v_data[i])
                        u_data[i] = float(u_data[i])
                        out_data[i] = float(out_data[i])
                        in_data[i] = float(in_data[i])
                        err_data[i] = float(err_data[i])

                    # Must be done separate because it varies
                    for i in range(len(spks_data)):
                        spks_data[i] = int(spks_data[i])
                        
                    neuron.AddTimes(time_data)
                    neuron.AddLats(lat_data)
                    neuron.AddLons(lon_data)
                    neuron.AddRads(rad_data)
                    neuron.AddVs(v_data)
                    neuron.AddUs(u_data)
                    neuron.AddOutputs(out_data)
                    neuron.AddInputs(in_data)
                    neuron.AddErrors(err_data)
                    neuron.AddSpikes(spks_data)

                    ids = f.readline()

            name = str(neuron.layer_id)+' '+str(neuron.neuron_id)
            self.neurons[name] = neuron

        for sfn in self.synapse_filenames:

            with open(self.path+sfn, 'r') as f:

                ids = f.readline()

                id_data = ids.strip().split()
                layer_id = int(id_data[0])
                neuron_id = int(id_data[1])
                synapse_id = int(id_data[2])
                
                synapse = Synapse(synapse_id, neuron_id, layer_id)

                while ids:

                    time_data = f.readline().strip().split()[1:]
                    lat_data = f.readline().strip().split()[1:]
                    lon_data = f.readline().strip().split()[1:]
                    rad_data = f.readline().strip().split()[1:]
                    str_data = f.readline().strip().split()[1:]
                    spk_data = f.readline().strip().split()[1:]
                    in_data = f.readline().strip().split()[1:]
                    err_data = f.readline().strip().split()[1:]
                    pid_data = f.readline().strip().split()[1:]
                    cid_data_pre = f.readline().strip().split()[1:]
                    

                    for i in range(len(time_data)):
                        time_data[i] = int(time_data[i])
                        lat_data[i] = float(lat_data[i])
                        lon_data[i] = float(lon_data[i])
                        rad_data[i] = float(rad_data[i])
                        str_data[i] = float(str_data[i])
                        spk_data[i] = int(spk_data[i])
                        in_data[i] = float(in_data[i])
                        err_data[i] = float(err_data[i])
                        pid_data[i] = int(pid_data[i])

                    cid_data = []
                    for cids in cid_data_pre:
                        cids = cids.strip('[]').split()
                        cid_list = []
                        for cid in cids:
                            cid_list.append(int(cid))
                        cid_data.append(cid_list)


                    synapse.AddTimes(time_data)
                    synapse.AddLats(lat_data)
                    synapse.AddLons(lon_data)
                    synapse.AddRads(rad_data)
                    synapse.AddStrengths(str_data)
                    synapse.AddSpikes(spk_data)
                    synapse.AddInputs(in_data)
                    synapse.AddErrors(err_data)
                    synapse.AddParents(pid_data)
                    synapse.AddChildren(cid_data)

                    name = str(synapse.layer_id)+" "+str(synapse.neuron_id)+" "+str(synapse.synapse_id)

                    self.synapses[name] = synapse

                    ids = f.readline()


