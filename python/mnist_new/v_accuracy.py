import matplotlib.pyplot as plt
import numpy as np
import math
import output
import defs


out = output.Output(defs.OUTPUT_PATH)

out.LoadExample()

correct = []
incorrect = []
acc = []
acc_per_block = []
block_size = 100
neurons_with_more_spikes = []
spikes_by_id = {}
correct_by_id = {}
for i in range(10):
    spikes_by_id[i]=[]
    correct_by_id[i]=[]

for i in range(10):
    out.LoadNeuron(1,i)


num_corr = 0

block_count = 0
block_corr = 0

for i in range(len(out.examples)):
    # block_count += 1

    correct_id = int(out.examples[i].example)

    rnd = []

    for k in range(10):
        rnd.append(len(out.neurons[f"1 {k}"].spikes[i]))
        spikes_by_id[k].append(rnd[k])

    maxrnd = max(rnd)
    cntrnd = rnd.count(maxrnd)

    # for m in range(10):
    #     if correct_id==m:
    #         if maxrnd==rnd[m]:
    #             correct_by_id[m].append(1)
    #         else:
    #             correct_by_id[m].append(0)
    #     else:
    #         if maxrnd!=rnd[m]:
    #             correct_by_id[m].append(1)
    #         else:
    #             correct_by_id[m].append(0)

    # if maxrnd==rnd[correct_id] and cntrnd==1:
    #     correct.append(1)
    #     # correct_by_id[correct_id].append(1)
    #     # num_corr += 1
    #     # block_corr += 1
    # else:
    #     correct.append(0)
        # correct_by_id[correct_id].append(0)
    # acc.append(num_corr/(i+1))

    num_geq = 0
    for k in range(10):
        if k==correct_id: continue
        if rnd[k] >= rnd[correct_id]:
            num_geq += 1

    neurons_with_more_spikes.append(num_geq)

    # if i%block_size==0 and i > 0:
    #     acc_per_block.append(block_corr/block_count)
    #     block_corr=0
    #     block_count=0

norm_spikes_by_id = {}
for i in range(10):
    norm_spikes_by_id[i]=[]

for k,v in spikes_by_id.items():
    total = sum(v)
    N = len(v)-1
    avg = total / len(v)
    for i in range(len(v)):
        norm_spikes_by_id[k].append((v[i] - avg)/N)

for i in range(len(out.examples)//2,len(out.examples)):
    block_count += 1
    correct_id = int(out.examples[i].example)

    rnd = []
    for m in range(10):
        rnd.append(norm_spikes_by_id[m][i])

    maxrnd = max(rnd)
    cntrnd = rnd.count(maxrnd)

    if maxrnd==rnd[correct_id] and cntrnd==1:
        correct.append(1)
        correct_by_id[correct_id].append(1)
        num_corr+=1
        block_corr+=1
    else:
        correct.append(0)
        correct_by_id[correct_id].append(0)

    acc.append(num_corr/(i+1))

    if i%block_size==0 and i > 0:
        acc_per_block.append(block_corr/block_count)
        block_corr=0
        block_count=0

plt.figure()
plt.plot(acc,label='accuracy')
plt.legend()
plt.show()

plt.figure()
plt.plot(acc_per_block,label='accuracy per block')
# z = np.polyfit(range(len(acc_per_block)),acc_per_block,2)
# p = np.poly1d(z)
# plt.plot(p(range(len(acc_per_block))),label="Trend line")
plt.legend()
plt.show()

# plt.figure()
# plt.plot(neurons_with_more_spikes,label='num_geq')
# plt.legend()
# plt.show()

plt.figure()
plt.plot(correct,label="Correct")
plt.legend()
plt.show()

plt.figure()
for k,v in norm_spikes_by_id.items():
    plt.plot(v,label=str(k))
plt.legend()
plt.show()

plt.figure()
for k,v in correct_by_id.items():
    A = []
    total = 0
    for i in range(len(v)):
        total += v[i]
        A.append(total/(i+1))
    plt.plot(A,label=str(k))
plt.legend()
plt.show()




###############################
# Norm acc




