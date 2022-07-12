import random
import math
import matplotlib.pyplot as plt

strength = 500
signals = []
signal = 0.0
decay = math.exp(-1/20)
rate = 0.1
spikes = 0

for i in range(1000):
    
    if random.random() < rate:
        signal = strength
        spikes+=1
    else:
        signal = signal * decay
    signals.append(signal)
    
print(spikes)
plt.plot(signals)
plt.show()