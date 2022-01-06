import math
import random

import matplotlib.pyplot as plt
import numpy as np

import output
import defs

NEURON_ID = 0
LAYER_ID = 1
SIDS = list(range(64))

out = output.Output(defs.OUTPUT_PATH, lid=LAYER_ID, nid=NEURON_ID, sid=SIDS)



xs = []
ys = []
zs = []

comps = []
comp_colors = {}
syn_colors = {}

points = {}

class Point:
    ID = None
    pid = None
    x = 0.0
    y =0.0
    z = 0.0
    lat = 0.0
    lon = 0.0
    rad = 0.0
    comp = None


soma = Point()
soma.ID = -1
soma.comp = -1
points = {-1:soma}
comp_colors[soma.comp]=(0.0,0.0,0.0)
syn_colors[soma.ID]=(0.0,0.0,0.0)

for name,syn in out.synapses.items():
    if syn.neuron_id == NEURON_ID and syn.layer_id == LAYER_ID and syn.synapse_id in SIDS:
        lat = syn.lats[-1][-1]
        lon = syn.lons[-1][-1]
        rad = syn.rads[-1][-1]
        pid = syn.parents[-1][-1]
        comp = syn.compartments[-1][-1]
        if comp==None:
            print(pid,comp)

        if comp not in comps:
            comps.append(comp)
            comp_colors[comp]=(random.uniform(0.2,0.8),random.uniform(0.2,0.8),random.uniform(0.2,0.8))

        if int(syn.synapse_id)<16:
            syn_colors[syn.synapse_id]='lime'
        elif int(syn.synapse_id)<32:
            syn_colors[syn.synapse_id]='green'
        elif int(syn.synapse_id)<48:
            syn_colors[syn.synapse_id]='lightcoral'
        elif int(syn.synapse_id)<64:
            syn_colors[syn.synapse_id]='firebrick'

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
        p.comp = comp
        # if p.ID!=-1 and p.ID<16:
        points[p.ID] = p

        xs.append(x)
        ys.append(y)
        zs.append(z)

fig = plt.figure()
ax = fig.add_subplot(projection='3d')
a = max(np.ptp(xs), np.ptp(ys), np.ptp(zs))
ax.set_box_aspect((np.ptp(xs), np.ptp(ys), np.ptp(zs)))
ax.set_xlim3d(-a,a)
ax.set_ylim3d(-a,a)
ax.set_zlim3d(-a,a)
for k,v in points.items():

    ax.scatter(v.x, v.y, v.z, color=syn_colors[v.ID], cmap='jet')
    label = str(v.ID)
    # ax.text(v.x, v.y, v.z, '%s' % (label), size=10, zorder=1 )


for k,v in points.items():
    if v.pid!=None:
        # if v.pid==-1:
        #     continue

        parent = points[v.pid]
 
        ax.plot3D([v.x, parent.x], [v.y, parent.y], [v.z, parent.z], 'gray')

plt.title("Synaptic Locations and Dendritic Connections")
plt.show()


