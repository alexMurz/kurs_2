#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QListWidget>

#include <QTableWidget>
#include <QFileDialog>

#include "lightview.h"

#include "info.h"

#include <QBoxLayout>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QMouseEvent>


class Node : public QPushButton, protected LightViewUpdateListener {
    Q_OBJECT
    
    LightSource * source;
    
    QPushButton * enable;
    QLabel * pos;
    QLabel * diff, * spec;
    
  public:
    explicit Node(LightSource * light, QWidget * parent = 0) : QPushButton(parent), source(light) {
      setFixedSize(250, 150);
      setStyleSheet("border: 1px solid black");
      
      pos = new QLabel(this);
      pos->setGeometry(10, 10, 110, 25);
      
      enable = new QPushButton(this);
      enable->setGeometry(130, 10, 110, 25);
      enable->setText("Выключить");
      enable->setStyleSheet("border: 1px solid black; background:#A0FFA0");
      connect(enable, SIGNAL(clicked(bool)), SLOT(onEnableClick()));
      
      diff = new QLabel(this);
      diff->setGeometry(10, 45, 110, 25);
      
      spec = new QLabel(this);
      spec->setGeometry(130, 45, 110, 25);
      
      connect(this, SIGNAL(clicked(bool)), SLOT(onClick()));
    }
    
    void updateLights() { 
      reset();
      Info::scene.updateOpenCLLights();
    }
    
    void reset(LightSource * s) {
      source = s;
      
      pos->setText( QString("Pos: {%1; %2; %3}").arg(s->origin[0]).arg(s->origin[1]).arg(s->origin[2]) );
      diff->setText(QString("Dif: {%1; %2, %3}").arg(s->diffuseLight[0]).arg(s->diffuseLight[1]).arg(s->diffuseLight[2]));
      spec->setText(QString("Spe: {%4; %5; %6}").arg(s->specularLight[0]).arg(s->specularLight[1]).arg(s->specularLight[2]));
    }
    void reset() { reset(source); }
    
  public slots:
    void onEnableClick() {
      if (source->enabled) { 
        enable->setText("Включить");
        enable->setStyleSheet("border: 1px solid black; background:#FFA0A0");
      } else {
        enable->setText("Выключить");
        enable->setStyleSheet("border: 1px solid black; background:#A0FFA0");        
      }
      source->enabled = !source->enabled;
      Info::scene.updateOpenCLLights();      
    }

    void onClick() {
      LightView * lightView = new LightView(source, this);
      lightView->show();
    }
};

class LightTable : public QWidget {
    Q_OBJECT
    
    QVBoxLayout * layout;
    std::vector<LightSource> & lights;
    
  public:
    explicit LightTable(QWidget * parent = 0) : QWidget(parent), lights(Info::scene.lights) {
      
      QScrollArea * scroll = new QScrollArea(this);
      scroll->setFixedSize(300, 400);
      layout = new QVBoxLayout;
            
      QWidget *centralW = new QWidget;
      centralW->setLayout(layout);
      scroll->setWidgetResizable(true);
      scroll->setWidget(centralW);
      
      reset();
    }
    
    void reset() {
      
      while (lights.size() > layout->count()) layout->addWidget(new Node(0));
      while (lights.size() < layout->count()) layout->removeWidget( layout->itemAt(layout->count()-1)->widget() );
      
      for (int i = 0; i < lights.size(); i++) {
        Node * l = dynamic_cast<Node *>( layout->itemAt(i)->widget() );
        l->reset( &lights[i] );
      }
      
    }
    
};



class MainWindow : public QMainWindow {
    Q_OBJECT
    
    QPushButton * bboxRenderer, * rayTraceRenderer, * raycastRenderer, * editor, * open;
    LightTable  * table;
    QLabel * sceneLabel;
    
    void resetLightList();
    
  public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
  public slots:
    
    void openNewScene();
    void editScene();
    
    void runbboxRenderer();
    void runRayTraceRenderer();
    void runRaycastRenderer();
};

#endif // MAINWINDOW_H
