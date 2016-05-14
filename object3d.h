#ifndef OBJECT3D_H
#define OBJECT3D_H

#include "geom.h"
#include <vector>
#include <stdlib.h>
#include <string>
#include <fstream>

// Rasterize Info
struct Material {

    Col3f diffuseReflection, specularReflection;
    Col3f reflection;
    double reflectivity, shininess;
    int   isMirror;
    double transparency;
    
    Material(const int &diffuse = 0xFFFFFF, const int &specular = 0xFFFFFF) : 
      diffuseReflection(makeCol3f(diffuse)), specularReflection(makeCol3f(specular)) {}
    Material(const Vec3f &diffuse, const Vec3f &specular) : diffuseReflection(diffuse), specularReflection(specular) {}
};

static Material * Material_White = new Material(0xFFFFFF);
static Material * Material_Black = new Material(0x000000);
static Material * Material_Red   = new Material(0xFF0000);
static Material * Material_Green = new Material(0x00FF00);
static Material * Material_Blue  = new Material(0x0000FF);

// Geometry
struct Triangle3D {
    Vec4f v[3];
    Vec3f n;
    Material * material;
    
    // Smoothing
    Vec3f sn[3];
    
    Matrix4x4 * modelRef;

    Triangle3D() {}
    Triangle3D(Vec3f v1, Vec3f v2, Vec3f v3, Vec3f vn, Material * material) {
        v[0] = Vec4f(v1[0], v1[1], v1[2], 1.0);
        v[1] = Vec4f(v2[0], v2[1], v2[2], 1.0);
        v[2] = Vec4f(v3[0], v3[1], v3[2], 1.0); 
        n = vn.normalize();
        this->material = material;
    }
    Triangle3D(float v1x, float v1y, float v1z,
               float v2x, float v2y, float v2z,
               float v3x, float v3y, float v3z,
               float vnx, float vny, float vnz,
               Material * material) {
        v[0] = Vec4f(v1x, v1y, v1z, 1.0);
        v[1] = Vec4f(v2x, v2y, v2z, 1.0);
        v[2] = Vec4f(v3x, v3y, v3z, 1.0);
        n    = Vec3f(vnx, vny, vnz);
        this->material = material;
    }    
};

class Object3D {
    std::vector< Triangle3D* > triangles;
    Matrix4x4 model;

public:
    std::string name;

    Object3D() : model(Matrix4x4::identity()) {
        static int counter = 0;
        counter++;
        name = "Object " + toString(counter);
    }
    
    static Object3D * fromObj(std::ifstream & obj, std::ifstream & mtl);
    
    Matrix4x4 & getModel() { return model; }
    const Matrix4x4 & getModel() const { return model; }
    std::vector<Triangle3D *> & getTriangles() { return triangles; }
    const std::vector<Triangle3D *> & getTriangles() const { return triangles; }

    inline void add(Triangle3D * t) { triangles.push_back(t); }
    void add(const Vec3f &v1, const Vec3f &v2, const Vec3f &v3, const Vec3f &n, Material * mat) {
        add(new Triangle3D(v1, v2, v3, n, mat));
    }
};

#endif // OBJECT3D_H
