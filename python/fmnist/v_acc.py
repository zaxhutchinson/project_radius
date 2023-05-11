import matplotlib.pyplot as plt
import numpy as np
import math
import output
import defs


CORR_RATE = 3
INCORR_RATE = 1
labels = [0,5]
SIZE = len(labels)
BATCH=10
EPOCH=SIZE*BATCH

out = output.Output(defs.OUTPUT_PATH)

out.LoadExample()

START = 0
END = len(out.examples)
LENGTH = END - START

correct = []
incorrect = []
acc = []
spikes_by_label = {}
mean_spikes_by_label = {}
spikes_per_example = {}
correct = []
correct_by_id = {}
accuracy_by_id = {}

# Init dicts
for l in labels:
    spikes_by_label[l] = []
    mean_spikes_by_label[l] = []
    spikes_per_example[l] = 0
    correct_by_id[l] = []
    accuracy_by_id[l] = []

# Load neurons data
for i in range(SIZE):
    out.LoadNeuron(1,i)

# Load Synapse data
for i in range(START, END):
    for k in range(SIZE):
        spikes_by_label[labels[k]].append(len(out.neurons[f"1 {k}"].spikes[i]))

# Find the mean spikes for each neuron


# Find the correct v incorrect trials
for e in range(LENGTH//EPOCH):

    epoch_correct = 0
    epoch_correct_by_label = {}

    S = e*EPOCH
    E = S+EPOCH

    for k,v in spikes_by_label.items():
        m = np.mean(v[S:E])
        mean_spikes_by_label[k] = m
        epoch_correct_by_label[k] = 0
    
    # print(mean_spikes_by_label)

    for i in range(S,E):

        

        correct_id = int(out.examples[i].example)
        
        # Logs the spikes for each output neuron for this example
        for l in labels:
            if(mean_spikes_by_label[l] != 0):
                spikes_per_example[l] = (spikes_by_label[l][i] / mean_spikes_by_label[l])
            else:
                spikes_per_example[l] = spikes_by_label[l][i]

        # print(mean_spikes_by_label,spikes_per_example,correct_id)

        # Finds out which neuron won
        winner_label = None
        winner_spikes = -1000
        equivalent = True
        for k,v in spikes_per_example.items():
            if v > winner_spikes:
                winner_spikes = v
                winner_label = k
                equivalent = False
            elif v == winner_spikes:
                equivalent = True

        # If the winning label matches the correct id and no other neuron
        # had the same rate, correct.
        if correct_id==winner_label and equivalent == False:
            epoch_correct += 1
            epoch_correct_by_label[correct_id] += 1


    correct.append(epoch_correct/EPOCH)
    for l in labels:
        correct_by_id[l].append(epoch_correct_by_label[l]/BATCH)

# Calculate overall accuracy
total = 0
corr = 0
accuracy = []
for c in correct:
    total += 1
    corr += c
    accuracy.append(corr/total)


# Find Accuracy by label
for l in labels:
    total = 0
    corr = 0
    for c in correct_by_id[l]:
        total+=1
        corr+=c
        accuracy_by_id[l].append(corr/total)

# Plot overall accuracy
plt.figure()
plt.plot(correct)
# plt.title("Overall accuracy")
plt.xlabel("Epoch")
plt.ylabel("Accuracy")
plt.show()

# Plot accuracy for each neuron/label
plt.figure()
for k,v in correct_by_id.items():
    plt.plot(v,label=str(k))
# plt.title("Accuracy by Label")
plt.xlabel("Epoch")
plt.ylabel("Accuracy")
plt.legend()
plt.show()

plt.figure()
# for k,v in correct_by_id.items():
plt.plot(correct_by_id[0])
# plt.title("Accuracy by Label")
plt.xlabel("Epoch")
plt.ylabel("Accuracy")
# plt.legend()
plt.show()

# acc_per_block = []
# block_size = 2
# neurons_with_more_spikes = []
# spikes_by_id = {}
# correct_by_id = {}
# loss_by_id = {}
# loss = []


# for i in range(SIZE):
#     spikes_by_id[i]=[]
#     correct_by_id[i]=[]
#     loss_by_id[i] = []

# for i in range(SIZE):
#     out.LoadNeuron(1,i)


# num_corr = 0

# block_count = 0
# block_corr = 0



# for i in range(len(out.examples)):

#     # correct_id = int(out.examples[i].example)

#     for k in range(SIZE):
#         spikes_by_id[k].append(len(out.neurons[f"1 {k}"].spikes[i]))


# norm_spikes_by_id = {}
# for_acc_spikes_by_id = {}
# for i in range(SIZE):
#     norm_spikes_by_id[i]=[]
#     for_acc_spikes_by_id[i]=[]

# for k,v in spikes_by_id.items():
#     total = sum(v)
#     N = len(v)-1
#     avg = total / len(v)
#     for i in range(len(v)):
#         norm_spikes_by_id[k].append((v[i] - avg)/N)




# for k,v in spikes_by_id.items():
#     # half = v[len(v)//2:]
#     total = sum(v)
#     N = len(v)-1
#     avg = total / len(v)
#     for i in range(len(v)):
#         for_acc_spikes_by_id[k].append((v[i] - avg)/N)


# count = 1
# start = 0#7*len(out.examples)//8
# for i in range(start,len(out.examples)):

#     correct_id = int(out.examples[i].example)

#     rnd = {}
#     l=0
#     for n in range(EPOCH):
#         m = n%SIZE
#         rnd[m]=spikes_by_id[m][i]

#         if labels[m]==correct_id:
#             loss_by_id[m].append(math.sqrt((CORR_RATE-spikes_by_id[m][i])**2))
#         else:
#             loss_by_id[m].append(math.sqrt((INCORR_RATE-spikes_by_id[m][i])**2))

#         l+=loss_by_id[m][-1]

#     loss.append(l/(EPOCH))

#     maxrnd_k = None
#     maxrnd_v = -1
#     cntrnd = 0

#     for k,v in rnd.items():
#         if v > maxrnd_v:
#             maxrnd_k = k
#             maxrnd_v = v
#             cntrnd=1
#         elif v == maxrnd_v:
#             cntrnd+=1

#     corr_neuron = labels.index(correct_id)

#     if maxrnd_v==rnd[corr_neuron] and cntrnd==1:
#         correct.append(1)
#         correct_by_id[corr_neuron].append(1)
#         num_corr+=1
#         block_corr+=1
#     else:
#         correct.append(0)
#         correct_by_id[corr_neuron].append(0)

#     acc.append(num_corr/count)
#     count += 1

#     # if i%block_size==0 and i > 0:
#     #     acc_per_block.append(block_corr/block_count)
#     #     block_corr=0
#     #     block_count=0

# plt.figure()
# plt.plot(acc,label='accuracy')
# plt.legend()
# plt.show()

# plt.figure()
# plt.plot(loss,label='accuracy')
# z = np.polyfit(range(len(loss)),loss,5)
# p = np.poly1d(z)
# plt.plot(p(range(len(loss))),label="Trend line")
# plt.title("Loss")
# plt.legend()
# plt.show()

# # plt.figure()
# # # plt.plot(acc_per_block,label='accuracy per block')
# # # z = np.polyfit(range(len(acc_per_block)),acc_per_block,5)
# # # p = np.poly1d(z)
# # plt.plot(p(range(len(acc_per_block))),label="Trend line")
# # plt.legend()
# # plt.show()

# # plt.figure()
# # plt.plot(neurons_with_more_spikes,label='num_geq')
# # plt.legend()
# # plt.show()

# LEN = len(correct)//EPOCH

# avg_correct = []
# for i in range(LEN):
#     avgC = 0.0
#     for k in range(EPOCH):
#         avgC += correct[i*EPOCH+k]
#     avg_correct.append(avgC/EPOCH)


# plt.figure()
# plt.plot(avg_correct,label="Correct")
# plt.legend()
# plt.show()

# plt.figure()
# for k,v in spikes_by_id.items():
#     plt.plot(v,label=str(labels[k]))
# plt.title("Spikes by ID")
# plt.legend()
# plt.show()

# plt.figure()
# for k,v in norm_spikes_by_id.items():
#     plt.plot(v,label=str(labels[k]))
# plt.title("Norm Spikes by ID")
# plt.legend()
# plt.show()

# plt.figure()
# for k,v in loss_by_id.items():
#     plt.plot(v,label=str(labels[k]))
# plt.title("Loss by ID")
# plt.legend()
# plt.show()

# # print(correct_by_id[0])

# plt.figure()
# for k,v in correct_by_id.items():

#     A = []
#     total = 0
#     for i in range(len(v)):
#         total += v[i]
#         A.append(total/(i+1))
#     plt.plot(A,label=str(labels[k]))
# plt.legend()
# plt.title("Accuracy by ID")
# plt.show()




# ###############################
# # Norm acc




