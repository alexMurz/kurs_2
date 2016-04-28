#include <vector>
#include <cassert>
#include <cmath>
#include <iostream>
#include "geom.h"

std::string toString(int i) {
  std::string s;
  while (i > 0) {
    s += i%10 + '0';
    i /= 10;
  }
  return s;
}

int makeColor(int r, int g, int b) {
  if (r > 0xFF) r = 0xFF;
  if (r < 0)    r = 0;
  if (g > 0xFF) g = 0xFF;
  if (g < 0)    g = 0;
  if (b > 0xFF) b = 0xFF;
  if (b < 0)    b = 0;
  
  return (r << 16) + (g << 8) + b;      
}

Col3i makeCol3i(const Col3f & c) {
  return Col3i(c[0]*255, c[1]*255, c[2]*255);
}

Col3i makeCol3i(const int & c) {
    Col3i r;
    r[0] = getR(c);
    r[1] = getG(c);
    r[2] = getB(c);
    return r;
}
int makeColor(const Col3i & col) { return makeColor(col[0], col[1], col[2]); }

Col3f makeCol3f(const int & c) {
  Col3f r;
  r[0] = getR(c) / 255.0f;
  r[1] = getG(c) / 255.0f;
  r[2] = getB(c) / 255.0f;
  return r;
}
int makeColor(const Col3f & col) { return makeColor(col[0]*255, col[1]*255, col[2]*255); }

int getR(int c) {
  return (c & 0xFF0000) >> 16;
}
int getG(int c) {
  return (c & 0x00FF00) >> 8;
}
int getB(int c) {
  return c & 0x0000FF;
}

/*
template <> template <> Vec3<int>::Vec3(const Vec3<float> &v) :
    x(int(v.x+.5)), y(int(v.y+.5)), z(int(v.z+.5)) {
}

template <> template <> Vec3<float>::Vec3(const Vec3<int> &v) :
    x(v.x), y(v.y), z(v.z) {
}
*/

///////////////////////////////////

Matrix4x4::Matrix4x4() { }

Matrix4x4 Matrix4x4::identity() {
    Matrix4x4 E;
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            E[i][j] = (i==j ? 1.f : 0.f);
        }
    }
    return E;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& a) {
    Matrix4x4 result;
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            result.m[i][j] = 0.f;
            for (int k=0; k<4; k++) {
                result.m[i][j] += m[i][k]*a.m[k][j];
            }
        }
    }
    return result;
}

Matrix4x4 Matrix4x4::transposed() const {
    Matrix4x4 result;
    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++)
            result[j][i] = m[i][j];
    return result;
}

Matrix4x4 Matrix4x4::inversed() const {
  Matrix4x4 r;
  r.m[0][0] = m12*m23*m31 - m13*m22*m31 + m13*m21*m32 - m11*m23*m32 - m12*m21*m33 + m11*m22*m33;
  r.m[0][1] = m03*m22*m31 - m02*m23*m31 - m03*m21*m32 + m01*m23*m32 + m02*m21*m33 - m01*m22*m33;
  r.m[0][2] = m02*m13*m31 - m03*m12*m31 + m03*m11*m32 - m01*m13*m32 - m02*m11*m33 + m01*m12*m33;
  r.m[0][3] = m03*m12*m21 - m02*m13*m21 - m03*m11*m22 + m01*m13*m22 + m02*m11*m23 - m01*m12*m23;
  r.m[1][0] = m13*m22*m30 - m12*m23*m30 - m13*m20*m32 + m10*m23*m32 + m12*m20*m33 - m10*m22*m33;
  r.m[1][1] = m02*m23*m30 - m03*m22*m30 + m03*m20*m32 - m00*m23*m32 - m02*m20*m33 + m00*m22*m33;
  r.m[1][2] = m03*m12*m30 - m02*m13*m30 - m03*m10*m32 + m00*m13*m32 + m02*m10*m33 - m00*m12*m33;
  r.m[1][3] = m02*m13*m20 - m03*m12*m20 + m03*m10*m22 - m00*m13*m22 - m02*m10*m23 + m00*m12*m23;
  r.m[2][0] = m11*m23*m30 - m13*m21*m30 + m13*m20*m31 - m10*m23*m31 - m11*m20*m33 + m10*m21*m33;
  r.m[2][1] = m03*m21*m30 - m01*m23*m30 - m03*m20*m31 + m00*m23*m31 + m01*m20*m33 - m00*m21*m33;
  r.m[2][2] = m01*m13*m30 - m03*m11*m30 + m03*m10*m31 - m00*m13*m31 - m01*m10*m33 + m00*m11*m33;
  r.m[2][3] = m03*m11*m20 - m01*m13*m20 - m03*m10*m21 + m00*m13*m21 + m01*m10*m23 - m00*m11*m23;
  r.m[3][0] = m12*m21*m30 - m11*m22*m30 - m12*m20*m31 + m10*m22*m31 + m11*m20*m32 - m10*m21*m32;
  r.m[3][1] = m01*m22*m30 - m02*m21*m30 + m02*m20*m31 - m00*m22*m31 - m01*m20*m32 + m00*m21*m32;
  r.m[3][2] = m02*m11*m30 - m01*m12*m30 - m02*m10*m31 + m00*m12*m31 + m01*m10*m32 - m00*m11*m32;
  r.m[3][3] = m01*m12*m20 - m02*m11*m20 + m02*m10*m21 - m00*m12*m21 - m01*m10*m22 + m00*m11*m22;
  r *= 1.0f/det();
  return r;
}
