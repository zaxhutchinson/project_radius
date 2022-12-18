import random

#######################################################
# Finds the average interspike interval for a set of inputs
# with given spike rates. Any ISI greater than the window
# maxT is ignored. Based on the Law of Large Numbers.
#
# Code: zsh

#######################################################
# Parameters:
#
# NUM_INPUTS: number of inputs per pattern
# I: dict of spike times per input
# R: spike rates per input
# flags: tracks whether input i has spiked
# dI: interspike intervals
# T: make sim time
# maxT: max ISI. ISI beyond max will not be counted.
########################################################
NUM_INPUTS = 24
I = {}
R = [0.01]*24
flags = []
dI = {}
T = 100000
maxT = 100

# Initialization
for i in range(NUM_INPUTS):
    I[i] = []
    dI[i] = []
    flags.append(False)

# Run sim
for t in range(T):

    # Reset all flags
    for f in range(len(flags)):
        flags[f] = False

    for i in range(NUM_INPUTS):

        # if there's a spike
        if random.uniform(0,1) < R[i]:
            flags[i] = True
            dAll = 0
            dCount = 0

            # calculate the average of all ISIs less than maxT
            for k in range(NUM_INPUTS):
                if k==i: continue
                if len(I[k]) > 0:
                    d = t-I[k][-1]
                    if d <= maxT:
                        dAll += d
                        dCount += 1
            if dCount > 0:
                dI[i].append(dAll/dCount)


    # If input f had a spike, append the current sim time.
    for f in range(len(flags)):
        if flags[f]:
            I[f].append(t)

# Output
total = 0
for d in range(len(dI)):
    avg = sum(dI[d])/len(dI[d])
    print(f"{d} average: {avg}")
    total += avg
print(f"Overal average: {total/len(dI)}")