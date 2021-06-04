import numpy
import matplotlib.pyplot as plt
import sys
from mpl_toolkits import mplot3d


Wa = []
Wc = []

w_max = 10.0
w_cur = -100.0
for i in range(0,20000):
    Wc.append(w_cur)
    w_cur+=0.01


for x in Wc:
    y = x*w_max/(abs(x)+abs(w_max))
    Wa.append(y)



plt.plot(Wc,Wa)
plt.show()