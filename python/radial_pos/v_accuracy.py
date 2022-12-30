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
loss_by_id = {}
loss = []


for i in range(10):
    spikes_by_id[i]=[]
    correct_by_id[i]=[]
    loss_by_id[i] = []

for i in range(10):
    out.LoadNeuron(1,i)


num_corr = 0

block_count = 0
block_corr = 0

for i in range(len(out.examples)):
    # block_count += 1

    # correct_id = int(out.examples[i].example)

    rnd = []

    for k in range(10):
        rnd.append(len(out.neurons[f"1 {k}"].spikes[i]))
        spikes_by_id[k].append(rnd[k])

    # maxrnd = max(rnd)
    # cntrnd = rnd.count(maxrnd)

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

    # num_geq = 0
    # for k in range(10):
    #     if k==correct_id: continue
    #     if rnd[k] >= rnd[correct_id]:
    #         num_geq += 1

    # neurons_with_more_spikes.append(num_geq)

    # if i%block_size==0 and i > 0:
    #     acc_per_block.append(block_corr/block_count)
    #     block_corr=0
    #     block_count=0

norm_spikes_by_id = {}
for_acc_spikes_by_id = {}
for i in range(10):
    norm_spikes_by_id[i]=[]
    for_acc_spikes_by_id[i]=[]

for k,v in spikes_by_id.items():
    total = sum(v)
    N = len(v)-1
    avg = total / len(v)
    for i in range(len(v)):
        norm_spikes_by_id[k].append((v[i] - avg)/N)




for k,v in spikes_by_id.items():
    half = v[len(v)//2:]
    total = sum(half)
    N = len(half)-1
    avg = total / len(half)
    for i in range(len(half)):
        for_acc_spikes_by_id[k].append((half[i] - avg)/N)



mid = len(out.examples)//2
for i in range(len(out.examples[mid:])):
    block_count += 1
    correct_id = int(out.examples[i+mid].example)

# for k,v in spikes_by_id.items():
#     total = sum(v)
#     N = len(v)-1
#     avg = total / len(v)
#     for i in range(len(v)):
#         for_acc_spikes_by_id[k].append((v[i] - avg)/N)

# for i in range(len(out.examples)):
#     block_count += 1
#     correct_id = int(out.examples[i].example)

    rnd = []
    for m in range(10):
        l=0
        # rnd.append(spikes_by_id[m][i])
        rnd.append(for_acc_spikes_by_id[m][i])

        if m==correct_id:
            loss_by_id[m].append(math.sqrt((20-spikes_by_id[m][i])**2))
        else:
            loss_by_id[m].append(math.sqrt((0-spikes_by_id[m][i])**2))

        l+=loss_by_id[m][-1]

    loss.append(l/10)

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
plt.plot(loss,label='accuracy')
z = np.polyfit(range(len(loss)),loss,5)
p = np.poly1d(z)
plt.plot(p(range(len(loss))),label="Trend line")
plt.title("Loss")
plt.legend()
plt.show()

plt.figure()
# plt.plot(acc_per_block,label='accuracy per block')
# z = np.polyfit(range(len(acc_per_block)),acc_per_block,5)
# p = np.poly1d(z)
plt.plot(p(range(len(acc_per_block))),label="Trend line")
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
for k,v in spikes_by_id.items():
    plt.plot(v,label=str(k))
plt.title("Spikes by ID")
plt.legend()
plt.show()

plt.figure()
for k,v in norm_spikes_by_id.items():
    plt.plot(v,label=str(k))
plt.title("Norm Spikes by ID")
plt.legend()
plt.show()

plt.figure()
for k,v in loss_by_id.items():
    plt.plot(v,label=str(k))
plt.title("Loss by ID")
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




