from time import time
import numpy as np
import matplotlib.pyplot as plt
import random


D = 1e-12

class clock:
    def __init__(self):
        self.b = random.random()
        self.f = 1/random.uniform(32,33) * 1e-6
        print('b =', self.b, 'f =', self.f)
    def clockdata(self,t):
            y = self.b + self.f * t + 1/2 * D * t * t
            return y

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

C1 = clock()
C2 = clock()

nodeA = node()

M = 800

t = 0
for i in range(0,M):
    nodeA.t1.append(C1.clockdata(t))
    t = t + random.random() * 1e-1 + 0
    nodeA.t2.append(C2.clockdata(t))
    t = t + random.random() * 1e-6
    nodeA.t3.append(C2.clockdata(t))
    t = t + random.random() * 1e-1 + 0
    nodeA.t4.append(C1.clockdata(t))
    t = t + random.random() * 1e-6

t = np.arange(0,M)
plt.plot(t, nodeA.t1)

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

    print(node_v.offset_update_pidinc)
    axes_v[3].plot(t, node_v.offset_update_pidinc, 'y')
    axes_v[3].set_ylabel('时间/s')
    axes_v[3].set_ylim(-3.0e-9,2.0e-9)  
    axes_v[3].set_title('时钟偏移量(引入PID控制器同步后)')

    plt.tight_layout()


node_handle(nodeA)
plt.show()

