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
        self.delay = []
        self.offset = []
        self.offset_update_ptp = []
        self.offset_update_kalman = []


nodeA = node()
nodeB = node()
nodeC = node()

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
        nodeA.t1.append(t1_tmp)
        nodeA.t2.append(t2_tmp)
        nodeA.t3.append(t3_tmp)
        nodeA.t4.append(t4_tmp)
    if M%3 == 2:
        nodeB.t1.append(t1_tmp)
        nodeB.t2.append(t2_tmp)
        nodeB.t3.append(t3_tmp)
        nodeB.t4.append(t4_tmp)
    if M%3 == 0:
        nodeC.t1.append(t1_tmp)
        nodeC.t2.append(t2_tmp)
        nodeC.t3.append(t3_tmp)
        nodeC.t4.append(t4_tmp)

nodeA.t1 = np.array(nodeA.t1)
nodeA.t2 = np.array(nodeA.t2)
nodeA.t3 = np.array(nodeA.t3)
nodeA.t4 = np.array(nodeA.t4)

nodeB.t1 = np.array(nodeB.t1)
nodeB.t2 = np.array(nodeB.t2)
nodeB.t3 = np.array(nodeB.t3)
nodeB.t4 = np.array(nodeB.t4)

nodeC.t1 = np.array(nodeC.t1)
nodeC.t2 = np.array(nodeC.t2)
nodeC.t3 = np.array(nodeC.t3)
nodeC.t4 = np.array(nodeC.t4)


def node_handle(node_v):
    M = len(node_v.t1)
    t = np.arange(0,M)
    for i in range(M):
        delay_tmp = (node_v.t2[i] - node_v.t1[i] + node_v.t4[i] - node_v.t3[i])/2
        node_v.delay.append(delay_tmp)
        offset_tmp = (node_v.t2[i] - node_v.t1[i] + node_v.t3[i] - node_v.t4[i])/2
        node_v.offset.append(offset_tmp)

    node_v.delay = np.array(node_v.delay)
    node_v.offset = np.array(node_v.offset)

    fig_v, axes_v = plt.subplots(3,1)
    plt.rcParams['font.sans-serif'] = ['SimHei'] 
    plt.rcParams['axes.unicode_minus'] = False  
    axes_v[0].plot(t, node_v.offset, 'r')
    axes_v[0].set_ylabel('时间/s')
    axes_v[0].set_title('时钟偏移量(未同步)')

    node_v.offset_update_ptp.append(node_v.offset[0]) # 初始时钟偏差

    for i in range(1,M):
        offset_tmp = node_v.offset[i] - node_v.offset[i-1] # 上一时刻修正当前时刻
        node_v.offset_update_ptp.append(offset_tmp)

    # print(node_v.offset_update_ptp)
    axes_v[1].plot(t, node_v.offset_update_ptp, 'g')
    axes_v[1].set_xlim(-50,950)
    if node_v == nodeA:
        axes_v[1].set_ylim(2.0e-7,4.5e-7)
    if node_v == nodeB:
        axes_v[1].set_ylim(-0.5e-7,1.5e-7)
    if node_v == nodeC:
        axes_v[1].set_ylim(0.9e-7,2.1e-7)       
    axes_v[1].set_ylabel('时间/s')
    axes_v[1].set_title('时钟偏移量(同步后)')

    # 卡尔曼滤波器
    I = np.mat([[1,0],[0,1]])
    A = np.mat([[1,0.1],[0,1]])
    B = np.mat([[-1,-0.1],[0,-1]])

    x = np.mat([[0],[0]])
    u = np.mat([[0],[0]])

    P = [[np.var(np.random.randn(1,M)),0],
        [0,np.var(np.random.randn(1,M))]]
    Q = P
    R = [[np.var(np.random.randn(1,M)),0],
        [0,np.var(np.random.randn(1,M))]]

    node_offset_k_tmp = []
    for i in range(0,M):
        x = A * x + B * u
        P = A * P * A.T + Q
        K = P * np.linalg.inv(R + P)
        z_off = node_v.offset[i]
        z = [[z_off],[0]]
        x = x + K * (z - x)
        u = x
        node_offset_k_tmp.append(x[0,0])
        P = (I-K) * P    

    node_v.offset_update_kalman.append(node_offset_k_tmp[0])
    for i in range(1,M):
        offset_tmp = node_offset_k_tmp[i] - node_offset_k_tmp[i-1] # 上一时刻修正当前时刻
        node_v.offset_update_kalman.append(offset_tmp)

    # print(node_v.offset_update_kalman)
    axes_v[2].plot(t, node_v.offset_update_kalman, 'b')
    axes_v[2].set_xlim(-50,950)
    if node_v == nodeA:
        axes_v[2].set_ylim(1.3e-7,2.7e-7)
    if node_v == nodeB:
        axes_v[2].set_ylim(-0.30e-7,1.0e-7)
    if node_v == nodeC:
        axes_v[2].set_ylim(0.60e-7,1.3e-7)
    axes_v[2].set_ylabel('时间/s')
    axes_v[2].set_title('时钟偏移量(引入卡尔曼滤波器同步后)')
    if node_v == nodeA:
        plt.suptitle('节点A')
        plt.tight_layout()
        plt.savefig('A_.png', dpi=400, bbox_inches = 'tight')
    if node_v == nodeB:
        plt.suptitle('节点B')
        plt.tight_layout()
        plt.savefig('B_.png', dpi=400, bbox_inches = 'tight')
    if node_v == nodeC:
        plt.suptitle('节点C')
        plt.tight_layout()
        plt.savefig('C_.png', dpi=400, bbox_inches = 'tight')

node_handle(nodeA)
node_handle(nodeB)
node_handle(nodeC)

plt.show()
