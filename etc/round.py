
def xfrange(start, stop, step):
    while start < stop:
        yield start
        start += step

def round(v, x):
	return int(v/x+x*0.5)*x

x = 0.2

for v in xfrange(-1, 1, 0.05):
	print v, round(v, x)