import matplotlib.pyplot as plt
import numpy as np
import time
import decimal

class node:
    def __init__(self):
        self.t1 = []
        self.t2 = []
        self.t3 = []
        self.t4 = []

        self.delay = []
        self.offset = []
        self.offset_update_pid = []

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

        self.ctr_out = self.d_ctr_out

        return self.ctr_out

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
    t1_tmp,t2_tmp,t3_tmp,t4_tmp = [decimal.Decimal(x) for x in lines.split()]
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

def node_handle(node_v):

    node_v.t1 = np.array(node_v.t1)
    node_v.t2 = np.array(node_v.t2)
    node_v.t3 = np.array(node_v.t3)
    node_v.t4 = np.array(node_v.t4)

    M = len(node_v.t1)
    t = np.arange(0,M-1)

    for i in range(M):
        delay_tmp = (node_v.t2[i] - node_v.t1[i] + node_v.t4[i] - node_v.t3[i])/2
        node_v.delay.append(delay_tmp)
        offset_tmp = (node_v.t2[i] - node_v.t1[i] + node_v.t3[i] - node_v.t4[i])/2
        node_v.offset.append(offset_tmp)

    node_v.delay = np.array(node_v.delay)
    node_v.offset = np.array(node_v.offset)

    pid = pidAbs(decimal.Decimal(0.005),decimal.Decimal(0.00001),
                    decimal.Decimal(0),decimal.Decimal(100))
    res = 0
    for i in range(1,M):
        val = res - (node_v.offset[i] - node_v.offset[i-1])
        res = pid.update(0,val)
        node_v.offset_update_pid.append(res)
    plt.plot(t,node_v.offset_update_pid)
    plt.ylim(-0.5e-10,2.5e-9) 

node_handle(A)

plt.show()
