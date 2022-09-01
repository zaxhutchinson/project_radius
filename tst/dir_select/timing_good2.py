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
        return math.tanh(self.signal)
        # return self.signal

    def witch(self, td, dist):
        return self.c / (((td-dist)*self.b)**2.0 + 1.0)

def linknodes(parent:Node, child:Node, dist:float) -> None:
    childconn = Conn(parent,dist)
    parentconn = Conn(child,dist)
    parent.children.append(parentconn)
    child.parent = childconn


nodes = []

for i in range(15):
    node = Node(f"n{i}")
    node.spiketime = 1000000.0
    nodes.append(node)



linknodes(nodes[0],nodes[1],10.0)
linknodes(nodes[0],nodes[1],10.0)

linknodes(nodes[1],nodes[3],10.0)
linknodes(nodes[1],nodes[4],10.0)
linknodes(nodes[2],nodes[5],10.0)
linknodes(nodes[2],nodes[6],10.0)

linknodes(nodes[3],nodes[7],10.0)
linknodes(nodes[3],nodes[8],10.0)
linknodes(nodes[4],nodes[9],10.0)
linknodes(nodes[4],nodes[10],10.0)
linknodes(nodes[5],nodes[11],10.0)
linknodes(nodes[5],nodes[12],10.0)
linknodes(nodes[6],nodes[13],10.0)
linknodes(nodes[6],nodes[14],10.0)

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

nodes[7].spiketime = 80
# nodes[3].spiketime = 10
nodes[1].spiketime = 100
# nodes[0].spiketime = 120

ticks = []
one = np.zeros([200])
for i in range(0,200):
    ticks.append(i)
    one[i]=nodes[0].getsignal(i)

for i in range(15):
    nodes[i].spiketime = 1000000.0

nodes[14].spiketime = 90
nodes[6].spiketime = 100
nodes[2].spiketime = 110
nodes[0].spiketime = 120

two = np.zeros([200])
for i in range(0,200):
    two[i]=nodes[0].getsignal(i)


for i in range(15):
    nodes[i].spiketime = 1000000.0

plt.figure()
plt.plot(ticks,one,label='one')
# plt.plot(ticks,two,label='two')
# plt.plot(ticks,backward,label='backward',color='red')
# plt.plot(ticks,simultaneous,label='simultaneous',color='black')
# plt.plot(ticks,too_fast,label='forward (fast)',color='green')
# plt.plot(ticks,too_slow,label='forward (slow)',color='orange')
# ax = plt.gca()
# ax.get_yaxis().get_major_formatter().set_scientific(False)
plt.xlabel("Time Step")
plt.ylabel("Output (M-compartment)")

plt.legend()
plt.show()

