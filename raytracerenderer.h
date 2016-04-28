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

#include <QFuture>

#include "geom.h"
#include "scene3d.h"
#include <cl.h>


typedef struct {
  cl_float3 v[3];
  cl_float3 n, sn[3];
  cl_float3 diff, spec;
  cl_float3 refl;
  cl_int isMirror;
  cl_float reflectivity, shininess;
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
    
    void resetLightIfNeeded();
    int lightCount;
    cl::Buffer b_light, b_lightCount;
    
    Matrix4x4 biasMatrix, biasMatrixInverse;
    Matrix4x4 proj, view, rayMatrix, vpMatrix;
    QImage * frame;
    QComboBox * samplesBox, * softLightBox;
    QFrame * uiFrame;
    QPushButton * drawingButton; // Loop painting
    QPushButton * frameButton; // Paint 1 frame
    
    QPushButton * prepareObjects; // Prepare All Objects
    
    QFuture<void> updateFuture;
    
    int buttonsState; // 0 - none, 1 - frame, 2 - drawing, 3 - all
    
    union {
        struct { int width, height, samplesInput, softLightInput; };
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
    
    int getSamplesCount();
    int getSoftLightSamples();
    
    /*
     * Matrix Control Slots
     */
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
