import math

import matplotlib.pyplot as plt
import numpy as np

import output
import defs


# Need to load them both
NEURON_ID = [0,1]
LAYER_ID = 1
SIDS = []


CORRECT_ID = 'c'
if NEURON_ID == 0:
    CORRECT_ID = 'a'

out = output.Output(defs.OUTPUT_PATH, lid=LAYER_ID, nid=NEURON_ID, sid=SIDS)

spikes_by_nid = {0:[], 1:[]}
acc = []
corr = 0
total = 0
acc_by_pat = {'a':0,'c':0}
spikes_by_pat = {'a':[],'c':[]}

for name,neu in out.neurons.items():
    if NEURON_ID and neu.neuron_id in NEURON_ID and neu.layer_id == LAYER_ID:
        print(name, len(neu.spikes))
        for s in neu.spikes:
            spikes_by_nid[neu.neuron_id].append(len(s))

print(max(spikes_by_nid[0]), max(spikes_by_nid[1]))

# avg_spikes = np.mean(spikes)
# print("AVG SPIKES: ", avg_spikes)

for i in range(0,len(out.examples)):
    pattern_id = out.examples[i].info
    # num_spikes = spikes[i]
    total += 1
    # spikes_by_pat[pattern_id].append(num_spikes)
    if pattern_id == 'a':
        if spikes_by_nid[0][i] > spikes_by_nid[1][i]:
            corr += 1
            acc_by_pat[pattern_id] += 1
    else:
        if spikes_by_nid[1][i] > spikes_by_nid[0][i]:
            corr += 1
            acc_by_pat[pattern_id] += 1


    acc.append(corr/total)

# for k,v in spikes_by_nid.items():
#     print(k, v/(total))


fig = plt.figure()
ax = fig.add_subplot()
for k,v in spikes_by_nid.items():
    ax.plot(v,label=k)
plt.legend()
plt.show()

fig = plt.figure()
ax = fig.add_subplot()
ax.plot(acc)
# plt.legend()
plt.show()