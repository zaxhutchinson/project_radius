# Demonstration of Importance, Relevance and Value


import math
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.ticker import LinearLocator
from mpl_toolkits.axes_grid1 import make_axes_locatable
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

b1 = Conn(-3, 3, 1)
b2 = Conn(3, 3, 1)
b3 = Conn(-3, 3, 1)
b4 = Conn(3, 3, 1)
B = CompRELU(2,1,1,{'b1':b1, 'b2':b2, 'b3':b3, 'b4':b4},{})

a1 = Conn(5, 3, 1)
a2 = Conn(1, 0.5, 1)
A = CompRELU(2,1,1,{'a1':a1, 'a2':a2},{})

start = -10
end = 10
step = 0.01

X = np.arange(start,end,step)
Y = np.arange(start,end,step)

Aout = np.zeros([len(X),len(Y)])
Bout = np.zeros([len(X),len(Y)])



for i in range(len(X)):
    b1.set_input(X[i])
    b2.set_input(X[i])
    a1.set_input(X[i])
    for j in range(len(Y)):
        b3.set_input(Y[j])
        b4.set_input(Y[j])
        a2.set_input(Y[j])
        Aout[i][j] = A.psi()
        Bout[i][j] = B.psi()



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


grid = [None, None]
grid[0] = fig.add_subplot(121)
im0=grid[0].imshow(Aout)
grid[0].set_title("A")
grid[0].set_xlabel("Input to L")
grid[0].set_ylabel("Input to R")
grid[0].set_xticks(range(0, len(Bout), len(Bout)//10))
grid[0].set_xticklabels(range(start, end, (end-start)//10))
grid[0].set_yticks(range(0, len(Bout), len(Bout)//10))
grid[0].set_yticklabels(range(start, end, (end-start)//10))
# print(grid[0].get_xticks(), grid[0].get_xticklabels())
grid[0].invert_yaxis()
div0 = make_axes_locatable(grid[0])
cax = div0.append_axes('right', size='5%', pad=0.05)
fig.colorbar(im0, cax=cax, orientation='vertical')

grid[1] = fig.add_subplot(122, sharey=grid[0])
im1=grid[1].imshow(Bout)
grid[1].set_title("B")
grid[1].set_xlabel("Input to L")
grid[1].set_xticks(np.arange(0, len(Bout), len(Bout)/10))
grid[1].set_xticklabels(np.arange(start, end, (end-start)/10))
grid[1].set_yticks(np.arange(0, len(Bout), len(Bout)/10))
grid[1].set_yticklabels(np.arange(start, end, (end-start)/10))
grid[1].invert_yaxis()
div1 = make_axes_locatable(grid[1])
cax = div1.append_axes('right', size='5%', pad=0.05)
fig.colorbar(im1, cax=cax, orientation='vertical')





plt.show()
