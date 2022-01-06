import math

import matplotlib.pyplot as plt
import numpy as np

import output
import defs


# Need to load them both
NEURON_ID = 0
LAYER_ID = 1
SIDS = []


H=0
V=1

outH = output.Output(defs.OUTPUT_PATH, lid=LAYER_ID, nid=H, sid=SIDS)
outV = output.Output(defs.OUTPUT_PATH, lid=LAYER_ID, nid=V, sid=SIDS)

spikes_by_nid = {0:[], 1:[]}


spikes_corr = {H:[],V:[]}
spikes_incorr = {H:[],V:[]}


for name,neu in outH.neurons.items():
    if neu.neuron_id==H and neu.layer_id == LAYER_ID:
        for s in neu.spikes:
            spikes_by_nid[H].append(len(s))

for name,neu in outV.neurons.items():
    if neu.neuron_id==V and neu.layer_id == LAYER_ID:
        for s in neu.spikes:
            spikes_by_nid[V].append(len(s))






for i in range(len(outH.examples)):
    pattern_id = outH.examples[i].info

    if pattern_id == 'H':
        spikes_corr[H].append(spikes_by_nid[H][i])
        spikes_incorr[V].append(spikes_by_nid[V][i])

    elif pattern_id=='V':
        spikes_corr[V].append(spikes_by_nid[V][i])
        spikes_incorr[H].append(spikes_by_nid[H][i])


corrA_avg = sum(spikes_corr[H])/len(spikes_corr[H])
corrC_avg = sum(spikes_corr[V])/len(spikes_corr[V])
incorrA_avg = sum(spikes_incorr[H])/len(spikes_incorr[H])
incorrC_avg = sum(spikes_incorr[V])/len(spikes_incorr[V])

print(f"H {corrA_avg} {incorrA_avg}")
print(f"V {corrC_avg} {incorrC_avg}")

fig = plt.figure()
ax = fig.add_subplot()
for k,v in spikes_by_nid.items():
    ax.plot(v,label=k)
plt.legend()
plt.show()