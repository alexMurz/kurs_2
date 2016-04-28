#ifndef RAYCASTRENDERER_H
#define RAYCASTRENDERER_H

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>

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
    static const int samplesPerDraw = 1000;
    
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
    QPushButton * cleanButton; // Loop clean canvas
    QCheckBox   * useOpenCL;
    QPushButton * prepareObjects;
    
    bool mutexPaint, mutexMatrix;
    QLineEdit * matView[16];
    QPushButton * setMatrixButton;
    
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
    
    void drawingButtonCheck();
    void cleanButtonCheck();
    
    void renderFrame(bool force = false);
    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *);
    
    void keyPressEvent(QKeyEvent * e);
    void mousePressEvent(QMouseEvent *);    
    /*
     * Matrix Control Slots
     */
    void setMatrix();
    void updateMatView();
    void slotFW(); // Right
    void slotLW(); // Left
    void slotBW(); // Back
    void slotRW(); // Right
    void slotUW(); // Up
    void slotDW(); // Down
    
    void slotTL(); // Left
    void slotTR(); // Right
    void slotTU(); // Up
    void slotTD(); // Down
    void slotSL(); // Scene Left
    void slotSR(); // Scene Right
    
};

#endif // RAYCASTRENDERER_H
