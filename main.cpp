#include "mainwindow.h"
#include <QApplication>
#include "info.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  
  LightSource l = LightSource(Vec3f(-2.0f, 4.0f, -2.0f), Vec3f(1.0, 1.0, 1.0), Vec3f(1.0, 1.0, 1.0), 5.0f, 
                              Vec3f(0.0, 1.0, 0.0), false, true);
  Info::scene.lights.push_back(l);
  
  /*
  std::ifstream obj("../../../../obj/inn_1.obj");
  std::ifstream mtl("../../../../obj/inn_1.mtl");
  Object3D *object = Object3D::fromObj(obj, mtl);
  object->getModel().translate(-2, 0, 2.5);
  qDebug() << object->getTriangles().size();
  Info::scene.objects.push_back(*object);
  /* */
  
//  /*
  std::ifstream obj("../../../../obj/refl_scene.obj");
  std::ifstream mtl("../../../../obj/refl_scene.mtl");
  Object3D *object = Object3D::fromObj(obj, mtl);
  Info::scene.objects.push_back(*object);
  /* */
  /*
  std::ifstream obj("../../../../obj/plane.obj");
  std::ifstream mtl("../../../../obj/plane.mtl");
  Object3D *object = Object3D::fromObj(obj, mtl);
  Info::scene.objects.push_back(*object);
  /* */
  
  /*
  Material * mat = new Material();
  Object3D o;
  o.add(Vec3f(-1.0,-1.0,-5.0), Vec3f(-1.0, 1.0,-5.0), Vec3f( 1.0,-1.0,-5.0),
        Vec3f( 0.0, 0.0, 1.0), mat);
  Info::scene.objects.push_back(o);
  */
  
  /*
  {
    std::ifstream obj("../../../../obj/cube.obj");
    std::ifstream mtl("../../../../obj/cube.mtl");
    Object3D *object = Object3D::fromObj(obj, mtl);
    Info::scene.objects.push_back(*object);
  }
  Material * mat;
  
  float def = 0.95f;
  Object3D right;
  mat = new Material(Vec3f(0.0, 0.0, 0.0), Vec3f(def, def, def));
  right.add(Vec3f( 5.0,-5.0,-5.0), Vec3f( 5.0,-5.0, 5.0), Vec3f( 5.0, 5.0,-5.0), 
                 Vec3f(-1.0, 0.0, 0.0), mat);
  right.add(Vec3f( 5.0, 5.0, 5.0), Vec3f( 5.0,-5.0, 5.0), Vec3f( 5.0, 5.0,-5.0), 
                 Vec3f(-1.0, 0.0, 0.0), mat);
  Info::scene.objects.push_back(right);
  
  Object3D left;
  mat = new Material(Vec3f(0.0, 0.0, 0.0), Vec3f(def, def, def));
  left.add(Vec3f(-5.0,-5.0,-5.0), Vec3f(-5.0,-5.0, 5.0), Vec3f(-5.0, 5.0,-5.0), 
           Vec3f( 1.0, 0.0, 0.0), mat);
  left.add(Vec3f(-5.0, 5.0, 5.0), Vec3f(-5.0,-5.0, 5.0), Vec3f(-5.0, 5.0,-5.0), 
           Vec3f( 1.0, 0.0, 0.0), mat);
  Info::scene.objects.push_back(left);
  
  Object3D far;
  mat = new Material(Vec3f(0.5, 0.5, 0.5), Vec3f(0.0, 0.0, 0.05));
  far.add(Vec3f(-5.0,-5.0,-5.0), Vec3f(-5.0, 5.0,-5.0), Vec3f( 5.0,-5.0,-5.0), 
          Vec3f( 0.0, 0.0, 1.0), mat);
  far.add(Vec3f( 5.0, 5.0,-5.0), Vec3f(-5.0, 5.0,-5.0), Vec3f( 5.0,-5.0,-5.0), 
          Vec3f( 0.0, 0.0, 1.0), mat);
  Info::scene.objects.push_back(far);
    
  /* */
  
//  float y = -1;
//  float v = 1;
//  Material * mat = new Material(0xF0F000);
//  Object3D plane = Object3D();
//  plane.add(Vec3f(-v, y, -v), Vec3f( v, y, -v), Vec3f(-v, y,  v), 
//            Vec3f(0.0, 1.0, 0.0), mat);
//  plane.add(Vec3f( v, y,  v), Vec3f( v, y, -v), Vec3f(-v, y,  v), 
//            Vec3f(0.0, 1.0, 0.0), mat);
//  Info::scene.objects.push_back(plane);
  
  MainWindow w;
  w.show();
//  qDebug() << sizeof(Object3D) << sizeof(float) << sizeof(std::vector<Triangle3D>);
  
  return a.exec();
}
