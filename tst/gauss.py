import math
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.ticker import LinearLocator
import numpy as np
from matplotlib.colors import ListedColormap, LinearSegmentedColormap

def gauss(x, mu, sigma):
    return (1.0/(sigma*math.sqrt(2.0*math.pi))) * math.exp(-0.5*((x-mu)/sigma)**2.0)

def witch(x, a, b, c=1):
    return c / (((x-a)/b)**2 +1.0)

def witch2(x1, y1, xc, yc, b):
    d = math.sqrt( (xc-x1)**2 + (yc-y1)**2 )
    return 1 / ((d/b)**2 +1.0)

data = {}


for i in range(10):
    data[i]=[]

for i in range(-200,1300):

    # For all: witch
    for j in range(10):
        data[j].append(witch(i/100.0, j+1 , 0.01, 1))
        # data[j].append(gauss(i/100.0, j+1 , 0.1))

    # For all: gauss
    # for j in range(10):
    #     data[j].append(gauss(i/100.0, j+1, .5))

    # For just one.
    # data[0].append(witch(i/100.0, 3+1 , 0.5, 1))
    # data[1].append(witch(i/100.0, 5+1 , 0.5, 1))

fig = plt.figure()
for k,v in data.items():
    plt.plot(v,linewidth=2)
plt.xlabel("Time Steps")
plt.xticks(ticks=list(range(0,1500,100)),labels=list(range(-200,1300,100)))
plt.ylabel("Input")
plt.show()

dataB = []
dataR = []
tsB = 400
tsR = 650
for j in range(1000):
    # dataB.append(witch(tsB/100.0, j+1 , 0.5, 1)) 
    # dataR.append(witch(tsR/100.0, j+1 , 0.5, 1))

    dataB.append(witch(tsB, j , 50)) 
    dataR.append(witch(tsR, j , 50))
fig = plt.figure()
# plt.plot(dataB,linewidth=2,c='blue')
plt.plot(dataR,linewidth=2,c='red')
plt.xlabel("Neuron")
plt.xticks(list(range(0,1000,100)),[0,1,2,3,4,5,6,7,8,9])
plt.ylabel("Input")
plt.show()



top = cm.get_cmap('Blues', 128)
bottom = cm.get_cmap('gray', 128)

bluecolor = np.vstack((bottom(np.linspace(0.25, 0.5, 128)),top(np.linspace(0.5, 1, 128))))
newcmp_blue = ListedColormap(bluecolor, name='BlueGray')

# top = cm.get_cmap('Reds', 128)
# bottom = cm.get_cmap('gray', 128)

# redcolor = np.vstack((top(np.linspace(0, 1, 128)),
#                        bottom(np.linspace(0, 1, 128))))
# newcmp_red = ListedColormap(redcolor, name='RedGray')

fig,ax = plt.subplots(subplot_kw={"projection":"3d"})

Xc = 400
Yc = 100

XcRed = 700
YcRed = 650

X = np.arange(0,1000,1)
Y = np.arange(0,1000,1)
XM,YM = np.meshgrid(X,Y)
Zblue = np.zeros([len(X),len(Y)])
Zred = np.zeros([len(X),len(Y)])

for x in range(len(X)):
    for y in range(len(Y)):
        Zblue[y][x] = witch2(x,y,Xc,Yc,50)
        Zred[y][x] = witch2(x,y,XcRed,YcRed,50)

blue_map=cm.get_cmap('Blues')
red_map = cm.get_cmap("Reds")
red_colors = red_map(np.arange(red_map.N))
red_colors[:,-1] = np.linspace(0,1,red_map.N)
red_map = ListedColormap(red_colors)

surf = ax.plot_surface(XM,YM,Zblue,cmap=blue_map,linewidth=0,antialiased=False)
ax.plot_surface(XM,YM,Zred,cmap=red_map,linewidth=0,antialiased=False)
ax.invert_xaxis()
ax.set_xlabel("X")
ax.set_ylabel("Y")
ax.set_zlabel("Input")
ax.set_xticks(list(range(0,1000,100)))
ax.set_xticklabels([0,1,2,3,4,5,6,7,8,9])
ax.set_yticks(list(range(0,1000,100)))
ax.set_yticklabels([0,1,2,3,4,5,6,7,8,9])
plt.show()