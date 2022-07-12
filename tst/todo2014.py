# This is just a test of the equation
# from Todo 2014 to see how the
# softmax for dendritic integration
# works.

import math

# A is a list of dendritic outputs.
# v is the positive constant.
#   v can't be too big or we get
#   and overflow.
def softmax(A, v):
    top = 0.0
    bot = 0.0

    for x in A:
        top += x*math.exp(v*x)
        bot += math.exp(v*x)

    return top/bot





denouts = [
   0.1, 0.2,0.3
]

print(softmax(denouts,100.0))