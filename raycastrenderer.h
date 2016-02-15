#ifndef RAYCASTRENDERER_H
#define RAYCASTRENDERER_H

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QFrame>
#include <QLabel>

#include <QFuture>
#include <QtConcurrent>

#include <QResizeEvent>

#include <QImage>
#include <QPainter>

#include "info.h"

class RaycastRenderer : public QWidget
{
    Q_OBJECT
    static const int toolBarWidth = 200;
    static const int photonMaxLength = 100000;
    static const int jumpCount = 12;
    static const int samples = 100000;
    
    /*
     * RayTrace stuff to cast rays
     */
    bool sendRay(const Vec3f & origin, const Vec3f & ray, float maxDistance = photonMaxLength, const Triangle3D * skip = 0);
    bool sendRay(const Vec3f & origin, const Vec3f & ray, Vec3f & point, float & dist, Triangle3D *& triangle, float maxDistance = photonMaxLength, Triangle3D * skip = 0);
    bool intersect(Triangle3D * tr, const Vec3f &Origin, const Vec3f &Ray, float MaxDistance, float &Distance, Vec3f &Point,
                   float * os = 0, float * ot = 0);
    
    Vec2f rayInEyePos(const Vec3f & p1, const Vec3f & p2, bool & hit);
    
    Matrix4x4 view;
    
    QImage * frame;    
    
    QFrame * uiFrame;
    QPushButton * drawingButton; // Loop painting
    QPushButton * frameButton; // Paint 1 frame
    QCheckBox   * useOpenCL;
    QPushButton * prepareObjects;
    
    std::vector<Triangle3D*> triangles;
    
    QFuture<void> updateFuture;
    
    void cpuRender(QImage * buffer);
    
    union {
        struct { int width, height; };
        int viewport[2];
    };
  public:
    RaycastRenderer();
    
  public slots:
    void prepareObj();
    
    void paintButtonCheck();
    void drawingButtonCheck();
    
    void renderFrame(bool force = false);
    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *);
};

#endif // RAYCASTRENDERER_H
