import random


A = []
B = []
dA = []
dB = []
T = 1000000
maxT = 100

rateA = 0.01
rateB = 0.01

for t in range(T):
    a = False
    b = False
    if random.random() < rateA:
        a = True
        if len(B) > 0:
            d = t-B[-1]
            if d <= maxT:
                dA.append(d)
    if random.random() < rateB:
        b = True
        if len(A) > 0:
            d = t-A[-1]
            if d <= maxT:
                dB.append(d)

    if a:
        A.append(t)
    if b:
        B.append(t)


print(f"A avgerage: {sum(dA)/len(dA)}")
print(f"B avgerage: {sum(dB)/len(dB)}")