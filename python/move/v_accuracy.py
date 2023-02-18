import matplotlib.pyplot as plt
import numpy as np
import math
import output
import defs


CORR_RATE = 10
INCORR_RATE = 1
labels = [0,1]
SIZE = len(labels)
BATCH=10
EPOCH=SIZE*BATCH

out = output.Output(defs.OUTPUT_PATH)

out.LoadExample()

correct = []
incorrect = []
acc = []
acc_per_block = []
block_size = 2
neurons_with_more_spikes = []
spikes_by_id = {}
correct_by_id = {}
loss_by_id = {}
loss = []
spike_bins_corr = []
spike_bins_incorr = []
spikes_corr = []
spikes_incorr = []
for i in range(1100):
    spike_bins_corr.append(0)
    spike_bins_incorr.append(0)

for i in range(SIZE):
    spikes_by_id[i]=[]
    correct_by_id[i]=[]
    loss_by_id[i] = []

for i in range(SIZE):
    out.LoadNeuron(1,i)


num_corr = 0

block_count = 0
block_corr = 0


START = 0
END = 1100
for i in range(len(out.examples)):

    correct_id = int(out.examples[i].example)

    for k in range(SIZE):
        spikes = out.neurons[f"1 {k}"].spikes[i]

        spikes_in_range = []
        for s in spikes:
            if START <= s <= END:
                spikes_in_range.append(s-START)
        spikes_by_id[k].append(len(spikes_in_range))


        if correct_id==k:
            spikes_corr.append(len(spikes_in_range))
            for s in spikes_in_range:
                spike_bins_corr[s]+=1
                
        else:
            spikes_incorr.append(len(spikes_in_range))
            for s in spikes_in_range:
                spike_bins_incorr[s]+=1


norm_spikes_by_id = {}
for_acc_spikes_by_id = {}
for i in range(SIZE):
    norm_spikes_by_id[i]=[]
    for_acc_spikes_by_id[i]=[]

for k,v in spikes_by_id.items():
    total = sum(v)
    N = len(v)-1
    avg = total / len(v)
    for i in range(len(v)):
        norm_spikes_by_id[k].append((v[i] - avg)/N)




for k,v in spikes_by_id.items():
    # half = v[len(v)//2:]
    total = sum(v)
    N = len(v)-1
    avg = total / len(v)
    for i in range(len(v)):
        for_acc_spikes_by_id[k].append((v[i] - avg)/N)


count = 1
start = 0#7*len(out.examples)//8
for i in range(start,len(out.examples)):

    correct_id = int(out.examples[i].example)

    rnd = {}
    l=0
    for n in range(EPOCH):
        m = n%SIZE
        rnd[m]=spikes_by_id[m][i]

        if labels[m]==correct_id:
            loss_by_id[m].append(math.sqrt((CORR_RATE-spikes_by_id[m][i])**2))
        else:
            loss_by_id[m].append(math.sqrt((INCORR_RATE-spikes_by_id[m][i])**2))

        l+=loss_by_id[m][-1]

    loss.append(l/(EPOCH))

    maxrnd_k = None
    maxrnd_v = -1
    cntrnd = 0

    for k,v in rnd.items():
        if v > maxrnd_v:
            maxrnd_k = k
            maxrnd_v = v
            cntrnd=1
        elif v == maxrnd_v:
            cntrnd+=1

    corr_neuron = labels.index(correct_id)
    # print(correct_id,corr_neuron)

    if maxrnd_v==rnd[corr_neuron] and cntrnd==1:
        correct.append(1)
        correct_by_id[corr_neuron].append(1)
        num_corr+=1
        block_corr+=1
    else:
        correct.append(0)
        correct_by_id[corr_neuron].append(0)

    acc.append(num_corr/count)
    count += 1

    # if i%block_size==0 and i > 0:
    #     acc_per_block.append(block_corr/block_count)
    #     block_corr=0
    #     block_count=0

total_corr = sum(spike_bins_corr)
total_incorr = sum(spike_bins_incorr)
spike_bins_corr = np.divide(spike_bins_corr,total_corr)
spike_bins_incorr = np.divide(spike_bins_incorr,total_incorr)



plt.figure()
plt.plot(spike_bins_corr,label='Correct',color='tab:blue')
plt.xlabel("Time step")
plt.ylabel("Probability of S-comp output")
plt.legend()
plt.show()

plt.figure()
plt.plot(spike_bins_incorr,label='Incorrect',color='tab:orange')
plt.xlabel("Time step")
plt.ylabel("Probability of S-comp output")
plt.legend()
plt.show()

corr = []
for i in range(len(spikes_corr)):
    if spikes_corr[i] >= spikes_incorr[i]:
        corr.append(1)
    else:
        corr.append(0)

corr_by_epoch = []
spikes_corr_by_epoch = []
spikes_incorr_by_epoch = []
for i in range(0,len(corr),EPOCH):
    corr_by_epoch.append(sum(corr[i:i+EPOCH])/EPOCH)
    spikes_corr_by_epoch.append(sum(spikes_corr[i:i+EPOCH])/EPOCH)
    spikes_incorr_by_epoch.append(sum(spikes_incorr[i:i+EPOCH])/EPOCH)



plt.figure()
plt.plot(spikes_corr_by_epoch,label="Correct")
plt.plot(spikes_incorr_by_epoch,label="Incorrect")
plt.xlabel("Epoch")
plt.ylabel("Number of spikes")
plt.legend()
plt.show()

plt.figure()
plt.plot(corr_by_epoch)
plt.show()


acc_by_epoch = []
L = len(acc)//EPOCH

for i in range(L):
    avgC = 0.0
    for k in range(EPOCH):
        avgC += acc[i*EPOCH+k]
    acc_by_epoch.append(avgC/EPOCH)

plt.figure()
plt.plot(acc_by_epoch,color='black')
plt.xlabel("Epoch")
plt.ylabel("Accuracy")
# plt.legend()
plt.show()

plt.figure()
plt.plot(loss,label='accuracy')
z = np.polyfit(range(len(loss)),loss,5)
p = np.poly1d(z)
plt.plot(p(range(len(loss))),label="Trend line")
plt.title("Loss")
plt.legend()
plt.show()

# plt.figure()
# # plt.plot(acc_per_block,label='accuracy per block')
# # z = np.polyfit(range(len(acc_per_block)),acc_per_block,5)
# # p = np.poly1d(z)
# plt.plot(p(range(len(acc_per_block))),label="Trend line")
# plt.legend()
# plt.show()

# plt.figure()
# plt.plot(neurons_with_more_spikes,label='num_geq')
# plt.legend()
# plt.show()

LEN = len(correct)//EPOCH

avg_correct = []
for i in range(LEN):
    avgC = 0.0
    for k in range(EPOCH):
        avgC += correct[i*EPOCH+k]
    avg_correct.append(avgC/EPOCH)


plt.figure()
plt.plot(avg_correct,label="Correct")
plt.legend()
plt.show()

plt.figure()
for k,v in spikes_by_id.items():
    plt.plot(v,label=str(labels[k]))
plt.title("Spikes by ID")
plt.legend()
plt.show()

plt.figure()
for k,v in norm_spikes_by_id.items():
    plt.plot(v,label=str(labels[k]))
plt.title("Norm Spikes by ID")
plt.legend()
plt.show()

plt.figure()
for k,v in loss_by_id.items():
    plt.plot(v,label=str(labels[k]))
plt.title("Loss by ID")
plt.legend()
plt.show()

# print(correct_by_id[0])

LABELS = ['Vertical','Horizontal']

per_epoch = {0:[],1:[]}

for k,v in correct_by_id.items():
    for i in range(0,len(v),EPOCH):
        per_epoch[k].append(sum(v[i:i+EPOCH])/EPOCH)



plt.figure()
for k,v in per_epoch.items():
    A = []
    total = 0
    for i in range(len(v)):
        total += v[i]
        A.append(total/(i+1))
    plt.plot(A,label=str(LABELS[k]))
plt.legend()
plt.xlabel("Epoch")
plt.ylabel("Accuracy")
# plt.title("Accuracy by ID")
plt.show()

plt.figure()
for k,v in correct_by_id.items():

    A = []
    total = 0
    for i in range(len(v)):
        total += v[i]
        A.append(total/(i+1))
    plt.plot(A,label=str(LABELS[k]))
plt.legend()
plt.title("Accuracy by ID")
plt.show()




###############################
# Norm acc




