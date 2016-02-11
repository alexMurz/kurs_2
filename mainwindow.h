#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

class MainWindow : public QMainWindow {
    Q_OBJECT
    
    QPushButton * bboxRenderer, * rayTraceRenderer;
    
  public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
  public slots:
    void runbboxRenderer();
    void runRayTraceRenderer();
};

#endif // MAINWINDOW_H
