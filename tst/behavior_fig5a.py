# Demonstration of Importance, Relevance and Value

# Demonstrates multiple compartments

import math
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.ticker import LinearLocator
from mpl_toolkits.axes_grid1 import make_axes_locatable
from mpl_toolkits.axes_grid1 import ImageGrid

import numpy as np
import copy
import random

# class Conn:
#     def __init__(self, c, b, w, vt):
#         self.c = c*vt
#         self.b = b
#         self.w = w
#         self.vt = vt
#         self.i = 0.0

#     def set_input(self, i):
#         self.i = i*self.vt
#     def phi(self):
#         return (self.w) / (
#             ((self.b*(self.i-self.c))**2.0) + 1.0
#         )
class Conn:
    def __init__(self, c, b, w):
        self.c = c
        self.b = b
        self.w = w
        self.i = 0.0

    def set_input(self, i):

        self.i = i

    def phi(self):
        return (self.w) / (
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

# Version 1: b=1
a1 = Conn(1, 1, 1)
b1 = Conn(1, 1, 1)
c1 = Conn(1, 1, 1)

# Version 2: wide-to-narrow. Produces a wide plateau of activation.
# a1 = Conn(1, 0.1, 1,10)
# b1 = Conn(1, 0.5, 1,10)
# c1 = Conn(1, 1, 1,10)

# A = CompRELU_T(1,1,1,0,{'a1':a1},{'b1':b1})
# B = CompRELU_T(1,1,1,0,{'b1':b1},{'c1':c1})
# C = CompRELU_T(1,1,1,0,{'c1':c1},{})

A = CompRBF(1,10,1,{'a1':a1},{'b1':b1})
B = CompRBF(1,10,1,{'b1':b1},{'c1':c1})
C = CompRBF(1,10,1,{'c1':c1},{})

X = np.arange(0,2,0.01)
# Y = np.arange(start,end,step)

A_out = []
B_out = []
C_out = []

for i in range(len(X)):

    a1.set_input(X[i])

    A_out.append(A.psi())
    B_out.append(B.psi())
    C_out.append(C.psi())

    

fig = plt.figure()

plt.plot(X,A_out,label="A")
plt.plot(X,B_out,label="B")
plt.plot(X,C_out,label="C")
plt.legend()
plt.xlabel("Input to A")
plt.ylabel("Output")

plt.show()
















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
# grid = [None, None]
# grid[0] = fig.add_subplot(121)
# grid[0].plot(Aout,color='black',linewidth=0.5)
# grid[0].set_title("B (4 conns / single source)")
# grid[0].set_xlabel("Signal (x_a)")
# grid[0].set_ylabel("Compartment Input")
# grid[0].set_xticks(np.arange(0, len(Bout), len(Bout)/10))
# grid[0].set_xticklabels(np.arange(start, end, (end-start)/10))

# grid[1] = fig.add_subplot(122)
# grid[1].plot(Bout,color='black',linewidth=0.5)
# grid[1].set_title("B (100 conns / single source)")
# grid[1].set_xlabel("Signal (x_a)")
# grid[1].set_ylabel("Compartment Input")
# grid[1].set_xticks(np.arange(0, len(Bout), len(Bout)/10))
# grid[1].set_xticklabels(np.arange(start, end, (end-start)/10))

# plt.show()


# fig = plt.figure()

# for b in Brec:
#     plt.plot(b)
# plt.show()