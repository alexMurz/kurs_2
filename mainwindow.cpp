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

  l = new QLabel(thus);
  l->setAlignment(Qt::AlignCenter);
  l->setGeometry(0, h+24, w, 23);
  l->setText("Lighting");
  
  lightList = new QListWidget(this);
  lightList->setGeometry(0, h+50, w, 400);
  connect(lightList, SIGNAL(activated(QModelIndex)), SLOT(showLightInfo(QModelIndex)));
  resetLightList();
}


void MainWindow::resetLightList() {
  lightList->clear();
  foreach (const LightSource & l, Info::scene.lights) {
    
    QString s;
    s.append( l.enabled ? "+ " : "- " );
    
    s.append("O:");
    s.append(QString::number(l.origin[0])); s.append(":");
    s.append(QString::number(l.origin[1])); s.append(":");
    s.append(QString::number(l.origin[2])); s.append(", ");
    
    s.append("D:"); 
    s.append(QString::number(l.diffuseLight[0])); s.append(":");
    s.append(QString::number(l.diffuseLight[1])); s.append(":");
    s.append(QString::number(l.diffuseLight[2])); s.append(", ");
    
    s.append("E:"); s.append(QString::number(l.energy));
    s.append(", ");
    
    s.append("Att:"); 
    s.append(QString::number(l.attenuation[0])); s.append(":");
    s.append(QString::number(l.attenuation[1])); s.append(":");
    s.append(QString::number(l.attenuation[2])); s.append(", ");    
    
    s.append("isEye:"); s.append( l.isEye?'1':'0' );
    
    lightList->addItem(s);
  }
}


void MainWindow::showLightInfo(QModelIndex idx) {
  LightSource & l = Info::scene.lights[idx.row()];
  LightView * lightView = new LightView(&l, this);
  lightView->show();
}


MainWindow::~MainWindow() {}

void MainWindow::runRayTraceRenderer () {
  (new RayTraceRenderer)->show();
  resetLightList();
}

void MainWindow::runbboxRenderer() {
  (new BBoxRenderer())->show();
}

void MainWindow::runRaycastRenderer() {
  (new RaycastRenderer())->show();
}
