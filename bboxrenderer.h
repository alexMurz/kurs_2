#ifndef BBOXRENDERER_H
#define BBOXRENDERER_H

#include <QWidget>
#include <QImage>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QThread>
#include <QTimer>
#include <QFuture>
#include <QKeyEvent>

#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>

#include "info.h"

class BBoxRenderer : public QWidget {
    Q_OBJECT
    
    // Transforming
    float * zbuffer;
    bool mutexMatrix;
    Vec4f completeProjection(Vec4f v, float & sign);
    Col3i colorCalculation(Matrix4x4 mv, Vec4f v, Vec3f n, Material * mat);
    Matrix4x4 proj;
    Matrix4x4 view;
    
    // OpenCL
    cl::Context context;
    cl::CommandQueue queue;
    cl::Program program;
    
    cl::Kernel pixelKernel;
    
    // OpenCL Buffers
    cl::Buffer b_viewport;
    cl::Buffer b_matrixes;
    cl::Buffer b_v;
    cl::Buffer b_V;
    cl::Buffer b_n;
    cl::Buffer b_diff;
    cl::Buffer b_zbuffer;
    cl::Buffer b_frame;
    cl::Buffer b_material;
    
    void prepareCL();
    
    // Rendering
    void renderPixel();
    void renderPixelCL();
    int pointVisibility(Vec4f p);
    
    static const int toolBarWidth = 200;
    
    // UI
    QFrame * uiFrame;
    QPushButton * drawingButton; // Loop painting
    QPushButton * frameButton; // Paint 1 frame
    QCheckBox   * useCLCheckBox;
    
    QFuture<void> updateFuture;
    bool mutexPaint, firstRun;
    QImage * frame;
    QLineEdit * matView[16];
    QPushButton * setMatrixButton;
    
    union {
      struct { int width, height; };
      int viewport[2];
    };
    
  public:
    explicit BBoxRenderer();
    
  private slots:
    
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
    
    void keyPressEvent(QKeyEvent *);
    
    void drawingButtonCheck();
    
    void renderFrame();
    void resizeEvent(QResizeEvent *);    
    void paintEvent(QPaintEvent *);
};

#endif // BBOXRENDERER_H
