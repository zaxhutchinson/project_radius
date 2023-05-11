import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1 import make_axes_locatable
import numpy as np
import math
import output
import defs
import random
import mycolors

out = output.Output(defs.OUTPUT_PATH)
LID = 1
NID = [0,1]

WEIGHT = 500

for i in range(784):
    out.LoadSynapse(LID,NID,i)

out.LoadNeuron(LID,NID)

ids = []
lats = []
lons = []
rads = []
strs = []
actual_strs = []
comps = []
colors = mycolors.GetColors(784)
comp_sizes=[]
comp_strs = []

neu0 = out.neurons[f'{LID} {NID}']
nlat = neu.lats[-1][-1]
nlon = neu.lons[-1][-1]

for i in range(784):
    syn = out.synapses[f'{LID} {NID} {i}']

    lats.append(syn.lats[-1][-1])
    lons.append(syn.lons[-1][-1])
    rads.append(syn.rads[-1][-1])
    strs.append(syn.strengths[-1][-1])
    comps.append(syn.compartments[-1][-1])
    actual_strs.append(strs[-1]*WEIGHT/(abs(strs[-1])+WEIGHT))
    comp_strs.append(syn.comp_strs[-1][-1])

num_comps = max(comps)
comp_colors = []
for i in range(num_comps+1):
    comp_colors.append((
        random.uniform(0.2,0.8),
        random.uniform(0.2,0.8),
        random.uniform(0.2,0.8)
    ))
    comp_sizes.append(0)

for x in comps:
    # colors.append(comp_colors[x])
    comp_sizes[x]+=1

max_comp_size = max(comp_sizes)
comp_alphas = []
for s in comp_sizes:
    comp_alphas.append(s/max_comp_size)
    
plt.figure()
sc = plt.scatter(lons,lats,c=strs,cmap="turbo")
plt.colorbar(sc)
plt.xlabel("Longitude")
plt.ylabel("Latitude")
# plt.title("STRENGTHS")
plt.show()

plt.figure()
sc = plt.scatter(lons,lats,c=rads,cmap="turbo")
plt.colorbar(sc)
plt.xlabel("Longitude")
plt.ylabel("Latitude")
# plt.title("RADII")
plt.show()



plt.figure()
plt.gca().invert_yaxis()
for c in range(784):
    plt.plot(c%28,c//28,color=colors[comps[c]],marker='s',markersize=10)#,alpha=comp_alphas[comps[c]])    
plt.show()

plt.figure()
plt.scatter(nlon,nlat,c='black')
sc = plt.scatter(lons,lats,c=colors)
plt.colorbar(sc)
plt.xlabel("Longitude")
plt.ylabel("Latitude")
# plt.title("RADII")
plt.show()

x = []
y = []
for i in range(784):
    x.append(i%28)
    y.append(i//28)


plt.figure()
plt.gca().invert_yaxis()
sc = plt.scatter(x,y,c=np.array(strs),cmap='bwr',marker='s',linewidths=4)  
plt.colorbar(sc)  
plt.show()


plt.figure()
plt.gca().invert_yaxis()
sc = plt.scatter(x,y,c=np.array(actual_strs),cmap='bwr',marker='s',linewidths=4)  
plt.colorbar(sc)  
plt.show()

plt.figure()
plt.gca().invert_yaxis()
sc = plt.scatter(x,y,c=np.array(comp_strs),cmap='bwr',marker='s',linewidths=4)  
plt.colorbar(sc)
plt.title("Compartment Strengths by Synapse")
plt.show()





dist_matrix = np.zeros([28,28])
avg_dists = np.zeros([784])


for i in range(784):

    dist = 0.0

    for k in range(784):

        alon = lons[k] - lons[i]
        alat = lats[k] - lats[i]
        a = math.pow( math.sin(alat/2.0), 2.0 ) + \
            math.cos(lats[i]) * math.cos(lats[k]) * \
            math.pow( math.sin(alon/2.0), 2.0 )
        if a > 1:
            a = 1.0
        if a < 0:
            a = 0.0
        
        avg_dists[i] += 2.0 * math.atan2(math.sqrt(a), math.sqrt(1.0-a))

    avg_dists[i] /= 784
        # dist_matrix[k1][k2] += dist
    

for i in range(len(avg_dists)):
    dist_matrix[i//28][i%28] = avg_dists[i]

ax = plt.subplot()
plt.title("Avg angular distance")
im = ax.imshow(dist_matrix, cmap='jet', aspect='auto', interpolation='nearest')
divider = make_axes_locatable(ax)
cax = divider.append_axes("right", size="5%", pad=0.05)
plt.colorbar(im, cax=cax)
plt.show()





comp_heat_map = np.zeros([28,28])
for k in range(784):
    c = comps[k]
    comp_heat_map[k//28][k%28] = comp_sizes[c]

ax = plt.subplot()
plt.title("Compartment size by neural input")
im = ax.imshow(comp_heat_map, cmap='coolwarm', aspect='auto', interpolation='nearest')
divider = make_axes_locatable(ax)
cax = divider.append_axes("right", size="5%", pad=0.05)
plt.colorbar(im, cax=cax)
plt.show()