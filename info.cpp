
#include "info.h"

namespace Info {
  Scene3D scene;
  std::vector<cl::Device> clDevice;
  cl::Context clContext;
}

namespace Math { 
  Vec3f barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P) {

//    float m11, m12, m13;
//    float m21, m22, m23;
    
//    m11 = P[0] - P[0]; m12 = B[0] - A[0]; m13 = C[0] - A[0]; 
//    m21 = P[1] - P[1]; m22 = B[1] - A[1]; m23 = C[1] - A[1]; 
    
    float y2y3 = B[1] - C[1];
    float xpx3 = P[0] - C[0];
    float x3x2 = C[0] - B[0];
    float ypy3 = P[1] - C[1];
    float x1x3 = A[0] - C[0];
    float y1y3 = A[1] - C[1];
    float y3y1 = C[1] - A[1];
    
    Vec3f c; // Barycentric
    c[0] = (y2y3*xpx3 + x3x2*ypy3)/(y2y3*x1x3 + x3x2*y1y3);
    c[1] = (y3y1*xpx3 + x1x3*ypy3)/(y2y3*x1x3 + x3x2*y1y3);
    c[2] = 1.0 - c[0] - c[1];
    
    return c;
    
//    Vec3f r;
    
//    float y2y3 = B[1]-C[1];
//    float xpx3 = P[0]-C[0];
//    float x3x2 = C[0]-B[0];
//    float ypy3 = P[1]-C[1];
//    float x1x3 = A[0]-C[0];
//    float y1y3 = A[1]-C[1];
//    float y3y1 = C[1]-A[1];
    
//    r[0] = (y2y3*xpx3 + x3x2*ypy3)/(y2y3*x1x3 + x3x2*y1y3);
//    r[1] = (y3y1*xpx3 + x1x3*ypy3)/(y2y3*x1x3 + x3x2*y1y3);
//    r[2] = 1-r[0]-r[1];
    
//    return r;
  }
}
