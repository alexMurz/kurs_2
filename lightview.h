#ifndef LIGHTVIEW_H
#define LIGHTVIEW_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QDoubleValidator>
#include <QPushButton>
#include <scene3d.h>

class LightViewUpdateListener {
  public:
    virtual void updateLights() = 0;
};

class LightView : public QWidget {
    Q_OBJECT
    
    /*Vec3f origin, diffuseLight;
    float energy;
    bool  isEye;
    Vec3f attenuation; // Constant, Linear, Quadratic
    */
    QLineEdit * diffRed, * diffGreen, * diffBlue; // Diffuse
    QLineEdit * specRed, * specGreen, * specBlue; // Diffuse
    QLineEdit * originX, * originY, * originZ; // Origin
    QLineEdit * energy; // Energy
    QComboBox * isEye; 
    QLineEdit * attConst, * attLin, * attQuad;
    
    QPushButton * apply, * cancel;
    
  public:
    LightSource * source;
    explicit LightView(LightSource * source, LightViewUpdateListener * delegate, QWidget *parent = 0);
    LightViewUpdateListener * delegate;
    
  signals:
    
  public slots:
    void cancelSlot();
    void applySlot();
};

#endif // LIGHTVIEW_H
