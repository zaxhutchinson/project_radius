import math
import random
import matplotlib.pyplot as plt
import numpy as np

def RandPhi():
    return math.acos(
        random.random() * 2.0 - 1.0
    ) - math.pi/2.0



arr = []

for i in range(10000):
    arr.append(RandPhi())


plt.hist(arr)
plt.show()