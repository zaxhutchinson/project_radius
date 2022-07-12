import math
import matplotlib.pyplot as plt



def phi(x,c, b, w):
    return w / (((b*(x-c))**2) +1.0)

def gau(x,c,b,w):
    return w*math.exp(-(b*(x-c))**2.0)



data = []
xvals = []

i = -10.0
c = 1.0

while i <= 10.0:
    result = gau(i,c,5,1)
    data.append(result)
    xvals.append(i)
    i += 0.1


plt.figure()
plt.plot(xvals,data,color='green',linewidth=2)
plt.xlabel(r"$v_a$")
plt.ylabel("\u03C6"+r"   $v_{e}=0$" )
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