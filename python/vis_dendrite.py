import math

import matplotlib.pyplot as plt

import output
import defs

out = output.Output(defs.OUTPUT_PATH)

# xs_max = []
# ys_max = []
# zs_max = []

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

soma = Point()
soma.ID = -1
points = {-1:soma}

for name,syn in out.synapses.items():
    lat = syn.lats[-1][-1]
    lon = syn.lons[-1][-1]
    rad = syn.rads[-1][-1]
    pid = syn.parents[-1][-1]



    x = rad * math.cos(lat) * math.cos(lon)
    y = rad * math.cos(lat) * math.sin(lon)
    z = rad * math.sin(lat)
    c = 100-rad

    p = Point()
    p.ID = syn.synapse_id
    p.pid = pid
    p.x = x
    p.y = y
    p.z = z
    points[p.ID] = p

    xs.append(x)
    ys.append(y)
    zs.append(z)
    colors.append(c)



fig = plt.figure()
ax = fig.add_subplot(projection='3d')
ax.scatter(xs, ys, zs, c=colors, cmap='jet')
# ax.scatter(xs_max, ys_max, zs_max, marker='o')

for k,v in points.items():
    if v.pid:
        parent = points[v.pid]
        ax.plot3D([v.x, parent.x], [v.y, parent.y], [v.z, parent.z], 'gray')

plt.show()


