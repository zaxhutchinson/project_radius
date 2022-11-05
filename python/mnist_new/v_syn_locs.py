import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1 import make_axes_locatable
import numpy as np
import math
import output
import defs
import random

out = output.Output(defs.OUTPUT_PATH)
LID = 1
NID = 4

for i in range(784):
    out.LoadSynapse(LID,NID,i)



ids = []
lats = []
lons = []
rads = []
strs = []
comps = []
colors = []
comp_sizes=[]

for i in range(784):
    syn = out.synapses[f'{LID} {NID} {i}']

    lats.append(syn.lats[-1][-1])
    lons.append(syn.lons[-1][-1])
    rads.append(syn.rads[-1][-1])
    strs.append(syn.strengths[-1][-1])
    comps.append(syn.compartments[-1][-1])


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
    colors.append(comp_colors[x])
    comp_sizes[x]+=1

max_comp_size = max(comp_sizes)
comp_alphas = []
for s in comp_sizes:
    comp_alphas.append(s/max_comp_size)
    
plt.figure()
sc = plt.scatter(lons,lats,c=strs,cmap="turbo")
plt.colorbar(sc)
plt.xlabel("LONS")
plt.ylabel("LATS")
plt.title("STRENGTHS")
plt.show()

plt.figure()
sc = plt.scatter(lons,lats,c=rads,cmap="turbo")
plt.colorbar(sc)
plt.xlabel("LONS")
plt.ylabel("LATS")
plt.title("RADII")
plt.show()

plt.figure()
sc = plt.scatter(lons,lats,c=colors)
plt.colorbar(sc)
plt.xlabel("LONS")
plt.ylabel("LATS")
plt.title("RADII")
plt.show()

plt.figure()
plt.gca().invert_yaxis()
for c in range(784):
    plt.plot(c%28,c//28,color=comp_colors[comps[c]],marker='s',markersize=10,alpha=comp_alphas[comps[c]])    
plt.show()

x = []
y = []
for i in range(784):
    x.append(i%28)
    y.append(i//28)


plt.figure()
plt.gca().invert_yaxis()

plt.scatter(x,y,c=np.array(strs),cmap='bwr',marker='s',linewidths=10)    
plt.show()