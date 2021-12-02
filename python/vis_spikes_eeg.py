import math

import matplotlib.pyplot as plt
import numpy as np

import output
import defs


# Need to load them both
NEURON_ID = 0
LAYER_ID = 1
SIDS = []


CORRECT_ID = 'c'
if NEURON_ID == 0:
    CORRECT_ID = 'a'

out0 = output.Output(defs.OUTPUT_PATH, lid=LAYER_ID, nid=0, sid=SIDS)
out1 = output.Output(defs.OUTPUT_PATH, lid=LAYER_ID, nid=1, sid=SIDS)

spikes_by_nid = {0:[], 1:[]}
acc = []
corr = 0
total = 0
acc_by_pat = {'a':[],'c':[]}
corr_by_pat = {'a':0, 'c':0}
total_by_pat = {'a':0,'c':0}
spikes_by_pat = {'a':[],'c':[]}

for name,neu in out0.neurons.items():
    if neu.neuron_id==0 and neu.layer_id == LAYER_ID:
        for s in neu.spikes:
            spikes_by_nid[0].append(len(s))

for name,neu in out1.neurons.items():
    if neu.neuron_id==1 and neu.layer_id == LAYER_ID:
        for s in neu.spikes:
            spikes_by_nid[1].append(len(s))



avg0 = sum(spikes_by_nid[0])/len(spikes_by_nid[0])
avg1 = sum(spikes_by_nid[1])/len(spikes_by_nid[1])


for i in range(len(out1.examples)):
    pattern_id = out1.examples[i].info
    # num_spikes = spikes[i]
    total += 1
    total_by_pat[pattern_id] += 1
    # spikes_by_pat[pattern_id].append(num_spikes)
    if pattern_id == 'a':
        
        # if spikes_by_nid[0][i] > avg0 and spikes_by_nid[1][i] < avg1:#spikes_by_nid[1][i]:
        # if spikes_by_nid[0][i] > avg0:
        if spikes_by_nid[0][i] > spikes_by_nid[1][i]:
            corr += 1
            
            corr_by_pat[pattern_id]+=1
    else:
        #if spikes_by_nid[1][i] > avg1 and spikes_by_nid[0][i] < avg0:#spikes_by_nid[0][i]:
        #if spikes_by_nid[1][i] > avg1:
        if spikes_by_nid[1][i] > spikes_by_nid[0][i]:
            corr += 1
            corr_by_pat[pattern_id]+=1


    acc.append(corr/total)
    acc_by_pat[pattern_id].append(corr_by_pat[pattern_id]/total_by_pat[pattern_id])

# for k,v in spikes_by_nid.items():
#     print(k, sum(v)/total)


fig = plt.figure()
ax = fig.add_subplot()
for k,v in spikes_by_nid.items():
    ax.plot(v,label=k)
plt.legend()
plt.show()

fig = plt.figure()
ax = fig.add_subplot()
for k,v in acc_by_pat.items():
    ax.plot(v,label=k)
plt.legend()
plt.show()

# print(acc_by_pat)

fig = plt.figure()
ax = fig.add_subplot()
ax.plot(acc)
# plt.legend()
plt.show()