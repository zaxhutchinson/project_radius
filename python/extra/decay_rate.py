import math


x1 = 1.0
x2 = 1.0

for i in range(10):
    print(x1)
    x1 = math.exp(-0.1)*x1

for i in range(10):
    print(math.exp(-0.1*i)*x2)