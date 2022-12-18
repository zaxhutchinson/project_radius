import matplotlib.pyplot as plt
import numpy as np
import math

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


# Each parameter is a tuple of (lat, lon, rad).
# Does not include the radius
def angular_dist(A,B):
    alon = B[1] - A[1]
    alat = B[0] - A[0]
    a = math.pow( math.sin(alat/2.0), 2.0 ) + \
        math.cos(A[0]) * math.cos(B[0]) * \
        math.pow( math.sin(alon/2.0), 2.0 )
    if a > 1:
        a = 1.0
    elif a < 0:
        a = 0.0

    ang_dist = 2.0 * math.atan2(math.sqrt(a), math.sqrt(1.0-a))
    return ang_dist