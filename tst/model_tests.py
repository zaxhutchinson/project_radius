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
    def __init__(self,c,b,conns):
        self.conns = conns
        self.c = c
        self.b = b
    def psi(self):
        I = 0.0
        for k,v in self.conns.items():
            I += v.phi()
        return 1 / (
            ((self.b*(I-self.c))**2.0) + 1.0
        )


a = Conn(1.0, 1.0, 1.0)
b = Conn(1.0, 1.0, 1.0)
AND = Comp(2.0, 1.0, {'a':a, 'b':b})
XOR = Comp(1.0, 1.0, {'a':a, 'b':b})
NOR = Comp(0.0, 1.0, {'a':a, 'b':b})


X = np.arange(-5,7,0.01)
Y = np.arange(-5,7,0.01)
X,Y = np.meshgrid(X,Y)
Z_AND = np.zeros([len(X),len(Y)])
Z_XOR = np.zeros([len(X),len(Y)])
Z_NOR = np.zeros([len(X),len(Y)])

for i in range(len(X)):
    for j in range(len(X[i])):

        x = X[i][j]
        y = Y[i][j]
        a.set_input(x)
        b.set_input(y)
        Z_AND[i][j] = AND.psi()
        Z_XOR[i][j] = XOR.psi()
        Z_NOR[i][j] = NOR.psi()

fig, (ax0, ax1, ax2) = plt.subplots(1,3,subplot_kw={"projection": "3d"})
ax0.plot_surface(X,Y,Z_AND,linewidth=0,antialiased=False, cmap='turbo')
ax1.plot_surface(X,Y,Z_XOR,linewidth=0,antialiased=False, cmap='turbo')
ax2.plot_surface(X,Y,Z_NOR,linewidth=0,antialiased=False, cmap='turbo')
plt.show()



# fig, ax = plt.subplots(subplot_kw={"projection": "3d"})

# # Make data.
# X = np.arange(-5, 5, 0.25)
# Y = np.arange(-5, 5, 0.25)
# X, Y = np.meshgrid(X, Y)
# R = np.sqrt(X**2 + Y**2)
# Z = np.sin(R)
# print(X.shape, Y.shape, Z.shape, R.shape)

# # Plot the surface.
# surf = ax.plot_surface(X, Y, Z, cmap=cm.coolwarm,
#                        linewidth=0, antialiased=False)

# # Customize the z axis.
# ax.set_zlim(-1.01, 1.01)
# ax.zaxis.set_major_locator(LinearLocator(10))
# # A StrMethodFormatter is used automatically
# ax.zaxis.set_major_formatter('{x:.02f}')

# # Add a color bar which maps values to colors.
# fig.colorbar(surf, shrink=0.5, aspect=5)

# plt.show()