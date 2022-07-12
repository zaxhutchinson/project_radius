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

conn_b1 = 10
conn_b2 = 0.5
conn_b3 = 1
conn_b4 = 1
conn_b5 = 1

comp_b1 = 0.1
comp_b2 = 0.5
comp_b3 = 10
comp_b4 = 1
comp_b5 = 10

a0 = Conn(-2, conn_b1, 10.0)
a1 = Conn(-1, conn_b1, 10.0)
a2 = Conn(0, conn_b1, 10.0)
a3 = Conn(1, conn_b1, 10.0)
a4 = Conn(2, conn_b1, 10.0)
b = Conn(1.0, conn_b2, 10.0)
c = Conn(1.0, conn_b3, 10.0)
d = Conn(1.0, conn_b4, 10.0)
e = Conn(1.0, conn_b5, 10.0)

A = Comp(10.0, comp_b1, 1.0, {'a0':a0, 'a1':a1, 'a2':a2, 'a3':a3, 'a4':a4}, {'b':b})
B = Comp(10.0, comp_b2, 1.0, {'b':b}, {'c':c})
C = Comp(10.0, comp_b3, 1.0, {'c':c}, {'d':d})
D = Comp(10.0, comp_b4, 1.0, {'d':d}, {'e':e})
E = Comp(10.0, comp_b5, 1.0, {'e':e}, {})


X = np.arange(-5,5,0.01)

# X,Y = np.meshgrid(X,Y)
Aout = np.zeros([len(X)])
Bout = np.zeros([len(X)])
Cout = np.zeros([len(X)])
Dout = np.zeros([len(X)])
Eout = np.zeros([len(X)])

# a.set_input(10)
# b.set_input(10)
# print(f"AND  {AND.psi()}")
# print(f"XOR1 {XOR1.psi()}")
# print(f"C    {c.phi()}")
# print(f"D    {d.phi()}")
# print(f"OR   {OR.psi()}")


for i in range(len(X)):

    x = X[i]
    a0.set_input(x)
    a1.set_input(x)
    a2.set_input(x)
    a3.set_input(x)
    a4.set_input(x)
    Aout[i] = A.psi()
    Bout[i] = B.psi()
    Cout[i] = C.psi()
    Dout[i] = D.psi()
    Eout[i] = E.psi()

fig = plt.figure()
plt.plot(Aout, label="A")
plt.plot(Bout, label="B")
plt.plot(Cout, label="C")
plt.plot(Dout, label="D")
plt.plot(Eout, label="E")
plt.legend()


# ax1.plot_surface(X,Y,Z_NAND,linewidth=0,antialiased=False, cmap='turbo')
# ax2.plot_surface(X,Y,Z_NOR,linewidth=0,antialiased=False, cmap='turbo')
plt.show()
