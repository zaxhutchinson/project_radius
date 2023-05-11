import math
import matplotlib.pyplot as plt

cap=100.0
vr=-60.0
vt=-40.0
k=0.7
vpeak=35.0
a=0.03
b=-2.0
c=-50.0
d=100.0

def Spiking(v,u,I):
    vnew = v + (
        k * (v - vr) * (v - vt) -
        u + I
    ) / cap
    unew = u + a * (b * (v - vr) - u)

    if v > vpeak:
        vnew = c
        unew = unew + d

    return vnew, unew


def Rate(I):
    # return math.tanh(I)
    return (I if I > 0 else 0)



def main():
    point_output = []
    v = c
    u = d
    rate_output = []
    neural_input = []
    spike_threshold = []

    time_max = 1000


    for i in range(time_max):
        neural_input.append(100*i/time_max)


    for I in neural_input:
        I = I
        v,u = Spiking(v,u,I)
        r = Rate(I)
        point_output.append(v)
        rate_output.append(r)
        spike_threshold.append(52.0)

    plt.figure()
    plt.plot(neural_input,label="Input")
    plt.plot(spike_threshold, label="Spike Threshold", linestyle='--')
    plt.xlabel("Time Step (ms)")
    plt.ylabel("Current (pA)")
    plt.legend()
    plt.show()

    plt.figure()
    plt.plot(point_output)
    plt.xlabel("Time Step")
    plt.ylabel("Membrane potential (mV)")
    plt.show()

    plt.figure()
    plt.plot(rate_output)
    plt.xlabel("Time Step (ms)")
    plt.ylabel("Output (percent of max Hz)")
    plt.show()
        
main()
        
        
        
        
        
        