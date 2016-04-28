#include "mainwindow.h"
#include <QApplication>
#include "info.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  
  Info::scene.lights.push_back(LightSource(Vec3f(-2.0f, 4.0f, -2.0f), Vec3f(1.0, 1.0, 1.0), Vec3f(1.0, 1.0, 1.0), 5.0f, 
                                           Vec3f(0.0, 1.0, 0.0), false, true));
  Info::scene.lights.push_back(LightSource(Vec3f( 2.0f, 4.0f, -2.0f), Vec3f(1.0, 1.0, 1.0), Vec3f(1.0, 1.0, 1.0), 5.0f, 
                                           Vec3f(0.0, 1.0, 0.0), false, true));
//   /*
  std::ifstream obj("../../../../obj/refl_scene.obj");
  std::ifstream mtl("../../../../obj/refl_scene.mtl");
  Object3D *object = Object3D::fromObj(obj, mtl);
  Info::scene.objects.push_back(*object);
  // */
  
  std::cerr << sizeof(cl_double3) << " " << sizeof(Col3f) << "\n";
  
   /*
  std::ifstream obj("../../../../obj/room.obj");
  std::ifstream mtl("../../../../obj/room.mtl");
  Object3D *object = Object3D::fromObj(obj, mtl);
  Info::scene.objects.push_back(*object);
  // */
  
  MainWindow w;
  w.show();
  
  return a.exec();
}
