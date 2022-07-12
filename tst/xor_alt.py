import matplotlib.pyplot as plt
import math

def fun(a, b, x):
    return math.tanh(a*x-b)/2.0 +0.5

y = []
xs = []
x = 0.0
delta = 0.001
a = 10.0
b = 7.0
for i in range(1500):
    xs.append(x)
    y.append(fun(a,b,x))
    x += delta


fig = plt.figure()
ax = fig.add_subplot()

plt.axvline(0.5,c='lightgray',linestyle='--')
plt.axvline(1.0,c='darkgray',linestyle='--')
plt.axhline(0.5,c='tab:pink')
ax.plot(xs,y,linewidth=3)
plt.xlabel("x")
plt.ylabel("phi(x)")
plt.show()