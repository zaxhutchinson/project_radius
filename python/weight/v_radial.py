import matplotlib.pyplot as plt
import numpy as np
import math
import output
import defs
import random
import mycolors

out = output.Output(defs.OUTPUT_PATH)
LID = 1
NID = 0

SIZE = 20

rate = {
    0:1.0,
    1:0.8,
    2:0.6,
    3:0.4,
    4:0.2,
    5:1.0,
    6:0.8,
    7:0.6,
    8:0.4,
    9:0.2,
    10:1.0,
    11:0.8,
    12:0.6,
    13:0.4,
    14:0.2,
    15:1.0,
    16:0.8,
    17:0.6,
    18:0.4,
    19:0.2,
}
min_rate = 0.2
max_rate = 1.0

for i in range(SIZE):
    out.LoadSynapse(LID,NID,i,False)

out.LoadNeuron(LID,NID,False)
neu = out.neurons[f'{LID} {NID}']
nlat = neu.lats[-1][-1]
nlon = neu.lons[-1][-1]

spikes = []

for item in neu.spikes:
    if len(item) > 0:
        spikes.append(len(item))
    else:
        spikes.append(0)

lats = []
lons = []
rads = []
xs = []
ys = []
zs = []
colors = mycolors.GetColors(SIZE+1)
ids = []
pids = []
comps = []
points = []

class Point:
    ID = None
    pid = None
    x =[]
    y =[]
    z = []
    lat = []
    lon = []
    rad = []
    comp = None


soma = Point()
soma.lat = [nlat]
soma.lon = [nlon]
soma.ID = -1
soma.comp = -1
points = {-1:soma}
colors[soma.comp]=(0.0,0.0,0.0)


for i in range(SIZE):
    syn = out.synapses[f'{LID} {NID} {i}']
    
    lat = [item[0] for item in syn.lats]
    lon = [item[0] for item in syn.lons]
    rad = [item[0] for item in syn.rads]

    xs=[]
    ys=[]
    zs=[]
    for k in range(len(lat)):

        xs.append(rad[k]* math.cos(lat[k]) * math.cos(lon[k]))
        ys.append(rad[k] * math.cos(lat[k]) * math.sin(lon[k]))
        zs.append(rad[k] *math.sin(lat[k]))
    # x = math.cos(lat) * math.cos(lon)
    # y = math.cos(lat) * math.sin(lon)
    # z = math.sin(lat)
    # c = rad

    p = Point()
    p.ID = syn.synapse_id
    p.pid = syn.parents[-1][-1]
    p.x = xs
    p.y = ys
    p.z = zs
    p.rad = rad
    p.lat = lat
    p.lon = lon
    p.comp = syn.compartments[-1][-1]
    points[p.ID] = p

    if p.comp not in comps:
            comps.append(p.comp)
            #mycolors[p.comp]=(random.uniform(0.2,0.8),random.uniform(0.2,0.8),random.uniform(0.2,0.8))

    # xs.append(x)
    # ys.append(y)
    # zs.append(z)

C = {
    -1:'black',
    0:'gray',
    1:'tab:blue',
    2:'tab:blue',
    3:'tab:blue',
    4:'tab:blue',
    5:'tab:blue',
    6:'tab:orange',
    7:'tab:orange',
    8:'tab:orange',
    9:'tab:orange',
    10:'tab:orange',
    11:'tab:green',
    12:'tab:green',
    13:'tab:green',
    14:'tab:green',
    15:'tab:green',
    16:'tab:red',
    17:'tab:red',
    18:'tab:red',
    19:'tab:red',
    20:'tab:red',
    21:'tab:purple',
    22:'tab:purple',
    23:'tab:purple',
    24:'tab:purple',
}


lat_pos = {}

START = 300

fig = plt.figure()
for k,v in points.items():
    if k==-1: continue
    plt.plot(v.rad,label=f"{rate[k]} Hz",color=C[k])
    print(k,np.std(v.rad[START:]))
    print(k,np.mean(v.rad[START:]))
    print(k,np.min(v.rad[START:]))
    print(k,np.max(v.rad[START:]))

    lat_pos[k]=v.lat[-1]


lat_dist = 0.0
for i in range(SIZE//5):

    min_lat_index = i
    max_lat_index = i
    min_lat = lat_pos[i*5]
    max_lat = lat_pos[i*5]

    for k in range(5):
        if lat_pos[i*5+k] < min_lat:
            min_lat_index = i*5+k
            min_lat = lat_pos[i*5+k]
        if lat_pos[i*5+k] > max_lat:
            max_lat_index = i*5+k
            max_lat = lat_pos[i*5+k]

    lat_dist += abs(min_lat-max_lat)



print(f"AVG LAT DIST {lat_dist/(SIZE//5)}")

# plt.plot(spikes,color='black')
# plt.legend()
plt.xlabel("Epoch")
plt.ylabel("Radius")
plt.show()



# fig = plt.figure()
# ax = fig.add_subplot(projection='3d',computed_zorder=False)
# # a = max(np.ptp(xs), np.ptp(ys), np.ptp(zs))
# # ax.set_box_aspect((np.ptp(xs), np.ptp(ys), np.ptp(zs)))
# # ax.set_xlim3d(-a,a)
# # ax.set_ylim3d(-a,a)
# # ax.set_zlim3d(-a,a)
# for k,v in points.items():
#     if k==-1:
#         continue
#     # ax.scatter(v.lon,v.lat, color=colors[v.comp], cmap='jet',linewidths=5)
#     # ax.scatter(v.x,v.y,v.z, color=C[k], cmap='jet',linewidths=8,zorder=1)
#     ax.scatter(v.x,v.y,v.z, color='tab:blue', cmap='jet',linewidths=8,zorder=1)
#     label = str(v.ID)
#     ax.text(v.x,v.y,v.z, '%s' % (label), size=8, zorder=4,horizontalalignment='center',verticalalignment='center')
# # ax.scatter(xs_max, ys_max, zs_max, marker='o')

# # UNCOMMENT TO ADD DENDRITES
# for k,v in points.items():
#     if v.pid!=None:

#         parent = points[v.pid]
#         # if v.pid==-1:
#         #     print(v.ID, v.lat, v.lon, v.rad, v.x, v.y, v.z)    
#         # ax.plot([v.lon, parent.lon], [v.lat, parent.lat], 'gray')
#         ax.plot([v.x,parent.x],[v.y,parent.y],[v.z,parent.z],'gray')

# plt.xlabel("x")
# plt.ylabel("y")
# ax.set_zlabel("z")
# plt.show()
