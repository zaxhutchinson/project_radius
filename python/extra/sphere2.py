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

p3_lat = -math.pi/8.0
p3_lon = math.pi/4.0

x1,y1,z1 = GetXYZ(p1_lat,p1_lon,rad)
x2,y2,z2 = GetXYZ(p2_lat,p2_lon,rad)
x3,y3,z3 = GetXYZ(p3_lat,p3_lon,rad)

slerp1 = []
slerp2 = []
slerp3 = []
path1 = []
path2 = []
path3 = []

# dist1 = AngularDistanceTo(p1_lat,p1_lon,p2_lat,p2_lon)
# step = dist1/100
# lat = p1_lat
# lon = p1_lon
# for i in range(100):
#     head = HeadingTo(lat,lon,p2_lat,p2_lon)
#     dlat,dlon = OrbitTo(lat,lon,rad,head,step)
#     slerp1.append((dlat,dlon))
#     lat = dlat
#     lon = dlon
# for e in slerp1:
#     path1.append(GetXYZ(e[0],e[1],rad))


# dist2 = AngularDistanceTo(p2_lat,p2_lon,p3_lat,p3_lon)
# step = dist2/100
# lat = p2_lat
# lon = p2_lon
# for i in range(100):
#     head = HeadingTo(lat,lon,p3_lat,p3_lon)
#     dlat,dlon = OrbitTo(lat,lon,rad,head,step)
#     slerp2.append((dlat,dlon))
#     lat = dlat
#     lon = dlon
# for e in slerp2:
#     path2.append(GetXYZ(e[0],e[1],rad))

# dist3 = AngularDistanceTo(p3_lat,p3_lon,p1_lat,p1_lon)
# step = dist3/100
# lat = p3_lat
# lon = p3_lon
# for i in range(100):
#     head = HeadingTo(lat,lon,p1_lat,p1_lon)
#     dlat,dlon = OrbitTo(lat,lon,rad,head,step)
#     slerp3.append((dlat,dlon))
#     lat = dlat
#     lon = dlon
# for e in slerp3:
#     path3.append(GetXYZ(e[0],e[1],rad))


s1_head = HeadingTo(p1_lat,p1_lon,p3_lat,p3_lon)
s1_lat,s1_lon = OrbitTo(p1_lat,p1_lon,rad,s1_head,0.5)
s2_head = HeadingTo(s1_lat,s1_lon,p2_lat,p2_lon)
s2_lat,s2_lon = OrbitTo(s1_lat,s1_lon,rad,s2_head,0.3)

s1x,s1y,s1z = GetXYZ(s1_lat,s1_lon,rad)
s2x,s2y,s2z = GetXYZ(s2_lat,s2_lon,rad)

s3_head = HeadingTo(p1_lat,p1_lon,p2_lat,p2_lon)
s3_lat,s3_lon = OrbitTo(p1_lat,p1_lon,rad,s3_head,0.3)
s4_head = HeadingTo(s3_lat,s3_lon,p3_lat,p3_lon)
s4_lat,s4_lon = OrbitTo(s3_lat,s3_lon,rad,s4_head,0.5)

s1x,s1y,s1z = GetXYZ(s1_lat,s1_lon,rad)
s2x,s2y,s2z = GetXYZ(s2_lat,s2_lon,rad)
s3x,s3y,s3z = GetXYZ(s3_lat,s3_lon,rad)
s4x,s4y,s4z = GetXYZ(s4_lat,s4_lon,rad)


dist1 = AngularDistanceTo(p1_lat,p1_lon,s1_lat,s1_lon)
step = dist1/100
lat = p1_lat
lon = p1_lon
for i in range(100):
    head = HeadingTo(lat,lon,s1_lat,s1_lon)
    dlat,dlon = OrbitTo(lat,lon,rad,head,step)
    slerp1.append((dlat,dlon))
    lat = dlat
    lon = dlon
for e in slerp1:
    path1.append(GetXYZ(e[0],e[1],rad))

dist2 = AngularDistanceTo(s1_lat,s1_lon,s2_lat,s2_lon)
step = dist2/100
lat = s1_lat
lon = s1_lon
for i in range(100):
    head = HeadingTo(lat,lon,s2_lat,s2_lon)
    dlat,dlon = OrbitTo(lat,lon,rad,head,step)
    slerp2.append((dlat,dlon))
    lat = dlat
    lon = dlon
for e in slerp2:
    path2.append(GetXYZ(e[0],e[1],rad))


fig = plt.figure()
ax = fig.add_subplot(projection='3d')


u, v = np.mgrid[0:2.0*np.pi:60j, 0:np.pi:60j]
x = np.cos(u)*np.sin(v)
y = np.sin(u)*np.sin(v)
z = np.cos(v)
ax.plot_surface(x, y, z, cmap="gist_gray",alpha=0.2,zorder=0)

ax.plot([0,x1],[0,y1],[0,z1],c='gray',zorder=1)
ax.plot([0,x2],[0,y2],[0,z2],c='gray',zorder=1)
ax.plot([0,x3],[0,y3],[0,z3],c='gray',zorder=1)
ax.plot([0,s1x],[0,s1y],[0,s1z],c='gray',zorder=1,linewidth=0.5)
ax.plot([0,s2x],[0,s2y],[0,s2z],c='gray',zorder=1,linewidth=0.5)

ax.scatter(x1, y1, z1,c='red',linewidths=1,zorder=3)
ax.scatter(x2, y2, z2,c='red',linewidths=1,zorder=3)
ax.scatter(x3, y3, z3,c='red',linewidths=1,zorder=3)
ax.scatter(s1x, s1y, s1z,c='blue',linewidths=1,zorder=3)
ax.scatter(s2x, s2y, s2z,c='blue',linewidths=1,zorder=3)
ax.scatter(s3x, s3y, s3z,c='blue',linewidths=1,zorder=3)
ax.scatter(s4x, s4y, s4z,c='blue',linewidths=1,zorder=3)
ax.scatter(0,0,0,c='black',linewidths=1,zorder=3)

ax.text(x1+0.2,y1-0.2,z1+0.0,'A',fontsize=12,zorder=3)
ax.text(x2+0.05,y2+0.05,z2+0.05,'B',fontsize=12,zorder=3)
ax.text(x3+0.05,y3+0.05,z3-0.08,'C',fontsize=12,zorder=3)
ax.text(0.0,0.0,-0.2,'O',fontsize=12)
ax.text(s1x+0.2,s1y-0.1,s1z+0.0,'1',fontsize=10,zorder=3)
ax.text(s2x+0.05,s2y+0.05,s2z+0.05,'2',fontsize=10,zorder=3)



# ax.text(x1/2,y1/2,z1/2,'a',fontsize=12)
# ax.text(x2/2,y2/2,z2/2+0.07,'b',fontsize=12)

for i in range(len(path1)-1):
    ax.plot(
        [path1[i][0],path1[i+1][0]],
        [path1[i][1],path1[i+1][1]],
        [path1[i][2],path1[i+1][2]],
        linestyle='dotted',
        linewidth='0.5',
        c='blue'
    )

# mid = len(path1)//2
# ax.text(
#     path1[mid][0]+0.05, path1[mid][1]+0.05, path1[mid][2]+0.05, 'c',fontsize=12
# )

for i in range(len(path2)-1):
    ax.plot(
        [path2[i][0],path2[i+1][0]],
        [path2[i][1],path2[i+1][1]],
        [path2[i][2],path2[i+1][2]],
        linestyle='dotted',
        linewidth='0.5',
        c='blue'
    )

# mid = len(path2)//2
# ax.text(
#     path2[mid][0]+0.05, path2[mid][1]+0.05, path2[mid][2]+0.05, 'a',fontsize=12
# )

# for i in range(len(path3)-1):
#     ax.plot(
#         [path3[i][0],path3[i+1][0]],
#         [path3[i][1],path3[i+1][1]],
#         [path3[i][2],path3[i+1][2]],
#         linestyle='dotted',
#         linewidth='0.5'
#     )

ax.set_xlabel('x')
ax.set_ylabel('y')
ax.set_zlabel('z')
ax.set_box_aspect([1,1,1])
set_axes_equal(ax)
ax.axis('off')
plt.show()
