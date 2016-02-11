#include "object3d.h"
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>

/*********************
 * String Stuff
 *********************/
std::vector<std::string> &s_split(const std::string &s, char delim,std::vector<std::string> &elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);  
  }
  return elems;
}
std::vector<std::string> s_split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    s_split(s, delim, elems);
    return elems;
}

bool s_startWith(const std::string & s, const std::string & fix) {
  if (fix.size() > s.size()) return false;
  for (size_t i = 0; i < fix.size(); i++) if (s[i] != fix[i]) return false;
  return true;
}
bool s_endWith(const std::string & s, const std::string & fix) {
  if (fix.size() > s.size()) return false;
  size_t l = s.size() - fix.size();
  for (size_t i = s.size()-1; i >= l; i++) if (s[i] != fix[i-l]) return false;
  return true;
}
void s_removeWhiteSpaces(std::string & s) {
  while (s_startWith(s, " ")) s.erase(0, 0);
  while (s_endWith(s, " ")) s.erase(s.size()-1, s.size()-1);
}
double s_toDouble(const std::string & s) {
  double r = 0;
  int l = int(s.size());
  int dp = -1;
  bool m = s[0] == '-';
  for (int i = m?1:0; i < l; i++) {
    if (s[i] == '.') { dp = i; continue; }
    if (dp == -1)
      r = r * 10 + (s[i] - '0');
    else {
      r = r + (s[i]-'0')/(pow(10, i-dp));
    }
  }
  return m?-r:r;
}
int s_toInt(const std::string & s) {
  int r = 0;
  int l = int(s.size());
  bool m = s[0] == '-';
  for (int i = m?1:0; i < l; i++) {
    r = r * 10 + (s[i] - '0');
  }
  return m?-r:r;
}

/***********************
 * Element Processing
 ***********************/
Vec3f _getVector(std::string string) { // Convert full string to vector (e.g. "v 1 2 3" = {1, 2, 3})
  s_removeWhiteSpaces(string);
  std::vector<std::string> elm = s_split(string, ' ');
  Vec3f v;
  for (int i = 1; i < 4; i++) {
    std::string s = elm[i];
    s_removeWhiteSpaces(s);
    v[i-1] = s_toDouble(s);
  }
  return v;
}
double _getDouble(std::string string) {
  s_removeWhiteSpaces(string);
  std::vector<std::string> elm = s_split(string, ' ');
  return s_toDouble(elm[1]);
}
int _getInt(std::string string) {
  s_removeWhiteSpaces(string);
  std::vector<std::string> elm = s_split(string, ' ');
  return s_toInt(elm[1]);
}

/********************
 * MTL processing
 ********************/
std::string m_newmtl(std::string s) { // Return new mtl name
  s_removeWhiteSpaces(s);
  std::vector<std::string> e = s_split(s, ' ');
  s_removeWhiteSpaces(e[1]);
  return e[1];
}

/*********************
 * OBJ processing
 *********************/
void o_polygon(Object3D *obj, Material * mat, 
               const std::vector<Vec3f> & vert, const std::vector<Vec3f> & norm, 
               std::string s) {
  s_removeWhiteSpaces(s);
  std::vector<std::string> e = s_split(s, ' ');
  Vec3f v[3], n[3];
  for (int i = 1; i < 4; i++) {
    s_removeWhiteSpaces(e[i]);
    std::vector<std::string> args = s_split(e[i], '/');
    v[i-1] = vert[s_toInt(args[0]) - 1];
    n[i-1] = norm[s_toInt(args[2]) - 1];
  }
  Vec3f nn = (n[0] + n[1] + n[2]).normalized();
  obj->add(v[0], v[1], v[2], nn, mat);
}

std::string o_usemtl(std::string s) {
  s_removeWhiteSpaces(s);
  std::string r = s_split(s, ' ')[1];
  s_removeWhiteSpaces(r);
  return r;
}

/********************
 * Generate Object3D from obj files
 ********************/
Object3D * Object3D::fromObj(std::ifstream & obj, std::ifstream & mtl) { 
#define startWith(l) s_startWith(line, l)
  
  std::string line;
  std::vector<Vec3f> vertices;
  std::vector<Vec3f> normals;
  Material * mat = new Material();
  
  std::map<std::string, Material *> materialMap;
  // Decode MTL
  
  while (std::getline(mtl, line)) {
    if (startWith("#")) continue;
    if (startWith("newmtl")) {
      mat = new Material(); 
      materialMap.insert(std::pair<std::string, Material*>(m_newmtl(line), mat)); 
    }
    if (startWith("Ka")) continue;
    if (startWith("Kd")) mat->diffuseReflection = _getVector(line);
    if (startWith("Ks")) mat->specularReflection = _getVector(line);
    if (startWith("Ni")) mat->shininess = _getDouble(line);
    if (startWith("Kr")) mat->reflection = _getVector(line);
    if (startWith("Ur")) mat->isMirror = _getInt(line);
    if (startWith("Fr")) mat->reflectivity = _getDouble(line);
  }
  
  // Decode Obj file
  Object3D * object = new Object3D();
  while (std::getline(obj, line)) {
    if (startWith("#"))  continue;
    if (startWith("v ")) vertices.push_back(_getVector(line));
    if (startWith("vn")) normals.push_back(_getVector(line));
    if (startWith("vt")) continue; // No tex coords for now
    if (startWith("f"))  o_polygon(object, mat, vertices, normals, line);
    if (startWith("usemtl")) mat = materialMap[o_usemtl(line)];
  }
  
#undef startWith
  
  for (std::map<std::string, Material *>::iterator it = materialMap.begin(); it != materialMap.end(); it++) {
    Material * m = (Material *) it->second;
    Vec3i v = makeCol3i(m->diffuseReflection);
//    std::cout << it->first << " " << v[0] << " " << v[1] << " " << v[2] << "\n";
  }
  
  return object;
}
