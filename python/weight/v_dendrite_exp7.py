import matplotlib.pyplot as plt
import numpy as np
import math
import output
import defs
import random
import mycolors
import helpers

out = output.Output(defs.OUTPUT_PATH)
LID = 1
NID = 0

SIZE = 20



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
    comp_weight = 0.0


soma = Point()
soma.lat = nlat
soma.lon = nlon
soma.ID = -1
soma.comp = -1
points = {-1:soma}
colors[soma.comp]=(0.0,0.0,0.0)
weights = []

for i in range(SIZE):
    syn = out.synapses[f'{LID} {NID} {i}']
    
    lat = syn.lats[-1][-1]
    lon = syn.lons[-1][-1]
    rad = syn.rads[-1][-1]

    x = rad* math.cos(lat) * math.cos(lon)
    y = rad * math.cos(lat) * math.sin(lon)
    z = rad *math.sin(lat)
    # x = math.cos(lat) * math.cos(lon)
    # y = math.cos(lat) * math.sin(lon)
    # z = math.sin(lat)
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
    p.comp_weight = syn.comp_strs[-1][-1]
    weights.append(p.weight)
    rads.append(p.rad)
    points[p.ID] = p


    if p.comp not in comps:
            comps.append(p.comp)
            # mycolors[p.comp]=(random.uniform(0.2,0.8),random.uniform(0.2,0.8),random.uniform(0.2,0.8))

    xs.append(x)
    ys.append(y)
    zs.append(z)

# C = {
#     -1:'black',
#     #0:'gray',
#     1:'tab:blue',
#     2:'tab:blue',
#     3:'tab:blue',
#     4:'tab:blue',
#     0:'tab:blue',
#     6:'tab:orange',
#     7:'tab:orange',
#     8:'tab:orange',
#     9:'tab:orange',
#     5:'tab:orange',
#     11:'tab:green',
#     12:'tab:green',
#     13:'tab:green',
#     14:'tab:green',
#     10:'tab:green',
#     16:'tab:red',
#     17:'tab:red',
#     18:'tab:red',
#     19:'tab:red',
#     15:'tab:red',
# }
MAX_WEIGHT = 500

actual_weights = []
for w in weights:
    actual_weights.append(
        (w*MAX_WEIGHT)/(abs(w)+MAX_WEIGHT)
    )

xlabels=[]
for i in range(SIZE):
    xlabels.append(str(i))

fig,ax1 = plt.subplots()
ax2 = ax1.twinx()

ax1.plot(weights,label='Raw weight',color='tab:blue')
ax1.set_xlabel("Connections by Input ID")
ax1.set_xticks(list(range(SIZE)))
ax1.set_xticklabels(xlabels)
ax1.set_ylabel("Raw Weight")
ax1.tick_params(axis='y',labelcolor='tab:blue')
ax1.yaxis.label.set_color('tab:blue')


ax2.plot(actual_weights,label='Actual weight',color='tab:red')
ax2.set_ylabel("Actual Weight")
ax2.tick_params(axis='y',labelcolor='tab:red')
ax2.yaxis.label.set_color('tab:red')
# plt.legend()
plt.xlabel("Connections by Input ID")
plt.show()


MIN_WEIGHT = min(actual_weights)
MAX_WEIGHT = max(actual_weights)
MINMAX_W = MIN_WEIGHT+MAX_WEIGHT
MIN_RAD = min(rads)
MAX_RAD = max(rads)
MINMAX_R = MIN_RAD+MAX_RAD

weight_rad = []

for i in range(len(actual_weights)):
    w = (actual_weights[i])/(MAX_WEIGHT-MIN_WEIGHT)
    r = (rads[i])/(MAX_RAD-MIN_RAD)
    weight_rad.append(w-r)

plt.figure()
plt.plot(weight_rad,color='black')
plt.xlabel("Connections by Input ID")

plt.ylabel(r"$\hat{w}$ - $\hat{r}$")
plt.show()



# COMP to WEIGHT comparison

weights_by_comp = {}
rads_by_comp = {}

for k,v in points.items():
    if k < 0: continue
    if v.comp not in weights_by_comp:
        weights_by_comp[v.comp] = []
        rads_by_comp[v.comp] = []
        weights_by_comp[v.comp].append(v.weight)
        rads_by_comp[v.comp].append(v.rad)
    else:
        weights_by_comp[v.comp].append(v.weight)
        rads_by_comp[v.comp].append(v.rad)

plt.figure()
for i in weights_by_comp.keys():
    plt.scatter(np.mean(rads_by_comp[i]),np.mean(weights_by_comp[i]))

plt.show()




COMP_C = mycolors.GetColors(SIZE)
COMP_C[-1]='black'


C = {-1:'black'}
for i in range(SIZE):
    C[i]='tab:blue'



A = [points[5].lat,points[5].lon]
B = [points[6].lat,points[6].lon]
radA=points[5].rad
radB=points[6].rad
angle = helpers.angular_dist(A,B)
dist = math.sqrt(
    radA**2 + radB**2 - 2.0*radA*radB*math.cos(angle)
)
print(radA,radB)
print(f"Distance {dist}")
print(radA*0.5+dist)
print(radA*0.1+dist)

fig = plt.figure()
ax = fig.add_subplot(projection='3d',computed_zorder=False)
# a = max(np.ptp(xs), np.ptp(ys), np.ptp(zs))
# ax.set_box_aspect((np.ptp(xs), np.ptp(ys), np.ptp(zs)))
# ax.set_xlim3d(-a,a)
# ax.set_ylim3d(-a,a)
# ax.set_zlim3d(-a,a)

toshow = list(range(10))+[-1]

VMIN = min(weights)
VMAX = max(weights)

# UNCOMMENT TO ADD DENDRITES
for k,v in points.items():
    if v.pid!=None:# and k in toshow:

        parent = points[v.pid]
        # if v.pid==-1:
        #     print(v.ID, v.lat, v.lon, v.rad, v.x, v.y, v.z)    
        # ax.plot([v.lon, parent.lon], [v.lat, parent.lat], 'gray')
        ax.plot([v.x,parent.x],[v.y,parent.y],[v.z,parent.z],'gray',zorder=1)


for k,v in points.items():
    if v.ID == -1:
        ax.scatter(v.x,v.y,v.z, color='black', cmap='bwr',linewidths=8,zorder=1)
    else:
        # ax.scatter(v.x,v.y,v.z, color=COMP_C[v.comp], cmap='bwr',linewidths=8,zorder=1,vmin=VMIN,vmax=VMAX)
        ax.scatter(v.x,v.y,v.z, c=v.weight, cmap='bwr',linewidths=8,zorder=1,vmin=VMIN,vmax=VMAX)
    label = str(v.ID)
    ax.text(v.x,v.y,v.z, '%s' % (label), size=8, zorder=4,horizontalalignment='center',verticalalignment='center')
# ax.scatter(xs_max, ys_max, zs_max, marker='o')

plt.xlabel("x")
plt.ylabel("y")
ax.set_zlabel("z")
plt.show()



pi2 = 2*math.pi
fig = plt.figure()
for k,v in points.items():
    # xcoord = v.lon
    xcoord = v.lat
    if v.ID==-1:
        plt.scatter(xcoord,v.rad, color='black', cmap='bwr',linewidths=12,zorder=3,marker='o',s=100)
    else:
        plt.scatter(xcoord,v.rad, color=COMP_C[v.comp], cmap='bwr',linewidths=12,zorder=3,marker='o',s=100,vmin=VMIN,vmax=VMAX)
        # plt.scatter(xcoord,v.rad, c=v.weight, cmap='bwr',linewidths=12,zorder=3,marker='o',s=100,vmin=VMIN,vmax=VMAX)
    label = str(v.ID)
    plt.text(xcoord,v.rad, '%s' % (label), size=8, zorder=4,horizontalalignment='center',verticalalignment='center')

    if v.pid!=None:
        parent = points[v.pid]
        parent_xcoord = parent.lat
        plt.plot([xcoord,parent_xcoord],[v.rad,parent.rad],color='gray',zorder=1)


plt.xlabel("Latitude")
plt.ylabel("Radius")
plt.show()


# fig = plt.figure()
# ax = fig.add_subplot(projection='3d')
# a = max(np.ptp(xs), np.ptp(ys), np.ptp(zs))
# ax.set_box_aspect((np.ptp(xs), np.ptp(ys), np.ptp(zs)))
# ax.set_xlim3d(-a,a)
# ax.set_ylim3d(-a,a)
# ax.set_zlim3d(-a,a)
# for k,v in points.items():
#     ax.scatter(v.x, v.y, v.z, color=colors[v.comp], cmap='jet')
#     label = str(v.ID)
#     #ax.text(v.x, v.y, v.z, '%s' % (label), size=10, zorder=1 )
# # ax.scatter(xs_max, ys_max, zs_max, marker='o')

# for k,v in points.items():
#     if v.pid!=None:

#         parent = points[v.pid]
#         # if v.pid==-1:
#         #     print(v.ID, v.lat, v.lon, v.rad, v.x, v.y, v.z)    
#         ax.plot3D([v.x, parent.x], [v.y, parent.y], [v.z, parent.z], 'gray')

# plt.title("Synaptic Locations and Dendritic Connections")
# plt.show()
