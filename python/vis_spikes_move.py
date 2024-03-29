import math

import matplotlib.pyplot as plt
import numpy as np
from scipy.interpolate import interp1d
from scipy.signal import savgol_filter

import output
import defs


# Need to load them both
NEURON_ID = 0
LAYER_ID = 1
SIDS = []



RIGHT=0
DOWN=1

LABELS = {
    RIGHT:'Horizontal',
    DOWN:'Vertical'
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

    if pattern_id == 'H':
        spikes_corr[RIGHT].append(spikes_by_nid[RIGHT][i])
        spikes_incorr[DOWN].append(spikes_by_nid[DOWN][i])

        # corr[RIGHT].append(int(spikes_by_nid[RIGHT][i] > spikes_by_nid[DOWN][i]))
             

    elif pattern_id=='V':
        spikes_corr[DOWN].append(spikes_by_nid[DOWN][i])
        spikes_incorr[RIGHT].append(spikes_by_nid[RIGHT][i])

        # corr[DOWN].append(int(spikes_by_nid[DOWN][i] > spikes_by_nid[RIGHT][i]))


if len(spikes_corr[RIGHT])==0: corrR_avg = 0.0
else: corrR_avg = sum(spikes_corr[RIGHT])/len(spikes_corr[RIGHT])
if len(spikes_corr[DOWN])==0: corrD_avg = 0.0
else: corrD_avg = sum(spikes_corr[DOWN])/len(spikes_corr[DOWN])
incorrR_avg = sum(spikes_incorr[RIGHT])/len(spikes_incorr[RIGHT])
incorrD_avg = sum(spikes_incorr[DOWN])/len(spikes_incorr[DOWN])

print(f"RIGHT {corrR_avg} {incorrR_avg}")
print(f"DOWN {corrD_avg} {incorrD_avg}")

# midR = (corrR_avg+incorrR_avg)/2.0
# midD = (corrD_avg+incorrD_avg)/2.0
# print(midR, midD)

corr_ratio_R = []
corr_ratio_D = []
corr_mag_R = []
corr_mag_D = []
yR = []
nR = []
yD = []
nD = []


for i in range(len(outH.examples)):
    pattern_id = outH.examples[i].info

    if pattern_id == 'H':
        if incorrR_avg:
            y = spikes_by_nid[RIGHT][i] / incorrR_avg
        else: 
            y = spikes_by_nid[RIGHT][i]

        if incorrD_avg:
            n = spikes_by_nid[DOWN][i] / incorrD_avg
        else:
            n = spikes_by_nid[DOWN][i]

        yR.append(y)
        nD.append(n)

        corr[RIGHT].append(int(y>n))

        if n>=y:
            print("H",outH.examples[i].example, spikes_by_nid[RIGHT][i], spikes_by_nid[DOWN][i])

        corr_mag_R.append(spikes_by_nid[RIGHT][i] - incorrR_avg)
        corr_ratio_R.append(y)
             

    elif pattern_id=='V':

        if incorrR_avg:
            n = spikes_by_nid[RIGHT][i] / incorrR_avg
        else:
            n = spikes_by_nid[RIGHT][i]
        
        if incorrD_avg:
            y = spikes_by_nid[DOWN][i] / incorrD_avg
        else:
            y = spikes_by_nid[DOWN][i]

        yD.append(y)
        nR.append(n)

        corr[DOWN].append(int(y > n))

        if n>=y:
            print("V",outH.examples[i].example, spikes_by_nid[DOWN][i], spikes_by_nid[RIGHT][i])

        corr_mag_D.append(spikes_by_nid[DOWN][i] - incorrD_avg)
        corr_ratio_D.append(y)


# xR = np.linspace(0,len(corr_mag_R), num=len(corr_mag_R),endpoint='True')
# xD = np.linspace(0,len(corr_mag_D), num=len(corr_mag_D),endpoint='True')
# fR = interp1d(xR,corr_mag_R,kind='cubic')
# fD = interp1d(xD,corr_mag_D,kind='cubic')
# xnewR = np.linspace(0, len(corr_mag_R), num=100,endpoint='True')
# xnewD = np.linspace(0, len(corr_mag_D), num=100,endpoint='True')

fig = plt.figure()
ax = fig.add_subplot()
ax.plot(savgol_filter(spikes_corr[RIGHT],501,3),color='red',label='Horizontal')
ax.plot(savgol_filter(spikes_corr[DOWN],501,3),color='blue',label='Vertical')
ax.plot(savgol_filter(spikes_incorr[RIGHT],501,3),color='red',label='Horizontal',linestyle='--')
ax.plot(savgol_filter(spikes_incorr[DOWN],501,3),color='blue',label='Vertical',linestyle='--')
plt.xlabel("Examples")
plt.ylabel("Spike Rate - Correct")
plt.legend()
plt.show()


fig = plt.figure()
ax = fig.add_subplot()
for k,v in spikes_by_nid.items():
    ax.plot(v,label=k)
plt.legend()
plt.show()

fig = plt.figure()
ax = fig.add_subplot()
# ax.plot(corr_mag_R,label='R',color='red')
# ax.plot(corr_mag_D,label='D',color='blue')
ax.plot(savgol_filter(corr_ratio_R,501,3),color='red',label='Horizontal')
ax.plot(savgol_filter(corr_ratio_D,501,3),color='blue',label='Vertical')
plt.xlabel("Task")
plt.ylabel("Correct/AvgIncorrect")
plt.legend()
plt.show()

fig = plt.figure()
ax = fig.add_subplot()
# ax.plot(corr_mag_R,label='R',color='red')
# ax.plot(corr_mag_D,label='D',color='blue')
ax.plot(savgol_filter(corr_mag_R,501,3),color='red',label='Horizontal')
ax.plot(savgol_filter(corr_mag_D,501,3),color='blue',label='Vertical')
plt.xlabel("Task")
plt.ylabel("Correct - AvgIncorrect")
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
    
    if k==RIGHT:
        color='red'
    elif k==DOWN:
        color='blue'
    ax.plot(acc,label=LABELS[k],color=color)

plt.xlabel("Examples")
plt.ylabel("Accuracy")
plt.legend()
plt.show()



fig = plt.figure()
ax = fig.add_subplot()
ax.plot(savgol_filter(yR,501,3),color='red',label='Correct: Horizontal')
ax.plot(savgol_filter(nD,501,3),color='blue',label='Incorrect: Vertical')
plt.xlabel("Examples")
plt.ylabel("Sp")
plt.legend()
plt.show()


fig = plt.figure()
ax = fig.add_subplot()
ax.plot(savgol_filter(nR,501,3),color='red',label='Incorrect: Horizontal')
ax.plot(savgol_filter(yD,501,3),color='blue',label='Correct: Vertical')
plt.xlabel("Examples")
plt.ylabel("Correct/AvgIncorrect")
plt.legend()
plt.show()