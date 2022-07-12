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

a1 = Conn(-3, 3, 1)
a2 = Conn(3, 3, 2)
a3 = Conn(-3, 3, 1)
a4 = Conn(3, 3, 2)
A = CompRELU_T(1,1,1,2,{'a1':a1, 'a2':a2, 'a3':a3, 'a4':a4},{})

b1 = Conn(-3, 0.1, 2)
b2 = Conn(3, 0.1, 2)
b3 = Conn(-3, 0.1, 2)
b4 = Conn(3, 0.1, 2)
B = CompRELU_T(1,1,1,7,{'b1':b1, 'b2':b2, 'b3':b3, 'b4':b4},{})

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
    a2.set_input(X[i])
    for j in range(len(Y)):
        b3.set_input(Y[j])
        b4.set_input(Y[j])
        a3.set_input(Y[j])
        a4.set_input(Y[j])
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


# fig.tight_layout()
grid = [None, None]
grid[0] = fig.add_subplot(121)
im0=grid[0].imshow(Aout)
grid[0].set_title("B (3 activation points)")
grid[0].set_xlabel("Input to L")
grid[0].set_ylabel("Input to R")
grid[0].set_xticks(range(0, len(Bout), len(Bout)//10))
grid[0].set_xticklabels(range(start, end, (end-start)//10))
grid[0].set_yticks(range(0, len(Bout), len(Bout)//10))
grid[0].set_yticklabels(range(start, end, (end-start)//10))
grid[0].invert_yaxis()
div0 = make_axes_locatable(grid[0])
cax = div0.append_axes('right', size='5%', pad=0.05)
fig.colorbar(im0, cax=cax, orientation='vertical')

grid[1] = fig.add_subplot(122, sharey=grid[0])
im1=grid[1].imshow(Bout)
grid[1].set_title("B (cluster activation)")
grid[1].set_xlabel("Input to L")
grid[1].set_xticks(np.arange(0, len(Bout), len(Bout)/10))
grid[1].set_xticklabels(np.arange(start, end, (end-start)/10))
grid[1].set_yticks(np.arange(0, len(Bout), len(Bout)/10))
grid[1].set_yticklabels(np.arange(start, end, (end-start)/10))
grid[1].invert_yaxis()
div1 = make_axes_locatable(grid[1])
cax = div1.append_axes('right', size='5%', pad=0.05)
fig.colorbar(im1, cax=cax, orientation='vertical')


# import matplotlib.pyplot as plt
# from mpl_toolkits.axes_grid1 import make_axes_locatable

# import numpy as np

# m1 = np.random.rand(3, 3)
# m2 = np.arange(0, 3*3, 1).reshape((3, 3))

# fig = plt.figure(figsize=(16, 12))
# ax1 = fig.add_subplot(121)
# im1 = ax1.imshow(m1, interpolation='None')

# divider = make_axes_locatable(ax1)
# cax = divider.append_axes('right', size='5%', pad=0.05)
# fig.colorbar(im1, cax=cax, orientation='vertical')

# ax2 = fig.add_subplot(122)
# im2 = ax2.imshow(m2, interpolation='None')

# divider = make_axes_locatable(ax2)
# cax = divider.append_axes('right', size='5%', pad=0.05)
# fig.colorbar(im2, cax=cax, orientation='vertical');


plt.show()
