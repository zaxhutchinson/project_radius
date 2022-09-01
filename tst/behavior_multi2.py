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
        self.i = math.inf
    def set_input(self, i):
        self.i = i
    def phi(self):
        I = self.w / (
            ((self.b*(self.i-self.c))**2.0) + 1.0
        )
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
        for v in self.a_conns:
            I += v.phi()
        
        if I < 0.05:
            I = 0
        for v in self.e_conns:
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
        for v in self.a_conns:
            I += v.phi()
        
        # if I < 0:
        #     I = 0
        for v in self.e_conns:
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
        for v in self.a_conns:
            I += v.phi()
        
        I = self.w / (
            ((self.b*(I-self.c))**2.0) + 1.0
        )
        if I < 0.05:
            I = 0
        for v in self.e_conns:
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
        for v in self.a_conns:
            I += v.phi()
        
        I = self.w / (
            ((self.b*(I-len(self.a_conns)))**2.0) + 1.0
        )
        for v in self.e_conns:
            v.set_input(I)
        return I

b = 10
w = 1
conns0 = []
for i in range(8):
    c = Conn(w,b,w)
    conns0.append(c)

conns1 = []
for i in range(12):
    c = Conn(w,b,w)
    conns1.append(c)

conns2 = []
for i in range(6):
    c = Conn(2,b,w)
    conns2.append(c)

conns3 = []
for i in range(2):
    c = Conn(w,b,w)
    conns3.append(c)



# conn_out = Conn(1,0.25,1)

comps = [CompRBF(1,b,w,[],[])]
for i in range(14):
    c = CompRELU(w,b,w,[],[])
    comps.append(c)

# comps[0].w = 1

comps[14].c = 1
comps[13].c = 1
comps[12].c = 1
comps[11].c = 1
comps[10].c = 1
comps[9].c = 1
comps[8].c = 1
comps[7].c = 1

comps[6].c = 2
comps[5].c = 2
comps[4].c = 2
comps[3].c = 2
comps[2].c = 1
comps[1].c = 1
comps[0].c = 1

comps[0].a_conns.append(conns2[3])
comps[0].a_conns.append(conns3[1])

comps[1].e_conns.append(conns3[1])

comps[1].a_conns.append(conns3[0])
comps[1].a_conns.append(conns2[2])

comps[2].e_conns.append(conns3[0])

comps[2].a_conns.append(conns2[0])
comps[2].a_conns.append(conns2[1])

comps[3].e_conns.append(conns2[0])
comps[4].e_conns.append(conns2[1])
comps[5].e_conns.append(conns2[2])
comps[6].e_conns.append(conns2[3])

comps[3].a_conns.append(conns1[0])
comps[3].a_conns.append(conns1[1])
comps[4].a_conns.append(conns1[2])
comps[4].a_conns.append(conns1[3])
comps[5].a_conns.append(conns1[4])
comps[5].a_conns.append(conns1[5])
comps[6].a_conns.append(conns1[6])
comps[6].a_conns.append(conns1[7])
# comps[3].a_conns.append(conns1[8])
# comps[4].a_conns.append(conns1[9])
# comps[5].a_conns.append(conns1[10])
# comps[6].a_conns.append(conns1[11])

comps[7].e_conns.append(conns1[0])
comps[8].e_conns.append(conns1[1])
comps[9].e_conns.append(conns1[2])
comps[10].e_conns.append(conns1[3])
comps[11].e_conns.append(conns1[4])
comps[12].e_conns.append(conns1[5])
comps[13].e_conns.append(conns1[6])
comps[14].e_conns.append(conns1[7])

comps[7].a_conns.append(conns0[0])
comps[8].a_conns.append(conns0[1])
comps[9].a_conns.append(conns0[2])
comps[10].a_conns.append(conns0[3])
comps[11].a_conns.append(conns0[4])
comps[12].a_conns.append(conns0[5])
comps[13].a_conns.append(conns0[6])
comps[14].a_conns.append(conns0[7])

# for c in comps:
#     c.c = len(c.a_conns)

start = 0
end = 2
step = 0.01

I = np.arange(start,end,step)

out = np.zeros([len(comps),len(I)])

definput = math.inf

xaxis = []

exp = 4

for i in range(len(I)):
    xaxis.append(i)

    for j in range(len(conns0)):
        conns0[j].set_input(definput)
    for j in range(len(conns1)):
        conns1[j].set_input(definput)
    for j in range(len(conns2)):
        conns2[j].set_input(definput)
    for j in range(len(conns3)):
        conns3[j].set_input(definput)
    
    if exp==0:
        conns0[0].set_input(I[i])
        conns0[1].set_input(I[i])
    elif exp==1:
        conns0[0].set_input(I[i])
        conns0[1].set_input(I[i])
        conns0[2].set_input(I[i])
        conns0[4].set_input(I[i])
        conns0[6].set_input(I[i])
    elif exp==2:
        conns0[0].set_input(I[i])
        conns0[1].set_input(I[i])
        conns0[6].set_input(I[i])
        conns0[7].set_input(I[i])
    elif exp==3:
        conns0[0].set_input(I[i])
        conns0[1].set_input(I[i])
        conns0[2].set_input(I[i])
        conns0[3].set_input(I[i])
    elif exp==4:
        conns0[6].set_input(I[i])
        conns0[7].set_input(I[i])

    for j in range(len(comps)-1,-1,-1):
        out[j][i] = comps[j].psi()


    # b1.set_input(X[i])
    # b2.set_input(X[i])
    # a1.set_input(X[i])
    # a2.set_input(X[i])
    # for j in range(len(Y)):
    #     b3.set_input(Y[j])
    #     b4.set_input(Y[j])
    #     a3.set_input(Y[j])
    #     a4.set_input(Y[j])
    #     Aout[i][j] = A.psi()
    #     Bout[i][j] = B.psi()



# fig = plt.figure()
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

to_print = list(range(15))

fig,grid = plt.subplots(5,3,sharex=True)

# for i in range(15):

#     g = fig.add_subplot(5,3,i+1)
#     g.plot(I,out[i],label=f"{i}")
#     g.set_title(f"{i}")
#     grid.append(g)
# plt.xlabel("Time Step")
# plt.ylabel("Output (M-compartment)")
# plt.legend()

i = 0
for y in range(5):
    for x in range(3):
        grid[y][x].plot(I,out[i])
        grid[y][x].set_title(f"{i}")
        i+=1


# g.plot(I,out[0],label=f"{0}")
# g.set_title(f"{0}")
# grid.append(g)

# g.plot(I,out[1],label=f"{1}")
# g.set_title(f"{2}")
# grid.append(g)

# g.plot(I,out[2],label=f"{2}")
# g.set_title(f"{2}")
# grid.append(g)

# g.plot(I,out[3],label=f"{3}")
# g.set_title(f"{3}")
# grid.append(g)

# g.plot(I,out[4],label=f"{4}")
# g.set_title(f"{4}")
# grid.append(g)

# g.plot(I,out[5],label=f"{5}")
# g.set_title(f"{5}")
# grid.append(g)

# g.plot(I,out[6],label=f"{6}")
# g.set_title(f"{6}")
# grid.append(g)

# g.plot(I,out[7],label=f"{7}")
# g.set_title(f"{7}")
# grid.append(g)

# g.plot(I,out[8],label=f"{8}")
# g.set_title(f"{8}")
# grid.append(g)

# g.plot(I,out[9],label=f"{9}")
# g.set_title(f"{9}")
# grid.append(g)

# g.plot(I,out[10],label=f"{10}")
# g.set_title(f"{10}")
# grid.append(g)

# g.plot(I,out[11],label=f"{11}")
# g.set_title(f"{11}")
# grid.append(g)

# g.plot(I,out[12],label=f"{12}")
# g.set_title(f"{12}")
# grid.append(g)


# g.plot(I,out[13],label=f"{13}")
# g.set_title(f"{13}")
# grid.append(g)


# g.plot(I,out[14],label=f"{14}")
# g.set_title(f"{14}")
# grid.append(g)

plt.show()


# # fig.tight_layout()
# grid = [None, None]
# grid[0] = fig.add_subplot(121)
# im0=grid[0].imshow(Aout)
# grid[0].set_title("B (3 activation points)")
# grid[0].set_xlabel("Input to L")
# grid[0].set_ylabel("Input to R")
# grid[0].set_xticks(range(0, len(Bout), len(Bout)//10))
# grid[0].set_xticklabels(range(start, end, (end-start)//10))
# grid[0].set_yticks(range(0, len(Bout), len(Bout)//10))
# grid[0].set_yticklabels(range(start, end, (end-start)//10))
# grid[0].invert_yaxis()
# div0 = make_axes_locatable(grid[0])
# cax = div0.append_axes('right', size='5%', pad=0.05)
# fig.colorbar(im0, cax=cax, orientation='vertical')

# grid[1] = fig.add_subplot(122, sharey=grid[0])
# im1=grid[1].imshow(Bout)
# grid[1].set_title("B (cluster activation)")
# grid[1].set_xlabel("Input to L")
# grid[1].set_xticks(np.arange(0, len(Bout), len(Bout)/10))
# grid[1].set_xticklabels(np.arange(start, end, (end-start)/10))
# grid[1].set_yticks(np.arange(0, len(Bout), len(Bout)/10))
# grid[1].set_yticklabels(np.arange(start, end, (end-start)/10))
# grid[1].invert_yaxis()
# div1 = make_axes_locatable(grid[1])
# cax = div1.append_axes('right', size='5%', pad=0.05)
# fig.colorbar(im1, cax=cax, orientation='vertical')


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


# plt.show()
