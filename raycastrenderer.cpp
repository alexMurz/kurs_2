#include "raycastrenderer.h"

#include <time.h>
#include <sys/timeb.h>

extern int getMilliCount();

float nextFloat() {
  return ((float)rand()) / RAND_MAX;
}

RaycastRenderer::RaycastRenderer() : QWidget(0), frame(0) {
  setMinimumWidth(400);
  setMinimumHeight(300);
  
  
  
  view.identity();
//  view.translate(0, 7, 21);
//  view = view * Matrix4x4::createXRotation(-15 * 3.1415 / 180.0f);
  
  
  
  
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
  
  useOpenCL = new QCheckBox(uiFrame);
  useOpenCL->setGeometry(5, 70, 35, 35);
  QLabel * useOpenCLLabel = new QLabel(uiFrame);
  useOpenCLLabel->setText("Use OpenCL");
  useOpenCLLabel->setGeometry(40, 70, 100, 35);
  
  prepareObjects = new QPushButton(uiFrame);
  prepareObjects->setGeometry(0, 105, 200, 35);
  prepareObjects->setText("PrepareObj");
  connect(prepareObjects, SIGNAL(clicked()), SLOT(prepareObj()));
  
  prepareObj();
}


void RaycastRenderer::prepareObj() {
  triangles.clear();
  std::cerr << "Preparing objects ... ";
  int mill = getMilliCount();
  
  triangles.push_back(new Triangle3D(
                        Vec3f(-1.0f, -1.0f,  1.0f),
                        Vec3f( 1.0f, -1.0f,  1.0f),
                        Vec3f( 1.0f,  1.0f,  1.0f),
                        Vec3f( 0.0f,  0.0f,  1.0f), 0                        
  ));
  
//  foreach (Object3D o, Info::scene.objects) {
//    foreach (Triangle3D * t, o.getTriangles()) {
//      Triangle3D * tr = new Triangle3D();
//      memcpy(tr, t, sizeof(Triangle3D));
//      tr->v[0] = o.getModel() * t->v[0];
//      tr->v[1] = o.getModel() * t->v[1];
//      tr->v[2] = o.getModel() * t->v[2];
//      if (triangles.size() == 2) {
//        std::cerr << tr->v[0][0] << " " << tr->v[0][1] << " " << tr->v[0][2] << "\n";
//        // -2 0 -3.5
//      }
//      triangles.push_back(tr);
//    }
//  }
  
  
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
  
//  while(mutexMatrix); 
//  mutexMatrix = true;
//  rayMatrix = (view * proj) * biasMatrixInverse * vpMatrix;
//  mutexMatrix = false;
  
//  if (useOpenCL->isChecked())
//    renderCL();
//  else
//    renderPixel();
  
  cpuRender(frame);
  
  update(); // call paintEvent
  updateFuture = QtConcurrent::run(this, &RaycastRenderer::renderFrame, false); // Run new loop
}

#include <QDebug>
void RaycastRenderer::cpuRender(QImage * buffer) {
  
  Vec3f origin = view * Vec3f(0.0f, 0.0f, 0.0f);
  
  for (int itr = 0; itr < samples; itr++) {
    
    foreach (const LightSource & s, Info::scene.lights) {
      Vec3f p1 = s.origin;
      Vec3f dir = Vec3f( nextFloat()*2-1, nextFloat()*2-1, nextFloat()*2-1 );
      dir.normalize();
      Vec3f p2 = p1 + dir*photonMaxLength;
      
      Vec3f point;
      float dist;
      Triangle3D * tr;
      if (sendRay(p1, p2, point, dist, tr)) {
        
        // Diffuse Reflection
        if (!sendRay(point, origin, point, dist, tr, dist, tr)) {
          
          bool hit;
          Vec2f r = rayInEyePos(point, origin, hit);
          if (hit) {
            qDebug() << r[0] << r[1];
            
            r[0] = (r[0]*0.5 + 0.5) * width;
            r[1] = (r[1]*0.5 + 0.5) * height;
  
            buffer->setPixel(r[0], r[1], 0xFFFFFF);
          }
          
        }
        
      }
      
      /*
      bool hit;
      Vec2f r = rayInEyePos(p1, p2, hit);
      if (hit) {
        r[0] = (r[0]*0.5 + 0.5) * width;
        r[1] = (r[1]*0.5 + 0.5) * height;
        
        buffer->setPixel(r[0], r[1], 0xFFFFFF);
        
        qDebug() << "Hit at" << r[0] << r[1];
      }
      */
    } 
  }
}

Vec2f RaycastRenderer::rayInEyePos(const Vec3f & p1, const Vec3f & p2, bool & hit) {
  float f = 0.1f;
  Triangle3D t1 = Triangle3D(
    view * Vec3f(-f, -f,  f), 
    view * Vec3f( f, -f,  f), 
    view * Vec3f(-f,  f,  f),
    Vec3f( 0.f,  0.f,  1.f),
     NULL
  );
  static Triangle3D t2 = Triangle3D(
     view * Vec3f( f,  f,  f),
     view * Vec3f( f, -f,  f), 
     view * Vec3f(-f,  f,  f),
     Vec3f( 0.f,  0.f,  1.f),
     NULL
  );  
  
  bool b = false;
  Vec3f point;
  float distance;
  float s = 0, t = 0;
  if (!(b = intersect(&t1, p1, p2, photonMaxLength, distance, point, &s, &t))) {
    b = intersect(&t2, p1, p2, photonMaxLength, distance, point, &s, &t);
  }
  
  hit = b;
  Vec2f r;
  r[0] = point[0];
  r[1] = point[1];
  return r;
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



bool RaycastRenderer::intersect(//const Matrix4x4 & model,
                                 Triangle3D * tr, 
                                 const Vec3f &p0, // Ray origin 
                                 const Vec3f &p1, // 
                                 float maxDistance, float &distance, Vec3f &point,
                                 float * os, float * ot) {
  Vec3f v0 = tr->v[0];
  Vec3f v1 = tr->v[1];
  Vec3f v2 = tr->v[2];
  Vec3f N  = tr->n;
  
  Vec3f dir = (p1-p0).normalize();
  Vec3f u = v1 - v0;
  Vec3f v = v2 - v0;
  
  Vec3f w0 = p0 - v0;
  float a = -(N * w0);
  float b = (N * dir);
  if (std::abs(b) < 1e-5) {
    return false;
  }
  
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
    if (intersect(tr, origin, ray, maxDistance, d, testPoint)) {
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
    if (intersect(tr, origin, ray, maxDistance, d, testPoint)) {
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
  update();
}

void RaycastRenderer::paintButtonCheck() {
  if (drawingButton->isChecked()) return;
  updateFuture = QtConcurrent::run(this, &RaycastRenderer::renderFrame, true);  
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
