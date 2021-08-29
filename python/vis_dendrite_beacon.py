import math

import matplotlib.pyplot as plt
import numpy as np

import output
import defs

NEURON_ID = 0
LAYER_ID = 1
SIDS = list(range(59))

out = output.Output(defs.OUTPUT_PATH, lid=LAYER_ID, nid=NEURON_ID)

names = [
    'B1','B2','B3','B4','B5','B6','B7','B8','B9','B10','B11','B12','B13',
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
    'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R',
    'S','T','U','V','W','X','Y','Z'
]


xs = []
ys = []
zs = []

colors = []

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
# points = {}

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
# Hide grid lines
plt.axis('off')
plt.grid(b=None)
ax.view_init(elev=30,azim=-180)
a = max(np.ptp(xs), np.ptp(ys), np.ptp(zs))
ax.set_box_aspect((np.ptp(xs), np.ptp(ys), np.ptp(zs)))
ax.set_xlim3d(-a,a)
ax.set_ylim3d(-a,a)
ax.set_zlim3d(-a,a)
for k,v in points.items():
    if k==-1:
        ax.scatter(v.x, v.y, v.z, c='black', cmap='jet')
        ax.text(v.x, v.y, v.z, '%s' % ('origin'), size=10, zorder=10 )
    else:
        ax.scatter(v.x, v.y, v.z, c='green', cmap='jet')
        ax.text(v.x, v.y, v.z, '%s' % (str(names[v.ID])), size=10, zorder=10 )
# ax.scatter(xs_max, ys_max, zs_max, marker='o')

for k,v in points.items():
    if v.pid!=None and k!=-1:

        parent = points[v.pid]
        if v.pid==-1:
            print(v.ID, v.lat, v.lon, v.rad, v.x, v.y, v.z)    
        ax.plot3D([v.x, 0.0], [v.y, 0.0], [v.z, 0.0], 'lightblue',linestyle='--')

plt.title("Synaptic Locations and Dendritic Connections")
plt.show()


