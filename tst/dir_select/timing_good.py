import math
import matplotlib.pyplot as plt
import numpy as np

class Conn:
    def __init__(self,other,dist):
        self.distance = dist
        self.other = other

class Node:
    def __init__(self, _id, _b=0.1):
        self.ID = _id
        self.signal = 0.0
        self.parent = None
        self.children = []
        self.spiketime = None
        self.b = _b
        self.c = 1.0

    def getsignal(self, curtime,debug=False):
        self.signal = 0.0
        mysig = 0.0
        if self.spiketime != None:
            mysig = self.witch(curtime-self.spiketime,0.0)
            #self.signal = mysig
        for c in self.children:
            self.signal += c.other.getsignal(curtime-c.distance,debug)
            
            spiketime = c.other.spiketime
            if spiketime != None and self.spiketime != None:
                td = self.spiketime - spiketime
                # print(f"{self.ID} {c.other.ID} {td} {c.distance} {self.witch(td,c.distance)}")
                self.signal += self.witch(td,c.distance) * mysig
        
        if debug:
            print(f"{self.ID}: {self.signal:.8f}")
        # return math.tanh(self.signal)
        return self.signal

    def witch(self, td, dist):
        return self.c / (((td-dist)*self.b)**2.0 + 1.0)

def linknodes(parent:Node, child:Node, dist:float) -> None:
    childconn = Conn(parent,dist)
    parentconn = Conn(child,dist)
    parent.children.append(parentconn)
    child.parent = childconn


n0 = Node("n0")
n1 = Node("n1")
n2 = Node("n2")
n3 = Node("n3")
n4 = Node("n4")
n5 = Node("n5")
n6 = Node("n6")
n7 = Node("n7")
n8 = Node("n8")
n9 = Node("n9")

linknodes(n0,n1,10.0)
linknodes(n1,n2,10.0)
linknodes(n2,n3,10.0)
linknodes(n3,n4,10.0)
linknodes(n4,n5,10.0)
linknodes(n5,n6,10.0)
linknodes(n6,n7,10.0)
linknodes(n7,n8,10.0)
linknodes(n8,n9,10.0)


# Not evenly spread over the dendrite
# linknodes(n0,n1,2.0)
# linknodes(n1,n2,4.0)
# linknodes(n2,n3,6.0)
# linknodes(n3,n4,8.0)
# linknodes(n4,n5,10.0)
# linknodes(n5,n6,12.0)
# linknodes(n6,n7,14.0)
# linknodes(n7,n8,16.0)
# linknodes(n8,n9,18.0)

time = 90

n0.spiketime = 90.0
n1.spiketime = 80.0
n2.spiketime = 70.0
n3.spiketime = 60.0
n4.spiketime = 50.0
n5.spiketime = 40.0
n6.spiketime = 30.0
n7.spiketime = 20.0
n8.spiketime = 10.0
n9.spiketime = 0.0

ticks = []

forward = np.zeros([200])
for i in range(0,200):
    ticks.append(i)
    forward[i]=n0.getsignal(i)

n0.spiketime = 0.0
n1.spiketime = 10.0
n2.spiketime = 20.0
n3.spiketime = 30.0
n4.spiketime = 40.0
n5.spiketime = 50.0
n6.spiketime = 60.0
n7.spiketime = 70.0
n8.spiketime = 80.0
n9.spiketime = 90.0

backward = np.zeros([200])
for i in range(0,200):
    backward[i]=n0.getsignal(i)

n0.spiketime = 0.0
n1.spiketime = 0.0
n2.spiketime = 0.0
n3.spiketime = 0.0
n4.spiketime = 0.0
n5.spiketime = 0.0
n6.spiketime = 0.0
n7.spiketime = 0.0
n8.spiketime = 0.0
n9.spiketime = 0.0

simultaneous = np.zeros([200])
for i in range(0,200):
    simultaneous[i]=n0.getsignal(i)


# n0.spiketime = 72.0
# n1.spiketime = 64.0
# n2.spiketime = 56.0
# n3.spiketime = 48.0
# n4.spiketime = 40.0
# n5.spiketime = 32.0
# n6.spiketime = 24.0
# n7.spiketime = 16.0
# n8.spiketime = 8.0
# n9.spiketime = 0.0

n0.spiketime = 81.0
n1.spiketime = 72.0
n2.spiketime = 63.0
n3.spiketime = 54.0
n4.spiketime = 45.0
n5.spiketime = 36.0
n6.spiketime = 27.0
n7.spiketime = 18.0
n8.spiketime = 9.0
n9.spiketime = 0.0

too_fast = np.zeros([200])
for i in range(0,200):
    too_fast[i]=n0.getsignal(i)


n0.spiketime = 108.0
n1.spiketime = 96.0
n2.spiketime = 84.0
n3.spiketime = 72.0
n4.spiketime = 60.0
n5.spiketime = 48.0
n6.spiketime = 36.0
n7.spiketime = 24.0
n8.spiketime = 12.0
n9.spiketime = 0.0

# n0.spiketime = 81.0
# n1.spiketime = 72.0
# n2.spiketime = 63.0
# n3.spiketime = 54.0
# n4.spiketime = 45.0
# n5.spiketime = 36.0
# n6.spiketime = 27.0
# n7.spiketime = 18.0
# n8.spiketime = 9.0
# n9.spiketime = 0.0

too_slow = np.zeros([200])
for i in range(0,200):
    too_slow[i]=n0.getsignal(i)

plt.figure()
plt.plot(ticks,forward,label='forward')
plt.plot(ticks,backward,label='backward',color='red')
plt.plot(ticks,simultaneous,label='simultaneous',color='black')
plt.plot(ticks,too_fast,label='forward (fast)',color='green')
plt.plot(ticks,too_slow,label='forward (slow)',color='orange')
plt.xlabel("Time Step")
plt.ylabel("Output (M-compartment)")

plt.legend()
plt.show()


# data = np.zeros([100,100])
# for i in range(0,100):
#     for j in range(0,100):
#         n1.spiketime = float(i)
#         n2.spiketime = float(j)
#         data[i][j]=n0.getsignal(float(100))

# plt.figure()
# plt.imshow(data)
# plt.xlabel("n2 spike times")
# plt.ylabel("n1 spike times")
# plt.show()

# n0 = Node("n0")
# n1 = Node("n1")
# n2 = Node("n2")
# n3 = Node("n3")
# n4 = Node("n4")
# n5 = Node("n5")
# n6 = Node("n6")

# linknodes(n0,n1,10.0)
# linknodes(n0,n2,10.0)
# linknodes(n1,n3,10.0)
# linknodes(n1,n4,10.0)
# linknodes(n2,n5,10.0)
# linknodes(n2,n6,10.0)

# n1.spiketime = 10.0
# n2.spiketime = 10.0
# n3.spiketime = 0.0
# n4.spiketime = 0.0
# n5.spiketime = 0.0
# n6.spiketime = 0.0

# n0.getsignal(20.0)