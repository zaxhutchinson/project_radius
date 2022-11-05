# Demonstration of Importance, Relevance and Value

# Demonstrates complexity

import math
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.ticker import LinearLocator
from mpl_toolkits.axes_grid1 import make_axes_locatable
from mpl_toolkits.axes_grid1 import ImageGrid

import numpy as np
import copy
import random

class Conn:
    def __init__(self, c, b, w):
        self.c = c
        self.b = b
        self.w = w
        self.i = 0.0
    def set_input(self, i):
        self.i = i
    def phi(self):
        return self.w / (
            ((self.b*(self.i-self.c))**2.0) + 1.0
        )
class ConnSine:
    def __init__(self, c, b, w):
        self.c = c
        self.b = b
        self.w = w
        self.i = 0.0
    def set_input(self, i):
        self.i = i
    def phi(self):
        i = (self.w+1) / (
            (math.sin(self.b*(self.i-self.c))**2.0) + 1.0
        ) - 1
        return i

class ConnCosine:
    def __init__(self, c, b, w):
        self.c = c
        self.b = b
        self.w = w
        self.i = 0.0
    def set_input(self, i):
        self.i = i
    def phi(self):
        i = (self.w+2) / (
            ((self.b*math.cos(self.i-self.c))**2.0) + 1.0
        ) - 1
        return i

class CompRELU:
    def __init__(self,c,b,w,a_conns,e_conns):
        self.a_conns = a_conns
        self.e_conns = e_conns
        self.c = c
        self.b = b
        self.w = w
    def psi(self):
        I = 0.0
        for k,v in self.a_conns.items():
            I += v.phi()
        
        if I < 0:
            I = 0
        for k,v in self.e_conns.items():
            v.set_input(I)
        return I


class CompRELU_T:
    def __init__(self,c,b,w,t,a_conns,e_conns):
        self.a_conns = a_conns
        self.e_conns = e_conns
        self.c = c
        self.b = b
        self.w = w
        self.t = t
    def psi(self):
        I = -self.t
        for k,v in self.a_conns.items():
            I += v.phi()
        
        if I < 0:
            I = 0
        for k,v in self.e_conns.items():
            v.set_input(I)
        return I


class CompRBF:
    def __init__(self,c,b,w,a_conns,e_conns):
        self.a_conns = a_conns
        self.e_conns = e_conns
        self.c = c
        self.b = b
        self.w = w
    def psi(self):
        I = 0.0
        for k,v in self.a_conns.items():
            I += v.phi()
        
        I = self.w / (
            ((self.b*(I-self.c))**2.0) + 1.0
        )
        for k,v in self.e_conns.items():
            v.set_input(I)
        return I

class CompAUTO:
    def __init__(self,c,b,w,a_conns,e_conns):
        self.a_conns = a_conns
        self.e_conns = e_conns
        self.c = c
        self.b = b
        self.w = w
    def psi(self):
        I = 0.0
        for k,v in self.a_conns.items():
            I += v.phi()
        
        I = self.w / (
            ((self.b*(I-len(self.a_conns)))**2.0) + 1.0
        )
        for k,v in self.e_conns.items():
            v.set_input(I)
        return I

a1 = Conn(-5, 3, 1)
a2 = Conn(-1, 3, 1)
a3 = Conn(0, 0.2, 4)
a4 = Conn(2, 3, 1)
A = CompRELU_T(1,1,1,0,{'a1':a1, 'a2':a2, 'a3':a3, 'a4':a4},{})


b_in = {}
b_out = {}
for i in range(100):
    # if(random.randint(0,1)):
    b_in['b'+str(i)] = ConnSine(
        random.uniform(-math.pi,math.pi),
        random.uniform(0.1,100),
        random.uniform(0,1.01)
    )

    b_out = copy.deepcopy(b_in)
    # else:
    #     b_in['b'+str(i)] = ConnCosine(
    #         random.uniform(-10,10),
    #         random.uniform(0.1,10),
    #         random.uniform(1,2)
    #     )
# b1 = ConnSine(0, 0.5, 0.33)
# b2 = ConnSine(2, 2, 0.33)
# b3 = ConnSine(3, 4, 0.33)
# b4 = Conn(3, 0.1, 2)
B = CompRELU_T(1,1,1,0,b_in,b_out)

start = -10
end = 10
step = 0.01

X = np.arange(start,end,step)
# Y = np.arange(start,end,step)

Aout = np.zeros([len(X)])
Bout = np.zeros([len(X)])

Brec = np.zeros([len(b_out),len(X)])

for i in range(len(X)):
    for b in b_in.values():
        b.set_input(X[i])

    a1.set_input(X[i])
    a2.set_input(X[i])
    a3.set_input(X[i])
    a4.set_input(X[i])
    # for j in range(len(Y)):
    #     b3.set_input(Y[j])
    #     b4.set_input(Y[j])
        
    Aout[i] = A.psi()
    Bout[i] = B.psi()

    j = 0
    for b in b_out.values():
        Brec[j][i] = b.phi()
        j += 1


fig = plt.figure()
# grid = ImageGrid(
#     fig, 111,
#     nrows_ncols=(1,2),
#     axes_pad=0.15,
#     share_all=True,
#     cbar_location="right",
#     cbar_mode="single",
#     cbar_size="7%",
#     cbar_pad=0.15,
# )


# fig.tight_layout()
grid = [None, None]
grid[0] = fig.add_subplot(121)
grid[0].plot(Aout,color='black',linewidth=0.5)
grid[0].set_title("B (4 conns / single source)")
grid[0].set_xlabel("Signal (v_a)")
grid[0].set_ylabel("Compartment Input")
grid[0].set_xticks(np.arange(0, len(Bout), len(Bout)/10))
grid[0].set_xticklabels(np.arange(start, end, (end-start)/10))

grid[1] = fig.add_subplot(122)
grid[1].plot(Bout,color='black',linewidth=0.5)
grid[1].set_title("B (100 conns / single source)")
grid[1].set_xlabel("Signal (v_a)")
grid[1].set_ylabel("Compartment Input")
grid[1].set_xticks(np.arange(0, len(Bout), len(Bout)/10))
grid[1].set_xticklabels(np.arange(start, end, (end-start)/10))

plt.show()


fig = plt.figure()

for b in Brec:
    plt.plot(b)
plt.show()