#ifndef RAYTRACERENDERER_H
#define RAYTRACERENDERER_H

#include <QWidget>
#include <QImage>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QKeyEvent>

#include <QFrame>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>

#include <QFuture>

#include "geom.h"
#include "scene3d.h"
#include <cl.h>


typedef struct {
  cl_float3 v0, v1, v2;
  cl_float3 n, n0, n1, n2;
  cl_float3 diff, spec;
  cl_float3 refl;
  cl_int isMirror;
  cl_float reflectivity, shininess, transparency;
  cl_float refraction;
  
  cl_float3 min, max;
} CLTriangle3D;

class RayTraceRenderer : public QWidget {
    Q_OBJECT
    static const int toolBarWidth = 200;
    static const float MAXDISTANCE = 10000000;
    static const int RAYSAMPLES = 1;
    static const int jumpCount = 12;
    
    bool mutexPaint, mutexMatrix;
    
    // OpenCL
    void prepareCL();
    void renderCL();
    cl::Context context;
    cl::CommandQueue queue;
    cl::Program program;
    cl::Kernel pixelKernel;
    std::vector<Triangle3D*> triangles;
    CLTriangle3D * clTriangles;
    
    // OpenCL Buffers
    cl::Buffer b_viewport, b_origin, b_ray, b_triangles, b_trianglesCount, b_frame;
    cl::Buffer b_kdTree, b_kdTreeSize, b_kdTreeStart, b_kdTreeStartSize, b_kdTreeLength, b_kdTreeLengthSize, b_kdParams;
    
    // kD-tree
    std::vector<int> kdTree; // As Vector (actually 3D Vector)
    std::vector<int> kdTreeStart; // Contain amount of triangles per 3D Node of kdTree
    std::vector<int> kdTreeLength;
    union {
      struct { float kdXStart, kdYStart, kdZStart, kdXLength, kdYLength, kdZLength, kdStep; };
      float kdParams[7];
    };
    
    void resetLightIfNeeded();
    int lightCount;
    cl::Buffer b_light, b_lightCount;
    
    Matrix4x4 biasMatrix, biasMatrixInverse;
    Matrix4x4 proj, view, rayMatrix, vpMatrix;
    QImage * frame;
    QFrame * uiFrame;
    
    QPushButton * ssaax1, * ssaax4, * ssaax9, * ssaax16;
    int ssaa;
    QPushButton * quad, * half, * full; // Quality
    int quality;
    
    QPushButton * drawingButton; // Loop painting
    QPushButton * frameButton; // Paint 1 frame
    
    QPushButton * prepareObjects; // Prepare All Objects
    
    QFuture<void> updateFuture;
    QLineEdit * matView[16];
    QPushButton * setMatrixButton;
    
    int buttonsState; // 0 - none, 1 - frame, 2 - drawing, 3 - all
    
    union {
        struct { int width, height, samplesInput, unused; };
        int viewport[4];
    };
    
  public:
    explicit RayTraceRenderer();
    
  public slots:
    
    void prepareObj();
    
    void paintButtonCheck();
    void drawingButtonCheck();
    
    void renderFrame(bool force = false);
    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *);
    
    
    // Quality
    int getSamplesCount();
    void setQuality() {
      if (sender() == quad) {
        quality = 0;
        quad->setEnabled(false); half->setEnabled(true); full->setEnabled(true); 
      } else if (sender() == half) {
        quality = 1;
        quad->setEnabled(true); half->setEnabled(false); full->setEnabled(true); 
      } else if (sender() == full) { 
        quality = 2;
        quad->setEnabled(true); half->setEnabled(true); full->setEnabled(false);
      }
    }
    void setSamples() {
      if (sender() == ssaax1) {
        ssaa = 0;
        ssaax1->setEnabled(false); ssaax4->setEnabled(true); ssaax9->setEnabled(true); ssaax16->setEnabled(true); 
      } else if (sender() == ssaax4) {
        ssaa = 1;
        ssaax1->setEnabled(true); ssaax4->setEnabled(false); ssaax9->setEnabled(true); ssaax16->setEnabled(true); 
      } else if (sender() == ssaax9) {
        ssaa = 2;
        ssaax1->setEnabled(true); ssaax4->setEnabled(true); ssaax9->setEnabled(false); ssaax16->setEnabled(true); 
      } else if (sender() == ssaax16) {
        ssaa = 3;
        ssaax1->setEnabled(true); ssaax4->setEnabled(true); ssaax9->setEnabled(true); ssaax16->setEnabled(false); 
      }
    }
    
    /*
     * Matrix Control Slots
     */
    void mousePressEvent(QMouseEvent *) {
      for (int i = 0; i < 16; i++)
        if (focusWidget() == matView[i])
          focusWidget()->clearFocus();
    }
    void setMatrix() {
      for (int i = 0; i < 16; i++) {
        view.v[i] = matView[i]->text().toFloat();
      }
    }
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
};

#endif // RAYTRACERENDERER_H
