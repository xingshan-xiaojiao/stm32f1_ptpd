#!/usr/bin/env python
#coding:utf8

import matplotlib.pyplot as plt
import numpy as np

class node:
    def __init__(self):
        self.t1 = []
        self.t2 = []
        self.t3 = []
        self.t4 = []

A = node()
B = node()
C = node()

filename = 'ptp_py/HILS/0.1.txt'
file = open(filename, 'r')
M = 0

while True:
    lines = file.readline()
    M = M + 1
    if not lines:
        break
    t1_tmp,t2_tmp,t3_tmp,t4_tmp = [float(x) for x in lines.split()]
    if M%3 == 1:
        A.t1.append(t1_tmp)
        A.t2.append(t2_tmp)
        A.t3.append(t3_tmp)
        A.t4.append(t4_tmp)
    if M%3 == 2:
        B.t1.append(t1_tmp)
        B.t2.append(t2_tmp)
        B.t3.append(t3_tmp)
        B.t4.append(t4_tmp)
    if M%3 == 0:
        C.t1.append(t1_tmp)
        C.t2.append(t2_tmp)
        C.t3.append(t3_tmp)
        C.t4.append(t4_tmp)

A.t1 = np.array(A.t1)
A.t2 = np.array(A.t2)
A.t3 = np.array(A.t3)
A.t4 = np.array(A.t4)

B.t1 = np.array(B.t1)
B.t2 = np.array(B.t2)
B.t3 = np.array(B.t3)
B.t4 = np.array(B.t4)

C.t1 = np.array(C.t1)
C.t2 = np.array(C.t2)
C.t3 = np.array(C.t3)
C.t4 = np.array(C.t4)

M = len(A.t1)
t = np.arange(0,M)
# 节点A
figA,axesA = plt.subplots(2,2)
plt.rcParams['font.sans-serif'] = ['SimHei'] 
plt.rcParams['axes.unicode_minus']=False    

axesA[0][0].plot(t, A.t1, 'r')
axesA[0][0].set_ylabel('时间/s')
axesA[0][0].set_title('t1(主节点发送时间)')

axesA[0][1].plot(t, A.t2, 'g')
axesA[0][1].set_ylabel('时间/s')
axesA[0][1].set_title('t2(从节点接受时间)')

axesA[1][0].plot(t, A.t3, 'b')
axesA[1][0].set_ylabel('时间/s')
axesA[1][0].set_title('t3(从节点发送时间)')

axesA[1][1].plot(t, A.t4, 'y')
axesA[1][1].set_ylabel('时间/s')
axesA[1][1].set_title('t4(主节点接受时间)')
plt.suptitle('节点A')
plt.tight_layout()
plt.savefig('A.png', dpi=400, bbox_inches = 'tight')


# 节点B
figB,axesB = plt.subplots(2,2)
plt.rcParams['font.sans-serif'] = ['SimHei'] 
plt.rcParams['axes.unicode_minus']=False    

axesB[0][0].plot(t, B.t1, 'r')
axesB[0][0].set_ylabel('时间/s')
axesB[0][0].set_title('t1(主节点发送时间)')

axesB[0][1].plot(t, B.t2, 'g')
axesB[0][1].set_ylabel('时间/s')
axesB[0][1].set_title('t2(从节点接受时间)')

axesB[1][0].plot(t, B.t3, 'b')
axesB[1][0].set_ylabel('时间/s')
axesB[1][0].set_title('t3(从节点发送时间)')

axesB[1][1].plot(t, B.t4, 'y')
axesB[1][1].set_ylabel('时间/s')
axesB[1][1].set_title('t4(主节点接受时间)')
plt.suptitle('节点B')
plt.tight_layout()
plt.savefig('B.png', dpi=400, bbox_inches = 'tight')

# 节点B
figC,axesC = plt.subplots(2,2)
plt.rcParams['font.sans-serif'] = ['SimHei'] 
plt.rcParams['axes.unicode_minus']=False    

axesC[0][0].plot(t, C.t1, 'r')
axesC[0][0].set_ylabel('时间/s')
axesC[0][0].set_title('t1(主节点发送时间)')

axesC[0][1].plot(t, C.t2, 'g')
axesC[0][1].set_ylabel('时间/s')
axesC[0][1].set_title('t2(从节点接受时间)')

axesC[1][0].plot(t, C.t3, 'b')
axesC[1][0].set_ylabel('时间/s')
axesC[1][0].set_title('t3(从节点发送时间)')

axesC[1][1].plot(t, C.t4, 'y')
axesC[1][1].set_ylabel('时间/s')
axesC[1][1].set_title('t4(主节点接受时间)')
plt.suptitle('节点C')
plt.tight_layout()
plt.savefig('C.png', dpi=400, bbox_inches = 'tight')

plt.show()

