# Demonstration of Importance, Relevance and Value


import math
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.ticker import LinearLocator
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


class Comp:
    def __init__(self,c,b,a_conns,e_conns):
        self.a_conns = a_conns
        self.e_conns = e_conns
        self.c = c
        self.b = b
    def psi(self):
        I = 0.0
        for k,v in self.a_conns.items():
            I += v.phi()
        
        I = 1 / (
            ((self.b*(I-self.c))**2.0) + 1.0
        )
        for k,v in self.e_conns.items():
            v.set_input(I)
        return I

conn_b1 = 16
conn_b2 = 8

comp_b1 = 0.8
comp_b2 = 1

a = Conn(0.0, conn_b1, 1.0)
b = Conn(0.0, conn_b1, 1.0)
c = Conn(1.0, conn_b2, 1.0)
d = Conn(1.0, conn_b2, 1.0)
e = Conn(1.0, conn_b2, 1.0)
f = Conn(1.0, conn_b2, 1.0)

AND = Comp(2.0, comp_b1, {'a':a, 'b':b}, {'d':d})
XOR1 = Comp(1.0, comp_b1, {'a':a, 'b':b}, {'c':c})
XOR2 = Comp(1.0, comp_b1, {'a':a, 'b':b}, {'f':f})
NOR = Comp(0.0, comp_b1, {'a':a, 'b':b}, {'e':e})
OR = Comp(1.0, comp_b2, {'c':c, 'd':d},{})
NAND = Comp(1.0, comp_b2, {'e':e, 'f':f},{} )


X = np.arange(-5,5,0.01)
Y = np.arange(-5,5,0.01)
# X,Y = np.meshgrid(X,Y)
Z_AND = np.zeros([len(X),len(Y)])
Z_XOR1 = np.zeros([len(X),len(Y)])
Z_XOR2 = np.zeros([len(X),len(Y)])
Z_NOR = np.zeros([len(X),len(Y)])
Z_OR = np.zeros([len(X),len(Y)])
Z_NAND = np.zeros([len(X),len(Y)])

# a.set_input(10)
# b.set_input(10)
# print(f"AND  {AND.psi()}")
# print(f"XOR1 {XOR1.psi()}")
# print(f"C    {c.phi()}")
# print(f"D    {d.phi()}")
# print(f"OR   {OR.psi()}")


for i in range(len(X)):
    for j in range(len(Y)):

        x = X[i]
        y = Y[j]
        a.set_input(x)
        b.set_input(y)
        Z_AND[i][j] = AND.psi()
        Z_XOR1[i][j] = XOR1.psi()
        Z_XOR2[i][j] = XOR2.psi()
        Z_NOR[i][j] = NOR.psi()
        Z_OR[i][j] = OR.psi()
        Z_NAND[i][j] = NAND.psi()


fig, axs = plt.subplots(2,3)
axs[0,0].imshow(Z_XOR1,cmap='turbo')
axs[0,1].imshow(Z_AND,cmap='turbo')
axs[0,2].imshow(Z_OR, cmap='turbo')
axs[1,0].imshow(Z_XOR2,cmap='turbo')
axs[1,1].imshow(Z_NOR,cmap='turbo')
axs[1,2].imshow(Z_NAND, cmap='turbo')

axs[0,0].set_title("XOR")
axs[0,1].set_title("AND")
axs[0,2].set_title("OR")
axs[1,0].set_title("XOR")
axs[1,1].set_title("NOR")
axs[1,2].set_title("NAND")

# ax1.plot_surface(X,Y,Z_NAND,linewidth=0,antialiased=False, cmap='turbo')
# ax2.plot_surface(X,Y,Z_NOR,linewidth=0,antialiased=False, cmap='turbo')
plt.show()
