import math

import matplotlib.pyplot as plt
import numpy as np

import output
import defs

NEURON_ID = 0
LAYER_ID = 1
SIDS = list(range(784))

outA = output.Output("/run/media/zax/a06347ed-42d6-48d5-a380-ddcfcb7fcf75/output/project_radius/output/", lid=LAYER_ID, nid=0)
outB = output.Output("/run/media/zax/a06347ed-42d6-48d5-a380-ddcfcb7fcf75/output/project_radius/output/", lid=LAYER_ID, nid=9)

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
modelA = {}
modelB = {}

# Load data
for name,syn in outA.synapses.items():
    if syn.neuron_id == NEURON_ID and syn.layer_id == LAYER_ID:
        sid = syn.synapse_id
        lat = syn.lats[-1][-1]
        lon = syn.lons[-1][-1]
        while lon > math.pi:
            lon -= math.pi*2.0
        while lon < -math.pi:
            lon += math.pi*2.0
        modelA[sid] = (lat,lon)

for name,syn in outB.synapses.items():
    if syn.neuron_id == NEURON_ID and syn.layer_id == LAYER_ID:
        sid = syn.synapse_id
        lat = syn.lats[-1][-1]
        lon = syn.lons[-1][-1]
        while lon > math.pi:
            lon -= math.pi*2.0
        while lon < -math.pi:
            lon += math.pi*2.0
        modelB[sid] = (lat,lon)



model_size = len(modelA)
distsA = {}
distsB = {}

for k1,v1 in modelA.items():
    dist = 0.0
    for k2,v2 in modelA.items():
        
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
    distsA[k1] = dist

for k1,v1 in modelB.items():
    dist = 0.0
    for k2,v2 in modelB.items():
        
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
    distsB[k1] = dist


minmaxNorm(distsA, mn=0.0, mx=math.pi)
minmaxNorm(distsB, mn=0.0, mx=math.pi)

final_distsA = {}
final_distsB = {}

while len(final_distsA) < NUM_FINAL_PIXELS:
    minv=math.inf
    mini=0
    for k,v in distsA.items():
        if v < minv:
            minv = v
            mini=k
    del distsA[mini]
    final_distsA[mini]=minv

while len(final_distsB) < NUM_FINAL_PIXELS:
    minv=math.inf
    mini=0
    for k,v in distsB.items():
        if v < minv:
            minv = v
            mini=k
    del distsB[mini]
    final_distsB[mini]=minv

A = 0
B = 0
AB = 0

fig = plt.figure()
ax = fig.add_subplot()
for k,v in modelA.items():
    if k in final_distsA and k in final_distsB:
        ax.scatter(v[1],v[0],color='purple',zorder=10)
        AB+=1
    elif k in final_distsA:
        ax.scatter(v[1],v[0],color='red',zorder=10)
        A+=1
    elif k in final_distsB:
        ax.scatter(v[1],v[0],color='blue',zorder=10)
        B+=1
    else:
        ax.scatter(v[1],v[0],color='black',zorder=1)
plt.xlabel("Longitude")
plt.ylabel("Latitude")

print(f"A  {A}")
print(f"B  {B}")
print(f"AB {AB}")

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


