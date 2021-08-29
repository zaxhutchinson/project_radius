import math

import matplotlib.pyplot as plt
import numpy as np

import output
import defs

NEURON_ID = 0
LAYER_ID = 1
SIDS = list(range(784))

out = output.Output(defs.OUTPUT_PATH, lid=LAYER_ID, nid=NEURON_ID)

NEURON_ID = 0
LAYER_ID = 1
MNIST_DIGIT = 8
CUT_OFF_DIST = 0.5
NUM_FINAL_PIXELS = 100

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
    dist = 0.0
    for k2,v2 in model.items():
        
        alon = v2[1] - v1[1]
        alat = v2[0] - v1[0]
        a = math.pow( math.sin(alat/2.0), 2.0 ) + \
            math.cos(v1[0]) * math.cos(v2[0]) * \
            math.pow( math.sin(alon/2.0), 2.0 )
        if a > 1:
            a = 1.0
        if a < 0:
            a = 0.0
        
        dist += 2.0 * abs(math.atan2(math.sqrt(a), math.sqrt(1.0-a)))
    dists[k1] = dist /  len(model)


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

fig = plt.figure()
ax = fig.add_subplot()
for k,v in model.items():
    if k in final_dists:
        ax.scatter(v[1],v[0],color='red',zorder=10)
    else:
        ax.scatter(v[1],v[0],color='black',zorder=1)
plt.xlabel("Longitude")
plt.ylabel("Latitude")
plt.show()

# fig = plt.figure()
# ax = fig.add_subplot(projection='3d')
# a = max(np.ptp(xs), np.ptp(ys), np.ptp(zs))
# ax.set_box_aspect((np.ptp(xs), np.ptp(ys), np.ptp(zs)))
# ax.set_xlim3d(-a,a)
# ax.set_ylim3d(-a,a)
# ax.set_zlim3d(-a,a)
# for k,v in points.items():
#     ax.scatter(v.x, v.y, v.z, c=v.rad, cmap='jet')
#     ax.text(v.x, v.y, v.z, '%s' % (str(v.ID)), size=10, zorder=1 )
# # ax.scatter(xs_max, ys_max, zs_max, marker='o')

# # for k,v in points.items():
# #     if v.pid!=None:

# #         parent = points[v.pid]
# #         # if v.pid==-1:
# #         #     print(v.ID, v.lat, v.lon, v.rad, v.x, v.y, v.z)    
# #         ax.plot3D([v.x, parent.x], [v.y, parent.y], [v.z, parent.z], 'gray')

# plt.title("Synaptic Locations and Dendritic Connections")
# plt.show()


