import matplotlib.pyplot as plt
import math


def F(tc, ti, di):
    return 1.0 / (
        ((tc-ti)-di)**2 + 1.0
    )



wA = []
wB = []
wC = []
wD = []

S1 = 80
SA = 60
SB = 50
SC = 60
SD = 75
DA = 15
DB = 5
DC = 20
DD = 0

s = 50
e = 90
xaxis = list(range(s,e))

for i in range(s,e):
    wA.append(F(S1,i,DA))
    wB.append(F(S1,i,DB))
    wC.append(F(S1,i,DC))
    wD.append(F(S1,i,DD))


fig,axs = plt.subplots(2,2,sharex=True,sharey=True)
fig.supxlabel("Time")
fig.supylabel(r"$\hat{w}$")
axs[0,0].plot(xaxis,wA,color='black')
axs[0,0].set_title("A")
axs[0,1].plot(xaxis,wB,color='black')
axs[0,1].set_title("B")
axs[1,0].plot(xaxis,wC,color='black')
axs[1,0].set_title("C")
axs[1,1].plot(xaxis,wD,color='black')
axs[1,1].set_title("D")

axs[0,0].axvline(x = SA, color = 'r', label = 'axvline - full height')
axs[0,1].axvline(x = SB, color = 'r', label = 'axvline - full height')
axs[1,0].axvline(x = SC, color = 'r', label = 'axvline - full height')
axs[1,1].axvline(x = SD, color = 'r', label = 'axvline - full height')

plt.show()