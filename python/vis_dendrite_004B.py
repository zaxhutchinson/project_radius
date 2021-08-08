import math

import matplotlib.pyplot as plt
from matplotlib.lines import Line2D
import numpy as np

import output
import defs

NEURON_ID = 0
LAYER_ID = 1
SIDS = list(range(100))
DISPLAY_DENDRITE = False

out = output.Output(defs.OUTPUT_PATH, lid=LAYER_ID, nid=NEURON_ID)



xs = []
ys = []
zs = []

colors = []
colors += ['lightseagreen']
colors += ['tab:blue']*8
colors += ['tab:purple']*2
colors += ['tab:red']*8
colors += ['tab:orange']*2
colors += ['yellow']*8
colors += ['greenyellow']*2
colors += ['tab:green']*8
colors += ['lightseagreen']

legend_eles = [
    Line2D([0], [0], marker='o',markerfacecolor='tab:blue',label='A',markersize=10,color='w'),
    Line2D([0], [0], marker='o',markerfacecolor='tab:red',label='B',markersize=10,color='w'),
    Line2D([0], [0], marker='o',markerfacecolor='yellow',label='C',markersize=10,color='w'),
    Line2D([0], [0], marker='o',markerfacecolor='tab:green',label='D',markersize=10,color='w'),
    Line2D([0], [0], marker='o',markerfacecolor='tab:purple',label='AB',markersize=10,color='w'),
    Line2D([0], [0], marker='o',markerfacecolor='tab:orange',label='BC',markersize=10,color='w'),
    Line2D([0], [0], marker='o',markerfacecolor='greenyellow',label='CD',markersize=10,color='w'),
    Line2D([0], [0], marker='o',markerfacecolor='lightseagreen',label='DA',markersize=10,color='w'),
]

points = []

class Point:
    ID = None
    pid = None
    x = 0.0
    y =0.0
    z = 0.0
    lat = 0.0
    lon = 0.0
    rad = 0.0


soma = Point()
soma.ID = -1
points = {-1:soma}

for name,syn in out.synapses.items():
    if syn.neuron_id == NEURON_ID and syn.layer_id == LAYER_ID and syn.synapse_id in SIDS:
        lat = syn.lats[-1][-1]
        lon = syn.lons[-1][-1]
        rad = syn.rads[-1][-1]
        pid = syn.parents[-1][-1]

        x = rad * math.cos(lat) * math.cos(lon)
        y = rad * math.cos(lat) * math.sin(lon)
        z = rad * math.sin(lat)
        c = rad

        if math.isinf(x) or math.isinf(y) or math.isinf(z):
            print("NAN:",name, x,y,z)
        if math.isnan(x) or math.isnan(y) or math.isnan(z):
            print("INF:",name,x,y,z)

        p = Point()
        p.ID = syn.synapse_id
        p.pid = pid
        p.x = x
        p.y = y
        p.z = z
        p.rad = rad
        p.lat = lat
        p.lon = lon
        points[p.ID] = p

        xs.append(x)
        ys.append(y)
        zs.append(z)
        colors.append(c)

fig = plt.figure()
ax = fig.add_subplot(projection='3d')
a = max(np.ptp(xs), np.ptp(ys), np.ptp(zs))
ax.set_box_aspect((np.ptp(xs), np.ptp(ys), np.ptp(zs)))
ax.set_xlim3d(-a,a)
ax.set_ylim3d(-a,a)
ax.set_zlim3d(-a,a)
for k,v in points.items():
    if k != -1:
        ax.scatter(v.x, v.y, v.z, c=colors[k], cmap='jet',linewidths=4)
    #ax.text(v.x, v.y, v.z, '%s' % (str(v.ID)), size=10, zorder=1 )

if DISPLAY_DENDRITE:
    for k,v in points.items():
        if v.pid!=None:

            parent = points[v.pid]
            ax.plot3D([v.x, parent.x], [v.y, parent.y], [v.z, parent.z], 'gray')



# plt.title("Synaptic Location")
ax.legend(handles=legend_eles,loc=(1,0.33))
ax.set_xticklabels([])
ax.set_yticklabels([])
ax.set_zticklabels([])
plt.show()


