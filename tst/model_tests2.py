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

conn_b1 = 2
conn_b2 = 2

comp_b1 = 2
comp_b2 = 2

a = Conn(0.0, conn_b1, 1.0)
b = Conn(0.0, conn_b1, 1.0)
c = Conn(1.0, conn_b2, 1.0)
d = Conn(1.0, conn_b2, 1.0)
e = Conn(1.0, conn_b2, 1.0)
f = Conn(1.0, conn_b2, 1.0)
g = Conn(1.0, conn_b2, 1.0)
h = Conn(1.0, conn_b2, 1.0)

AND = Comp(2.0, comp_b1, {'a':a, 'b':b}, {'d':d})
XOR1 = Comp(1.0, comp_b1, {'a':a, 'b':b}, {'c':c})
XOR2 = Comp(1.0, comp_b1, {'a':a, 'b':b}, {'f':f})
NOR = Comp(0.0, comp_b1, {'a':a, 'b':b}, {'e':e})
OR = Comp(0.0, comp_b2, {'e':e},{})
NAND = Comp(0.0, comp_b2, {'d':d},{} )
IMPL_NOT = Comp(0.0, comp_b2, {'a':a}, {'g':g})
IMPL_NOR = Comp(0.0, comp_b2, {'g':g, 'b':b}, {'h':h})
IMPL = Comp(0.0,comp_b2, {'h':h},{})

start = -5
end = 5
step = 0.01

X = np.arange(start,end,step)
Y = np.arange(start,end,step)
# X,Y = np.meshgrid(X,Y)
Z_AND = np.zeros([len(X),len(Y)])
Z_XOR1 = np.zeros([len(X),len(Y)])
Z_XOR2 = np.zeros([len(X),len(Y)])
Z_NOR = np.zeros([len(X),len(Y)])
Z_OR = np.zeros([len(X),len(Y)])
Z_NAND = np.zeros([len(X),len(Y)])
Z_IMPL_NOT = np.zeros([len(X), len(Y)])
Z_IMPL_NOR = np.zeros([len(X), len(Y)])
Z_IMPL = np.zeros([len(X), len(Y)])

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
        Z_IMPL_NOT[i][j] = IMPL_NOT.psi()
        Z_IMPL_NOR[i][j] = IMPL_NOR.psi()
        Z_IMPL[i][j] = IMPL.psi()


fig, axs = plt.subplots(2,3)

im=axs[0,2].imshow(Z_AND,cmap='turbo')
axs[0,2].set_xticks(range(0, len(Z_XOR1)+1, len(Z_XOR1)//10))
axs[0,2].set_xticklabels(range(start, end+1, (end-start)//10))
axs[0,2].set_yticks(range(0, len(Z_XOR1)+1, len(Z_XOR1)//10))
axs[0,2].set_yticklabels(range(start, end+1, (end-start)//10))
axs[0,2].invert_yaxis()
axs[0,2].set_title("AND")

axs[0,0].imshow(Z_NOR,cmap='turbo')
axs[0,0].set_xticks(range(0, len(Z_XOR1)+1, len(Z_XOR1)//10))
axs[0,0].set_xticklabels(range(start, end+1, (end-start)//10))
axs[0,0].set_yticks(range(0, len(Z_XOR1)+1, len(Z_XOR1)//10))
axs[0,0].set_yticklabels(range(start, end+1, (end-start)//10))
axs[0,0].invert_yaxis()
axs[0,0].set_title("NOR")

axs[1,0].imshow(Z_OR, cmap='turbo')
axs[1,0].set_xticks(range(0, len(Z_XOR1)+1, len(Z_XOR1)//10))
axs[1,0].set_xticklabels(range(start, end+1, (end-start)//10))
axs[1,0].set_yticks(range(0, len(Z_XOR1)+1, len(Z_XOR1)//10))
axs[1,0].set_yticklabels(range(start, end+1, (end-start)//10))
axs[1,0].invert_yaxis()
axs[1,0].set_title("OR")

axs[0,1].imshow(Z_XOR1,cmap='turbo')
axs[0,1].set_xticks(range(0, len(Z_XOR1)+1, len(Z_XOR1)//10))
axs[0,1].set_xticklabels(range(start, end+1, (end-start)//10))
axs[0,1].set_yticks(range(0, len(Z_XOR1)+1, len(Z_XOR1)//10))
axs[0,1].set_yticklabels(range(start, end+1, (end-start)//10))
axs[0,1].invert_yaxis()
axs[0,1].set_title("XOR")

axs[1,2].imshow(Z_NAND, cmap='turbo')
axs[1,2].set_xticks(range(0, len(Z_XOR1)+1, len(Z_XOR1)//10))
axs[1,2].set_xticklabels(range(start, end+1, (end-start)//10))
axs[1,2].set_yticks(range(0, len(Z_XOR1)+1, len(Z_XOR1)//10))
axs[1,2].set_yticklabels(range(start, end+1, (end-start)//10))
axs[1,2].invert_yaxis()
axs[1,2].set_title("NAND")

# axs[1,0].imshow(Z_XOR2,cmap='turbo')
# axs[1,0].set_xticks(range(0, len(Z_XOR1)+1, len(Z_XOR1)//10))
# axs[1,0].set_xticklabels(range(start, end+1, (end-start)//10))
# axs[1,0].set_yticks(range(0, len(Z_XOR1)+1, len(Z_XOR1)//10))
# axs[1,0].set_yticklabels(range(start, end+1, (end-start)//10))
# axs[1,0].invert_yaxis()
# axs[1,0].set_title("XOR")

# axs[1,1].axis('off')

axs[1,1].imshow(Z_IMPL,cmap='turbo')
axs[1,1].set_xticks(range(0, len(Z_XOR1)+1, len(Z_XOR1)//10))
axs[1,1].set_xticklabels(range(start, end+1, (end-start)//10))
axs[1,1].set_yticks(range(0, len(Z_XOR1)+1, len(Z_XOR1)//10))
axs[1,1].set_yticklabels(range(start, end+1, (end-start)//10))
axs[1,1].invert_yaxis()
axs[1,1].set_title("IMPL")

# fig.colorbar(im, ax=axs.ravel().tolist())

fig.subplots_adjust(right=0.8)
cbar_ax = fig.add_axes([0.85, 0.15, 0.02, 0.70])
fig.colorbar(im, cax=cbar_ax)


fig.text(0.5, 0.04, 'Input B', ha='center')
fig.text(0.04, 0.5, 'Input A', va='center', rotation='vertical')

# fig.supxlabel("Input A")
# fig.supylabel("Input B")

plt.show()
