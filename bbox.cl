
int float3ToColor(float3 color) {
  return  clamp(int(color[0]*255), 0, 255) * 0x10000 + 
          clamp(int(color[1]*255), 0, 255) * 0x100 + 
          clamp(int(color[2]*255), 0, 255);
}

float4 fixProjDistortion4(float4 v0, float4 v1, float4 v2, float w0, float w1, float w2, float3 c) { 
  return (v0*c.x/w0 + v1*c.y/w1 + v2*c.z/w2)/
         (   c.x/w0 +    c.y/w1 +    c.z/w2); 
}
float3 fixProjDistortion3(float3 v0, float3 v1, float3 v2, float w0, float w1, float w2, float3 c) { 
  return (v0*c.x/w0 + v1*c.y/w1 + v2*c.z/w2)/
         (   c.x/w0 +    c.y/w1 +    c.z/w2); 
}

typedef struct MaterialStruct {
  double3 diffuseColor, specularColor;
  double3 reflection;
  double  reflectivity, shininess;
  int     isMirror;
} Material;

float getR(int c) { return ((c&0xFF0000) / 0x10000) / 255.0; }
float getG(int c) { return ((c&0x00FF00) / 0x100  ) / 255.0; }
float getB(int c) { return ((c&0x0000FF)          ) / 255.0; }
float3 getV(int c) { float3 f; f[0] = getR(c); f[1] = getG(c); f[2] = getB(c); return f; }

float clampf(float f, float l, float h) { 
  if (f > h) return h;
  if (f < l) return l;
  return f;
}

typedef struct {
  int enabled;
  float3 position;
  float3 diffuse;
  float3 specular;
  float  energy;
  int    isEye;
  float3 attenuation;
} Light_t;


float3 colorCalculator(const float4 v, const float3 n, const global Material* mat, 
      global const float* mv, // MV Transformed
      global const Light_t* lights,
      int lightsCount
      ) {
      
  float3 c = 0;
  
  for (int i = 0; i < lightsCount; i++) {
    if (lights[i].enabled == 0) continue;  

    float3 p;
    float3 lp = lights[i].position;
    if (lights[i].isEye) {
      p = lp;
    } else {
      p[0] = mv[0*4+0]*lp[0] + mv[0*4+1]*lp[1] + mv[0*4+2]*lp[2] + mv[0*4+3];
      p[1] = mv[1*4+0]*lp[0] + mv[1*4+1]*lp[1] + mv[1*4+2]*lp[2] + mv[1*4+3];
      p[2] = mv[2*4+0]*lp[0] + mv[2*4+1]*lp[1] + mv[2*4+2]*lp[2] + mv[2*4+3];
    }
  
    // Reflect(v, n) = return v - 2.f * v*n * n;
  
    float3 L = normalize(p-v.xyz).xyz;
    float3 E = normalize(-v.xyz);
    float3 R = normalize(-(L - 2*dot(n, L) * n));
    
    float dist = length(p-v.xyz);
    float e = lights[i].energy;
  
    float3 diffuseTerm = clampf(dot(L, n), 0.0, 1.0) * lights[i].diffuse;
    
    float3 specTerm = powf(clampf(dot(R, E), 0.0, 1.0), mat[0].shininess) * lights[i].specular;
    
    float falloff = lights[i].attenuation.x;
    falloff += lights[i].attenuation.y * (1-dist/e);
    falloff += lights[i].attenuation.z * (1-dist*dist/(e*e));
    falloff = clamp(falloff, 0.0f, 1.0f);      
    
    float3 diff = convert_float3(mat[0].diffuseColor);
    float3 spec = convert_float3(mat[0].specularColor);
  
    c += (diff*diffuseTerm + spec*specTerm) * falloff;
  }
  
  return c;
}

void kernel triangleRender(
      global const int* settings, // Width, Height, UseSS
      global const float* matrixes, // modelView
      global const float4* v, // Full Transformed
      global const float4* V, // MV Transformed
      global const float3* n, // Normal Vector
      global float* zbuffer,  // zbuffer
      global int*  frame, // Frame
      
      // Matrial
      global const Material* mat, // Diffuse, Specular
      
      // Lighting
      global const Light_t* lights,
      global const int* lightCount
) {
  int x = get_global_id(0);
  int y = get_global_id(1);
  
  float m11, m12, m13;
  float m21, m22, m23;
  float det, mainDet;
  float3 c;
  
  m11 = x - v[0][0]; m12 = v[1][0] - v[0][0]; m13 = v[2][0] - v[0][0]; 
  m21 = y - v[0][1]; m22 = v[1][1] - v[0][1]; m23 = v[2][1] - v[0][1]; 
  
  mainDet = m12*m23 - m22*m13;
  det = m11*m23 - m21*m13; // 2
  c[1] = det / mainDet;
  
  det = m21*m12 - m11*m22; // 3
  c[2] = det / mainDet;
  
  c[0] = 1 - c[1] - c[2];  // 1
  
  if (c.x<0 || c.y<0 || c.z<0) return;
  
  float z = c.x*v[0].z + c.y*v[1].z + c.z*v[2].z;
  if (z < 0 || z >= zbuffer[x+y*settings[0]]) return;
  
  float4 pos  = fixProjDistortion4(V[0], V[1], V[2], v[0][3], v[1][3], v[2][3], c);
  float3 norm = fixProjDistortion3(n[0], n[1], n[2], v[0][3], v[1][3], v[2][3], c);
  
  zbuffer[x+y*settings[0]] = z;
  frame[x+(settings[1]-y)*settings[0]] = float3ToColor(colorCalculator(pos, norm, mat, matrixes, lights, lightCount[0]));
}
