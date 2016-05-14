#include "raycastrenderer.h"

#include <time.h>
#include <sys/timeb.h>
#include <QDebug>

extern int getMilliCount();

float nextFloat() {
  return ((float)rand()) / RAND_MAX;
}

RaycastRenderer::RaycastRenderer() : QWidget(0), frame(0), mutexPaint(false), mutexMatrix(false) {
  setMinimumWidth(400);
  setMinimumHeight(300);
 
  view = Matrix4x4::identity();
//  view.translate(0, -7, 21);
//  view.translate(0, 0,-10);
//  view = view * Matrix4x4::createXRotation(-15 * 3.1415 / 180.0f);
  
  uiFrame = new QFrame(this);
  uiFrame->setFixedWidth(toolBarWidth);
  
  drawingButton = new QPushButton(uiFrame);
  drawingButton->setGeometry(0, 0, 200, 35);
  drawingButton->setText("Start");
  drawingButton->setCheckable(true);
  connect(drawingButton, SIGNAL(clicked(bool)), SLOT(drawingButtonCheck()));
  
  cleanButton = new QPushButton(uiFrame);
  cleanButton->setGeometry(0, 35, 200, 35);
  cleanButton->setText("Clean");
  connect(cleanButton, SIGNAL(clicked(bool)), SLOT(cleanButtonCheck()));
  
  useOpenCL = new QCheckBox(uiFrame);
  useOpenCL->setGeometry(5, 70, 35, 35);
  QLabel * useOpenCLLabel = new QLabel(uiFrame);
  useOpenCLLabel->setText("Use OpenCL");
  useOpenCLLabel->setGeometry(40, 70, 100, 35);
  
  prepareObjects = new QPushButton(uiFrame);
  prepareObjects->setGeometry(0, 105, 200, 35);
  prepareObjects->setText("PrepareObj");
  connect(prepareObjects, SIGNAL(clicked()), SLOT(prepareObj()));
  
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
  
  prepareObj();
}

void RaycastRenderer::mousePressEvent(QMouseEvent *) {
  for (int i = 0; i < 16; i++)
    if (focusWidget() == matView[i])
      focusWidget()->clearFocus();
}

void RaycastRenderer::setMatrix() {
  for (int i = 0; i < 16; i++) {
    view.v[i] = matView[i]->text().toFloat();
  }
}

void RaycastRenderer::updateMatView() {
  for (int i = 0; i < 16; i++) {
    matView[i]->setText( QString::number(view.v[i]) );
  }
}


void RaycastRenderer::prepareObj() {
  triangles.clear();
  std::cerr << "Preparing objects ... ";
  int mill = getMilliCount();
  
//  float z = 1.f;
//  float s = 1.f;
//  float o = 0.f;
//  Material * mat = new Material();
//  triangles.push_back(new Triangle3D(
//                        Vec3f(-s, -s,  z),
//                        Vec3f( o,  s,  z),
//                        Vec3f( s, -s,  z),
//                        Vec3f( 0.0f,  0.0f,  1.0f), 
//                        mat
//  ));
  
  foreach (Object3D o, Info::scene.objects) {
    foreach (Triangle3D * t, o.getTriangles()) {
      Triangle3D * tr = new Triangle3D();
      memcpy(tr, t, sizeof(Triangle3D));
      tr->v[0] = t->v[0];
      tr->v[1] = t->v[1];
      tr->v[2] = t->v[2];
      tr->modelRef = &o.getModel();
      if (triangles.size() == 2) {
        std::cerr << tr->v[0][0] << " " << tr->v[0][1] << " " << tr->v[0][2] << "\n";
        // -2 0 -3.5
      }
      triangles.push_back(tr);
    }
  }
  
  
  /*
  if (clTriangles) delete clTriangles;
  clTriangles = new CLTriangle3D[triangles.size()];
  for (size_t i = 0; i < triangles.size(); i++) {
    CLTriangle3D & t = clTriangles[i];
    const Triangle3D * tr = triangles[i];
    for (int j = 0; j < 3; j++) {
      for (int k = 0; k < 3; k++) t.v[j].s[k] = tr->v[j][k];
      t.n.s[j] = tr->n[j];
      t.diff.s[j] = tr->material->diffuseReflection[j];
      t.spec.s[j] = tr->material->specularReflection[j];
      t.refl.s[j] = tr->material->reflection[j];
    }
    t.isMirror = tr->material->isMirror;
    t.reflectivity = tr->material->reflectivity;
    t.shininess = tr->material->shininess;
  }
  
  b_triangles = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(CLTriangle3D) * triangles.size());
  pixelKernel.setArg(3, b_triangles);
  queue.enqueueWriteBuffer(b_triangles, CL_TRUE, 0, sizeof(CLTriangle3D) * triangles.size(), clTriangles);
  */
  
  std::cerr << "ready in " << getMilliCount() - mill << "ms!\n";
  
  
//  std::cerr << "CLSize: " << sizeof(cl_float3) * 7 + sizeof(cl_int) + sizeof(cl_float)*2 << " " << sizeof(CLTriangle3D) << "\n";
}

void RaycastRenderer::renderFrame(bool force) {
  if (!force)
    if (!isVisible() ||  // Window closed
        !drawingButton->isChecked() // Button unchecked
        ) return; // Stop loop  
  
  while(mutexPaint || mutexMatrix);
  
  cpuRender(frame);
  
  mutexPaint = true;
  update(); // call paintEvent
  updateFuture = QtConcurrent::run(this, &RaycastRenderer::renderFrame, false); // Run new loop
}

Col3f RaycastRenderer::calculateColor(const Triangle3D & tr, const LightSource & ls, const Vec3f & point, float lightTravled) {
  
  const Vec3f & p0 = ls.origin;
  const Vec3f & p1 = point;
  
  float dist = (p1-p0).length() + lightTravled;
  float e = ls.energy;
    
  float falloff = ls.attenuation[0];
  falloff += ls.attenuation[1] * (1-dist/e);
  falloff += ls.attenuation[2] * (1-dist*dist/(e*e));
  falloff = clamp(falloff, 0.0f, 1.0f);
  
  return tr.material->diffuseReflection * falloff;
}

void RaycastRenderer::cpuRender(QImage * b) {
  
  clock_t c = clock();
  qDebug() << "Start CPU";
  
  inv_view = view.inversed().transposed();  
  
  dataList.clear();
  
  for (int i = 0; i < Info::scene.lights.size(); i++) {
    qDebug() << "  Start" << i << "source";
    const LightSource & ls = Info::scene.lights[i];
    if (!ls.enabled) continue;
    disperse(ls.origin, ls);
  }
  
  qDebug() << " dataList size" << dataList.size();
  
  foreach (const ScreenRayData & d, dataList) {
    Col3f c1 = d.color;
    Col3f c2 = makeCol3f(b->pixel(d.pos[0], d.pos[1]));
    if (c1.length() > c2.length())
      b->setPixel(d.pos[0], d.pos[1], makeColor(d.color));
  }
  
  qDebug() << "End CPU in " << float(clock() - c) / CLOCKS_PER_SEC;
}

void RaycastRenderer::disperse(const Vec3f & origin, const LightSource & ls, float lightTravled, Triangle3D * isMirrored, int maxIterations, Triangle3D * skip) {
  Vec4f screenOrigin = Vec4f(0.0, 0.0, -1.0, 1.0);
  
  for (int i = 0; i < maxIterations; i++) {
    Vec3f dir = Vec3f(nextFloat()*2-1, nextFloat()*2-1, nextFloat()*2-1); 
    dir.normalize();
    
    Vec3f p1 = origin;
    Vec3f p2 = origin + dir;
    
    Vec3f point1, point2;
    float dist1, dist2;
    Triangle3D * triangle1, * triangle2;
    if (triangle1 == skip) continue;
    if (sendRay(p1, p2, point1, dist1, triangle1)) {
      
      if (triangle1->material->isMirror) {
        disperse(point1, ls, (p1-point1).length(), triangle1, samplesPerReflection, triangle1);
      } else {
        if (!sendRay(point1, screenOrigin, point2, dist2, triangle2, (point1-screenOrigin).length(), triangle1)) {
          
          bool hit;
          Vec2f r = rayInEyePos(point1, screenOrigin, hit);
          if (hit) {
            r[0] = r[0] * width;
            r[1] = r[1] * height;          
            
            Col3f color = calculateColor(*triangle1, ls, point1, lightTravled);
            
            ScreenRayData data;
            data.pos = r;
            data.color = color;
            dataList.push_back(data);
          }
        }
      }
    }
  }
}

Vec2f RaycastRenderer::rayInEyePos(const Vec3f & ray, const Vec3f & origin, bool & hit) {  
  static const Matrix4x4 idn = Matrix4x4::identity();
  
  float aspect = float(width) / height;
  static const float f = tan((45/180.f*3.1415926)/2);
  
  float fl = -f*aspect;
  float fr = f*aspect;
  float ft = f;
  float fb = -f;
  Vec4f lt = Vec4f(fl, ft, 0.0f, 1.0f);
  Vec4f rt = Vec4f(fr, ft, 0.0f, 1.0f);
  Vec4f lb = Vec4f(fl, fb, 0.0f, 1.0f);
  Vec4f rb = Vec4f(fr, fb, 0.0f, 1.0f);
  
  Triangle3D t1 = Triangle3D(
        lb, rb, lt,
//    view * Vec4f(l, b, f, 1.0f), 
//    view * Vec4f(r, b, f, 1.0f), 
//    view * Vec4f(l, t, f, 1.0f),
    Vec3f( 0.f,  0.f,  1.f),
     NULL
  );
  Triangle3D t2 = Triangle3D(  
        rt, lt, rb,
//    view * Vec4f(r, t, f, 1.0f),
//    view * Vec4f(l, t, f, 1.0f),
//    view * Vec4f(r, b, f, 1.0f),
    Vec3f( 0.f,  0.f,  1.f),
     NULL
  );
  Vec3f point;
  float distance;
  float _s = 1, _t = 1;
  
  Vec2f _r;
  
  if (!(hit = intersect(&t1, idn, idn, ray, origin, photonMaxLength, distance, point, &_s, &_t))) {
    hit = intersect(&t2, idn, idn, ray, origin, photonMaxLength, distance, point, &_s, &_t);
    _r[0] = 1-_s;
    _r[1] = 1-_t;
  } else {
    _r[0] = _s;
    _r[1] = _t;
  }
  
  return _r;
}

void RaycastRenderer::resizeEvent(QResizeEvent * e) {
  QWidget::resizeEvent(e);
  int ow = width; int oh = height;
  width = e->size().width() - toolBarWidth; height = e->size().height();
  uiFrame->setGeometry(width, 0, toolBarWidth, height);
  
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
}

bool RaycastRenderer::intersect( Triangle3D * tr, const Matrix4x4 & m, const Matrix4x4 & invm,
                                 const Vec3f &p0, // Ray origin 
                                 const Vec3f &p1, // 
                                 float maxDistance, float &distance, Vec3f &point,
                                 float * os, float * ot) {
  Vec3f v0 = m * tr->v[0];
  Vec3f v1 = m * tr->v[1];
  Vec3f v2 = m * tr->v[2];
  Vec3f N  = invm * tr->n;
  
  Vec3f dir = (p1-p0).normalize();
  Vec3f u = v1 - v0;
  Vec3f v = v2 - v0;
  
  Vec3f w0 = p0 - v0;
  float a = -(N * w0);
  float b = (N * dir);
  if (std::abs(b) < 1e-5) return false;
  
  float r = a/b;
  if (r <= 0.0f || r >= maxDistance) return false;

  distance = r;
  point = p0 + r*dir;
  
  float uu, uv, vv, wu, wv, D;
  uu = u * u;
  uv = u * v;
  vv = v * v;
  Vec3f w = point - v0;
  wu = w * u;
  wv = w * v;
  D = uv*uv - uu*vv;
  
  float s, t;
  
  s = (uv * wv - vv * wu) / D;
  if (s < 0.0f || s > 1.0f) return false;
  
  t = (uv * wu - uu * wv) / D;
  if (t < 0.0f || (s+t) > 1.0) return false;
  
  if (os) *os = s;
  if (ot) *ot = t;

  return true;
}

bool RaycastRenderer::sendRay(const Vec3f & origin, const Vec3f & ray, float maxDistance, const Triangle3D * skip) {
  Vec3f testPoint;
  float d;
  
  foreach (Triangle3D * tr, triangles) {
    if (tr == skip) continue;
    if (intersect(tr, view, inv_view, origin, ray, maxDistance, d, testPoint)) {
      maxDistance = d;
      return true;
    }
  }
  
  return false;
}
bool RaycastRenderer::sendRay(const Vec3f & origin, const Vec3f & ray, Vec3f & point, float & dist, Triangle3D *& triangle, float maxDistance, Triangle3D * skip) {
  Vec3f testPoint;
  bool hit = false;
  float d;
  
  foreach (Triangle3D * tr, triangles) {
    if (tr == skip) continue;
    if (intersect(tr, view, inv_view, origin, ray, maxDistance, d, testPoint)) {
      maxDistance = d;
      point = testPoint;
      triangle = tr;
      hit = true;
    }
  }
  
  dist = maxDistance;
  
  return hit;
}

void RaycastRenderer::paintEvent(QPaintEvent *) {
  QPainter p(this);
  p.drawImage(QPoint(0, 0), *frame);
  mutexPaint = false;
}

void RaycastRenderer::cleanButtonCheck() {
  for (int x = 0; x < width; x++)
    for (int y = 0; y < height; y++)
      frame->setPixel(x, y, 0x000000);
  update();
}

void RaycastRenderer::drawingButtonCheck() {
  if (drawingButton->isChecked()) {
    drawingButton->setText("Stop");
    if (!updateFuture.isRunning())
      updateFuture = QtConcurrent::run(this, &RaycastRenderer::renderFrame, false);
  } else {
    drawingButton->setText("Start");
  }
}

void RaycastRenderer::keyPressEvent(QKeyEvent * e) {
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

/*
 * Matrix Control
 */
static const float turnSpeed = 5.0 * 3.1415 / 180.0;
static const float moveSpeed = 0.25f;
void RaycastRenderer::slotTL() { 
  while(mutexMatrix); 
  mutexMatrix = true; 
  view = view * Matrix4x4::createYRotation( turnSpeed); 
  mutexMatrix = false; 
}
void RaycastRenderer::slotFW() { 
  while(mutexMatrix); 
  mutexMatrix = true; 
  view = view * Matrix4x4::createTranslate(0, 0,-moveSpeed);
  mutexMatrix = false;
}
void RaycastRenderer::slotTR() { 
  while(mutexMatrix); 
  mutexMatrix = true;
  view = view * Matrix4x4::createYRotation(-turnSpeed); 
  mutexMatrix = false; 
}
void RaycastRenderer::slotLW() { 
  while(mutexMatrix); 
  mutexMatrix = true; 
  view = view * Matrix4x4::createTranslate(-moveSpeed, 0, 0);
  mutexMatrix = false; 
}
void RaycastRenderer::slotBW() { 
  while(mutexMatrix); 
  mutexMatrix = true; 
  view = view * Matrix4x4::createTranslate(0, 0, moveSpeed);
  mutexMatrix = false; 
}
void RaycastRenderer::slotRW() { 
  while(mutexMatrix); 
  mutexMatrix = true; 
  view = view * Matrix4x4::createTranslate( moveSpeed, 0, 0);
  mutexMatrix = false; 
}
void RaycastRenderer::slotSL() { 
  while(mutexMatrix); 
  mutexMatrix = true; 
  view = Matrix4x4::createYRotation(-turnSpeed) * view;
  mutexMatrix = false; 
}
void RaycastRenderer::slotSR() { 
  while(mutexMatrix); 
  mutexMatrix = true; 
  view = Matrix4x4::createYRotation( turnSpeed) * view;
  mutexMatrix = false; 
}
void RaycastRenderer::slotTU() {
  while(mutexMatrix); 
  mutexMatrix = true;
  view = view * Matrix4x4::createXRotation( turnSpeed); 
  mutexMatrix = false; 
}
void RaycastRenderer::slotTD() {
  while(mutexMatrix); 
  mutexMatrix = true;
  view = view * Matrix4x4::createXRotation(-turnSpeed);
  mutexMatrix = false; 
}
void RaycastRenderer::slotUW() {
  while(mutexMatrix); 
  mutexMatrix = true; 
  view = view * Matrix4x4::createTranslate(0, moveSpeed, 0);
  mutexMatrix = false; 
}
void RaycastRenderer::slotDW() {
  while(mutexMatrix);
  mutexMatrix = true;
  view = view * Matrix4x4::createTranslate(0,-moveSpeed, 0);
  mutexMatrix = false;
}

