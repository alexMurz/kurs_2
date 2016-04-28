
int float3ToColor(float3 color) {
  return  clamp(int(color[0]*255), 0, 255) * 0x10000 + 
          clamp(int(color[1]*255), 0, 255) * 0x100 + 
          clamp(int(color[2]*255), 0, 255);
}
float3 makeFloat3(float f1, float f2, float f3) { float3 f; f[0] = f1; f[1] = f2; f[2] = f3; return f; }

float3 reflect(float3 I, float3 N) { return I - N * float3(2.0 * dot(N, I)); }

typedef struct {
  float3 v0, v1, v2;
  float3 n, n0, n1, n2;
  float3 diffuse, specular;
  float3 refl;
  int isMirror;
  float reflectivity, shininess;
} Triangle3D;

typedef struct {
  int enabled;
  float3 position;
  float3 diffuse;
  float3 specular;
  float  energy;
  int    isEye;
  float3 attenuation;
} Light_t;

typedef struct {
  bool ok;
  float distance;
  float3 point;
  int dbg;
  float3 c;
} IntersectResult;
IntersectResult IntersectFalse(int dbg) { 
  IntersectResult r;
  r.ok = false;
  r.dbg = dbg;
  return r;
}

IntersectResult intersect(Triangle3D tr, 
                          float3 p0, // Ray origin 
                          float3 p1, // 
                          float maxDistance) {
                          
  IntersectResult result;
  result.ok = true;
  
  float3 v0 = tr.v0;
  float3 v1 = tr.v1;
  float3 v2 = tr.v2;
  float3 N  = tr.n;
  
  float3 dir = normalize(p1 - p0);
  float3 u = v1 - v0;
  float3 v = v2 - v0;
  
  float3 w0 = p0 - v0;
  float a = -dot(N, w0);
  float b = dot(N, dir);

  if (fabs(b) < 1e-5) return IntersectFalse(1);
  
  float r = a/b;
  if (r <= 0.0f || r >= maxDistance) return IntersectFalse(2);
  
  float distance = r;
  float3 point = p0 + r*dir;
  
  result.point = point;
  result.distance = distance;
  
  float uu, uv, vv, wu, wv, D;
  uu = dot(u, u);
  uv = dot(u, v);
  vv = dot(v, v);
  float3 w = point - v0;
  wu = dot(w, u);
  wv = dot(w, v);
  D = uv*uv - uu*vv;
  
  float s, t;
  s = (uv * wv - vv * wu) / D;
  if (s < 0.0f || s > 1.0f) return IntersectFalse(3);
  
  t = (uv * wu - uu * wv) / D;
  if (t < 0.0f || (s+t) > 1.0) return IntersectFalse(4);
  
  result.c[0] = 1-s-t;
  result.c[1] = s;
  result.c[2] = t;

  return result;
}

typedef struct {
  bool ok;
  float3 point;
  float dist;
  int triangle;
  float3 n;
} SendRayResult;
SendRayResult sendRay(global const Triangle3D * triangles, const int trianglesCount, 
                      const float3 origin, const float3 ray, const float maxDistance, int skip) {
  SendRayResult result;
  result.dist = maxDistance; 
  result.ok = false;

  for (int i = 0; i < trianglesCount; i++) {
    if (i == skip) continue;
    IntersectResult r = intersect(triangles[i], origin, ray, result.dist); 
    if (r.ok) {
      result.dist = r.distance;
      result.triangle = i;
      result.point = r.point;
      result.ok = true;
      result.n = triangles[i].n0*r.c[0] + triangles[i].n1*r.c[1] + triangles[i].n2*r.c[2];
    }
  }
  
  return result;
}

float3 lighting(global const Triangle3D * triangles, float3 n, const int trianglesCount, 
                global const Light_t * lights, const int lightsCount, 
                int triangle, float3 p, float3 origin) {
  float3 col = 0;

  if (lightsCount == 0) {
    col = triangles[triangle].diffuse;
    return col;
  }

  for (int i = 0; i < lightsCount; i++) {
    if (lights[i].enabled != 1) continue;
  
    
      float3 v2 = lights[i].position;
      
      float3 v1 = p; // + normalize(v2-p) * 0.01f;
      float dist = length(v2-v1);

      SendRayResult r = sendRay(triangles, trianglesCount, v1, v2, dist, triangle);
      //float3 n = n; // r.n;
      if (!r.ok) {
        float e = lights[i].energy;
        
        float3 L = normalize(v2-v1);
        float3 E = normalize(origin-v1);
        float3 R = -reflect(L, n);
        
        // Diffuse
        float falloff = lights[i].attenuation.x;
        falloff += lights[i].attenuation.y * (1-dist/e);
        falloff += lights[i].attenuation.z * (1-dist*dist/(e*e));
        falloff = clamp(falloff, 0.0f, 1.0f);      
  
        if (dot(n, L) > 0) {
          float3 dif = triangles[triangle].diffuse * lights[i].diffuse * (clamp(dot(n, L), 0.0f, 1.0f) * falloff);
          
          dif[0] = clamp(dif[0], 0.0f, 1.0f);
          dif[1] = clamp(dif[1], 0.0f, 1.0f);
          dif[2] = clamp(dif[2], 0.0f, 1.0f);
          
          float3 spe = triangles[triangle].specular * lights[i].specular * pow( 
            clamp(dot(R, E), 0.0f, 1.0f), 
            float(0.3f * triangles[triangle].shininess)
          ) * falloff;
          
          
          spe[0] = clamp(spe[0], 0.0f, 1.0f);
          spe[1] = clamp(spe[1], 0.0f, 1.0f);
          spe[2] = clamp(spe[2], 0.0f, 1.0f);
          
          col += dif + spe;
        }
      }
  }

  return col;
}

float3 rayTrace(global const Triangle3D * triangles, const int trianglesCount, 
                const float3 origin, const float3 ray, int jump, int skip, 
                global const Light_t * lights, const int lightsCount, const float3 rorigin) {
  int maxDistance = 10000;
  int maxJump = 10;
  if (jump >= maxJump) return float3(0);
  
  SendRayResult r = sendRay(triangles, trianglesCount, origin, ray, maxDistance, skip);
  float3 color = 0;
  
  if (r.ok) {
    Triangle3D tr = triangles[r.triangle];
  
    if (tr.isMirror == 1) {
      float3 dir = normalize(ray - origin);
      float3 R = normalize(reflect(dir, tr.n));
      float3 v1 = r.point;
      float3 v2 = r.point + R;
      color = color + tr.reflectivity * (tr.refl*rayTrace(triangles, trianglesCount, v1, v2, jump+1, r.triangle, lights, lightsCount, rorigin));
    }
    
    float3 n = r.n; //triangles[r.triangle].n0 * r.c.x + triangles[r.triangle].n1 * r.c.y + triangles[r.triangle].n2 * r.c.z;
    color = color + lighting(triangles, n, trianglesCount, lights, lightsCount, r.triangle, r.point, origin); // tr.diffuse;
  }
  
  return color;
}

void kernel rayCast(
  /* 0 */ global const int * params /*width, height, anti-aliasing samples, softlight samples*/,
  /* 1 */ global const float3 * origin,
  /* 2 */ global const float * rayMatrix, 
  /* 3 */ global const Triangle3D * triangles,
  /* 4 */ global const int * trianglesCount,
  /* 5 */ global int * frame,
  /* 6 */ global const Light_t * lights,
  /* 7 */ global const int * lightsCount_p
) {
  int x = get_global_id(0);
  int y = get_global_id(1);
  float3 orig = origin[0];
  int samples = params[2];
  int slSamples = params[3];
  
  int lightCount = lightsCount_p[0];  
  
  float3 color = 0;
  float st = 1.0f / (samples+1.0f);
  for (float xx = st; xx < 1.0f; xx += st) {
    for (float yy = st; yy < 1.0f; yy += st) {
      float iray[4] = { x+xx, y+yy, 0.0, 1.0 };
      
      float bray[4];
      int idx = 0;
      for (int i = 0; i < 4; i++) {
        bray[i] = 0.0;
        for (int j = 0; j < 4; j++) {
          bray[i] += iray[j]*rayMatrix[i*4 + j];
        }
      }
      
      float3 ray; ray[0]=bray[0]; ray[1]=bray[1]; ray[2]=bray[2];
      color += rayTrace(triangles, trianglesCount[0], orig, ray, 0, -1, lights, lightCount, orig);
    }
  }
  
  color /= samples*samples;
  
  frame[x + (params[1] - 1 - y)*params[0]] = float3ToColor(color);
}
