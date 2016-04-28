
class Vec3:
	def __init__(self, x = 0, y = 0, z = 0):
		self.x = x
		self.y = y
		self.z = z


	def __sub__(self, b):
		return Vec3(self.x - b.x, self.y - b.y, self.z - b.z)

	def __str__(self): return "(%g; %g; %g)" % (self.x, self.y, self.z)

def dot(a, b):
	return a.x*b.x + a.y*b.y + a.z*b.z


def bary3d(v0, v1, v2, p):
	u = v1 - v0
	v = v2 - v0

	uu = dot(u, u);
	uv = dot(u, v);
	vv = dot(v, v);
	w  = p - v0;
	wu = dot(w, u);
	wv = dot(w, v);
	D  = uv*uv - uu*vv;

	s = (uv * wv - vv * wu) / D;	
	t = (uv * wu - uu * wv) / D;

	return (1 - s - t, s, t)

a = Vec3(0, 0, 0)
b = Vec3(5, 0, 0)
c = Vec3(0, 5, 1)
p = Vec3(0, 5, 1)
print(bary3d(a, b, c, p))