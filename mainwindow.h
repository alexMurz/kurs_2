#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QListWidget>

#include "lightview.h"

#include "info.h"
class MainWindow : public QMainWindow, LightViewUpdateListener {
    Q_OBJECT
    
    QPushButton * bboxRenderer, * rayTraceRenderer, * raycastRenderer;
    QListWidget * lightList;
    
    void resetLightList();
    
  public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
  public slots:
    virtual void updateLights() { 
      resetLightList(); 
      Info::scene.updateOpenCLLights();
    }
    
    void showLightInfo(QModelIndex idx);
    
    void runbboxRenderer();
    void runRayTraceRenderer();
    void runRaycastRenderer();
};

#endif // MAINWINDOW_H
