import math
import random
import numpy as np
import matplotlib.pyplot as plt

on_rate = 0.1
off_rate = 0.01
mask_rate = 0.1

p1 = np.zeros([100,1000])
p2 = np.zeros([100,1000])
p3 = np.zeros([100,1000])


for i in range(1000):
    for n in range(100):
        if n < 55:
            if random.random() < on_rate:
                p1[n][i] = 1
        else:
            if random.random() < off_rate:
                p1[n][i] = 1

        if n >= 45:
            if random.random() < on_rate:
                p2[n][i] = 1
        else:
            if random.random() < off_rate:
                p2[n][i] = 1

        if random.random() < mask_rate:
            p3[n][i] = 1


fig,axs = plt.subplots(3)
axs[0].imshow(p1, cmap='binary', aspect='auto', interpolation='nearest')
axs[0].set_title("Pattern 1")
axs[1].imshow(p2, cmap='binary', aspect='auto', interpolation='nearest')
axs[1].set_title("Pattern 2")
axs[2].imshow(p3, cmap='binary', aspect='auto', interpolation='nearest')
axs[2].set_title("Pattern 3")
plt.show()