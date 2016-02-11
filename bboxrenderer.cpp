#include "bboxrenderer.h"
#include <QDebug>
#include <QPainter>
#include <QtConcurrent>
#include <iostream>
#include <time.h>
#include <sys/timeb.h>
#include <QFile>
#include <QByteArray>

int getMilliCount(){
  timeb tb;
  ftime(&tb);
  int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
  return nCount;
}

///////////////////////////////////////////////////

BBoxRenderer::BBoxRenderer() : QWidget(0), zbuffer(0), frame(0) {
  setMinimumWidth(400);
  setMinimumHeight(300);
  setGeometry(geometry().x(), geometry().y(), 400, 200);
  mutexPaint = false;
  mutexMatrix = false;
  
//  /*
//  view = Matrix4x4::createTranslate(0, -7,-21);
  view = Matrix4x4::createTranslate(0, -2, 0);
//  view = view * Matrix4x4::createYRotation(45 * 3.1415 / 180.0f);
  /* */
  
  /*
  view = Matrix4x4::createTranslate(0, 0, -10);
  /* */
 
  uiFrame = new QFrame(this);
  uiFrame->setFixedWidth(200);
  
  drawingButton = new QPushButton(uiFrame);
  drawingButton->setGeometry(0, 0, 200, 35);
  drawingButton->setText("Start");
  drawingButton->setCheckable(true);
  connect(drawingButton, SIGNAL(clicked(bool)), SLOT(drawingButtonCheck()));
  
  frameButton = new QPushButton(uiFrame);
  frameButton->setGeometry(0, 35, 200, 35);
  frameButton->setText("1 Frame");
  connect(frameButton, SIGNAL(clicked(bool)), SLOT(renderFrame()));
  
  useCLCheckBox = new QCheckBox(uiFrame);
  useCLCheckBox->setGeometry(0, 70, 25, 25);
  QLabel * useCLLabel = new QLabel(uiFrame);
  useCLLabel->setGeometry(25, 70, 175, 25);
  useCLLabel->setText("use OpenCL");
  
  setMatrixButton = new QPushButton(uiFrame);
  setMatrixButton->setGeometry(25, 230, 150, 30);
  setMatrixButton->setText("Set View Matrix");
  connect(setMatrixButton, SIGNAL(clicked(bool)), SLOT(setMatrix()));
  
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      int idx = i*4 + j;
      QLineEdit * v = new QLineEdit(uiFrame);
      v->setGeometry(j*50, 100+i*30, 50, 30);
      matView[idx] = v;
    }
  }
  updateMatView();
  
  prepareCL();
}

void BBoxRenderer::mousePressEvent(QMouseEvent *) {
  for (int i = 0; i < 16; i++)
    if (focusWidget() == matView[i])
      focusWidget()->clearFocus();
}

void BBoxRenderer::setMatrix() {
  for (int i = 0; i < 16; i++) {
    view.v[i] = matView[i]->text().toFloat();
  }
}

void BBoxRenderer::updateMatView() {
  for (int i = 0; i < 16; i++) {
    matView[i]->setText( QString::number(view.v[i]) );
  }
}

/*
 * Matrix Control
 */
static const float turnSpeed = 5.0 * 3.1415 / 180.0;
static const float moveSpeed = 0.25f;
void BBoxRenderer::slotTL() { 
  while(mutexMatrix); 
  mutexMatrix = true; 
  view = Matrix4x4::createYRotation(-turnSpeed) * view; 
  updateMatView();
  mutexMatrix = false; 
}
void BBoxRenderer::slotFW() { 
  while(mutexMatrix); 
  mutexMatrix = true; 
  view.translate(0, 0, moveSpeed); 
  updateMatView();
  mutexMatrix = false;
}
void BBoxRenderer::slotTR() { 
  while(mutexMatrix); 
  mutexMatrix = true;
  view = Matrix4x4::createYRotation( turnSpeed) * view; 
  updateMatView();
  mutexMatrix = false; 
}
void BBoxRenderer::slotLW() { 
  while(mutexMatrix); 
  mutexMatrix = true; 
  view.translate( moveSpeed, 0, 0); 
  updateMatView();
  mutexMatrix = false; 
}
void BBoxRenderer::slotBW() { 
  while(mutexMatrix); 
  mutexMatrix = true; 
  view.translate( 0, 0,-moveSpeed); 
  updateMatView();
  mutexMatrix = false; 
}
void BBoxRenderer::slotRW() { 
  while(mutexMatrix); 
  mutexMatrix = true; 
  view.translate(-moveSpeed, 0, 0); 
  updateMatView();
  mutexMatrix = false; 
}
void BBoxRenderer::slotSL() { 
  while(mutexMatrix); 
  mutexMatrix = true; 
  view = view * Matrix4x4::createYRotation(-turnSpeed);
  updateMatView();
  mutexMatrix = false; 
}
void BBoxRenderer::slotSR() { 
  while(mutexMatrix); 
  mutexMatrix = true; 
  view = view * Matrix4x4::createYRotation( turnSpeed); 
  updateMatView();
  mutexMatrix = false; 
}
void BBoxRenderer::slotTU() {
  while(mutexMatrix); 
  mutexMatrix = true;
  view = Matrix4x4::createXRotation(-turnSpeed) * view; 
  updateMatView();
  mutexMatrix = false; 
}
void BBoxRenderer::slotTD() {
  while(mutexMatrix); 
  mutexMatrix = true;
  view = Matrix4x4::createXRotation( turnSpeed) * view; 
  updateMatView();
  mutexMatrix = false; 
}
void BBoxRenderer::slotUW() {
  while(mutexMatrix); 
  mutexMatrix = true; 
  view.translate(0,-moveSpeed, 0);
  updateMatView();
  mutexMatrix = false; 
}
void BBoxRenderer::slotDW() {
  while(mutexMatrix);
  mutexMatrix = true;
  view.translate(0, moveSpeed, 0);
  updateMatView();
  mutexMatrix = false;
}

void BBoxRenderer::keyPressEvent(QKeyEvent * e) {
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

void BBoxRenderer::prepareCL() {
  
  QFile file(":/kernel/bbox.cl");
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
    std::cerr <<" Error building: "<< program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device)<<"\n";
    exit(1);
  }
  
  // create buffers on the device
  b_viewport = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(cl_int)*2);
  b_matrixes = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(cl_float4)*4);
  b_v = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(cl_float4)*3);
  b_V = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(cl_float4)*3);
  b_n = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(cl_float3));
  b_material = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(cl_int)*2);
  
  //create queue to which we will push commands for the device.
  queue = cl::CommandQueue(context, device[0]);
  pixelKernel = cl::Kernel(program, "triangleRender");
  
  pixelKernel.setArg(0, b_viewport);
  pixelKernel.setArg(1, b_matrixes);
  pixelKernel.setArg(2, b_v);
  pixelKernel.setArg(3, b_V);
  pixelKernel.setArg(4, b_n);
  pixelKernel.setArg(7, b_material);
}

void BBoxRenderer::drawingButtonCheck() {
  if (drawingButton->isChecked()) {
    drawingButton->setText("Stop");
    if (!updateFuture.isRunning())
      updateFuture = QtConcurrent::run(this, &BBoxRenderer::renderFrame);
  } else {
    drawingButton->setText("Start");
  }
}

Vec4f BBoxRenderer::completeProjection(Vec4f v, float & sign) {
  v[0] *= 1/std::abs(v[3]);
  v[1] *= 1/std::abs(v[3]);
  v[2] *= 1/std::abs(v[3]);
  
  sign = v[2] > 1 ? -1 : 1;
  
//  qDebug() << "z = " << ((int)(v[2] * 100)) / 100.0f << " w = " << ((int)(v[3] * 100)) / 100.0f;
  
  
  v[0] = (v[0]*0.5+0.5)*width;
  v[1] = (v[1]*0.5+0.5)*height;
  v[2] = v[2]*0.5f + 0.5f;
//  v[2] = (v[2]*0.5+0.5);

    
  qDebug() << v.toString().c_str();
  
//  if (v[2] < 0) v[1] = -v[1];
  
  return v;
}

int BBoxRenderer::pointVisibility(Vec4f p) {
  int r = 0;
  if (p[0] > p[3]) r += 1;//0b000001;
  if (p[0] <-p[3]) r += 2;//0b000010;
  if (p[1] > p[3]) r += 4;//0b000100;
  if (p[1] <-p[3]) r += 8;//0b001000;
  if (p[2] > p[3]) r += 16;//0b010000;
  if (p[2] < 0)    r += 32;//0b100000;
  return r;
}

Col3i BBoxRenderer::colorCalculation(Matrix4x4 mv, Vec4f v, Vec3f n, Material * mat) {
  Vec3f L = (-v).normalize();
  Vec3f N = n.normalize();
  Vec3f E = -N;
  Vec3f R = (-reflect(L, N)).normalize();
  
  float diffuseTerm = clamp(L*N, 0.0, 1.0);
  
  Vec3f color = mat->diffuseReflection*diffuseTerm;
  
  return color * 255;
}

void BBoxRenderer::renderPixel() {
  int start = getMilliCount();
  
  foreach (const Object3D & object, Info::scene.objects) {
    
    mutexMatrix = true;
    Matrix4x4 mv = view * object.getModel();
    Matrix4x4 mvp = proj * mv;
    Matrix4x4 normalMatrix = mv.inversed().transposed();
    mutexMatrix = false;
    
    foreach (Triangle3D * triangle, object.getTriangles()) {
      
      Vec4f v[3] = {
        (mvp * triangle->v[0]),
        (mvp * triangle->v[1]),
        (mvp * triangle->v[2])
      };
      int vis = pointVisibility(v[0]) & pointVisibility(v[1]) & pointVisibility(v[2]);
      if (vis != 0) continue;
      
      float s[3];
      for (int i = 0; i < 3; i++) v[i] = completeProjection(v[i], s[i]);
       
      Vec4f V[3] = {
        mv * triangle->v[0],
        mv * triangle->v[1],
        mv * triangle->v[2]
      };
      for (int i = 0; i < 3; i++)
        for (int j = 0; j < 2; j++)
          V[i][j] *= s[i];
      
      Vec3f n = normalMatrix * triangle->n;
      
      Vec2i bboxmin = Vec2i(width, height);
      Vec2i bboxmax = Vec2i(0, 0);
      
      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
          float f = v[i][j];
          if (bboxmin[j] > f) bboxmin[j] = f;
          if (bboxmax[j] < f) bboxmax[j] = f;
        }
      }
      
      bboxmax[0] = bboxmax[0]>=width?width-1:bboxmax[0];
      bboxmax[1] = bboxmax[1]>=height?height-1:bboxmax[1];
      bboxmin[0] = bboxmin[0]<=0?1:bboxmin[0];
      bboxmin[1] = bboxmin[1]<=0?1:bboxmin[1];
      
      for (int x = bboxmin[0]; x <= bboxmax[0]; x++) {
        for (int y = bboxmin[1]; y <= bboxmax[1]; y++) {
          Vec3f c = Math::barycentric(v[0], v[1], v[2], Vec2i(x, y));
          if (c[0]<0 || c[1]<0 || c[2]<0) continue;
          
          float z = c[0]*v[0][2] + c[1]*v[1][2] + c[2]*v[2][2];
          if (z < 0 || z >= zbuffer[x+y*width]) continue;
          zbuffer[x+y*width] = z;
          
          Vec4f pos = Math::projectionCurvetureFix(
            V[0], V[1], V[2],
            v[0][3], v[1][3], v[2][3], 
            c
          );
          Col3i col = Col3i(255, 255, 255); // = colorCalculation(mv, pos, n, triangle->material);
          frame->setPixel(x, height-y, makeColor(col));
        }
      }
    }
  }
  
  qDebug() << getMilliCount() - start;
}

void BBoxRenderer::renderPixelCL() {
  int start = getMilliCount();
  
  const int & w = width;
  const int & h = height;
  const int s = w * h;
  
  queue.enqueueWriteBuffer(b_viewport, CL_TRUE, 0, sizeof(cl_int)*2, viewport);
  queue.enqueueWriteBuffer(b_frame, CL_TRUE, 0, sizeof(cl_int)*s, frame->bits());
  queue.enqueueWriteBuffer(b_zbuffer, CL_TRUE, 0, sizeof(cl_float)*s, zbuffer);
  
  float * matrixes = new float[4*4*2];
  float * v = new float[3*4];
  float * V = new float[3*4];
  float * n = new float[3];
  foreach (const Object3D & object, Info::scene.objects) {
    
    Matrix4x4 mv = view * object.getModel();
    Matrix4x4 mvp = proj * mv;
    Matrix4x4 normalMatrix = mv.inversed().transposed();
    
    for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) matrixes[i*4+j] = mv[i][j];
    for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) matrixes[i*4+j+16] = normalMatrix[i][j];
    
    foreach (Triangle3D * triangle, object.getTriangles()) {
      
      for (int i = 0; i < 3; i++) {
        float s = 0;
        Vec4f vert = completeProjection(mvp * triangle->v[i], s);
        for (int j = 0; j < 4; j++) v[i*4+j] = vert[j];
        Vec4f Vert = mv * triangle->v[i];
        for (int j = 0; j < 4; j++) V[i*4+j] = Vert[j];
      }
      
      Vec3f norm = normalMatrix * triangle->n;
      for (int i = 0; i < 3; i++) n[i] = norm[i];
      
      Vec2i bboxmin = Vec2i(width, height);
      Vec2i bboxmax = Vec2i(0, 0);
      
      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
          float f = v[i*4+j];
          if (bboxmin[j] > f) bboxmin[j] = f;
          if (bboxmax[j] < f) bboxmax[j] = f;
        }
      }
      
      bboxmax[0] = bboxmax[0]>=width?width-1:bboxmax[0];
      bboxmax[1] = bboxmax[1]>=height?height-1:bboxmax[1];
      bboxmin[0] = bboxmin[0]<=0?1:bboxmin[0];
      bboxmin[1] = bboxmin[1]<=0?1:bboxmin[1];
      
      int tX = bboxmin[0],      tY = bboxmin[1],
          tW = bboxmax[0]-tX+1, tH = bboxmax[1]-tY+1;
      
      queue.enqueueWriteBuffer(b_matrixes, CL_TRUE, 0, sizeof(cl_float4)*4*2, matrixes);
      queue.enqueueWriteBuffer(b_v, CL_TRUE, 0, sizeof(cl_float)*4*3, v);
      queue.enqueueWriteBuffer(b_V, CL_TRUE, 0, sizeof(cl_float4)*3, V);
      queue.enqueueWriteBuffer(b_n, CL_TRUE, 0, sizeof(cl_float3), n);
      queue.enqueueWriteBuffer(b_material, CL_TRUE, 0, sizeof(cl_int)*2, triangle->material);
      
      queue.enqueueNDRangeKernel(pixelKernel, cl::NDRange(tX, tY), cl::NDRange(tW, tH), cl::NullRange);
      queue.finish();
    }
  }
  
  delete matrixes;
  delete v;
  delete V;
  delete n;
  
  queue.enqueueReadBuffer(b_frame, CL_TRUE, 0, sizeof(int)*s, frame->bits());
  
  qDebug() << getMilliCount() - start;
}

float s = 0;
void BBoxRenderer::renderFrame() {
  
  while (mutexPaint) // frame(QImage*) in use in BBoxRenderer::paintEvent
    if (!isVisible() ||  // Window closed
        !drawingButton->isChecked() // Button unchecked
        ) return; // Stop loop
  
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      zbuffer[x+y*width] = 1.0f;
      frame->setPixel(x, y, 0);
    }
  }
//  view = view * Matrix::createYRotation(5*3.1415/180.0);
  
  if (useCLCheckBox->isChecked()) 
    renderPixelCL();
  else 
    renderPixel();
  
  mutexPaint = true; // is frame in use bool
  update(); // call paintEvent
  updateFuture = QtConcurrent::run(this, &BBoxRenderer::renderFrame); // Run new loop
}

void BBoxRenderer::resizeEvent(QResizeEvent * e) {
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
  
  uiFrame->setGeometry(width, 0, 200, height);
  
  float fovy = 45*3.1415/180.0f;
  float aspect = float(width)/height;
  proj = Matrix4x4::createPerspective(fovy, aspect, 0.1f, 100.0f);
//  proj = proj.transposed();
  
  if (zbuffer) delete zbuffer;
  zbuffer = new float[width*height];
  
  b_frame   = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(cl_int)*width*height);
  b_zbuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(cl_float)*width*height);
  pixelKernel.setArg(5, b_zbuffer);
  pixelKernel.setArg(6, b_frame);
}

void BBoxRenderer::paintEvent(QPaintEvent *) {
  QPainter p(this);
  p.drawImage(QPoint(0, 0), *frame);
  mutexPaint = false;
}
