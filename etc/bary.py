

def bary(a, b, c, p):
	m11 = p[0] - a[0]
	m21 = p[1] - a[1]

	m12 = b[0] - a[0]
	m22 = b[1] - a[1]

	m13 = c[0] - a[0]
	m23 = c[1] - a[1]

	dt = m12*m23 - m22*m13
	d  = m11*m23 - m21*m13
	b2 = d/dt

	d = m21*m12 - m11*m22
	b3 = d/dt;

	b1 = 1 - b2 - b3

	return (b1, b2, b3)

v = [(), (), ()]
v[0] = ( 0,  0)
v[1] = ( 5,  0)
v[2] = ( 0,  5)

low = (100, 100)
high = (0, 0)

for i in v:
	if i[0] > high[0]: high = (i[0], high[1])
	if i[1] > high[1]: high = (high[0], i[1])

	if i[0] < low[0]:  low = (i[0], low[1])
	if i[1] < low[1]:  low = (low[0], i[1])

for x in range(low[0], high[0]+1):
	for y in range(low[1], high[1]+1):
		b = bary(v[0], v[1], v[2], (x, y))
		if b[0] < 0 or b[1] < 0 or b[2] < 0: continue
		print((x, y), b)

