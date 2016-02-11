#ifndef SCENE3D
#define SCENE3D

#include "object3d.h"
#include "geom.h"

class LightSource {
    
  public:
    bool enabled;
    Vec3f origin, diffuseLight;
    float energy;
    bool  isEye;
    Vec3f attenuation; // Constant, Linear, Quadratic
    
    LightSource(const Vec3f & origin = Vec3f(0,0,0), const Vec3f & diffuseLight = Vec3f(1,1,1), 
                const float & energy = 5.0f, const Vec3f & attenuation = Vec3f(1, 0, 0), bool isEye = true, bool enabled = false) : 
      enabled(enabled),
      origin(origin), 
      diffuseLight(diffuseLight),
      energy(energy),
      isEye(isEye),
      attenuation(attenuation)
    {}
};


class Scene3D {
  public:
    Scene3D() {}
    
    std::vector<Object3D> objects;
    std::vector<LightSource> lights;
};

#endif // SCENE3D
