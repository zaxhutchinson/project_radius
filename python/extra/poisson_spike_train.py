import math
import random
import numpy as np
import matplotlib.pyplot as plt



###################################################################
#002 NoNo
# on_rate = 0.1
# off_rate = 0.0
# mask_rate = 0.05

# p1 = np.zeros([100,1000])
# p2 = np.zeros([100,1000])
# p3 = np.zeros([100,1000])

# for i in range(1000):
#     for n in range(100):
#         if n < 50:
#             if random.random() < on_rate:
#                 p1[n][i] = 1
#         else:
#             if random.random() < off_rate:
#                 p1[n][i] = 1

#         if n >= 50:
#             if random.random() < on_rate:
#                 p2[n][i] = 1
#         else:
#             if random.random() < off_rate:
#                 p2[n][i] = 1

#         if random.random() < mask_rate:
#             p3[n][i] = 1


# fig,axs = plt.subplots(3,sharex=True)
# axs[0].imshow(p1, cmap='binary', aspect='auto', interpolation='nearest')
# axs[0].set_title("Pattern 1",fontsize=9)
# axs[1].imshow(p2, cmap='binary', aspect='auto', interpolation='nearest')
# axs[1].set_title("Pattern 2",fontsize=9)
# axs[2].imshow(p3, cmap='binary', aspect='auto', interpolation='nearest')
# axs[2].set_title("Pattern 3",fontsize=9)
# fig.text(0.5,0.04,'Time',ha='center')
# fig.text(0.04,0.5,'Input Neurons',va='center',rotation='vertical')
# fig.text(0.5,0.95,'Example Input Spike Train', ha='center',fontsize=13)
# plt.show()

###################################################################
#002 YesYes
on_rate = 0.1
off_rate = 0.01
mask_rate = 0.05

p1 = np.zeros([100,1000])
p2 = np.zeros([100,1000])
p3 = np.zeros([100,1000])

for i in range(1000):
    for n in range(100):
        if n < 55:
            if random.random() < on_rate:
                p1[n][i] = 1
        else:
            if random.random() < off_rate:
                p1[n][i] = 1

        if n >= 55:
            if random.random() < on_rate:
                p2[n][i] = 1
        elif n <10:
            if random.random() < on_rate:
                p2[n][i] = 1
        else:
            if random.random() < off_rate:
                p2[n][i] = 1

        if random.random() < mask_rate:
            p3[n][i] = 1


fig,axs = plt.subplots(3,sharex=True)
axs[0].imshow(p1, cmap='binary', aspect='auto', interpolation='nearest')
axs[0].set_title("Pattern A",fontsize=9)
axs[1].imshow(p2, cmap='binary', aspect='auto', interpolation='nearest')
axs[1].set_title("Pattern B",fontsize=9)
axs[2].imshow(p3, cmap='binary', aspect='auto', interpolation='nearest')
axs[2].set_title("Random Mask",fontsize=9)
fig.text(0.5,0.04,'Time',ha='center')
fig.text(0.04,0.5,'Input Neurons',va='center',rotation='vertical')
fig.text(0.5,0.95,'Example Input Spike Train', ha='center',fontsize=13)
plt.show()