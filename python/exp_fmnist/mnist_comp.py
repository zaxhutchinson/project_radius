import mnist
import numpy as np
from scipy.stats.stats import pearsonr

def minmaxNorm(arr, mn=None, mx=None):
    # if not mx:
    #     mx = np.amax(arr)
    # if not mn:
    #     mn = np.amin(arr)
    mxmn = mx-mn
    shape = np.shape(arr)
    rtn = np.zeros(shape)

    for i in range(shape[0]):
        rtn[i] = (arr[i]-mn) / mxmn

    return rtn

# Import the SYN dists
SYN_DIGITS = [0,1,2,3,4,5,6,7,8,9]
syn_data = {}
syn_indx = {}
for d in SYN_DIGITS:
    data = []
    indx = []
    with open('MNIST_'+str(d)) as f:
        for line in f:
            line = line.strip().split()
            indx.append(int(line[0]))
            data.append(float(line[1]))

    syn_data[d] = data
    syn_indx[d] = indx

            
        
MNIST_DIGIT = 8
mndata = mnist.MNIST('../../exp_mnist/')
images, labels = mndata.load_testing()
images_to_use = []
num_images_to_use = 0

for i in range(len(labels)):
    if labels[i]==MNIST_DIGIT:
        images_to_use.append(images[i])
    # if len(images_to_use)==1:
    #     break

num_images_to_use = len(images_to_use)

winners=[]
for im in images_to_use:
    match = {}
    for d in SYN_DIGITS:
        arr = []
        for i in syn_indx[d]:
            arr.append(im[i])
    

        #arr = minmaxNorm(arr,mn=0,mx=255)
        mu = np.mean(np.absolute(arr))
        match[d]=mu
    minv = 0.0
    mink = 0
    for k,v in match.items():
        if v > minv:
            minv = v
            mink = k
    winners.append((mink,minv))

# print(winners)

win_list = {}
for i in SYN_DIGITS:
    win_list[i]=0

for w in winners:
    if w[0] in win_list:
        win_list[w[0]]+=1

print("Results for images of "+str(MNIST_DIGIT))
for i in SYN_DIGITS:
    print(i,win_list[i]/num_images_to_use)