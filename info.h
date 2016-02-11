#ifndef INFO
#define INFO

#include "scene3d.h"
#include "cl.h"

namespace Info {
  extern Scene3D scene;
  extern std::vector<cl::Device> clDevice;
  extern cl::Context clContext;
}

namespace Math {
  template<class T> T projectionCurvetureFix(const T &v0, const T &v1, const T &v2, 
                                             const float &w0, const float &w1, const float &w2,
                                             const Vec3f &bary) {
    return (v0*bary[0]/w0 + v1*bary[1]/w1 + v2*bary[2]/w2) /
           (   bary[0]/w0 +    bary[1]/w1 +    bary[2]/w2);
  }

  Vec3f barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P);
}

#endif // INFO

