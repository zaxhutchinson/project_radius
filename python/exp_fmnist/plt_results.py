import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1 import make_axes_locatable
import numpy as np

data = []

with open('Results') as f:
    for line in f:
        line = line.strip().split()
        row = []
        for e in line:
            row.append(float(e))
        data.append(row)

ax = plt.subplot()
plt.xlabel("Selected Category")
plt.ylabel("Actual Category")
ax.set_xticks([0,1,2,3,4,5,6,7,8,9])
ax.set_xticklabels([0,1,2,3,4,5,6,7,8,9])
ax.set_yticks([0,1,2,3,4,5,6,7,8,9])
ax.set_yticklabels([0,1,2,3,4,5,6,7,8,9])
im = ax.imshow(data,cmap='jet', aspect='auto', interpolation='nearest',origin='lower')
divider = make_axes_locatable(ax)
cax = divider.append_axes("right", size="5%", pad=0.05)
plt.colorbar(im, cax=cax)
plt.show()