import math
import sys
sys.path.append('..')
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1 import make_axes_locatable

import defs
import output

out = output.Output(defs.OUTPUT_PATH)
NEURON_ID = 0
LAYER_ID = 1
MNIST_DIGIT = 8
CUT_OFF_DIST = 0.5
NUM_FINAL_PIXELS = 100

# 3
# LON = -1.5
# LAT = -0.0

# 8
LON = -1.75
LAT = -0.4

def minmaxNorm(dic, mn=0.0, mx=1.0):
    # arr = list(dic.values())
    # mx = 0.0
    # mn = np.min(arr)
    mxmn = mx-mn
    for k,v in dic.items():
        dic[k] = (dic[k]-mn) / mxmn




# Pixel ID: (lat, lon)
model = {}

# Load data
for name,syn in out.synapses.items():
    if syn.neuron_id == NEURON_ID and syn.layer_id == LAYER_ID:
        sid = syn.synapse_id
        lat = syn.lats[-1][-1]
        lon = syn.lons[-1][-1]
        while lon > math.pi:
            lon -= math.pi*2.0
        while lon < -math.pi:
            lon += math.pi*2.0
        model[sid] = (lat,lon)

final_dists = {}

model_size = len(model)
dists = {}

for k1,v1 in model.items():

        
    alon = LON - v1[1]
    alat = LAT - v1[0]
    a = math.pow( math.sin(alat/2.0), 2.0 ) + \
        math.cos(v1[0]) * math.cos(LAT) * \
        math.pow( math.sin(alon/2.0), 2.0 )
    if a > 1:
        a = 1.0
    if a < 0:
        a = 0.0
    
    dists[k1] = math.tanh(2.0 * abs(math.atan2(math.sqrt(a), math.sqrt(1.0-a))))


minmaxNorm(dists, mn=0.0, mx=math.pi)

while len(final_dists) < NUM_FINAL_PIXELS:
    minv=1.0
    mini=0
    for k,v in dists.items():
        if v < minv:
            minv = v
            mini=k
    del dists[mini]
    final_dists[mini]=minv



    # for k,v in dists.items():
    #     if v > CUT_OFF_DIST:
    #         del model[k]
    
    # if model_size==len(model):
    #     for k,v in model.items():
    #         final_dists[k] = dists[k]
    #     break
    # # if CUT_OFF_DIST<=0.1:
    # #     break
    # # else:
    # CUT_OFF_DIST = np.mean(list(dists.values()))


# Output data
with open('MNIST_DUAL_'+str(MNIST_DIGIT),'w') as f:
    for k,v in final_dists.items():
        f.write(str(k)+" "+str(v)+"\n")


display_dists = np.zeros([28,28])

for k,v in final_dists.items():
    display_dists[k//28][k%28] = v

ax = plt.subplot()
plt.title("Dists for "+str(MNIST_DIGIT))
im = ax.imshow(display_dists, cmap='jet', aspect='auto', interpolation='nearest')
divider = make_axes_locatable(ax)
cax = divider.append_axes("right", size="5%", pad=0.05)
plt.colorbar(im, cax=cax)
plt.show()
    