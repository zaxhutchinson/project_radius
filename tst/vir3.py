# Demonstration of Importance, Relevance and Value


import math
import matplotlib.pyplot as plt
# plt.rcParams['text.usetex'] = True



# def phi(x,c, b, w):
#     return w / (((b*(x-c))**2) +1.0)
def Phi(x, comp):
    if x <= 0:
        return 0.0
    else:
        return comp.w / (
            ((comp.b*(x-comp.c))**2.0) + 1.0
        )
def Point(x, comp):
    if x <= 0:
        return 0.0
    else:
        return x*comp.w


class Comp:
    def __init__(self, c, b, w):
        self.c = c
        self.b = b
        self.w = w


c1 = Comp(2.0, 0.5, 0.5)
c2 = Comp(3.0, 0.5, 0.75)
c3 = Comp(4.0, 0.5, 1.0)
c4 = Comp(10.0, 0.25, 2.5)

cX = Comp(2.0, 5.0, 0.25)
# c4 = Comp(7.0, 2.5, 0.142857142857)

c1_data_phi = []
c2_data_phi = []
c3_data_phi = []
c4_data_phi = []
c_data_sum = []

c1_data_point = []
# c2_data_point = []

x = -10.0

xvals = []

while x <= 10.0:
    y = 0 if x < 0 else x
    out1_phi = Phi(x, c1)
    out2_phi = Phi(x, c2)
    out3_phi = Phi(x, c3)
    out4_phi = Phi(x, c4)
    out1_point = Point(x, cX)
    # out2_point = Point(y, c4)
    c1_data_phi.append(out1_phi)
    c2_data_phi.append(out2_phi)
    c3_data_phi.append(out3_phi)
    c4_data_phi.append(out4_phi)
    c_data_sum.append(out1_phi+out2_phi+out3_phi+out4_phi)
    c1_data_point.append(out1_point)
    # c2_data_point.append(out2_point)
    xvals.append(x)
    x += 0.01

fig, (ax1) = plt.subplots(1, 1)
# ax1.plot(xvals,c1_data_phi,label=r"$x_e=2$"+", w=0.5, b=2.5")
# ax1.plot(xvals,c2_data_phi,label=r"$x_e=3$"+", w=0.75, b=2.5")
# ax1.plot(xvals,c3_data_phi,label=r"$x_e=4$"+", w=1.0, b=2.5")
ax1.plot(xvals,c4_data_phi,label=r"$x_e=10$"+", w=2.5, b=0.25",color='tab:pink')
ax1.plot(xvals,c1_data_point,label="w=0.25",linestyle='--',color='tab:blue')
# ax1.plot(xvals,c_data_sum,label="sum of den. inputs",linestyle='dotted')
# ax1.plot(xvals,c2_data_point,label="w=0.14",linestyle='--',color='tab:orange')
# ax1.set_title("Dendritic Model")
ax1.set(xlabel=r"$\psi$ (Relevance)",ylabel=r"$\varphi$"+" (Importance)")
# ax1.set(xlabel=r"$\psi$ (Relevance)",ylabel=r"$\varphi=$"+"$\\frac{w}{(\\frac{x_a-x_e}{b})^2+1}$ (Importance)")
ax1.legend()

# ax2.plot(xvals,c1_data_point,label="w=0.05")
# ax2.plot(xvals,c2_data_point,label="w=0.1")
# ax2.set_title("Point Model")
# ax2.set(xlabel=r"$x_a$ (Relevance)",ylabel="\u03C6="+r"$x_a$w (Importance)")
# ax2.legend()
plt.show()