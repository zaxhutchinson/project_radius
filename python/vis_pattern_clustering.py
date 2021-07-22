import math
import copy

import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1 import make_axes_locatable

import numpy as np
from numpy.linalg import matrix_power

import output
import defs


##############################################################################
# Slummed from Don Hatch's SO post:
# https://stackoverflow.com/a/38201499/13413114
# Changed xrange to range.

def plus(a,b): return [x+y for x,y in zip(a,b)]
def minus(a,b): return [x-y for x,y in zip(a,b)]
def cross(a,b): return [a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0]]
def dot(a,b): return sum([x*y for x,y in zip(a,b)])
def length(v): return math.sqrt(dot(v,v))
def normalized(v): l = length(v); return [1,0,0] if l==0 else [x/l for x in v]
def addVectorTimesScalar(accumulator, vector, scalar):
    for i in range(len(accumulator)): accumulator[i] += vector[i] * scalar
def angleBetweenUnitVectors(a,b):
    # http://www.plunk.org/~hatch/rightway.php
    if dot(a,b) < 0:
        
        return math.pi - 2*math.asin(length(plus(a,b))/2.)
    else:
        return 2*math.asin(length(minus(a,b))/2.)

def sphericalPolygonMoment(verts):
    moment = [0.,0.,0.]
    for i in range(len(verts)):
        a = verts[i]
        b = verts[(i+1)%len(verts)]
        addVectorTimesScalar(moment, normalized(cross(a,b)),
                                     angleBetweenUnitVectors(a,b) / 2.)
    return moment
###############################################################################


NEURON_ID = 0
LAYER_ID = 1


patterns = [
    [0,1,2,3,4,5,6,7,8,9],
    [10,11,12,13,14,15,16,17,18,19],
    [20,21,22,23,24,25,26,27,28,29],
    [30,31,32,33,34,35,36,37,38,39]
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


# soma = Point()
# soma.ID = -1
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



########################################################################
# Bar graph of the straight-line distance between a synapse and its
# pattern's centroid.
syn_ids = []
pattern_centroids = [[0,0,0]]*len(patterns)
syn_to_cent_distances = []
color_list = ['blue','red','purple','green']
colors = []

for i in range(len(patterns)):
    pat = patterns[i]
    loc = [0.0,0.0,0.0]
    for sid in pat:
        point = points[sid]
        loc[0] += point.x
        loc[1] += point.y
        loc[2] += point.z
    loc[0] /= len(pat)
    loc[1] /= len(pat)
    loc[2] /= len(pat)
    pattern_centroids[i] = loc

    for sid in pat:
        syn_ids.append(sid)
        point = points[sid]
        dist = math.sqrt(
            (point.x-loc[0])**2.0 +
            (point.y-loc[1])**2.0 +
            (point.z-loc[2])**2.0
        )
        syn_to_cent_distances.append(dist)
        colors.append(color_list[i])


plt.title("Synapse S-L Distance to Pattern Centroid")
plt.xlabel("Synapse ID")
plt.ylabel("Distance to Centroid")
plt.bar(syn_ids,syn_to_cent_distances,color=colors)
plt.show()

########################################################################
