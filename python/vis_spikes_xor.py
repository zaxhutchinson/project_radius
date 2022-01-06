import math

import matplotlib.pyplot as plt
import numpy as np

import output
import defs

NEURON_ID = 0
LAYER_ID = 1
SIDS = []

out = output.Output(defs.OUTPUT_PATH, lid=LAYER_ID, nid=NEURON_ID, sid=SIDS)

spikes = []
acc = []
corr = 0
total = 0
acc_by_pat = {0:0,1:0,2:0}
spikes_by_pat = {0:[],1:[],2:[]}

CORRECT_PATTERNS = [0,1]
INCORRECT_PATTERNS = [2]

for name,neu in out.neurons.items():
    if neu.neuron_id == NEURON_ID and neu.layer_id == LAYER_ID:
        for s in neu.spikes:
            spikes.append(len(s))

avg_spikes = np.mean(spikes[len(spikes)//2:])
print("AVG SPIKES: ", avg_spikes)

for i in range(0,len(out.examples)):
    pattern_id = int(out.examples[i].info)
    num_spikes = spikes[i]
    total += 1
    spikes_by_pat[pattern_id].append(num_spikes)
    if pattern_id in CORRECT_PATTERNS:
        if num_spikes > avg_spikes:
            corr += 1
            acc_by_pat[pattern_id] += 1
    elif pattern_id in INCORRECT_PATTERNS:
        if num_spikes < avg_spikes:
            corr += 1
            acc_by_pat[pattern_id] += 1
    else:
        print("ERROR")

    acc.append(corr/total)

for k,v in spikes_by_pat.items():

    print(k, sum(v[:2000])/len(v[:2000]), sum(v[5000:])/len(v[5000:]))

fig = plt.figure()
ax = fig.add_subplot()
ax.plot(spikes)
plt.show()

fig = plt.figure()
ax = fig.add_subplot()
ax.plot(acc)
plt.show()

labels = ['A\u0305B','AB\u0305','AB']

fig = plt.figure()
ax = fig.add_subplot()
plt.xlabel("Iteration")
plt.ylabel("Num Spikes")
for k,v in spikes_by_pat.items():
    ax.plot(v, label=labels[k])
plt.legend()
plt.show()