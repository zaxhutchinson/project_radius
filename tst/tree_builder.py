import math
import random
import matplotlib.pyplot as plt


class Node:
    def __init__(self,i,x,y,z):
        self.i = i
        self.x = x
        self.y = y
        self.z = z
        self.parent = None
        self.children = []
        self.path_len = 0

    def GetID(self):
        return self.i

    def distance(self, o):
        return math.sqrt(
            (o.x-self.x)**2.0 +
            (o.y-self.y)**2.0 +
            (o.z-self.z)**2.0
        )

    def AddChild(self, c):
        self.children.append(c)

    def AddParent(self, p):
        self.parent = p

    def SetPathLen(self,l):
        self.path_len = l

    def ClearConnections(self):
        self.parent = None
        self.children = []
        self.path_len = 0


def ClearTree(nodes):
    for n in nodes:
        n.ClearConnections()

def CreateNodes(nodes, num_nodes, bnds):
    for i in range(1,num_nodes+1):
        x = random.uniform(bnds[0],bnds[1])
        y = random.uniform(bnds[2],bnds[3])
        z = 0#random.uniform(bnds[4],bnds[5])

        n = Node(i,x,y,z)
        nodes.append(n)


def ConnectNodes(nodes, bf):
    C = []
    U = nodes[1:]
    C.append(nodes[0])
    while len(U) > 0:
        mc = C[0]
        mu = U[0]
        mpl = mc.path_len * bf
        md = mc.distance(mu) + mpl
        
        for c in C:
            pl = c.path_len * bf
            for u in U:
                d = c.distance(u) + pl
                if d < md:
                    md = d
                    mu = u
                    mc = c

        mu.SetPathLen(mc.distance(mu)+mc.path_len)
        mc.AddChild(mu)
        mu.AddParent(mc)
        C.append(mu)
        for u in U:
            if u.GetID()==mu.GetID():
                U.remove(u)
                break


def DisplayTree(nodes,bf):

    fig = plt.figure()
    # ax = plt.axes(projection='3d')
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.title(f"Tree Building Algorithm (bf={bf})")

    for n in nodes:
        
        for c in n.children:
            plt.plot([n.x,c.x],[n.y,c.y],color='black',zorder=1,linewidth=2)

    for n in nodes:
        if n.x==0 and n.y==0:
            plt.scatter(n.x,n.y,color='tab:orange',zorder=2)
        else:
            plt.scatter(n.x,n.y,color='tab:blue',zorder=2)

    plt.show()


def main():

    num = 100
    bounds = (-100,100,-100,100,-100,100)
    nodes = [Node(0,0,0,0)]
    CreateNodes(nodes,num,bounds)

    # 0.1
    bf = 0.1
    ConnectNodes(nodes,bf)
    DisplayTree(nodes,bf)
    ClearTree(nodes)

    # 0.25
    bf = 0.25
    ConnectNodes(nodes,bf)
    DisplayTree(nodes,bf)
    ClearTree(nodes)

    # 0.5
    bf = 0.5
    ConnectNodes(nodes,bf)
    DisplayTree(nodes,bf)
    ClearTree(nodes)

    #0.75
    bf = 0.75
    ConnectNodes(nodes,bf)
    DisplayTree(nodes,bf)
    ClearTree(nodes)

    # 0.9
    bf = 0.9
    ConnectNodes(nodes,bf)
    DisplayTree(nodes,bf)

main()