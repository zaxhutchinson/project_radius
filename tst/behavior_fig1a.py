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

class ConnNCenters:
    def __init__(self, C, b, w):
        self.C = C
        self.b = b
        self.w = w
        self.i = 0.0
    def set_input(self, i):
        self.i = i
    def phi(self):
        i = 1
        for c in self.C:
            i *= (self.i-c)
        return self.w / (
            ((self.b*i)**2.0) + 1.0
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
        i = self.w / (
            ((self.b*math.sin(self.i-self.c))**2.0) + 1.0
        )
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
        i = self.w / (
            ((self.b*math.cos(self.i-self.c))**2.0) + 1.0
        )
        return i

class ConnLogistic:
    def __init__(self, c, b, w):
        self.c = c
        self.b = b
        self.w = w
        self.i = 0.0
    def set_input(self, i):
        self.i = i
    def phi(self):
        i = self.w / (
            ((self.b*self.i*(self.i-self.c))**2.0) + 1.0
        )
        return i

class ConnNeg:
    def __init__(self, c, b, w):
        self.c = c
        self.b = b
        self.w = w
        self.i = 0.0
    def set_input(self, i):
        self.i = i
    def phi(self):
        return -self.w / (
            ((self.b*(self.i-self.c))**2.0) + 1.0
        )

class Comp:
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
        
        # if I < 0:
        #     I = 0
        for k,v in self.e_conns.items():
            v.set_input(I)
        return I


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



a1 = ConnLogistic(5, 3, 1)
a2 = Conn(0, 3, 1)
A = CompRELU(2,1,1,{'a1':a1, 'a2':a2},{})

b1 = ConnSine(0, 3, 1)
b2 = ConnCosine(0, 3, 1)
B = CompRELU(2,1,1,{'b1':b1, 'b2':b2,},{})

c1 = ConnNeg(0, 0.5, 1)
c2 = Conn(0, 3, 1)
C = CompRELU(2,1,1,{'c1':c1, 'c2':c2,},{})

d1 = ConnNCenters([2, -2], 3, 1)
d2 = Conn(0, 3, 1)
D = CompRELU(2,1,1,{'d1':d1, 'd2':d2,},{})

start = -10
end = 10
step = 0.01

X = np.arange(start,end,step)
Y = np.arange(start,end,step)

Aout = np.zeros([len(X),len(Y)])
Bout = np.zeros([len(X),len(Y)])
Cout = np.zeros([len(X),len(Y)])
Dout = np.zeros([len(X),len(Y)])


for i in range(len(X)):
    b1.set_input(X[i])
    a1.set_input(X[i])
    c1.set_input(X[i])
    d1.set_input(X[i])
    for j in range(len(Y)):
        b2.set_input(Y[j])
        a2.set_input(Y[j])
        c2.set_input(Y[j])
        d2.set_input(Y[j])
        Aout[i][j] = A.psi()
        Bout[i][j] = B.psi()
        Cout[i][j] = C.psi()
        Dout[i][j] = D.psi()


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


grid = [None, None, None, None]
grid[0] = fig.add_subplot(221)
im0=grid[0].imshow(Aout)
grid[0].set_title("Logistic Equation")
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

grid[1] = fig.add_subplot(222, sharey=grid[0])
im1=grid[1].imshow(Bout)
grid[1].set_title("Trigonometric Functions")
grid[1].set_xticks(np.arange(0, len(Bout), len(Bout)/10))
grid[1].set_xticklabels(np.arange(start, end, (end-start)/10))
grid[1].set_yticks(np.arange(0, len(Bout), len(Bout)/10))
grid[1].set_yticklabels(np.arange(start, end, (end-start)/10))
grid[1].invert_yaxis()
div1 = make_axes_locatable(grid[1])
cax = div1.append_axes('right', size='5%', pad=0.05)
fig.colorbar(im1, cax=cax, orientation='vertical')

grid[2] = fig.add_subplot(223, sharex=grid[0])
im2=grid[2].imshow(Cout)
grid[2].set_title("Inhibitory Connection")
grid[2].set_ylabel("Input to R")
grid[2].set_xlabel("Input to L")
grid[2].set_xticks(np.arange(0, len(Bout), len(Bout)/10))
grid[2].set_xticklabels(np.arange(start, end, (end-start)/10))
grid[2].set_yticks(np.arange(0, len(Bout), len(Bout)/10))
grid[2].set_yticklabels(np.arange(start, end, (end-start)/10))
grid[2].invert_yaxis()
div2 = make_axes_locatable(grid[2])
cax = div2.append_axes('right', size='5%', pad=0.05)
fig.colorbar(im2, cax=cax, orientation='vertical')

grid[3] = fig.add_subplot(224, sharey=grid[2])
im3=grid[3].imshow(Dout)
grid[3].set_title("Multiple Centers")
grid[3].set_xlabel("Input to L")
grid[3].set_xticks(np.arange(0, len(Bout), len(Bout)/10))
grid[3].set_xticklabels(np.arange(start, end, (end-start)/10))
grid[3].set_yticks(np.arange(0, len(Bout), len(Bout)/10))
grid[3].set_yticklabels(np.arange(start, end, (end-start)/10))
grid[3].invert_yaxis()
div3 = make_axes_locatable(grid[3])
cax = div3.append_axes('right', size='5%', pad=0.05)
fig.colorbar(im3, cax=cax, orientation='vertical')

plt.show()
