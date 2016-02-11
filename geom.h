#ifndef GEOM
#define GEOM

#include <cmath>
#include <ostream>
#include <iostream>
#include <vector>
#include <string>
#include <float.h>
#include <typeinfo>
#include <stdarg.h>
#include <sstream>

std::string toString(int i);

int makeColor(int r, int g, int b);

int getR(int c);
int getG(int c);
int getB(int c);


template <class _T, class _T1, class _T2> _T clamp(_T v, _T1 min, _T2 max) {
  if (v < min) return min;
  if (v > max) return max;
  return v;
}

template<int DIM, class T> 
class Vec {
    T m[DIM];
  public:
    // Getter/Setter
    T * getV() { return m; }
    int getSize() const { return DIM; }
    const T & operator [](int i) const { return m[i]; }
    T & operator [](int i) { return m[i]; }
    
    // Init Zero
    Vec<DIM, T>() {
      for (int i = 0; i < DIM; i++) m[i] = T();
    }
    // From List
    Vec<DIM, T>(T first, ...) {
      m[0] = first;
      va_list vl;
      va_start(vl, first);
      for (int i = 1; i < DIM; i++) m[i] = va_arg(vl, T);
      va_end(vl);
    }
    // Copy
    template<int D, class C>
    Vec<DIM, T>(const Vec<D, C> & v) {
      int l = D<DIM ? D : DIM;
      for (int i = 0; i < l; i++) {
        if (typeid(T) == typeid(int) && typeid(C) == typeid(double)) m[i] = int(v[i] + 0.5f);
        else m[i] = v[i];
      }
      for (int i = l; i < DIM; i++) m[i] = 0.;
    }
    
    Vec<DIM, T> operator +(const Vec<DIM, T> & v) const {
      Vec<DIM, T> r;
      for (int i = 0; i < DIM; i++) r[i] = m[i]+v[i];
      return r;
    }
    Vec<DIM, T> operator -(const Vec<DIM, T> & v) const {
      Vec<DIM, T> r;
      for (int i = 0; i < DIM; i++) r[i] = m[i]-v[i];
      return r;
    }
    Vec<DIM, T> operator *(float f) const { 
      Vec<DIM, T> r(*this);
      for (int i = 0; i < DIM; i++) r[i] *= f;
      return r;
    }
    Vec<DIM, T> operator /(float f) const { 
      Vec<DIM, T> r(*this);
      for (int i = 0; i < DIM; i++) r[i] /= f;
      return r;
    }
    float operator *(const Vec<DIM, T> & v) const {
      float r = 0.0f;
      for (int i = 0; i < DIM; i++) r += m[i] * v[i];
      return r;
    }
    Vec<3, T> operator ^(const Vec<3, T> & v) const {
      return Vec<3, T>(m[1]*v[2]-m[2]*v[1], m[2]*v[0]-m[0]*v[2], m[0]*v[1]-m[1]*v[0]);
    }

    Vec<DIM, T> mul(const Vec<DIM, T> & v) const {
      Vec<DIM, T> r(*this);
      for (int i = 0; i < DIM; i++) r[i] *= v[i];
      return r;
    }
    
    bool isZero() const {
      for (int i = 0; i < DIM; i++) if (std::abs(m[i]) > 1e-5)  return false;
      return true;
    }
    
    std::string toString() const {
      std::stringstream str;
      str << "[";
      
      for (int i = 0; i < DIM-1; i++) str << m[i] << " ";
      str << m[DIM-1] << "]";
      
      return str.str();
    }
    
    float length2() const { return (*this)*(*this); }
    float length() const { return sqrt(length2()); }
    Vec<DIM, T> & normalize() { return (*this) = (*this) / length(); }
    Vec<DIM, T> normalized() const { return Vec<DIM, T>(*this) / length(); }
};

template<int D, class C>
Vec<D, C> operator*(float f, const Vec<D, C> & v) { return v*f; }

template<int D, class C>
Vec<D, C> operator-(const Vec<D, C> & v) { return v*-1; }

template<class C>
Vec<3, C> reflect(const Vec<3, C> &v, const Vec<3, C> &n) {
  return v - 2.f * v*n * n;
}

typedef Vec<2, double> Vec2f;
typedef Vec<2, int>    Vec2i;
typedef Vec<3, double> Vec3f;
typedef Vec<3, int>    Vec3i;
typedef Vec<4, double> Vec4f;
typedef Vec<4, int>    Vec4i;

// Color
typedef Vec<3, int>   Col3i;
typedef Vec<3, double>   Col3f;

Col3i makeCol3i(const int & c);
Col3i makeCol3i(const Col3f & c);
int makeColor(const Col3i & col);

Col3f makeCol3f(const int & c);
int makeColor(const Col3f & col);


//////////////////////////////////////////////////////////////////////////////////////////////

class Matrix4x4 {
    
public:
    union {
      float v[16];
      float m[4][4];
      struct {
          float m00, m01, m02, m03,
                m10, m11, m12, m13,
                m20, m21, m22, m23,
                m30, m31, m32, m33;
      };
    };
    
    Matrix4x4();
    Matrix4x4(const Matrix4x4 & other) {
      for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
          m[i][j] = other[i][j];
    }
    Matrix4x4(float m00, float m01, float m02, float m03,
              float m10, float m11, float m12, float m13,
              float m20, float m21, float m22, float m23,
              float m30, float m31, float m32, float m33):
      m00(m00), m01(m01), m02(m02), m03(m03),
      m10(m10), m11(m11), m12(m12), m13(m13),
      m20(m20), m21(m21), m22(m22), m23(m23),
      m30(m30), m31(m31), m32(m32), m33(m33) {}
    
    static Matrix4x4 createTranslate(float x, float y, float z) {
        Matrix4x4 m = identity();
        m[0][3] = x;
        m[1][3] = y;
        m[2][3] = z;
        return m;
    }
    static Matrix4x4 createOrtho(float l, float r, float b, float t, float n, float f) {
      Matrix4x4 m;
      m[0][0] = 2/(r-l);
      m[1][1] = 2/(t-b);
      m[2][2] = -2/(f-n);
      m[3][3] = 1;
      m[0][3] = -(r+l)/(r-l);
      m[1][3] = -(t+b)/(t-b);
      m[2][3] = (f+n)/(f-n);
      return m;
    }

    static Matrix4x4 createFrustum(float l, float r, float b, float t, float n, float f) {
      Matrix4x4 m;
      m.v[0] = 2 * n / (r - l);
      m.v[5] = 2 * n / (t - b);
      m.v[8] = (r + l) / (r - l);
      m.v[9] = (t + b) / (t - b);
      m.v[10]= -(f + n) / (f - n);
      m.v[11]= -1;
      m.v[14]= -2*f*n / (f - n);
      m.v[15]= 0;
      return m;
    }
    
    static Matrix4x4 createPerspective(float fovy, float aspect, float n, float f) {
        Matrix4x4 m;
        
//        float top = n * tan(fovy/* * 3.1415926 / 360.0f*/);
//        float bottom = -top;
//        float left = bottom * aspect;
//        float right = top * aspect;
        
//        return createFrustum(left, right, bottom, top, n, f);
        
        float t = tan(fovy/2);
        m[0][1] = m[0][2] = m[0][3] = 0;
        m[1][0] = m[1][2] = m[1][3] = 0;
        m[2][0] = m[2][1] = 0;
        m[3][0] = m[3][1] = m[3][3] = 0;
        m[0][0] = 1/(t*aspect);
        m[1][1] = 1/(t);
        m[2][2] = -(f+n)/(f-n);
        m[2][3] = -2*n*f/(f-n);
        m[3][2] = -1;
        return m;
    }
    static Matrix4x4 createXRotation(float a) {
        Matrix4x4 m = identity();
        m[1][1] = m[2][2] = cos(a);
        m[1][2] = -sin(a);
        m[2][1] = -m[1][2];
        return m;
    }
    static Matrix4x4 createYRotation(float a) {
        Matrix4x4 m = identity();
        m[0][0] = m[2][2] = cos(a);
        m[0][2] = sin(a);
        m[2][0] = -m[0][2];
        return m;
    }
    static Matrix4x4 createZRotation(float a) {
        Matrix4x4 m = identity();
        m[0][0] = m[1][1] = cos(a);
        m[1][0] = sin(a);
        m[0][1] = -m[1][0];
        return m;
    }
    static Matrix4x4 createRotation(float x, float y, float z) {
        return createXRotation(x) * createYRotation(y) * createZRotation(z);
    }

    static Matrix4x4 identity();
    float* operator[](const int i) { return m[i]; }
    const float* operator[](const int i) const { return m[i]; }
    Matrix4x4 operator*(const Matrix4x4& a);
    
    Matrix4x4& operator*=(const float & f) {
      for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) m[i][j] *= f;
      return (*this);
    }
    Matrix4x4 operator*(const float & f) {
      return Matrix4x4(*this) *= f;
    }
    
    Matrix4x4 transposed() const;
    Matrix4x4 inversed() const;
    float det() const {
      return  m[0][0]*m[1][1]*m[2][2]*m[3][3] + m[0][0]*m[1][2]*m[2][3]*m[3][1] + m[0][0]*m[1][3]*m[2][1]*m[3][2]
            + m[0][1]*m[1][0]*m[2][3]*m[3][2] + m[0][1]*m[1][2]*m[2][0]*m[3][3] + m[0][1]*m[1][3]*m[2][2]*m[3][0]
            + m[0][2]*m[1][0]*m[2][1]*m[3][3] + m[0][2]*m[1][1]*m[2][3]*m[3][0] + m[0][2]*m[1][3]*m[2][0]*m[3][1]
            + m[0][3]*m[1][0]*m[2][2]*m[3][1] + m[0][3]*m[1][1]*m[2][0]*m[3][2] + m[0][3]*m[1][2]*m[2][1]*m[3][0]
            - m[0][0]*m[1][1]*m[2][3]*m[3][2] - m[0][0]*m[1][2]*m[2][1]*m[3][3] - m[0][0]*m[1][3]*m[2][2]*m[3][1]
            - m[0][1]*m[1][0]*m[2][2]*m[3][3] - m[0][1]*m[1][2]*m[2][3]*m[3][0] - m[0][1]*m[1][3]*m[2][0]*m[3][2]
            - m[0][2]*m[1][0]*m[2][3]*m[3][1] - m[0][2]*m[1][1]*m[2][0]*m[3][3] - m[0][2]*m[1][3]*m[2][1]*m[3][0]
            - m[0][3]*m[1][0]*m[2][1]*m[3][2] - m[0][3]*m[1][1]*m[2][2]*m[3][1] - m[0][3]*m[1][2]*m[2][0]*m[3][1];
    }

    Matrix4x4 & translate(float x, float y, float z) {
      return *this = createTranslate(x, y, z) * (*this);
    }
    
    Matrix4x4 & move(float x, float y, float z) {
      return *this = (*this) * createTranslate(x, y, z);
    }
    
    // Will autocut matrix
    template <int DIM, class TYPE>
    Vec<DIM, TYPE> operator *(const Vec<DIM, TYPE> & v) const { return mul(v); }
    
    template <int DIM, class TYPE>
    Vec<DIM, TYPE> mul(const Vec<DIM, TYPE> & v) const {
      Vec<DIM, TYPE> r;
      for (int i = 0; i < DIM; i++) {
        r[i] = 0;
        for (int j = 0; j < DIM; j++) {
          r[i] += v[j]*m[i][j];
        }
      }
      return r;
    }

    template <int DIM, class TYPE>
    Vec<DIM, TYPE> mulDbg(const Vec<DIM, TYPE> & v) const {
      Vec<DIM, TYPE> r;
      for (int i = 0; i < DIM; i++) {
        r[i] = 0;
        for (int j = 0; j < DIM; j++) {
          std::cerr << "(" << v[j] << " " << m[i][j] << " " << v[j]*m[i][j] << ") ";
          r[i] += v[j]*m[i][j];
        }
        std::cerr << "\n";
      }
      return r;
    }
};



#endif //__GEOMETRY_H__

