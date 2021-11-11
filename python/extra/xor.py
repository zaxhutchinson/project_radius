import math
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Ellipse

WIDTH=50

fig = plt.figure()
ax = fig.add_subplot()

ax.scatter(0,0,edgecolor='black',s=WIDTH, facecolor='none')
ax.scatter(0,1,edgecolor='red',s=WIDTH, facecolor='none')
ax.scatter(1,0,edgecolor='red',s=WIDTH, facecolor='none')
ax.scatter(1,1,edgecolor='black',s=WIDTH, facecolor='none')


ellipse = Ellipse((0.5,0.5),2,0.5,angle=-45,facecolor='none',edgecolor='black', linestyle='--')
ax.add_patch(ellipse)

plt.xlabel("Input 1")
plt.ylabel("Input 2")
plt.title("XOR")

plt.show()