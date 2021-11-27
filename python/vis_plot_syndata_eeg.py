import math

import matplotlib.pyplot as plt
import numpy as np

import output
import defs

NEURON_ID = 0
LAYER_ID = 1
SIDS = list(range(64))
# SIDS = [0,1,2,3,4,5,6,7,8,9]
out = output.Output(defs.OUTPUT_PATH, lid=LAYER_ID, nid=NEURON_ID)

strs = {}
radius = {}

for name,syn in out.synapses.items():
    if syn.neuron_id == NEURON_ID and syn.layer_id == LAYER_ID and syn.synapse_id in SIDS:
        ID = int(name.split()[-1])

        str_list = []
        for s in syn.strengths:
            str_list += s

        radius_list = []
        for r in syn.rads:
            radius_list += r

        strs[ID] = str_list
        radius[ID] = radius_list


# STRENGTH
fig = plt.figure()
ax = fig.add_subplot()
for k,v in strs.items():
    ax.plot(v,label=str(k))
ax.legend()
plt.show()

# SIDS = [
#     [0,1,2,3,4],
#     [5,6,7,8,9],
#     [10,11,12,13,14],
#     [15,16,17,18,19],
#     [20,21,22,23,24],
# ]
# for s in SIDS:
#     fig = plt.figure()
#     ax = fig.add_subplot()
    
#     for k,v in strs.items():
#         if k in s:
#             ax.plot(v,label=str(k))
#     ax.legend()
#     plt.title("STRENGTH: "+str(s))
#     plt.show()


fig = plt.figure()
ax = fig.add_subplot()
for k,v in radius.items():
    ax.plot(v,label=str(k))
ax.legend()
plt.show()

# for s in SIDS:
#     fig = plt.figure()
#     ax = fig.add_subplot()
#     for k,v in radius.items():
#         if k in s:
#             ax.plot(v,label=str(k))
#     ax.legend()
#     plt.title("RADIUS: "+str(s))
#     plt.show()