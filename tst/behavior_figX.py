# Demonstration of Importance, Relevance and Value


import math
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.ticker import LinearLocator

from mpl_toolkits.axes_grid1 import ImageGrid

import numpy as np
import copy


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

a1 = Conn(1, 2, 1)
a2 = Conn(3, 0.5, 1)
a3 = Conn(-1, 2, 1)
A = CompRBF(2,2,1,{'a1':a1, 'a2':a2, 'a3':a3},{})

d1 = Conn(1,5,1)
d2 = Conn(1,5,1)

b1 = Conn(3, 5, 1)
b2 = Conn(3, 5, 1)
B = CompRBF(2,2,1,{'b1':b1, 'b2':b2},{'d1':d1})

c1 = Conn(-3, 5, 1)
c2 = Conn(-3, 5, 1)
C = CompRBF(2,2,1,{'c1':c1, 'c2':c2},{'d2':d2})


D = CompRBF(1,5,1,{'d1':d1, 'd2':d2},{})

start = -10
end = 10

X = np.arange(start,end+0.01,0.01)
Y = np.arange(start,end+0.01,0.01)

Aout = np.zeros([len(X),len(Y)])
Dout = np.zeros([len(X),len(Y)])

for i in range(len(X)):
    a1.set_input(X[i])
    a3.set_input(X[i])
    b1.set_input(X[i])
    c1.set_input(X[i])
    for j in range(len(Y)):
        a2.set_input(Y[j])
        b2.set_input(Y[j])
        c2.set_input(Y[j])
        Aout[i][j] = A.psi()
        B.psi()
        C.psi()
        Dout[i][j] = D.psi()



fig = plt.figure()
grid = ImageGrid(
    fig, 111,
    nrows_ncols=(1,2),
    axes_pad=0.15,
    share_all=True,
    cbar_location="right",
    cbar_mode="single",
    cbar_size="7%",
    cbar_pad=0.15,
)




# fig, ax = plt.subplots(1,2,sharex=True,sharey=True)
im=grid[0].imshow(Aout,vmin=0,vmax=1)
grid[0].set_xlabel("Input to L")
grid[0].set_ylabel("Input to R")
grid[0].set_xticks(np.arange(0, len(Aout), len(Aout)/10))
grid[0].set_xticklabels(np.arange(start, end, (end-start)/10))
grid[0].set_yticks(np.arange(0, len(Aout), len(Aout)/10))
grid[0].set_yticklabels(np.arange(start, end, (end-start)/10))
grid[0].invert_yaxis()

grid[1].imshow(Dout,vmin=0,vmax=1)
grid[1].set_xlabel("Input to L")
grid[1].set_ylabel("Input to R")
grid[1].set_xticks(np.arange(0, len(Dout), len(Dout)/10))
grid[1].set_xticklabels(np.arange(start, end, (end-start)/10))
grid[1].set_yticks(np.arange(0, len(Dout), len(Dout)/10))
grid[1].set_yticklabels(np.arange(start, end, (end-start)/10))
grid[1].invert_yaxis()

grid[-1].cax.colorbar(im)





plt.show()
