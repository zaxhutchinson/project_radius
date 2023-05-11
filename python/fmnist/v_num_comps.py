import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1 import make_axes_locatable
import numpy as np
import math
import output
import defs
import random
import mycolors

out = output.Output(defs.OUTPUT_PATH)
LID = 1
NID = 0
labels = [0,5]
SIZE = len(labels)
BATCH=10
EPOCH=SIZE*BATCH

WEIGHT = 500

# for i in range(784):
#     out.LoadSynapse(LID,NID,i,False)

out.LoadNeuron(LID,NID)
out.LoadExample()

LEN = len(out.examples)//EPOCH

comps = []

compsize = []
for i in range(LEN):
    comps.append(set())
    compsize.append({})

for i in range(784):
    syn = out.LoadSynapse(LID,NID,i,False)

    

    for c in range(LEN):
        index = c*EPOCH
        comps[c].add(syn.compartments[index][-1])


        if syn.compartments[index][-1] in compsize[c]:
            compsize[c][syn.compartments[index][-1]] += 1
        else:
            compsize[c][syn.compartments[index][-1]] = 1

compsizes = []
large_compsizes = []
large_counts = []
LARGE_COMP_SIZE = 2
for i in range(len(compsize)):

    # comp_mean = np.mean(list(compsize[i].values()))
    total = 0
    count = 0
    large_total = 0
    large_count = 0
    for k,v in compsize[i].items():
        
        total += v
        count += 1

        if v > LARGE_COMP_SIZE:
            large_count+=1
            large_total+=v

    compsizes.append(total/count)
    if(large_count > 0):
        large_compsizes.append(large_total/large_count)
    else:
        large_compsizes.append(0)
    large_counts.append(large_count)


    # lats.append(syn.lats[-1][-1])
    # lons.append(syn.lons[-1][-1])
    # rads.append(syn.rads[-1][-1])
    # strs.append(syn.strengths[-1][-1])
    # comps.append(syn.compartments[-1][-1])
    # actual_strs.append(strs[-1]*WEIGHT/(abs(strs[-1])+WEIGHT))
    # comp_strs.append(syn.comp_strs[-1][-1])
    

num_comps = []
for s in comps:
    num_comps.append(len(s))


plt.figure()
plt.plot(num_comps)
plt.xlabel("Epoch")
plt.ylabel("Number of compartments")
plt.show()


plt.figure()
plt.plot(compsizes)
plt.xlabel("Epoch")
plt.ylabel("Mean compartment size")
plt.show()

plt.figure()
plt.plot(large_compsizes)
plt.xlabel("Epoch")
plt.ylabel("Mean compartment size")
plt.show()

plt.figure()
plt.plot(large_counts)
plt.xlabel("Epoch")
plt.ylabel("Number of compartments")
plt.show()

# num_comps = max(comps)
# comp_colors = []
# for i in range(num_comps+1):
#     comp_colors.append((
#         random.uniform(0.2,0.8),
#         random.uniform(0.2,0.8),
#         random.uniform(0.2,0.8)
#     ))
#     comp_sizes.append(0)

# for x in comps:
#     # colors.append(comp_colors[x])
#     comp_sizes[x]+=1

# max_comp_size = max(comp_sizes)
# comp_alphas = []
# for s in comp_sizes:
#     comp_alphas.append(s/max_comp_size)
    
# plt.figure()
# sc = plt.scatter(lons,lats,c=strs,cmap="turbo")
# plt.colorbar(sc)
# plt.xlabel("Longitude")
# plt.ylabel("Latitude")
# # plt.title("STRENGTHS")
# plt.show()

# plt.figure()
# sc = plt.scatter(lons,lats,c=rads,cmap="turbo")
# plt.colorbar(sc)
# plt.xlabel("Longitude")
# plt.ylabel("Latitude")
# # plt.title("RADII")
# plt.show()



# plt.figure()
# plt.gca().invert_yaxis()
# for c in range(784):
#     plt.plot(c%28,c//28,color=colors[comps[c]],marker='s',markersize=10)#,alpha=comp_alphas[comps[c]])    
# plt.show()

# plt.figure()
# plt.scatter(nlon,nlat,c='black')
# sc = plt.scatter(lons,lats,c=colors)
# plt.colorbar(sc)
# plt.xlabel("Longitude")
# plt.ylabel("Latitude")
# # plt.title("RADII")
# plt.show()

# x = []
# y = []
# for i in range(784):
#     x.append(i%28)
#     y.append(i//28)


# plt.figure()
# plt.gca().invert_yaxis()
# sc = plt.scatter(x,y,c=np.array(strs),cmap='bwr',marker='s',linewidths=4)  
# plt.colorbar(sc)  
# plt.show()


# plt.figure()
# plt.gca().invert_yaxis()
# sc = plt.scatter(x,y,c=np.array(actual_strs),cmap='bwr',marker='s',linewidths=4)  
# plt.colorbar(sc)  
# plt.show()

# plt.figure()
# plt.gca().invert_yaxis()
# sc = plt.scatter(x,y,c=np.array(comp_strs),cmap='bwr',marker='s',linewidths=4)  
# plt.colorbar(sc)
# plt.title("Compartment Strengths by Synapse")
# plt.show()





# dist_matrix = np.zeros([28,28])
# avg_dists = np.zeros([784])


# for i in range(784):

#     dist = 0.0

#     for k in range(784):

#         alon = lons[k] - lons[i]
#         alat = lats[k] - lats[i]
#         a = math.pow( math.sin(alat/2.0), 2.0 ) + \
#             math.cos(lats[i]) * math.cos(lats[k]) * \
#             math.pow( math.sin(alon/2.0), 2.0 )
#         if a > 1:
#             a = 1.0
#         if a < 0:
#             a = 0.0
        
#         avg_dists[i] += 2.0 * math.atan2(math.sqrt(a), math.sqrt(1.0-a))

#     avg_dists[i] /= 784
#         # dist_matrix[k1][k2] += dist
    

# for i in range(len(avg_dists)):
#     dist_matrix[i//28][i%28] = avg_dists[i]

# ax = plt.subplot()
# plt.title("Avg angular distance")
# im = ax.imshow(dist_matrix, cmap='jet', aspect='auto', interpolation='nearest')
# divider = make_axes_locatable(ax)
# cax = divider.append_axes("right", size="5%", pad=0.05)
# plt.colorbar(im, cax=cax)
# plt.show()





# comp_heat_map = np.zeros([28,28])
# for k in range(784):
#     c = comps[k]
#     comp_heat_map[k//28][k%28] = comp_sizes[c]

# ax = plt.subplot()
# plt.title("Compartment size by neural input")
# im = ax.imshow(comp_heat_map, cmap='coolwarm', aspect='auto', interpolation='nearest')
# divider = make_axes_locatable(ax)
# cax = divider.append_axes("right", size="5%", pad=0.05)
# plt.colorbar(im, cax=cax)
# plt.show()