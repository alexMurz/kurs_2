
from math import *
import numpy as np

def xfrange(start, stop, step):
    while start < stop:
        yield start
        start += step

for y in xfrange(-gs, gs, s):
	for x in xfrange(-gs, gs, s):
		pass