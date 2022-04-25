# -*- coding: utf-8 -*-

from platform import node
import numpy as np
import matplotlib.pyplot as plt
import treePlotter as tp 

net1 = {'A': {0: 'B', 1: {'C': {0: 'D', 1: 'E'}},2: 'F'}}
net2 = {'A': {0: 'B', 1: {'C': {0: {'D': {0: 'F'}}, 1: 'E'}}}}
net3 = {'A': {0: {'B': {0: 'F'}}, 1: {'C': {0: 'D', 1: 'E'}}}}

def nodeLink(net):
    res = np.empty((0,2),dtype=str)
    for key in net.keys():
        first = net[key]
        if isinstance(first, dict):
            for i in range(len(first)):
                if isinstance(first[i], str):
                    res = np.append(res, [[key, first[i]]], axis=0)
                if isinstance(first[i], dict):
                    second = first[i]
                    res = np.append(res, [[key, list(first[i].keys())[0]]], axis=0)
                    tmp = nodeLink(second)
                    res = np.append(res, tmp, axis=0)
    return res

def netshow(net):
    res = nodeLink(net)
    tp.createPlot(net)
    return res

res = netshow(net1)

print(res)

plt.show()    
