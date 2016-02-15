#ifndef SCENE3D
#define SCENE3D

#include "object3d.h"
#include "geom.h"
#include "cl.h"

class LightSource {
    
  public:
    static const int clSize = sizeof(int) + sizeof(cl_float3)*3 + sizeof(cl_float) + sizeof(int) + sizeof(cl_float3);
    
    bool enabled;
    Vec3f origin, diffuseLight, specularLight;
    float energy;
    bool  isEye;
    Vec3f attenuation; // Constant, Linear, Quadratic
    
    LightSource(const Vec3f & origin = Vec3f(0.0f,0.0f,0.0f), 
                const Vec3f & diffuseLight = Vec3f(1.0f,1.0f,1.0f), const Vec3f & specularLight = Vec3f(1.0f,1.0f,1.0f), 
                const float & energy = 5.0f, 
                const Vec3f & attenuation = Vec3f(1.0f,0.0f,0.0f), 
                bool isEye = true, bool enabled = false) : 
      enabled(enabled),
      origin(origin), 
      diffuseLight(diffuseLight),
      specularLight(specularLight),
      energy(energy),
      isEye(isEye),
      attenuation(attenuation)
    {
      this->attenuation.normalize();
    }
};

#include <QString>
typedef struct ClLightSource {
    cl_int enabled;
    cl_float3 position;
    cl_float3 diffuse;
    cl_float3 specular;
    cl_float  energy;
    cl_int isEye;
    cl_float3 attenuation;
    
    QString toString() {
      ClLightSource & l = *this;
      
      QString s = "[";
      s.append(  QString::number(l.enabled) + ", " );
      s.append( "[" + QString::number(l.position.x) + ", " + QString::number(l.position.y) + ", " + QString::number(l.position.z) + "], " );
      s.append( "[" + QString::number(l.diffuse.x) + ", " + QString::number(l.diffuse.y) + ", " + QString::number(l.diffuse.z) + "], " );
      s.append( "[" + QString::number(l.specular.x) + ", " + QString::number(l.specular.y) + ", " + QString::number(l.specular.z) + "], " );
      s.append( QString::number(l.energy) + ", " );
      s.append( QString::number(l.isEye) + ", " );
      s.append( "[" + QString::number(l.attenuation.x) + ", " + QString::number(l.attenuation.y) + ", " + QString::number(l.attenuation.z) + "]]" );  
      return s;
    }
} ClLightSource;


#include <QDebug>

class Scene3D {
    cl_float3 asFloat3(const Vec3f & v) {
      cl_float3 f;
      f.x = v[0];
      f.y = v[1];
      f.z = v[2];
      return f;      
    }
    
  public:
    Scene3D() {}
    
    std::vector<Object3D> objects;
    std::vector<LightSource> lights;
    
    std::vector<ClLightSource> lightsAsOpenCL;
    void updateOpenCLLights() {
      qDebug() << "updateOpenCLLights();";
      while (lights.size() > lightsAsOpenCL.size()) lightsAsOpenCL.push_back(ClLightSource());
      while (lights.size() < lightsAsOpenCL.size()) lightsAsOpenCL.erase(lightsAsOpenCL.begin());
      
      for (int i = 0; i < lights.size(); i++) {
        LightSource & ls = lights[i];
        ClLightSource * cls = &lightsAsOpenCL[i];
        cls->enabled = ls.enabled;
        cls->position = asFloat3(ls.origin);
        cls->diffuse = asFloat3(ls.diffuseLight);
        cls->specular = asFloat3(ls.specularLight);
        cls->energy = ls.energy;
        cls->isEye = ls.isEye;
        cls->attenuation = asFloat3(ls.attenuation);
        
        qDebug() << cls->toString();
      }
    }
    
};

#endif // SCENE3D

