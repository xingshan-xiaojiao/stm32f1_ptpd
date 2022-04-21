# -*- coding: utf-8 -*-

import numpy as np
import matplotlib.pyplot as plt
import treePlotter as tp 



# 绘制树

myTree = {'A': {0: 'B', 1: {'C': {0: 'D', 1: 'E'}},2: 'F'}}

tp.createPlot(myTree)
