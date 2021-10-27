import math
import random
import matplotlib.pyplot as plt
import numpy as np
import output
import defs

NEURON_ID = 0
LAYER_ID = 1
SYN_IDS = list(range(16))

out = output.Output(defs.OUTPUT_PATH, sid=SYN_IDS, nid=NEURON_ID, lid=LAYER_ID)
PATTERNS = [0,1,2]

CORRECT_PATTERNS = [0,1]
INCORRECT_PATTERNS = [2]



acc = []
acc_by_pattern = {}
spikes = []
corr_spikes = []
incorr_spikes = []
corr_rate = []
incorr_rate = []
rate_diff = []

for p in PATTERNS:
    acc_by_pattern[p] = []

for name,neu in out.neurons.items():
    if neu.neuron_id == NEURON_ID and neu.layer_id == LAYER_ID:
        for s in neu.spikes:
            spikes.append(len(s))

print("Verify spikes==examples ")
if len(spikes)==len(out.examples):
    print("   True")
else:
    print("   False - spikes: ", len(spikes), "  examples: ", len(examples))

for i in range(len(out.examples)):
    pattern_id = int(out.examples[i].info)
    acc_by_pattern[pattern_id].append(spikes[i])
    if pattern_id in CORRECT_PATTERNS:
        corr_spikes.append(spikes[i])
    elif pattern_id in INCORRECT_PATTERNS:
        incorr_spikes.append(spikes[i])
    else:
        print("ERROR")

print("AVG Correct:   ",np.mean(corr_spikes))
print("AVG Incorrect: ",np.mean(incorr_spikes))

s = 0
while s < len(corr_spikes):
    c = sum(corr_spikes[s:s+5])
    i = sum(incorr_spikes[s:s+5])
    corr_rate.append(c/5)
    incorr_rate.append(i/5)
    rate_diff.append(c-i)
    total = c + i
    if total > 0:
        acc.append(int(c>i))
    else:
        acc.append(0.0)
    s += 5

fig = plt.figure()
ax = fig.add_subplot()
ax.plot(acc,color='k')
ax.plot(rate_diff,color='g')
# ax.plot(corr_rate,color='b')
# ax.plot(incorr_rate,color='r')
plt.show()

for k,v in acc_by_pattern.items():
    fig = plt.figure()
    ax = fig.add_subplot()
    ax.plot(v)
    plt.title(str(k))
    plt.show()