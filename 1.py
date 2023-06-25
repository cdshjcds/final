import numpy as np
N = 2000
X = np.random.randint(low=-100,high=100,size=(N,2))

from sklearn.metrics.pairwise import pairwise_distances

d = pairwise_distances(X)

d_str = str(N)+'\n'
for r in d:
    for c in r:
        d_str+=str(c)+' '
    d_str = d_str[:-1]+'\n'

with open('Project1//Project1//data.txt','w') as f:
    f.write(d_str)

