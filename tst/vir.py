# Demonstration of Importance, Relevance and Value


import math
import matplotlib.pyplot as plt
# plt.rcParams['text.usetex'] = True



# def phi(x,c, b, w):
#     return w / (((b*(x-c))**2) +1.0)
def Phi(x, comp):
    return comp.w / (
        ((comp.b*(x-comp.c))**2.0) + 1.0
    )
def Point(x, comp):
    return x*comp.w


class Comp:
    def __init__(self, c, b, w):
        self.c = c
        self.b = b
        self.w = w


c1 = Comp(2.0, 5.0, 0.5)
c2 = Comp(7.0, 2.5, 1.0)

c3 = Comp(2.0, 5.0, 0.25)
c4 = Comp(7.0, 2.5, 0.142857142857)

c1_data_phi = []
c2_data_phi = []
c1_data_point = []
c2_data_point = []

x = -10.0

xvals = []

while x <= 10.0:
    y = 0 if x < 0 else x
    out1_phi = Phi(y, c1)
    out2_phi = Phi(y, c2)
    out1_point = Point(y, c3)
    out2_point = Point(y, c4)
    c1_data_phi.append(out1_phi)
    c2_data_phi.append(out2_phi)
    c1_data_point.append(out1_point)
    c2_data_point.append(out2_point)
    xvals.append(x)
    x += 0.01

fig, (ax1) = plt.subplots(1, 1)
ax1.plot(xvals,c1_data_phi,label=r"$x_e=2$"+", w=0.5, b=5.0",color='tab:blue')
ax1.plot(xvals,c2_data_phi,label=r"$x_e=7$"+", w=1, b=2.5",color='tab:orange')
ax1.plot(xvals,c1_data_point,label="w=0.25",linestyle='--',color='tab:blue')
ax1.plot(xvals,c2_data_point,label="w=0.14",linestyle='--',color='tab:orange')
ax1.set_title("Dendritic Model")
ax1.set(xlabel=r"$\psi$ (Relevance)",ylabel=r"$\varphi$"+" (Importance)")
# ax1.set(xlabel=r"$\psi$ (Relevance)",ylabel=r"$\varphi=$"+"$\\frac{w}{(\\frac{x_a-x_e}{b})^2+1}$ (Importance)")
ax1.legend()

# ax2.plot(xvals,c1_data_point,label="w=0.05")
# ax2.plot(xvals,c2_data_point,label="w=0.1")
# ax2.set_title("Point Model")
# ax2.set(xlabel=r"$x_a$ (Relevance)",ylabel="\u03C6="+r"$x_a$w (Importance)")
# ax2.legend()
plt.show()