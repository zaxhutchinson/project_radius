import math
import matplotlib.pyplot as plt

def gauss(x, mu, sigma):
    return (1.0/(sigma*math.sqrt(2.0*math.pi))) * math.exp(-0.5*((x-mu)/sigma)**2.0)

def witch(x, a, b, c):
    return c / (((x-a)/b)**2 +1.0)

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
for j in range(10):
    # dataB.append(witch(tsB/100.0, j+1 , 0.5, 1)) 
    # dataR.append(witch(tsR/100.0, j+1 , 0.5, 1))

    dataB.append(gauss(tsB/100.0, j+1 , 0.5)) 
    dataR.append(gauss(tsR/100.0, j+1 , 0.5))
fig = plt.figure()
plt.plot(dataB,linewidth=2,c='blue')
plt.plot(dataR,linewidth=2,c='red')
plt.xlabel("Neuron")
plt.xticks(list(range(0,10)))
plt.ylabel("Input")
plt.show()