#include "mainwindow.h"
#include "bboxrenderer.h"
#include "raytracerenderer.h"
#include "raycastrenderer.h"

#define thus this

static const int w = 300;
MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent) {
  setFixedSize(w, 600);
  
  bboxRenderer = new QPushButton(this);
  bboxRenderer->setGeometry(0, 0, w, 25);
  bboxRenderer->setText("Bounding Box Renderer");
  connect(bboxRenderer, SIGNAL(clicked(bool)), SLOT(runbboxRenderer()));
  
  bboxRenderer = new QPushButton(this);
  bboxRenderer->setGeometry(0, 25, w, 25);
  bboxRenderer->setText("RayTrace Renderer");
  connect(bboxRenderer, SIGNAL(clicked(bool)), SLOT(runRayTraceRenderer()));  
  
  raycastRenderer = new QPushButton(this);
  raycastRenderer->setGeometry(0, 50, w, 25);
  raycastRenderer->setText("Raycast Renderer");
  connect(raycastRenderer, SIGNAL(clicked(bool)), SLOT(runRaycastRenderer()));  
  
  int h = 50;
  
  QLabel * l = new QLabel(this);
  l->setStyleSheet("QLabel {background:black;}");
  l->setGeometry(0, h+24, w, 2);

  sceneLabel = new QLabel(thus);
  sceneLabel->setAlignment(Qt::AlignCenter);
  sceneLabel->setGeometry(0, h+24, w, 23);
  sceneLabel->setText("Сцена");
  
  open = new QPushButton(this);
  open->setGeometry(0, h+50, w, 25);
  open->setText("Открыть файл ...");
  connect(open, SIGNAL(clicked(bool)), SLOT(openNewScene()));
  
  editor = new QPushButton(this);
  editor->setGeometry(0, h+75, w, 25);
  editor->setText("Редактировать сцену");
  connect(editor, SIGNAL(clicked(bool)), SLOT(editScene()));
  
  h += 100;
  
  l = new QLabel(this);
  l->setStyleSheet("QLabel {background:black;}");
  l->setGeometry(0, h+24, w, 2);

  l = new QLabel(thus);
  l->setAlignment(Qt::AlignCenter);
  l->setGeometry(0, h+24, w, 23);
  l->setText("Освещение");
  
  table = new LightTable(this);
  table->setGeometry(0, h+50, w, 500);
  
  Info::scene.updateOpenCLLights();
}

MainWindow::~MainWindow() {}

void MainWindow::openNewScene() {
  QFileDialog * fd = new QFileDialog();
  fd->setAcceptMode(QFileDialog::AcceptOpen);
  QString path = fd->getOpenFileName();
  path.remove(path.lastIndexOf("."), path.length() - path.lastIndexOf("."));
  qDebug() << path;
  
  QString objPath = path + ".obj";
  QString mtlPath = path + ".mtl";
  QString sceneName = path.mid(path.lastIndexOf("/"), path.length() - path.lastIndexOf("/"));
  qDebug () << sceneName;
  
  if (!QFile(objPath).exists() ) {
    qDebug() << "obj not exist";
    return;
  } 
  if (!QFile(mtlPath).exists()) {
    qDebug() << "mtl not exist";
    return;
  }
  
  sceneLabel->setText(QString("Сцена (%1)").arg(sceneName));
  qDebug() << "setText";
  
  std::ifstream obj(objPath.toStdString().c_str());
  std::ifstream mtl(mtlPath.toStdString().c_str());
  Object3D *object = Object3D::fromObj(obj, mtl);
  
  qDebug() << "Create obj";
  
  Info::scene.objects.clear();
  Info::scene.objects.push_back(*object);  
  
  qDebug() << "Finish";
}

void MainWindow::editScene() {
  
}

void MainWindow::runRayTraceRenderer () {
  (new RayTraceRenderer)->show();
}

void MainWindow::runbboxRenderer() {
  (new BBoxRenderer())->show();
}

void MainWindow::runRaycastRenderer() {
  (new RaycastRenderer())->show();
}
