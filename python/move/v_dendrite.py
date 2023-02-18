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
SIZE = 100

for i in range(SIZE):
    out.LoadSynapse(LID,NID,i)

out.LoadNeuron(LID,NID)
neu = out.neurons[f'{LID} {NID}']
nlat = neu.lats[-1][-1]
nlon = neu.lons[-1][-1]

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
weights = []

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
    weight = 0.0
    denpathlen = 0.0

soma = Point()
soma.lat = nlat
soma.lon = nlon
soma.ID = -1
soma.comp = -1
points = {-1:soma}
colors[soma.comp]=(0.0,0.0,0.0)


for i in range(SIZE):
    syn = out.synapses[f'{LID} {NID} {i}']
    
    lat = syn.lats[-1][-1]
    lon = syn.lons[-1][-1]
    rad = syn.rads[-1][-1]

    weights.append(syn.strengths[-1][-1])

    x = rad * math.cos(lat) * math.cos(lon)
    y = rad * math.cos(lat) * math.sin(lon)
    z = rad * math.sin(lat)
    c = rad

    p = Point()
    p.ID = syn.synapse_id
    p.pid = syn.parents[-1][-1]
    p.x = x
    p.y = y
    p.z = z
    p.rad = rad
    p.lat = lat
    p.lon = lon
    p.comp = syn.compartments[-1][-1]
    p.weight = syn.strengths[-1][-1]
    points[p.ID] = p


    if p.comp not in comps:
            comps.append(p.comp)
            #mycolors[p.comp]=(random.uniform(0.2,0.8),random.uniform(0.2,0.8),random.uniform(0.2,0.8))

    xs.append(x)
    ys.append(y)
    zs.append(z)


for k,v in points.items():
    if v.pid != None:
        parent = points[v.pid]
        v.denpathlen = math.sqrt(
            (parent.x-v.x)**2.0 +
            (parent.y-v.y)**2.0 +
            (parent.z-v.z)**2.0
        )
lengths = []
radii = []
for k,v in points.items():
    total_path_len = 0.0

    next = v
    while next.pid != None:
        
        total_path_len += points[next.ID].denpathlen
        next = points[next.pid]
        
    print(k,total_path_len, v.rad)
    if k >= 0:
        lengths.append(total_path_len)
        radii.append(v.rad)

prev_total = 0
prev_radtotal = 0
for i in range(0,len(lengths),10):
    total = sum(lengths[i:i+10])/10
    radtotal = sum(radii[i:i+10])/10
    print(i,total, radtotal, prev_total-total, prev_radtotal-radtotal)


# TWO D VERSION
fig = plt.figure()
ax = fig.add_subplot()
# a = max(np.ptp(xs), np.ptp(ys), np.ptp(zs))
# ax.set_box_aspect((np.ptp(xs), np.ptp(ys), np.ptp(zs)))
# ax.set_xlim3d(-a,a)
# ax.set_ylim3d(-a,a)
# ax.set_zlim3d(-a,a)
for k,v in points.items():
    if k==-1: continue
    ax.scatter(v.lon,v.lat, color=colors[v.comp], cmap='jet')
    label = str(v.ID)
    ax.text(v.lon,v.lat, '%s' % (label), size=10, zorder=1 )
# ax.scatter(xs_max, ys_max, zs_max, marker='o')

for k,v in points.items():
    if k==-1:continue
    if v.pid==-1:continue
    if v.pid!=None:

        parent = points[v.pid]
        # if v.pid==-1:
        #     print(v.ID, v.lat, v.lon, v.rad, v.x, v.y, v.z)    
        ax.plot([v.lon, parent.lon], [v.lat, parent.lat], 'gray')

# plt.title("Synaptic Locations and Dendritic Connections")
plt.xlabel("Longitude")
plt.ylabel("Latitude")
plt.show()



wmin = min(weights)
wmax = max(weights)

fig = plt.figure()
ax = fig.add_subplot(projection='3d')
# a = max(np.ptp(xs), np.ptp(ys), np.ptp(zs))
# ax.set_box_aspect((np.ptp(xs), np.ptp(ys), np.ptp(zs)))
# ax.set_xlim3d(-a,a)
# ax.set_ylim3d(-a,a)
# ax.set_zlim3d(-a,a)
for k,v in points.items():
    # if v.rad > 250: continue
    # print(v.weight)
    ax.scatter(v.x, v.y, v.z, color=colors[v.comp], cmap='jet')
    # ax.scatter(v.x, v.y, v.z, color='tab:blue',cmap='jet',vmin=wmin,vmax=wmax) #c=v.weight, 
    label = str(v.ID)
    ax.text(v.x, v.y, v.z, '%s' % (label), size=10, zorder=1 )
# ax.scatter(xs_max, ys_max, zs_max, marker='o')

for k,v in points.items():
    # if v.rad > 250: continue
    if v.pid!=None:

        parent = points[v.pid]
        # if v.pid==-1:
        #     print(v.ID, v.lat, v.lon, v.rad, v.x, v.y, v.z)    
        ax.plot3D([v.x, parent.x], [v.y, parent.y], [v.z, parent.z], 'gray')

plt.show()
