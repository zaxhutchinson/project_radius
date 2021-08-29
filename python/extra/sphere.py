import math
import numpy as np
import matplotlib.pyplot as plt
import random


def set_axes_equal(ax: plt.Axes):
    """Set 3D plot axes to equal scale.

    Make axes of 3D plot have equal scale so that spheres appear as
    spheres and cubes as cubes.  Required since `ax.axis('equal')`
    and `ax.set_aspect('equal')` don't work on 3D.
    """
    limits = np.array([
        ax.get_xlim3d(),
        ax.get_ylim3d(),
        ax.get_zlim3d(),
    ])
    origin = np.mean(limits, axis=1)
    radius = 0.5 * np.max(np.abs(limits[:, 1] - limits[:, 0]))
    _set_axes_radius(ax, origin, radius)

def _set_axes_radius(ax, origin, radius):
    x, y, z = origin
    ax.set_xlim3d([x - radius, x + radius])
    ax.set_ylim3d([y - radius, y + radius])
    ax.set_zlim3d([z - radius, z + radius])

def GetXYZ(lat, lon, rad):
    x=rad * math.cos(lat) * math.cos(lon)
    y=rad * math.cos(lat) * math.sin(lon)
    z=rad * math.sin(lat)
    return x,y,z

def HeadingTo(latA, lonA, latB, lonB):
    dlon = lonB-lonA
    return math.atan2(
        math.sin(dlon) * math.cos(latB),
        math.cos(latA) * math.sin(latB) - math.sin(latA) * math.cos(latB) * math.cos(dlon)
    )

def OrbitTo(lat, lon, rad, head, dist):
    delta = dist / rad
    nlat = math.asin(
        math.sin(lat) * math.cos(delta) + math.cos(lat) * math.sin(delta) * math.cos(head)
    )
    nlon = lon + math.atan2(
        math.sin(head) * math.sin(delta) * math.cos(lat),
        math.cos(delta) - math.sin(lat) * math.sin(nlat)
    )
    return nlat,nlon

def AngularDistanceTo(latA, lonA, latB, lonB):
    alon = lonB - lonA
    alat = latB - latA
    a = math.pow( math.sin(alat/2.0), 2.0) + \
        math.cos(latA) * math.cos(latB) * \
        math.pow(math.sin(alon/2.0),2.0)
    if a>1: a = 1
    if a<0: a = 0
    return 2.0 * math.atan2(math.sqrt(a), math.sqrt(1-a))

p1_lat = 0.0
p1_lon = 0.0
rad = 1.0

p2_lat = math.pi/4.0
p2_lon = math.pi/4.0


x1,y1,z1 = GetXYZ(p1_lat,p1_lon,rad)
x2,y2,z2 = GetXYZ(p2_lat,p2_lon,rad)

slerp = []
path = []

dist = AngularDistanceTo(p1_lat,p1_lon,p2_lat,p2_lon)
step = dist/100

lat = p1_lat
lon = p1_lon
for i in range(100):
    head = HeadingTo(lat,lon,p2_lat,p2_lon)
    dlat,dlon = OrbitTo(lat,lon,rad,head,step)
    slerp.append((dlat,dlon))
    lat = dlat
    lon = dlon

for e in slerp:
    path.append(GetXYZ(e[0],e[1],rad))


fig = plt.figure()
ax = fig.add_subplot(projection='3d')


u, v = np.mgrid[0:2.0*np.pi:60j, 0:np.pi:60j]
x = np.cos(u)*np.sin(v)
y = np.sin(u)*np.sin(v)
z = np.cos(v)
ax.plot_surface(x, y, z, cmap="gist_gray",alpha=0.5,zorder=0)

ax.plot([0,x1],[0,y1],[0,z1],linestyle='--',c='gray',zorder=3)
ax.plot([0,x2],[0,y2],[0,z2],linestyle='--',c='gray',zorder=3)

ax.scatter(x1, y1, z1,c='blue',linewidths=2,zorder=3)
ax.scatter(x2, y2, z2,c='red',linewidths=2,zorder=3)
ax.scatter(0,0,0,c='black',linewidths=2,zorder=3)

ax.text(x1+0.05,y1+0.05,z1+0.05,'A',fontsize=12,zorder=3)
ax.text(x2+0.05,y2+0.05,z2+0.05,'B',fontsize=12,zorder=3)
ax.text(-0.05,-0.05,0.1,'C',fontsize=12)



ax.text(x1/2,y1/2,z1/2,'a',fontsize=12)
ax.text(x2/2,y2/2,z2/2+0.07,'b',fontsize=12)

for i in range(len(path)-1):
    ax.plot(
        [path[i][0],path[i+1][0]],
        [path[i][1],path[i+1][1]],
        [path[i][2],path[i+1][2]],
    )

mid = len(path)//2
ax.text(
    path[mid][0]+0.05, path[mid][1]+0.05, path[mid][2]+0.05, 'd',fontsize=12
)

ax.set_xlabel('x')
ax.set_ylabel('y')
ax.set_zlabel('z')
ax.set_box_aspect([1,1,1])
set_axes_equal(ax)
ax.axis('off')
plt.show()
