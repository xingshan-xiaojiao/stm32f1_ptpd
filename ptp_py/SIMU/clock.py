import numpy as np
import matplotlib.pyplot as plt
import random


D = 1e-5

class clock:
    def __init__(self):
        self.t = []
        self.b = random.random()
        self.f = random.uniform(32,33) * 1e-4
        print('b =', self.b, 'f =', self.f)
    def clockdata(self,t):
            y = self.b + self.f * t + 1/2 * D * t * t
            return y

C1 = clock()
C2 = clock()
C3 = clock()
C4 = clock()

M = 1000
for t in range(M):
    C1.t.append(C1.clockdata(t))
    C2.t.append(C2.clockdata(t))
    C3.t.append(C3.clockdata(t))
    C4.t.append(C4.clockdata(t))

t = np.arange(0,M)

plt.plot(t,C1.t)

print(C1.t)

plt.show()

