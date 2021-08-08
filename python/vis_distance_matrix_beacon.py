import math
import copy

import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1 import make_axes_locatable

import numpy as np
from numpy.linalg import matrix_power

import output
import defs


NEURON_ID = 0
LAYER_ID = 1

order = [
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12
]

out = output.Output(defs.OUTPUT_PATH, lid=LAYER_ID, nid=NEURON_ID)



xs = []
ys = []
zs = []
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


soma = Point()
soma.ID = -1
points = {-1:soma}

for name,syn in out.synapses.items():
    if syn.neuron_id != NEURON_ID:
        continue
    lat = syn.lats[-1][-1]
    lon = syn.lons[-1][-1]
    rad = syn.rads[-1][-1]
    pid = syn.parents[-1][-1]



    x = rad * math.cos(lat) * math.cos(lon)
    y = rad * math.cos(lat) * math.sin(lon)
    z = rad * math.sin(lat)
    c = rad

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

dist_matrix = np.zeros([13,13])

for k1,v1 in points.items():
    if k1==-1:
        continue
    for k2, v2 in points.items():
        if k2==-1:
            continue
        dist = math.sqrt(
            (v2.x-v1.x)**2.0 +
            (v2.y-v1.y)**2.0 +
            (v2.z-v1.z)**2.0
        )

        dist_matrix[k1][k2] = dist

ax = plt.subplot()
ax.set_xticks([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12])
ax.set_xticklabels(['B1','B2','B3','B4','B5','B6','B7','B8','B9','B10','B11','B12','B13'])
ax.set_yticks([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12])
ax.set_yticklabels(['B1','B2','B3','B4','B5','B6','B7','B8','B9','B10','B11','B12','B13'])
plt.title("Straight-line distance")
plt.xlabel("Synapse")
plt.ylabel("Synapse")
im = ax.imshow(dist_matrix, cmap='jet', aspect='auto', interpolation='nearest',origin='lower')
divider = make_axes_locatable(ax)
cax = divider.append_axes("right", size="5%", pad=0.05)
plt.colorbar(im, cax=cax)
plt.show()




###############################################################################
# SYNAPSE to SYNAPSE angular distance matrix
#   Displays the average distance of each synapse to all other synapses.
#   Arranged by pixel location in the MNIST image.

dist_matrix = np.zeros([13,13])
# avg_dists = np.zeros([784])

for k1,v1 in points.items():
    if k1==-1:
        continue

    i1 = None
    try:
        i1 = order.index(k1)
    except:
        continue

    for k2, v2 in points.items():
        if k2==-1:
            continue

        i2 = None
        try:
            i2 = order.index(k2)
        except:
            continue

        alon = v2.lon - v1.lon
        alat = v2.lat - v1.lat
        a = math.pow( math.sin(alat/2.0), 2.0 ) + \
            math.cos(v1.lat) * math.cos(v2.lat) * \
            math.pow( math.sin(alon/2.0), 2.0 )
        if a > 1:
            a = 1.0
        if a < 0:
            a = 0.0
        
        ang_dist = 2.0 * math.atan2(math.sqrt(a), math.sqrt(1.0-a))
        
        dist_matrix[i1][i2] = ang_dist# * abs(v2.rad-v1.rad)
    

# for i in range(len(avg_dists)):
#     dist_matrix[i//28][i%28] = avg_dists[i]

ax = plt.subplot()
ax.set_xticks([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12])
ax.set_xticklabels(['B1','B2','B3','B4','B5','B6','B7','B8','B9','B10','B11','B12','B13'])
ax.set_yticks([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12])
ax.set_yticklabels(['B1','B2','B3','B4','B5','B6','B7','B8','B9','B10','B11','B12','B13'])
plt.title("Angular distance")
plt.xlabel("Synapse")
plt.ylabel("Synapse")
im = ax.imshow(dist_matrix, cmap='jet', aspect='auto', interpolation='nearest',origin='lower')
divider = make_axes_locatable(ax)
cax = divider.append_axes("right", size="5%", pad=0.05)
plt.colorbar(im, cax=cax)
plt.show()

# Write the angDists to file for use by expBeacon_dist.py
with open('python/exp_beacon/ang_dists', 'w') as f:
    for i in range(13):
        for j in range(13):
            f.write(str(dist_matrix[i][j])+' ')
        f.write('\n')

##############################################################################
# Note: The soma has a id (or k) of -1 and synapses that connect to the soma
#   have a pid of -1. So in this graph the else will indicate the somatic conns
#   in the last column due to Python's negative index wrapping.
# conn_matrix = np.zeros([len(xs), len(xs)+1])
# for k, v in points.items():
#     if k == -1:
#         pass
#     else:
#         conn_matrix[k][v.pid] = 0.5

# plt.title("Connectivity (Parent-Child Relationship)")
# plt.xlabel("Parent")
# plt.ylabel("Child")
# plt.imshow(conn_matrix, cmap='binary', aspect='auto', interpolation='nearest',origin='lower')
# plt.show()

# ##############################################################################
# # diff_mat = np.zeros([len(points)-1])
# # pid_std_mat = np.zeros([len(points)-1])
# # for k,v in points.items():
# #     if k == -1:
# #         pass
# #     else:
# #         diff = abs(v.pid-k)
# #         diff_mat[k] = diff

# # mean = np.mean(diff_mat)

# ##############################################################################
# edges = np.zeros([len(points),len(points)])
# dist = np.ones([len(points),len(points)])
# Next = np.empty([len(points),len(points)])
# for x in range(len(points)):
#     for y in range(len(points)):
#         Next[x][y]=None
#         dist[x][y] = np.inf

# for k,v in points.items():
#     if v.pid != None:
#         edges[k][v.pid] = 1
#         edges[v.pid][k] = 1

# for x in range(len(points)):
#     for y in range(len(points)):
#         if edges[x][y] > 0:
#             try:
#                 dist[x][y] = edges[x][y]
#                 Next[x][y] = y
#             except IndexError:
#                 print(x, y)
#                 print(len(dist[x]), len(edges[x]))

# for x in range(len(points)):
#     dist[x][x] = 0
#     Next[x][x] = x

# for k in range(len(points)):
#     for i in range(len(points)):
#         for j in range(len(points)):
#             if dist[i][j] > dist[i][k] + dist[k][j]:
#                 dist[i][j] = dist[i][k] + dist[k][j]
#                 Next[i][j] = Next[i][k]

# ax = plt.subplot()

# im = ax.imshow(dist, cmap='jet', aspect='auto', interpolation='nearest',origin='lower')
# plt.title("Dendritic (Network) Distance")
# plt.xlabel("Synapse")
# plt.ylabel("Synapse")
# divider = make_axes_locatable(ax)
# cax = divider.append_axes("right", size="5%", pad=0.05)
# plt.colorbar(im, cax=cax)
# plt.show()


# #############################################################################3
# # Radial Distances
# radial_dist = np.zeros([len(points)-1])
# synapse_list = np.zeros([len(points)-1])
# # colors = ['blue']*10+['red']*10+['purple']*10+['green']*10 #004D
# colors = ['red','blue','purple','green','orange','grey','yellow']*5 #004C
# for k,v in points.items():
#     synapse_list[k] = k
#     radial_dist[k] = v.rad

# plt.title("Radial Distances")
# plt.xlabel("Synapse ID")
# plt.ylabel("Distance")
# plt.scatter(synapse_list, radial_dist,c=colors)
# plt.show()