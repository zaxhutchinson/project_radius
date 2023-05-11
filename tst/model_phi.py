import math
import matplotlib.pyplot as plt



def phi(x,c, b, w):
    return w / (((b*(x-c))**2) +1.0)

def gau(x,c,b,w):
    return w*math.exp(-(b*(x-c))**2.0)



data1 = []
data2 = []
data3 = []
xvals = []

i = 0.0
c = 0.0

while i <= 20.0:
    data1.append(phi(i,5,1,2))
    data2.append(phi(i,10,2,1))
    data3.append(phi(i,15,0.5,0.5))
    xvals.append(i)
    i += 0.1


plt.figure()
plt.plot(xvals,data1,linewidth=2,label=r"$v_e$=5, b=1, w=2")
plt.plot(xvals,data2,linewidth=2,label=r"$v_e$=10, b=2, w=1")
plt.plot(xvals,data3,linewidth=2,label=r"$v_e$=15, b=0.5, w=0.5")
plt.xlabel(r"$v_a$")
plt.ylabel("\u03C6"+r"($v_a$)" )
plt.legend()
plt.show()




# This graphs the point at which, for a given value of x (or x-c),
# a given b causes the output to drop below 0.01 (~0)

phi_growth = []
gau_growth = []
data = []

w = 1.0
c = 0.0

for x in range(1,100):
    x /= 10.0

    data.append(x)

    b = 0.0
    out = 1.0
    while out > 0.01:
        out = phi(x,c,b,w)
        b += 0.001
    phi_growth.append(b)

    b = 0.0
    out = 1.0
    while out > 0.01:
        out = gau(x,c,b,w)
        b += 0.001
    gau_growth.append(b)


plt.figure()
plt.plot(data,phi_growth,label="phi")
plt.plot(data,gau_growth,label="gau")
plt.show()