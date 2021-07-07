import math

import matplotlib.pyplot as plt
import numpy as np

import output
import defs

out = output.Output(defs.OUTPUT_PATH)

NEURON_ID = 0
LAYER_ID = 1
SYN_IDS = list(range(0,5))+list(range(95,100))
xs = {}
ys = {}
zs = {}

colors = []

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

    for i in range(len(syn.lats)):

        lat = syn.lats[i][-1]
        lon = syn.lons[i][-1]
        rad = syn.rads[i][-1]
        pid = syn.parents[i][-1]

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
    ax.plot3D(xs[k], ys[k], zs[k], label=k)
ax.legend()
plt.show()


