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

SIZE = 40

pattern0=list(range(10))
# pattern1=[
#     33,48,20,34,49,6,28,14,42,36,8,22,29,44,
#     16,37,30,2,45,31,38,11,18,25,39,47,5,26,40
# ]
# pattern0=[]
# for i in range(50):
#     if i not in pattern1:
#         pattern0.append(i)

for i in range(SIZE):
    out.LoadSynapse(LID,NID,i,last=False)

out.LoadNeuron(LID,NID)
neu = out.neurons[f'{LID} {NID}']
nlat = neu.lats[-1][-1]
nlon = neu.lons[-1][-1]
spikes = neu.spikes
SPIKES = []
for s in spikes:
    SPIKES.append(len(s))

LATS = {}
LONS = {}
RADS = {}
NUM_DENDRITES = []
xs = {}
ys = {}
zs = {}
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


soma = Point()
soma.lat = nlat
soma.lon = nlon
soma.ID = -1
soma.comp = -1
points = {-1:soma}
colors[soma.comp]=(0.0,0.0,0.0)

def dist(A,B):
    return math.sqrt(
        (A[0]-B[0])**2 +
        (A[1]-B[1])**2 +
        (A[2]-B[2])**2
    )


for i in range(SIZE):

    LATS[i] = []
    LONS[i] = []
    RADS[i] = []

    syn = out.synapses[f'{LID} {NID} {i}']
    
    lats = syn.lats
    lons = syn.lons
    rads = syn.rads
    parents = syn.parents
    num_dends = 0
    if len(NUM_DENDRITES)==0:
        NUM_DENDRITES = [0]*len(lats)
    for k in range(len(lats)):
        lat = lats[k][-1]
        lon = lons[k][-1]
        rad = rads[k][-1]
        # xk = rad * math.cos(lat) * math.cos(lon)
        # yk = rad * math.cos(lat) * math.sin(lon)
        # zk = rad * math.sin(lat)
        LATS[i].append(lat)
        LONS[i].append(lon)
        RADS[i].append(rad)

        if parents[k][-1]==-1:
            NUM_DENDRITES[k]+=1

avgdist = []
stddev = []
circ = 10

for i in range(len(LATS[pattern0[0]])):
    D = 0.0
    dists = []
    for k in range(len(pattern0)):
        for m in range(k,len(pattern0)):
            p1=pattern0[k]
            p2=pattern0[m]
            if p1==p2:
                continue
            A = (LATS[p1][i], LONS[p1][i], RADS[p1][i])
            B = (LATS[p2][i], LONS[p2][i], RADS[p2][i])
            d =  helpers.angular_dist(A,B)
            D += d
            dists.append(d)
    D /= (len(pattern0)*(len(pattern0)-1))/2
    stddev.append(np.std(dists))
    avgdist.append(D)
stdev_avgdist = np.std(avgdist[2:])
print("SD of avg:",stdev_avgdist)
print("AVG of avg: ",sum(avgdist)/len(avgdist))
print("FINAL dist: ",avgdist[-1])

fig,ax1 = plt.subplots()
fig.subplots_adjust(right=0.75)

ax2 = ax1.twinx()

p1=ax2.plot(avgdist,color='black',label="Avg. Ang. Dist.",zorder=1)
ax1.set_xlabel("Epoch")
ax2.set_ylabel("Avg. Angular Distance")
ax2.tick_params(axis='y', labelcolor='black')



p2=ax1.plot(SPIKES,color='tab:orange',label="Spike Rate",zorder=3)
ax1.tick_params(axis='y', labelcolor='tab:orange')
ax1.set_ylabel("Spike Rate")


ax3 = ax1.twinx()
# Offset the right spine of par2.  The ticks and label have already been
# placed on the right by twinx above.
ax3.spines["right"].set_position(("axes", 1.1))
# Having been created by twinx, par2 has its frame off, so the line of its
# detached spine is invisible.  First, activate the frame but make the patch
# and spines invisible.
def make_patch_spines_invisible(ax):
    ax.set_frame_on(True)
    ax.patch.set_visible(False)
    for sp in ax.spines.values():
        sp.set_visible(False)

make_patch_spines_invisible(ax3)
# Second, show the right spine.
ax3.spines["right"].set_visible(True)
p3=ax3.plot(NUM_DENDRITES,color='tab:blue',label="Dendritic Trees",zorder=2)
ax3.set_ylabel("Dendritic Trees")
ax3.tick_params(axis='y', labelcolor='tab:blue')

ax1.yaxis.label.set_color('tab:orange')
ax2.yaxis.label.set_color('black')
ax3.yaxis.label.set_color('tab:blue')

# plt.legend()
plt.show()

plt.figure()
plt.plot(stddev,color='black')
plt.xlabel("Epoch")
plt.ylabel("SD of Angular Distances")
plt.show() 

    # x = rad* math.cos(lat) * math.cos(lon)
    # y = rad * math.cos(lat) * math.sin(lon)
    # z = rad *math.sin(lat)
    # c = rad

    # p = Point()
    # p.ID = syn.synapse_id
    # p.pid = syn.parents[-1][-1]
    # p.x = x
    # p.y = y
    # p.z = z
    # p.rad = rad
    # p.lat = lat
    # p.lon = lon
    # p.comp = syn.compartments[-1][-1]
    # points[p.ID] = p

    # if p.comp not in comps:
    #         comps.append(p.comp)
    #         #mycolors[p.comp]=(random.uniform(0.2,0.8),random.uniform(0.2,0.8),random.uniform(0.2,0.8))

    # xs.append(x)
    # ys.append(y)
    # zs.append(z)

# C = {
#     0:'gray',
#     1:'tab:blue',
#     2:'tab:blue',
#     3:'tab:blue',
#     4:'tab:blue',
#     5:'tab:blue',
#     6:'tab:orange',
#     7:'tab:orange',
#     8:'tab:orange',
#     9:'tab:orange',
#     10:'tab:orange',
#     11:'tab:green',
#     12:'tab:green',
#     13:'tab:green',
#     14:'tab:green',
#     15:'tab:green',
#     16:'tab:red',
#     17:'tab:red',
#     18:'tab:red',
#     19:'tab:red',
#     20:'tab:red',
#     21:'tab:purple',
#     22:'tab:purple',
#     23:'tab:purple',
#     24:'tab:purple',
# }

# fig = plt.figure()
# ax = fig.add_subplot(projection='3d',computed_zorder=False)
# a = max(np.ptp(xs), np.ptp(ys), np.ptp(zs))
# ax.set_box_aspect((np.ptp(xs), np.ptp(ys), np.ptp(zs)))
# ax.set_xlim3d(-a,a)
# ax.set_ylim3d(-a,a)
# ax.set_zlim3d(-a,a)
# for k,v in points.items():
#     if k==-1:
#         continue
#     # ax.scatter(v.lon,v.lat, color=colors[v.comp], cmap='jet',linewidths=5)
#     ax.scatter(v.x,v.y,v.z, color=C[k], cmap='jet',linewidths=8,zorder=1)
#     label = str(v.ID)
#     ax.text(v.x,v.y,v.z, '%s' % (label), size=8, zorder=4,horizontalalignment='center',verticalalignment='center')
# # ax.scatter(xs_max, ys_max, zs_max, marker='o')

# # UNCOMMENT TO ADD DENDRITES
# # for k,v in points.items():
# #     if v.pid!=None:

# #         parent = points[v.pid]
# #         # if v.pid==-1:
# #         #     print(v.ID, v.lat, v.lon, v.rad, v.x, v.y, v.z)    
# #         ax.plot([v.lon, parent.lon], [v.lat, parent.lat], 'gray')

# plt.xlabel("x")
# plt.ylabel("y")
# ax.set_zlabel("z")
# plt.show()
# # fig = plt.figure()
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
