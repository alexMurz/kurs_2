#include "raytracerenderer.h"
#include <QDebug>
#include <QPainter>
#include <QtConcurrent>
#include <iostream>
#include <time.h>
#include <sys/timeb.h>
#include <QFile>
#include <QByteArray>

#include "info.h"

extern int getMilliCount();

static const int b_frame_id = 5;
static const int b_light_id = 6;
static const int b_lightCount_id = 7;

///////////////////////////////////////////////////

void RayTraceRenderer::updateMatView() {
  for (int i = 0; i < 16; i++) {
    matView[i]->setText( QString::number(view.v[i]) );
  }
}


RayTraceRenderer::RayTraceRenderer() : QWidget(0), clTriangles(0), frame(0), lightCount(0) {
  setMinimumWidth(400);
  setMinimumHeight(300);
  mutexPaint = false;
  mutexMatrix = false;
  quality = ssaa = 0;
  
  view = Matrix4x4::identity();
  /*
  view.translate(-4.5f, 0.0f, 4.5f);
  view = view * Matrix4x4::createYRotation(-82.5 * 3.1415 / 180.0f);
  /* */
//  /*
//  view.translate(0, 1.75, 7.25);
  view.translate(0, 0, 5);
//  view = view * Matrix4x4::createXRotation(-90 * 3.1415 / 180.0f);
  /* */
  /*
  view.translate(0, 7, 21);
  view = view * Matrix4x4::createXRotation(-15 * 3.1415 / 180.0f);
  /* */
  
  /*
  view.translate(0, 10, 10);
  view = view * Matrix4x4::createXRotation(-45 * 3.1415 / 180.0f);
  /* */
  /*
  view.translate(-3, 3, 0);
  view = view * Matrix4x4::createYRotation(-90 * 3.1415 / 180.0f);
  view = view * Matrix4x4::createXRotation(-15 * 3.1415 / 180.0f);
  /* */

  vpMatrix = Matrix4x4::identity();
  proj = Matrix4x4::identity();
  
  biasMatrix = Matrix4x4(
        0.5f, 0.0f, 0.0f, 0.5f,
        0.0f, 0.5f, 0.0f, 0.5f,
        0.0f, 0.0f, 0.5f, 0.5f,
        0.0f, 0.0f, 0.0f, 1.0f);
  biasMatrixInverse = Matrix4x4(
        2.0f, 0.0f, 0.0f,-1.0f, 
        0.0f, 2.0f, 0.0f,-1.0f, 
        0.0f, 0.0f, 2.0f,-1.0f, 
        0.0f, 0.0f, 0.0f, 1.0f);
  
  uiFrame = new QFrame(this);
  uiFrame->setFixedWidth(toolBarWidth);
  
  drawingButton = new QPushButton(uiFrame);
  drawingButton->setGeometry(0, 0, 200, 35);
  drawingButton->setText("Start");
  drawingButton->setCheckable(true);
  connect(drawingButton, SIGNAL(clicked(bool)), SLOT(drawingButtonCheck()));
  
  frameButton = new QPushButton(uiFrame);
  frameButton->setGeometry(0, 35, 200, 35);
  frameButton->setText("1 Frame");
  connect(frameButton, SIGNAL(clicked(bool)), SLOT(paintButtonCheck()));
  
  prepareObjects = new QPushButton(uiFrame);
  prepareObjects->setGeometry(0, 70, 200, 35);
  prepareObjects->setText("PrepareObj");
  connect(prepareObjects, SIGNAL(clicked()), SLOT(prepareObj()));
  
  int h = 100;

  quad = new QPushButton(uiFrame);
  quad->setGeometry(0, h, 60, 25);
  quad->setText("Quad");
  quad->setEnabled(false);
  connect(quad, SIGNAL(clicked(bool)), SLOT(setQuality()));
  
  half = new QPushButton(uiFrame);
  half->setGeometry(70, h, 60, 25);
  half->setText("Half");
  connect(half, SIGNAL(clicked(bool)), SLOT(setQuality()));
  
  full = new QPushButton(uiFrame);
  full->setGeometry(140, h, 60, 25);
  full->setText("Full");
  connect(full, SIGNAL(clicked(bool)), SLOT(setQuality()));
  
  h += 20;
  ssaax1 = new QPushButton(uiFrame);
  ssaax1->setGeometry(0, h, 60, 25);
  ssaax1->setText("x1");
  ssaax1->setEnabled(false);
  connect(ssaax1, SIGNAL(clicked(bool)), SLOT(setSamples()));
  
  ssaax4 = new QPushButton(uiFrame);
  ssaax4->setGeometry(50, h, 60, 25);
  ssaax4->setText("x4");
  connect(ssaax4, SIGNAL(clicked(bool)), SLOT(setSamples()));
  
  ssaax9 = new QPushButton(uiFrame);
  ssaax9->setGeometry(100, h, 60, 25);
  ssaax9->setText("x9");
  connect(ssaax9, SIGNAL(clicked(bool)), SLOT(setSamples()));
  
  ssaax16 = new QPushButton(uiFrame);
  ssaax16->setGeometry(150, h, 60, 25);
  ssaax16->setText("x16");
  connect(ssaax16, SIGNAL(clicked(bool)), SLOT(setSamples()));
  
  
  
//  samplesBox = new QComboBox(uiFrame);
//  samplesBox->setGeometry(0, h+15, 200, 50);
//  samplesBox->addItem("SSAAx1");
//  samplesBox->addItem("SSAAx4");
//  samplesBox->addItem("SSAAx9");
//  samplesBox->addItem("SSAAx16");  
  
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      int idx = i*4 + j;
      QLineEdit * v = new QLineEdit(uiFrame);
      v->setGeometry(j*50, h+50+i*30, 50, 30);
      matView[idx] = v;
    }
  }
  updateMatView();
  
  setMatrixButton = new QPushButton(uiFrame);
  setMatrixButton->setGeometry(25, h+170, 150, 30);
  setMatrixButton->setText("Set View Matrix");
  connect(setMatrixButton, SIGNAL(clicked(bool)), SLOT(setMatrix()));
  
  buttonsState = 0b11;  
  
  setFocusPolicy(Qt::WheelFocus);
      
  prepareCL();
}

int RayTraceRenderer::getSamplesCount() {
  return ssaa + 1;
}

void RayTraceRenderer::prepareCL() {
  
  QFile file(":/kernel/raytrace.cl");
  if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
    qDebug() << "Cant open kernel source" << file.error() << file.errorString();
    exit(1);
  }
  std::string kernel_code = file.readAll().toStdString();
  std::cerr << "Kernel Code Loaded!\n";
  
  //get all platforms (drivers)
  std::vector<cl::Platform> all_platforms;
  cl::Platform::get(&all_platforms);
  if(all_platforms.size()==0){
    qDebug() <<" No platforms found. Check OpenCL installation!\n";
    exit(1);
  }
  cl::Platform default_platform = all_platforms[0];
  
  //get default device of the default platform
  std::vector<cl::Device> all_devices;
  default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
  if(all_devices.size()==0){
    qDebug() <<" No devices found. Check OpenCL installation!\n";
    exit(1);
  }
  cl::Device default_device=all_devices[0];
  
  std::vector<cl::Device> device; device.push_back(default_device);
  
  context = cl::Context(device);
  
  cl::Program::Sources sources;
  
  sources.push_back(std::pair<const char*, unsigned long>(kernel_code.c_str(),kernel_code.length()));
  
  program = cl::Program(context, sources);
  if(program.build(device) != CL_SUCCESS){
    qDebug() << "Error building programm";
    std::cerr <<" Error building: "<< program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << "\n";
    exit(1);
  }
  
  // create buffers on the device
  b_viewport = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(int)*4);
  b_origin = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(float)*4);
  b_ray = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(float)*16);
  b_trianglesCount = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(int));
  b_lightCount = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(int)); 
  
  //create queue to which we will push commands for the device.
  queue = cl::CommandQueue(context, device[0]);
  pixelKernel = cl::Kernel(program, "rayCast");
  
  pixelKernel.setArg(0, b_viewport);
  pixelKernel.setArg(1, b_origin);
  pixelKernel.setArg(2, b_ray);
  pixelKernel.setArg(4, b_trianglesCount);
  pixelKernel.setArg(b_lightCount_id, b_lightCount);
  
  prepareObj();
}

void RayTraceRenderer::prepareObj() {
  triangles.clear();
  std::cerr << "Preparing objects ... ";
  int mill = getMilliCount();
  foreach (Object3D o, Info::scene.objects) {
    Matrix4x4 inv = o.getModel().inversed().transposed();
    foreach (Triangle3D * t, o.getTriangles()) {
      Triangle3D * tr = new Triangle3D();
      memcpy(tr, t, sizeof(Triangle3D));
      tr->v[0] = o.getModel() * t->v[0];
      tr->v[1] = o.getModel() * t->v[1];
      tr->v[2] = o.getModel() * t->v[2];
      tr->n = inv * t->n;
      tr->sn[0] = inv * t->sn[0];
      tr->sn[1] = inv * t->sn[1];
      tr->sn[2] = inv * t->sn[2];
      tr->material = t->material;
      triangles.push_back(tr);
    }
  }
  
  if (clTriangles) delete clTriangles;
  clTriangles = new CLTriangle3D[triangles.size()];
  for (size_t i = 0; i < triangles.size(); i++) {
    
    
    CLTriangle3D & t = clTriangles[i];
    const Triangle3D * tr = triangles[i];
    
    for (int j = 0; j < 3; j++) {
      t.min.s[j] = tr->v[0][j];
      t.max.s[j] = tr->v[0][j];
    }
    
    for (int j = 0; j < 3; j++) {
      t.v0.s[j]  = tr->v[0][j];
      t.v1.s[j]  = tr->v[1][j];
      t.v2.s[j]  = tr->v[2][j];
      
      if (t.min.s[j] > t.v0.s[j]) t.min.s[j] = t.v0.s[j];
      if (t.min.s[j] > t.v1.s[j]) t.min.s[j] = t.v1.s[j];
      if (t.min.s[j] > t.v2.s[j]) t.min.s[j] = t.v2.s[j];
      if (t.max.s[j] > t.v0.s[j]) t.max.s[j] = t.v0.s[j];
      if (t.max.s[j] > t.v1.s[j]) t.max.s[j] = t.v1.s[j];
      if (t.max.s[j] > t.v2.s[j]) t.max.s[j] = t.v2.s[j];
      
      t.n.s[j] = tr->n[j];
      t.n0.s[j] = tr->sn[0][j];
      t.n1.s[j] = tr->sn[1][j];
      t.n2.s[j] = tr->sn[2][j];
      
      t.diff.s[j] = tr->material->diffuseReflection[j];
      t.spec.s[j] = tr->material->specularReflection[j];
      t.refl.s[j] = tr->material->reflection[j];
    }
    t.isMirror = tr->material->isMirror;
    t.reflectivity = tr->material->reflectivity;
    t.shininess = tr->material->shininess;
    t.transparency = tr->material->transparency;
    
    t.refraction = 1.f;
    if (t.diff.s[2] == 0.8f) t.refraction = 1.22f;
  }
  
  b_triangles = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(CLTriangle3D) * triangles.size());
  pixelKernel.setArg(3, b_triangles);
  queue.enqueueWriteBuffer(b_triangles, CL_TRUE, 0, sizeof(CLTriangle3D) * triangles.size(), clTriangles);
  std::cerr << "ready in " << getMilliCount() - mill << "ms!\n";
}

void RayTraceRenderer::resetLightIfNeeded() {
  if (lightCount != Info::scene.lights.size()) {
    lightCount = Info::scene.lights.size();
    
    b_light = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(ClLightSource)*lightCount);
    pixelKernel.setArg(b_light_id, b_light);
    
    queue.enqueueWriteBuffer(b_lightCount, CL_TRUE, 0, sizeof(int), &lightCount);
    
    qDebug() << "enqueue new light info : size =" << lightCount;
    qDebug() << "light[0] =" << sizeof(ClLightSource);
  }
  
  queue.enqueueWriteBuffer(b_light, CL_TRUE, 0, sizeof(ClLightSource)*lightCount, &Info::scene.lightsAsOpenCL[0]);
}

void RayTraceRenderer::renderCL() {
  qDebug() << "Start ... ";
  int start = getMilliCount();

  resetLightIfNeeded();
  
  Vec3f origin = (view * Vec4f(0.0, 0.0, 0.0, 1.0));
  
  int s = 1;
  if (quality == 0) s = 4;
  else if (quality == 1) s = 2;
  int w = width / s;
  int h = height / s;
  
  // Set matrix
  while(mutexMatrix); 
  mutexMatrix = true;

  vpMatrix[0][0] = 1.0 / w;
  vpMatrix[1][1] = 1.0 / h;
  
  rayMatrix = (view * proj) * biasMatrixInverse * vpMatrix;
  mutexMatrix = false;
  
 
  static int param[4];
  param[0] = w; param[1] = h;
  param[2] = getSamplesCount();
  
  queue.enqueueWriteBuffer(b_viewport, CL_TRUE, 0, sizeof(int)*4, param);
  static float orig[4];
  for (int i = 0; i < 3; i++) orig[i] = origin[i];
  static float mat[16];
  for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) mat[i*4+j] = rayMatrix[i][j];
  queue.enqueueWriteBuffer(b_origin, CL_TRUE, 0, sizeof(float)*4, orig);
  queue.enqueueWriteBuffer(b_ray, CL_TRUE, 0, sizeof(float)*16, mat);
  
  int count = triangles.size();
  queue.enqueueWriteBuffer(b_trianglesCount, CL_TRUE, 0, sizeof(int), &count);
  
  queue.enqueueNDRangeKernel(pixelKernel, cl::NDRange(0, 0), cl::NDRange(w, h), cl::NullRange);
  queue.finish();
  
  int * r = new int[w*h];
  queue.enqueueReadBuffer(b_frame, CL_TRUE, 0, sizeof(int)*w*h, r);
  
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
       frame->setPixel(x, y, r[(x/s)+(y/s)*w]);
    }
  }
  
  delete r;
  
  qDebug() << getMilliCount() - start;
}

void RayTraceRenderer::renderFrame(bool force) {
  
  if (!force)
    if (!isVisible() ||  // Window closed
        !drawingButton->isChecked() // Button unchecked
        ) return; // Stop loop  
  
  renderCL();
  
  if (force) buttonsState = 0b11;
        
  update(); // call paintEvent
  updateFuture = QtConcurrent::run(this, &RayTraceRenderer::renderFrame, false); // Run new loop
}

void RayTraceRenderer::resizeEvent(QResizeEvent * e) {
  QWidget::resizeEvent(e);
  int ow = width; int oh = height;
  width = e->size().width() - toolBarWidth; height = e->size().height();
  
  QImage * newFrame = new QImage(width, height, QImage::Format_RGB32);
  if (frame) // Is not first frame
    for (int x = 0; x < width; x++)
      for (int y = 0; y < height; y++) 
        if (x >= ow || y >= oh) 
          newFrame->setPixel(x, y, 0x000000);
        else
          newFrame->setPixel(x, y, frame->pixel(x, y));
  else
    for (int x = 0; x < width; x++)
      for (int y = 0; y < height; y++) 
        newFrame->setPixel(x, y, 0x000000);
  
  if (frame) delete frame;
  frame = newFrame;
  
  
  uiFrame->setGeometry(width, 0, toolBarWidth, height);
  b_frame   = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(cl_int)*width*height);
  pixelKernel.setArg(b_frame_id, b_frame);
  
  float fovy = 45*3.1415/180.0f;
  float aspect = float(width)/height;
  float tany = tan(fovy/2);
//  proj = Matrix4x4::createPerspective(fovy, aspect, 0.5f, 10.0f);
//  proj = Matrix4x4::identity();
//  proj[0][0] = aspect;
  proj[0][0] = tany * aspect;
  proj[1][1] = tany;
//  proj[2][2] = 0.0f;
//  proj[3][2] =-1.0f;
//  proj[2][3] = -2.0f;
//  proj[3][2] = -1.0f;
  
//  if (zbuffer) delete zbuffer;
//  zbuffer = new float[width*height];
}

void RayTraceRenderer::drawingButtonCheck() {
  if (drawingButton->isChecked()) {
    drawingButton->setText("Stop");
    buttonsState = 0b10;
    if (!updateFuture.isRunning())
      updateFuture = QtConcurrent::run(this, &RayTraceRenderer::renderFrame, false);
  } else {
    buttonsState = 0b11;    
    drawingButton->setText("Start");
  }
}

void RayTraceRenderer::paintButtonCheck() {
  if (drawingButton->isChecked()) return;
  
  frameButton->setEnabled(false);
  buttonsState = 0b00;
  updateFuture = QtConcurrent::run(this, &RayTraceRenderer::renderFrame, true);  
}

void RayTraceRenderer::paintEvent(QPaintEvent *) {
//  frameButton->setEnabled(true);
  
  frameButton->setEnabled(    (buttonsState&0b01) > 0 );
  drawingButton->setEnabled(  (buttonsState&0b10) > 0 );
  
  QPainter p(this);
  p.drawImage(QPoint(0, 0), *frame);
  update();
}


/*
 * Matrix Control
 */
static const float turnSpeed = 5.0 * 3.1415 / 180.0;
static const float moveSpeed = 0.25f;
void RayTraceRenderer::slotTL() { 
  while(mutexMatrix); 
  mutexMatrix = true; 
  view = view * Matrix4x4::createYRotation( turnSpeed); 
  mutexMatrix = false; 
}
void RayTraceRenderer::slotFW() { 
  while(mutexMatrix); 
  mutexMatrix = true; 
  view = view * Matrix4x4::createTranslate(0, 0,-moveSpeed);
  mutexMatrix = false;
}
void RayTraceRenderer::slotTR() { 
  while(mutexMatrix); 
  mutexMatrix = true;
  view = view * Matrix4x4::createYRotation(-turnSpeed); 
  mutexMatrix = false; 
}
void RayTraceRenderer::slotLW() { 
  while(mutexMatrix); 
  mutexMatrix = true; 
  view = view * Matrix4x4::createTranslate(-moveSpeed, 0, 0);
  mutexMatrix = false; 
}
void RayTraceRenderer::slotBW() { 
  while(mutexMatrix); 
  mutexMatrix = true; 
  view = view * Matrix4x4::createTranslate(0, 0, moveSpeed);
  mutexMatrix = false; 
}
void RayTraceRenderer::slotRW() { 
  while(mutexMatrix); 
  mutexMatrix = true; 
  view = view * Matrix4x4::createTranslate( moveSpeed, 0, 0);
  mutexMatrix = false; 
}
void RayTraceRenderer::slotSL() { 
  while(mutexMatrix); 
  mutexMatrix = true; 
  view = Matrix4x4::createYRotation(-turnSpeed) * view;
  mutexMatrix = false; 
}
void RayTraceRenderer::slotSR() { 
  while(mutexMatrix); 
  mutexMatrix = true; 
  view = Matrix4x4::createYRotation( turnSpeed) * view;
  mutexMatrix = false; 
}
void RayTraceRenderer::slotTU() {
  while(mutexMatrix); 
  mutexMatrix = true;
  view = view * Matrix4x4::createXRotation( turnSpeed); 
  mutexMatrix = false; 
}
void RayTraceRenderer::slotTD() {
  while(mutexMatrix); 
  mutexMatrix = true;
  view = view * Matrix4x4::createXRotation(-turnSpeed);
  mutexMatrix = false; 
}
void RayTraceRenderer::slotUW() {
  while(mutexMatrix); 
  mutexMatrix = true; 
  view = view * Matrix4x4::createTranslate(0, moveSpeed, 0);
  mutexMatrix = false; 
}
void RayTraceRenderer::slotDW() {
  while(mutexMatrix);
  mutexMatrix = true;
  view = view * Matrix4x4::createTranslate(0,-moveSpeed, 0);
  mutexMatrix = false;
}

void RayTraceRenderer::keyPressEvent(QKeyEvent * e) {
  int k = e->key();
  switch (k) {
    case Qt::Key_W: slotFW(); break;
    case Qt::Key_A: slotLW(); break;
    case Qt::Key_S: slotBW(); break;
    case Qt::Key_D: slotRW(); break;
    case Qt::Key_Space: slotUW(); break;
    case Qt::Key_Shift: slotDW(); break;
      
    case Qt::Key_Q: slotTL(); break;
    case Qt::Key_E: slotTR(); break;
    case Qt::Key_Z: slotSL(); break;
    case Qt::Key_C: slotSR(); break;

    case Qt::Key_R: slotTU(); break;
    case Qt::Key_F: slotTD(); break;      
  }
  updateMatView();
}
