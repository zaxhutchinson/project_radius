import math

import matplotlib.pyplot as plt
import numpy as np

import output
import defs


# Need to load them both
NEURON_ID = 0
LAYER_ID = 1
SIDS = []


A=0
C=1

outA = output.Output(defs.OUTPUT_PATH, lid=LAYER_ID, nid=A, sid=SIDS)
outC = output.Output(defs.OUTPUT_PATH, lid=LAYER_ID, nid=C, sid=SIDS)

spikes_by_nid = {0:[], 1:[]}


spikes_corr = {A:[],C:[]}
spikes_incorr = {A:[],C:[]}


for name,neu in outA.neurons.items():
    if neu.neuron_id==A and neu.layer_id == LAYER_ID:
        for s in neu.spikes:
            spikes_by_nid[A].append(len(s))

for name,neu in outC.neurons.items():
    if neu.neuron_id==C and neu.layer_id == LAYER_ID:
        for s in neu.spikes:
            spikes_by_nid[C].append(len(s))






for i in range(len(outA.examples)):
    pattern_id = outA.examples[i].info

    if pattern_id == 'a':
        spikes_corr[A].append(spikes_by_nid[A][i])
        spikes_incorr[C].append(spikes_by_nid[C][i])

    elif pattern_id=='c':
        spikes_corr[C].append(spikes_by_nid[C][i])
        spikes_incorr[A].append(spikes_by_nid[A][i])


corrA_avg = sum(spikes_corr[A])/len(spikes_corr[A])
corrC_avg = sum(spikes_corr[C])/len(spikes_corr[C])
incorrA_avg = sum(spikes_incorr[A])/len(spikes_incorr[A])
incorrC_avg = sum(spikes_incorr[C])/len(spikes_incorr[C])

print(f"A {corrA_avg} {incorrA_avg}")
print(f"C {corrC_avg} {incorrC_avg}")

# fig = plt.figure()
# ax = fig.add_subplot()
# for k,v in spikes_by_nid.items():
#     ax.plot(v,label=k)
# plt.legend()
# plt.show()

# fig = plt.figure()
# ax = fig.add_subplot()
# for k,v in acc_by_pat.items():
#     ax.plot(v,label=k)
# plt.legend()
# plt.show()

# # print(acc_by_pat)

# fig = plt.figure()
# ax = fig.add_subplot()
# ax.plot(acc)
# # plt.legend()
# plt.show()