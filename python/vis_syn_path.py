import math

import matplotlib.pyplot as plt
import numpy as np

import output
import defs



NEURON_ID = 0
LAYER_ID = 1
SYN_IDS = list(range(64))

out = output.Output(defs.OUTPUT_PATH, sid=SYN_IDS, nid=NEURON_ID, lid=LAYER_ID)

colors = []

#002
# SYN_IDS = list(range(0,100))
# colors = ['purple']*10+['blue']*45+['red']*45

#004B
# SYN_IDS = [6,9,11]
# SYN_IDS = list(range(40))

#004C
# SYN_IDS = list(range(0,35))
# colors = ['red','blue','purple','green','orange','gray','yellow']*5

#004D
# SYN_IDS = list(range(0,40))
# colors = ['red']*10+['blue']*10+['purple']*10+['green']*10

#Beacon
# SYN_IDS = list(range(0,13))

#MNIST
# SYN_IDS = [3]

#LINES VERT
# SYN_IDS = list(range(10,15))
# SYN_IDS = list(range(25))
# SYN_IDS = [0,5,10,15,20]
# SYN_IDS = [1,2,3,4,19]

xs = {}
ys = {}
zs = {}


class Point:
    ID = None
    pid = None
    x = 0.0
    y =0.0
    z = 0.0
    lat = 0.0
    lon = 0.0
    rad = 0.0

for sid in SYN_IDS:
    name = str(LAYER_ID)+" "+str(NEURON_ID)+" "+str(sid)
    syn = out.synapses[name]

    xs[sid] = []
    ys[sid] = []
    zs[sid] = []


    # Use this to see the final movements
    #for i in range(len(syn.lats)-1000,len(syn.lats)):
    for i in range(len(syn.lats)):



        lat = syn.lats[i][-1]
        lon = syn.lons[i][-1]
        rad = syn.rads[i][-1]
        pid = syn.parents[i][-1]

        # print(lon)

        x = rad * math.cos(lat) * math.cos(lon)
        y = rad * math.cos(lat) * math.sin(lon)
        z = rad * math.sin(lat)

        xs[sid].append(x)
        ys[sid].append(y)
        zs[sid].append(z)

fig = plt.figure()
ax = fig.add_subplot(projection='3d')
# a = max(np.ptp(xs), np.ptp(ys), np.ptp(zs))
# ax.set_box_aspect((np.ptp(xs), np.ptp(ys), np.ptp(zs)))
# ax.set_xlim3d(-a,a)
# ax.set_ylim3d(-a,a)
# ax.set_zlim3d(-a,a)
for k,v in xs.items():
    if colors:
        ax.plot3D(xs[k], ys[k], zs[k], label=k, c=colors[k])
    else:
        ax.plot3D(xs[k], ys[k], zs[k], label=k)
ax.legend()
plt.show()


