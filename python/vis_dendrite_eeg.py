import math
import random

import matplotlib.pyplot as plt
import matplotlib.colors as clr
import numpy as np

import output
import defs

NEURON_ID = 1
LAYER_ID = 1
SIDS = list(range(64))

out = output.Output(defs.OUTPUT_PATH, lid=LAYER_ID, nid=NEURON_ID, sid=SIDS)

labels = [
    'FP1','FP2','F7','F8','AF1','AF2','FZ','F4',
    'F3','FC6','FC5','FC2','FC1','T8','T7','CZ',
    'C3','C4','CP5','CP6','CP1','CP2','P3','P4',
    'PZ','P8','P7','PO2','PO1','O2','O1','X',
    'AF7','AF8','F5','F6','FT7','FT8','FPZ','FC4',
    'FC3','C6','C5','F2','F1','TP8','TP7','AFZ',
    'CP3','CP4','P5','P6','C1','C2','PO7','PO8',
    'FCZ','POZ','OZ','P2','P1','CPZ','nd','Y'
]


xs = []
ys = []
zs = []
ss = []

comps = []
colors = {}

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
    strength = 0.0


soma = Point()
soma.ID = -1
soma.comp = -1
points = {-1:soma}
colors[soma.comp]=(0.0,0.0,0.0)

for name,syn in out.synapses.items():
    if syn.neuron_id == NEURON_ID and syn.layer_id == LAYER_ID and syn.synapse_id in SIDS:
        lat = syn.lats[-1][-1]
        lon = syn.lons[-1][-1]
        rad = syn.rads[-1][-1]
        pid = syn.parents[-1][-1]
        comp = syn.compartments[-1][-1]
        strength = syn.strengths[-1][-1]
        if comp==None:
            print(pid,comp)

        if comp not in comps:
            comps.append(comp)
            colors[comp]=(random.uniform(0.2,0.8),random.uniform(0.2,0.8),random.uniform(0.2,0.8))

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
        p.comp = comp
        p.strength = strength
        #print(labels[p.ID], strength)
        points[p.ID] = p

        xs.append(x)
        ys.append(y)
        zs.append(z)
        ss.append(strength)

minstr = min(ss)
maxstr = max(ss)
norm = clr.Normalize(vmin=minstr,vmax=maxstr)

ss = [norm(x) for x in ss]

fig = plt.figure()
ax = fig.add_subplot(projection='3d')
a = max(np.ptp(xs), np.ptp(ys), np.ptp(zs))
ax.set_box_aspect((np.ptp(xs), np.ptp(ys), np.ptp(zs)))
ax.set_xlim3d(-a,a)
ax.set_ylim3d(-a,a)
ax.set_zlim3d(-a,a)
for k,v in points.items():
    # ax.scatter(v.x, v.y, v.z, color=colors[v.comp], cmap='jet')
    label = labels[v.ID]
    ax.text(v.x, v.y, v.z, '%s' % (label), size=10, zorder=1 )

ax.scatter(xs, ys,zs, c=ss, cmap='RdBu')

for k,v in points.items():
    if v.pid!=None:

        parent = points[v.pid]
        # if v.pid==-1:
        #     print(v.ID, v.lat, v.lon, v.rad, v.x, v.y, v.z)    
        ax.plot3D([v.x, parent.x], [v.y, parent.y], [v.z, parent.z], 'gray')

plt.title("Synaptic Locations and Dendritic Connections")
plt.show()

F = 'dodgerblue'
FC = 'lightskyblue'
T = 'springgreen'
C = 'slategray'
CP = 'lemonchiffon'
P = 'yellow'
PO = 'orange'
FT = 'turquoise'
AF = F
FP = F
TP = P
O = 'red'
X = 'black'

colors = {
    'FP1':F,'FP2':F,
    'F7':F,'F8':F,
    'AF1':F,'AF2':F,
    'FZ':F,'F4':F,
    'F3':F,
    'FC6':FC,'FC5':FC,
    'FC2':FC,'FC1':FC,
    'T8':T,'T7':T,
    'CZ':C,
    'C3':C,'C4':C,
    'CP5':CP,'CP6':CP,'CP1':CP,'CP2':CP,'P3':P,'P4':P,
    'PZ':P,'P8':P,'P7':P,'PO2':PO,'PO1':PO,'O2':O,'O1':O,'X':X,
    'AF7':AF,'AF8':AF,'F5':F,'F6':F,'FT7':FT,'FT8':FT,'FPZ':FP,'FC4':FC,
    'FC3':FC,'C6':C,'C5':C,'F2':F,'F1':F,'TP8':TP,'TP7':TP,'AFZ':AF,
    'CP3':CP,'CP4':CP,'P5':P,'P6':P,'C1':C,'C2':C,'PO7':PO,'PO8':PO,
    'FCZ':FC,'POZ':PO,'OZ':O,'P2':P,'P1':P,'CPZ':CP,'nd':X,'Y':X
}

fig = plt.figure()
ax = fig.add_subplot()
for k,v in points.items():
    if k==-1:
        continue
    else:
        label = labels[v.ID]
        ax.scatter(v.lon,v.lat,c=colors[label])
        
        ax.text(v.lon,v.lat, '%s' % (label), size=10, zorder=1 )
plt.show()