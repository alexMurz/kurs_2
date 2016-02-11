#include "mainwindow.h"
#include "bboxrenderer.h"
#include "raytracerenderer.h"

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent) {
  
  bboxRenderer = new QPushButton(this);
  bboxRenderer->setGeometry(0, 0, 200, 25);
  bboxRenderer->setText("Bounding Box Renderer");
  connect(bboxRenderer, SIGNAL(clicked(bool)), SLOT(runbboxRenderer()));
  
  bboxRenderer = new QPushButton(this);
  bboxRenderer->setGeometry(0, 25, 200, 25);
  bboxRenderer->setText("RayTrace Renderer");
  connect(bboxRenderer, SIGNAL(clicked(bool)), SLOT(runRayTraceRenderer()));  
}
MainWindow::~MainWindow() {}

void MainWindow::runRayTraceRenderer () {
  (new RayTraceRenderer)->show();
}

void MainWindow::runbboxRenderer() {
  (new BBoxRenderer())->show();
}
