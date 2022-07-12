

def woa(x, a, b, c):
    return c / (( (x-a)/b )**2 + 1 )


A1 = 10
B1 = 10
C1 = 1
X1 = 20

A2 = 10
B2 = 10
C2 = 1
X2 = 5

W1 = woa(X1,A1,B1,C1)
W2 = woa(X2,A2,B2,C2)

print("1: ", W1)
print("2: ", W2)

print(W2*W1)
print(woa( (X1+X2)/2, A1,B1,C1))