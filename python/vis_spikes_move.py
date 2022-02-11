import math

import matplotlib.pyplot as plt
import numpy as np

import output
import defs


# Need to load them both
NEURON_ID = 0
LAYER_ID = 1
SIDS = []



RIGHT=0
DOWN=1

LABELS = {
    RIGHT:'Right',
    DOWN:'Down'
}

outH = output.Output(defs.OUTPUT_PATH, lid=LAYER_ID, nid=RIGHT, sid=SIDS)
outV = output.Output(defs.OUTPUT_PATH, lid=LAYER_ID, nid=DOWN, sid=SIDS)

spikes_by_nid = {0:[], 1:[]}


spikes_corr = {RIGHT:[],DOWN:[]}
spikes_incorr = {RIGHT:[],DOWN:[]}


for name,neu in outH.neurons.items():
    if neu.neuron_id==RIGHT and neu.layer_id == LAYER_ID:
        for s in neu.spikes:
            spikes_by_nid[RIGHT].append(len(s))

for name,neu in outV.neurons.items():
    if neu.neuron_id==DOWN and neu.layer_id == LAYER_ID:
        for s in neu.spikes:
            spikes_by_nid[DOWN].append(len(s))



corr = {RIGHT:[],DOWN:[]}
corr_val = {RIGHT:[],DOWN:[]}


for i in range(len(outH.examples)):
    pattern_id = outH.examples[i].info

    if pattern_id == 'RIGHT':
        spikes_corr[RIGHT].append(spikes_by_nid[RIGHT][i])
        spikes_incorr[DOWN].append(spikes_by_nid[DOWN][i])

        # corr[RIGHT].append(int(spikes_by_nid[RIGHT][i] > spikes_by_nid[DOWN][i]))
             

    elif pattern_id=='DOWN':
        spikes_corr[DOWN].append(spikes_by_nid[DOWN][i])
        spikes_incorr[RIGHT].append(spikes_by_nid[RIGHT][i])

        # corr[DOWN].append(int(spikes_by_nid[DOWN][i] > spikes_by_nid[RIGHT][i]))



corrR_avg = sum(spikes_corr[RIGHT])/len(spikes_corr[RIGHT])
corrD_avg = sum(spikes_corr[DOWN])/len(spikes_corr[DOWN])
incorrR_avg = sum(spikes_incorr[RIGHT])/len(spikes_incorr[RIGHT])
incorrD_avg = sum(spikes_incorr[DOWN])/len(spikes_incorr[DOWN])

print(f"RIGHT {corrR_avg} {incorrR_avg}")
print(f"DOWN {corrD_avg} {incorrD_avg}")

# midR = (corrR_avg+incorrR_avg)/2.0
# midD = (corrD_avg+incorrD_avg)/2.0
# print(midR, midD)

corr_mag_R = []
corr_mag_D = []

for i in range(len(outH.examples)):
    pattern_id = outH.examples[i].info

    if pattern_id == 'RIGHT':

        y = spikes_by_nid[RIGHT][i] / incorrR_avg
        n = spikes_by_nid[DOWN][i] / incorrD_avg

        corr[RIGHT].append(int(y>n))

        corr_mag_R.append(y)
             

    elif pattern_id=='DOWN':

        n = spikes_by_nid[RIGHT][i] / incorrR_avg
        y = spikes_by_nid[DOWN][i] / incorrD_avg

        corr[DOWN].append(int(y > n))

        corr_mag_D.append(y)

fig = plt.figure()
ax = fig.add_subplot()
for k,v in spikes_by_nid.items():
    ax.plot(v,label=k)
plt.legend()
plt.show()

fig = plt.figure()
ax = fig.add_subplot()
ax.plot(corr_mag_R,label='R',color='red')
ax.plot(corr_mag_D,label='D',color='green')
plt.xlabel("Task")
plt.ylabel("Percent of mean incorrect spike rate")
plt.legend()
plt.show()


R_CORR = [0]*(max(spikes_corr[RIGHT])+1)
R_ICORR = [0]*(max(spikes_incorr[RIGHT])+1)
for x in spikes_corr[RIGHT]:
    R_CORR[x]+=1
for x in spikes_incorr[RIGHT]:
    R_ICORR[x]+=1

w = 0.5

fig = plt.figure()
ax = fig.add_subplot()
ax.plot(R_CORR,color='green')
ax.plot(R_ICORR,color='red')
plt.legend()
# ax.scatter(spikes_corr[RIGHT],spikes_incorr[DOWN],color='green')
# ax.scatter(spikes_incorr[RIGHT],spikes_corr[DOWN],color='red')
plt.show()






fig = plt.figure()
ax = fig.add_subplot()

for k,v in corr.items():
    acc = []
    total = 0
    correct = 0
    for d in v:
        total+=1
        correct+=d
        acc.append(correct/total)
        
    ax.plot(acc,label=LABELS[k])

plt.xlabel("Examples")
plt.ylabel("Accuracy")
plt.legend()
plt.show()