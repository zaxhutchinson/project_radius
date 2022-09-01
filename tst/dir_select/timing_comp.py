import math
import matplotlib.pyplot as plt
import numpy as np

class Conn:
    def __init__(self,other,dist, new_comp):
        self.distance = dist
        self.new_comp = new_comp
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
            mysig = self.witch(curtime-self.spiketime,0.0,False)
        for c in self.children:
            sig = c.other.getsignal(curtime-c.distance,debug)
            
            spiketime = c.other.spiketime
            if spiketime != None and self.spiketime != None:
                td = self.spiketime - spiketime
                # print(f"{self.ID} {c.other.ID} {td} {c.distance} {self.witch(td,c.distance)}")
                sig += self.witch(td,c.distance,c.new_comp) * mysig

            if c.new_comp:
                sig = math.tanh(sig)

            mysig += sig
        
        self.signal = mysig

        if debug:
            print(f"{self.ID}: {self.signal:.8f}")
        return self.signal

    def witch(self, td, dist, comp):
        return self.c / (((td-dist)/self.b)**2.0 + 1.0)

def linknodes(parent:Node, child:Node, dist:float, newcomp:bool) -> None:
    childconn = Conn(parent,dist,newcomp)
    parentconn = Conn(child,dist,newcomp)
    parent.children.append(parentconn)
    child.parent = childconn

def main():
    Test2()

def Test1():
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

    linknodes(n0,n1,10.0,False)
    linknodes(n1,n2,10.0,False)
    linknodes(n2,n3,10.0,False)
    linknodes(n3,n4,10.0,False)
    linknodes(n4,n5,10.0,False)
    linknodes(n5,n6,10.0,False)
    linknodes(n6,n7,10.0,False)
    linknodes(n7,n8,10.0,False)
    linknodes(n8,n9,10.0,False)

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

    forward = np.zeros([100])
    for i in range(0,100):
        ticks.append(i+40)
        forward[i]=n0.getsignal(i+40)

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

    backward = np.zeros([100])
    for i in range(0,100):
        backward[i]=n0.getsignal(i+40)

    n0.spiketime = 40.0
    n1.spiketime = 40.0
    n2.spiketime = 40.0
    n3.spiketime = 40.0
    n4.spiketime = 40.0
    n5.spiketime = 40.0
    n6.spiketime = 40.0
    n7.spiketime = 40.0
    n8.spiketime = 40.0
    n9.spiketime = 40.0

    simultaneous = np.zeros([100])
    for i in range(0,100):
        simultaneous[i]=n0.getsignal(i+40)

    plt.figure()
    plt.plot(ticks,forward,label='forward')
    plt.plot(ticks,backward,label='backward')
    plt.plot(ticks,simultaneous,label='simultaneous')

    plt.legend()
    plt.show()

def Test2():
    N = []
    for i in range(16):
        N.append(Node(f"n{i}"))

    comp_bounds = [
        1
    ]

    for i in range(7):
        for j in range(1,3):
            print(f"Linking: {i} to {2*i+j}")
            if i in comp_bounds:
                linknodes(N[i],N[2*i+j],10.0,True)
            else:
                linknodes(N[i],N[2*i+j],10.0,False)

    for i in range(16):
        time = 10.0*(3-int(math.log(i+0.001)))
        N[i].spiketime=time
        print(f"{i}: {time}")
        
    ticks = []

    signal = np.zeros([100])
    for i in range(0,100):
        ticks.append(i)
        signal[i]=N[0].getsignal(i)

    plt.figure()
    plt.plot(ticks,signal,label='signal')
    
    plt.legend()
    plt.show()








if __name__ == "__main__":
    main()