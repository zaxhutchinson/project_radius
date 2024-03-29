import math

import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1 import make_axes_locatable
import numpy as np

import output
import defs

LAYER_ID = 1
NEURON_ID = 0

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



points = {}

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




###############################################################################
# SYNAPSE to SYNAPSE straight line distance matrix
#   Displays the average distance of each synapse to all other synapses.
#   Arranged by pixel location in the MNIST image.
sl_dist_matrix = np.zeros([28, 28])
avg_dists = np.zeros([784])

for k1,v1 in points.items():
    if k1==-1:
        continue
    dist = 0.0
    for k2, v2 in points.items():
        if k2==-1:
            continue
        dist += math.sqrt(
            (v2.x-v1.x)**2.0 +
            (v2.y-v1.y)**2.0 +
            (v2.z-v1.z)**2.0
        )
    avg_dists[k1] = dist / len(points)
    

for i in range(len(avg_dists)):
    sl_dist_matrix[i//28][i%28] = avg_dists[i]

plt.title("Avg Straight-line distance")
plt.imshow(sl_dist_matrix, cmap='jet', aspect='auto', interpolation='nearest')
plt.show()

###############################################################################
# SYNAPSE to SYNAPSE angular distance matrix
#   Displays the average distance of each synapse to all other synapses.
#   Arranged by pixel location in the MNIST image.
PRE_SPIKE_TIME_WINDOW = 30

dist_matrix = np.zeros([28,28])
avg_dists = np.zeros([784])

for k1,v1 in points.items():
    if k1==-1:
        continue
    dist = 0.0
    for k2, v2 in points.items():
        if k2==-1:
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
        
        avg_dists[k1] += 2.0 * math.atan2(math.sqrt(a), math.sqrt(1.0-a))

    avg_dists[k1] /= len(points)
        # dist_matrix[k1][k2] += dist
    

for i in range(len(avg_dists)):
    dist_matrix[i//28][i%28] = avg_dists[i]

ax = plt.subplot()
plt.title("Avg angular distance")
im = ax.imshow(dist_matrix, cmap='jet', aspect='auto', interpolation='nearest')
divider = make_axes_locatable(ax)
cax = divider.append_axes("right", size="5%", pad=0.05)
plt.colorbar(im, cax=cax)
plt.show()




############################################################################
# ax = plt.subplot()
# plt.title("Lat Long")
# for k,v in points.items():
#     ax.scatter(v.lon,v.lat,color='b')

# plt.show()


dist_matrix = np.zeros([28,28])
avg_dists = np.zeros([784])

# 3
lon = -1.5
lat = -0.0

# 8
# lon = -1.7
# lat = -0.45

for k1,v1 in points.items():



    alon = lon - v1.lon
    alat = lat - v1.lat
    a = math.pow( math.sin(alat/2.0), 2.0 ) + \
        math.cos(v1.lat) * math.cos(lat) * \
        math.pow( math.sin(alon/2.0), 2.0 )
    if a > 1:
        a = 1.0
    if a < 0:
        a = 0.0
    
    #3
    avg_dists[k1] = math.tanh(2.0 * math.atan2(math.sqrt(a), math.sqrt(1.0-a)))
    
    #8
    # avg_dists[k1] = 2.0 * math.atan2(math.sqrt(a), math.sqrt(1.0-a))
    # avg_dists[k1] = avg_dists[k1] / (1.0 + abs(avg_dists[k1]))


    

for i in range(len(avg_dists)):
    dist_matrix[i//28][i%28] = avg_dists[i]

ax = plt.subplot()
plt.title("Avg angular distance")
im = ax.imshow(dist_matrix, cmap='jet', aspect='auto', interpolation='nearest')
divider = make_axes_locatable(ax)
cax = divider.append_axes("right", size="5%", pad=0.05)
plt.colorbar(im, cax=cax)
plt.show()



###############################################################################
# SYNAPSE to SYNAPSE angular distance matrix
#   Displays the average distance of each synapse to all other synapses.
#   Arranged by pixel location in the MNIST image.

# dist_matrix = np.zeros([28,28])
# # avg_dists = np.zeros([784])

# for k1,v1 in points.items():
#     if k1==-1:
#         continue
#     dist = 0.0
#     for k2, v2 in points.items():
#         if k2==-1:
#             continue

#         alon = v2.lon - v1.lon
#         alat = v2.lat - v1.lat
#         a = math.pow( math.sin(alat/2.0), 2.0 ) + \
#             math.cos(v1.lat) * math.cos(v2.lat) * \
#             math.pow( math.sin(alon/2.0), 2.0 )
#         if a > 1:
#             a = 1.0
#         if a < 0:
#             a = 0.0
        
#         ang_dist = 2.0 * math.atan2(math.sqrt(a), math.sqrt(1.0-a))

#         sl_dist = math.sqrt(
#             (v2.x-v1.x)**2.0 +
#             (v2.y-v1.y)**2.0 +
#             (v2.z-v1.z)**2.0
#         )

#         dist += ang_dist * abs(v2.rad-v1.rad)
#     dist_matrix[k1//28][k1%28] = dist
    

# # for i in range(len(avg_dists)):
# #     dist_matrix[i//28][i%28] = avg_dists[i]


# plt.title("Avg angular distance * radial difference")
# plt.imshow(dist_matrix, cmap='jet', aspect='auto', interpolation='nearest')
# plt.show()


# ###############################################################################
# # Radius
# #   Displays the radius of each synapses
# #   Arranged by pixel location in the MNIST image.
# dist_matrix = np.zeros([28,28])

# for k1,v1 in points.items():
#     if k1==-1:
#         continue
#     dist_matrix[k1//28][k1%28] = v1.rad

# plt.title("Radial distance")
# plt.imshow(dist_matrix, cmap='jet', aspect='auto', interpolation='nearest')
# plt.show()


# ###############################################################################
# # Connectivity
# #   Displays which parental/child relationships in a 2d graph.
# # Note: The soma has a id (or k) of -1 and synapses that connect to the soma
# #   have a pid of -1. So in this graph the else will indicate the somatic conns
# #   in the last column due to Python's negative index wrapping.
# conn_matrix = np.zeros([len(xs), len(xs)+1])
# for k, v in points.items():
#     if k == -1:
#         pass
#     else:
#         conn_matrix[k][v.pid] = 0.5

# plt.title("Parent-Child relationships")
# plt.imshow(conn_matrix, cmap='binary', aspect='auto', interpolation='nearest')
# plt.show()


# # fig = plt.figure()
# # ax = fig.add_subplot(projection='3d')
# # for i in range(len(xs)):
# #     ax.scatter(xs[i], ys[i], zs[i], c=colors[i], cmap='jet')
# #     ax.text(xs[i], ys[i], zs[i], '%s' % (str(i)), size=10, zorder=1 )
# # # ax.scatter(xs_max, ys_max, zs_max, marker='o')

# # for k,v in points.items():
# #     if v.pid!=None:

# #         parent = points[v.pid]
# #         # if v.pid==-1:
# #         #     print(v.ID, v.lat, v.lon, v.rad, v.x, v.y, v.z)    
# #         ax.plot3D([v.x, parent.x], [v.y, parent.y], [v.z, parent.z], 'gray')




