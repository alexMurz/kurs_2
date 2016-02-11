
float4 fixProjDistortion(float4 v0, float4 v1, float4 v2, float w0, float w1, float w2, float3 c) { 
  return (v0*c.x/w0 + v1*c.y/w1 + v2*c.z/w2)/
         (   c.x/w0 +    c.y/w1 +    c.z/w2); 
}

typedef struct MaterialStruct {
  int diffuseColor
} Material;

int colorCalculator(const float4 v, const float3 n, const global Material* mat) {
  float3 L = normalize(-v).xyz;
  float diffuseTerm = dot(L, n);
  if (diffuseTerm > 1.0) diffuseTerm = 1.0;
  if (diffuseTerm < 0.0) diffuseTerm = 0.0;
  int r = ((mat[0].diffuseColor&0xFF0000) >> 16) * diffuseTerm;
  int g = ((mat[0].diffuseColor&0x00FF00) >>  8) * diffuseTerm;
  int b = ((mat[0].diffuseColor&0x0000FF)      ) * diffuseTerm;
  return (r<<16) + (g<<8) + b;
}

void kernel triangleRender(
      global const int* viewport, // Width, Height
      global const float* matrixes, // modelView
      global const float4* v, // Full Transformed
      global const float4* V, // MV Transformed
      global const float3* n, // Normal Vector
      global float* zbuffer,  // zbuffer
      global int*  frame, // Frame
      
      // Matrial
      global const Material* mat // Diffuse, Specular
) {
  int x = get_global_id(0);
  int y = get_global_id(1);
  
  float y2y3 = v[1][1] - v[2][1];
  float xpx3 = x       - v[2][0];
  float x3x2 = v[2][0] - v[1][0];
  float ypy3 = y       - v[2][1];
  float x1x3 = v[0][0] - v[2][0];
  float y1y3 = v[0][1] - v[2][1];
  float y3y1 = v[2][1] - v[0][1];
  
  float3 c; // Barycentric
  c[0] = (y2y3*xpx3 + x3x2*ypy3)/(y2y3*x1x3 + x3x2*y1y3);
  c[1] = (y3y1*xpx3 + x1x3*ypy3)/(y2y3*x1x3 + x3x2*y1y3);
  c[2] = 1.0 - c[0] - c[1];
  
  if (c.x<0 || c.y<0 || c.z<0) return;
  
  float z = c.x*v[0].z + c.y*v[1].z + c.z*v[2].z;
  if (z < 0 || z >= zbuffer[x+y*viewport[0]]) return;
  
  zbuffer[x+y*viewport[0]] = z;
  
  float4 pos = fixProjDistortion(V[0], V[1], V[2], v[0][3], v[1][3], v[2][3], c);
  int col = colorCalculator(pos, n[0], mat);
  frame[x+(viewport[1]-y)*viewport[0]] = col;
}
