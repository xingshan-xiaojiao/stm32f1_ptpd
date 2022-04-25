#!/usr/bin/env python
#coding:utf8

from cmath import nan
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
        self.offset_update_pidinc = []


class pidAbs:
    def __init__(self,kp,ki,kd,inc_lim):
        self.kp = kp
        self.ki = ki
        self.kd = kd
        self.inc_lim = inc_lim
        
        self.err_now = 0
        self.d_ctr_out = 0
        self.ctr_out = 0

        self.err_old_1 = 0
        self.err_old_2 = 0


    def update(self,target,current):
        self.err_now = target - current
        d_err_p = self.err_now - self.err_old_1
        d_err_i = self.err_now
        d_err_d = self.err_now - 2 * self.err_old_1 + self.err_old_2
        self.d_ctr_out = self.kp * d_err_p + self.ki * d_err_i + self.kd * d_err_d
        self.err_old_2 = self.err_old_1
        self.err_old_1 = self.err_now  
        
        if self.d_ctr_out < -self.inc_lim:
            self.d_ctr_out = -self.inc_lim
        if self.d_ctr_out > self.inc_lim:
            self.d_ctr_out = self.inc_lim

        self.ctr_out += self.d_ctr_out

        return self.ctr_out


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

    fig_v, axes_v = plt.subplots(4,1)
    plt.rcParams['font.sans-serif'] = ['SimHei'] 
    plt.rcParams['axes.unicode_minus'] = False  

    axes_v[0].plot(t, node_v.delay, 'r')
    axes_v[0].set_ylabel('时间/s')
    axes_v[0].set_title('链路延时量')   

    axes_v[1].plot(t, node_v.offset, 'g')
    axes_v[1].set_ylabel('时间/s')
    axes_v[1].set_title('时钟偏移量(未同步)')

    for i in range(0,M):
        offset_tmp = node_v.t1[i] - node_v.t2[i] + node_v.offset[i]
        node_v.offset_update_ptp.append(offset_tmp)

    axes_v[2].plot(t, node_v.offset_update_ptp, 'b')
    axes_v[2].set_ylabel('时间/s')
    axes_v[2].set_title('时钟偏移量(同步后)')

    pid = pidAbs(0.2,0.4,0,0.1)
    out = 0
    for i in range(0,M):
        offset_tmp = node_v.t1[i] - node_v.t2[i] + node_v.offset[i] + out
        out = pid.update(0,offset_tmp)
        node_v.offset_update_pidinc.append(offset_tmp)

    axes_v[3].plot(t, node_v.offset_update_pidinc, 'y')
    axes_v[3].set_ylabel('时间/s')
    axes_v[3].set_title('时钟偏移量(引入PID控制器同步后)')
    if node_v == nodeA:
        plt.suptitle('节点A')
        plt.tight_layout()
    if node_v == nodeB:
        plt.suptitle('节点B')
        plt.tight_layout()
    if node_v == nodeC:
        plt.suptitle('节点C')
        plt.tight_layout()

node_handle(nodeA)
node_handle(nodeB)
node_handle(nodeC)

plt.show()
