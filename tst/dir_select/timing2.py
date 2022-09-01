# Minsky said, "By all means, help neuroscientists, but don't listen to them."
# Watching the man speak and his ever-present grin when in front of a classroom
# of students, I wonder if the man ever said anything completely devoid of
# facetiousness. I have learned at least one lesson from this field: the field
# might be called `artificial intelligence' but none of its solutions have
# any.

# If accepted as seriously made, this statement comes from a point of view,
# from someone who was there at the enthusiastic beginning,
# that is throughly smitten by its enthusiasm.
# AI was, and in some ways still is, the cute new kid (AI, not Minsky) that
# just enrolled in your sixth grade class.
# His point appears to be that the practicianers of AI are best suited to
# understand the computational nature of intelligence, not neuroscientists
# and their grubby biology. This work has tried its best to reject Minsky's
# unipolar worldview.





import math
import matplotlib.pyplot as plt
import numpy as np
import random

class Conn:
    def __init__(self,other,dist):
        self.distance = dist
        self.other = other

class Node:
    def __init__(self, _id):
        self.ID = _id
        self.signal = 0.0
        self.parent = None
        self.children = []
        self.spiketime = None
        self.b = 1.0
        self.c = 1.0

    def getsignal(self, curtime,debug=False):
        self.signal = 0.0
        mysig = 0.0
        if self.spiketime != None:
            mysig = self.witch(curtime-self.spiketime,0.0)
            self.signal = mysig
        for c in self.children:
            self.signal += c.other.getsignal(curtime-c.distance,debug)
            
            spiketime = c.other.spiketime
            if spiketime != None and self.spiketime != None:
                td = self.spiketime - spiketime
                # print(f"{self.ID} {c.other.ID} {td} {c.distance} {self.witch(td,c.distance)}")
                self.signal += self.witch(td,c.distance) * mysig
        
        if debug:
            print(f"{self.ID}: {self.signal:.8f}")
        return self.signal

    def witch(self, td, dist):
        return self.c / (((td-dist)/self.b)**2.0 + 1.0)

def linknodes(parent:Node, child:Node, dist:float) -> None:
    childconn = Conn(parent,dist)
    parentconn = Conn(child,dist)
    parent.children.append(parentconn)
    child.parent = childconn


def gauss(rate):
    if random.random()<rate:
        return True
    else:
        return False


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

nodes = [n0,n1,n2,n3,n4,n5,n6,n7,n8,n9]

linknodes(n0,n1,10)
linknodes(n0,n2,20)

linknodes(n7,n8,10)
linknodes(n8,n9,10)


n0rate = 0.0
n1rate = 0.1
n2rate = 0.1
n3rate = 0.0
n4rate = 0.0
n5rate = 0.0
n6rate = 0.0
n7rate = 0.0
n8rate = 0.1
n9rate = 0.1

rates = [
    n0rate,
    n1rate,
    n2rate,
    n3rate,
    n4rate,
    n5rate,
    n6rate,
    n7rate,
    n8rate,
    n9rate
]

roots = [
    n0,
    n7
]

data = []
for x in roots:
    data.append([])

for t in range(10000):
    
    for i in range(len(nodes)):
        rate = rates[i]
        # if i==8:
        #     if nodes[9].spiketime==t-1:
        #         rate += rate
            
        # if i==1:
        #     if nodes[2].spiketime==t-1:
        #         rate += rate
        if gauss(rate):
            nodes[i].spiketime = t

    for r in range(len(roots)):
        data[r].append(roots[r].getsignal(t))


plt.figure()
for d in range(len(data)):
    plt.plot(data[d],label=roots[d].ID)
    print(f"{roots[d].ID}: {sum(data[d])/len(data[d])}")

plt.legend()
plt.show()

