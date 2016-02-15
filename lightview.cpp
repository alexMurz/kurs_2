#include "lightview.h"

double toDouble(QString str) {
    return str.replace(",", ".").toDouble();
}

LightView::LightView(LightSource * source, LightViewUpdateListener * delegate, QWidget *parent) : 
  QWidget(parent), source(source), delegate(delegate) {
  setFixedSize(200, 210);
  setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
  
  // Diffuse
  {
    QLabel * diffLabel = new QLabel(this);
    diffLabel->setGeometry(0, 0, 50, 25);
    diffLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    diffLabel->setText("Diff:");
    
    diffRed = new QLineEdit(this);
    diffRed->setGeometry(50, 0, 50, 25);
    diffRed->setText( QString::number(source->diffuseLight[0]) );
    diffRed->setValidator(new QDoubleValidator(this));
    
    diffGreen = new QLineEdit(this);
    diffGreen->setGeometry(100, 0, 50, 25);
    diffGreen->setText( QString::number(source->diffuseLight[1]) );
    diffGreen->setValidator(new QDoubleValidator(this));
    
    diffBlue = new QLineEdit(this);
    diffBlue->setGeometry(150, 0, 50, 25);
    diffBlue->setText( QString::number(source->diffuseLight[2]) );
    diffBlue->setValidator(new QDoubleValidator(this));    
  }
  
  // Specular
  {
    QLabel * diffLabel = new QLabel(this);
    diffLabel->setGeometry(0, 30, 50, 25);
    diffLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    diffLabel->setText("Spec:");
    
    specRed = new QLineEdit(this);
    specRed->setGeometry(50, 30, 50, 25);
    specRed->setText( QString::number(source->specularLight[0]) );
    specRed->setValidator(new QDoubleValidator(this));
    
    specGreen = new QLineEdit(this);
    specGreen->setGeometry(100, 30, 50, 25);
    specGreen->setText( QString::number(source->specularLight[1]) );
    specGreen->setValidator(new QDoubleValidator(this));
    
    specBlue = new QLineEdit(this);
    specBlue->setGeometry(150, 30, 50, 25);
    specBlue->setText( QString::number(source->specularLight[2]) );
    specBlue->setValidator(new QDoubleValidator(this));    
  }
  
  // Origin
  {
    QLabel * label = new QLabel(this);
    label->setGeometry(0, 60, 50, 25);
    label->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    label->setText("Origin:");
    
    originX = new QLineEdit(this);
    originX->setGeometry(50, 60, 50, 25);
    originX->setText( QString::number(source->origin[0]) );
    originX->setValidator(new QDoubleValidator(this));
    
    originY = new QLineEdit(this);
    originY->setGeometry(100, 60, 50, 25);
    originY->setText( QString::number(source->origin[1]) );
    originY->setValidator(new QDoubleValidator(this));
    
    originZ = new QLineEdit(this);
    originZ->setGeometry(150, 60, 50, 25);
    originZ->setText( QString::number(source->origin[2]) );
    originZ->setValidator(new QDoubleValidator(this));  
  }
  
  // Origin
  {
    QLabel * label = new QLabel(this);
    label->setGeometry(0, 90, 50, 25);
    label->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    label->setText("Enegry:");
    
    energy = new QLineEdit(this);
    energy->setGeometry(50, 90, 150, 25);
    energy->setText( QString::number(source->energy) );
    energy->setValidator(new QDoubleValidator(this));
  }
  
  // isEye
  {
    QLabel * label = new QLabel(this);
    label->setGeometry(0, 120, 50, 25);
    label->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    label->setText("isEye:");
    
    isEye = new QComboBox(this);
    isEye->setGeometry(50, 120, 150, 25);
    isEye->addItem("True");
    isEye->addItem("False");
    isEye->setCurrentIndex( source->isEye?0:1 );
  }
  
  
  // Origin
  {
    QLabel * label = new QLabel(this);
    label->setGeometry(0, 150, 50, 25);
    label->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    label->setText("Att:");
    
    attConst = new QLineEdit(this);
    attConst->setGeometry(50, 150, 50, 25);
    attConst->setText( QString::number(source->attenuation[0]) );
    attConst->setValidator(new QDoubleValidator(this));
    
    attLin = new QLineEdit(this);
    attLin->setGeometry(100, 150, 50, 25);
    attLin->setText( QString::number(source->attenuation[1]) );
    attLin->setValidator(new QDoubleValidator(this));
    
    attQuad = new QLineEdit(this);
    attQuad->setGeometry(150, 150, 50, 25);
    attQuad->setText( QString::number(source->attenuation[2]) );
    attQuad->setValidator(new QDoubleValidator(this)); 
  }
  
  apply = new QPushButton(this);
  apply->setGeometry(0, 180, 100, 25);
  apply->setText("Применить");
  connect(apply, SIGNAL(clicked(bool)), SLOT(applySlot()));
  
  cancel = new QPushButton(this);
  cancel->setGeometry(100, 180, 100, 25);
  cancel->setText("Закрыть");
  connect(cancel, SIGNAL(clicked(bool)), SLOT(cancelSlot()));
  
//  QLineEdit * attConst, * attLin, * attQuad;
}

void LightView::applySlot() {
  source->attenuation[0] = toDouble(attConst->text());
  source->attenuation[1] = toDouble(attLin->text());
  source->attenuation[2] = toDouble(attQuad->text());
  
  source->diffuseLight[0] = toDouble(diffRed->text());
  source->diffuseLight[1] = toDouble(diffGreen->text());
  source->diffuseLight[2] = toDouble(diffBlue->text());
  
  source->specularLight[0] = toDouble(specRed->text());
  source->specularLight[1] = toDouble(specGreen->text());
  source->specularLight[2] = toDouble(specBlue->text());  
  
  source->energy = toDouble(energy->text());
  
  source->isEye = isEye->currentIndex() == 0 ? true : false;
  
  source->origin[0] = toDouble(originX->text());
  source->origin[1] = toDouble(originY->text());
  source->origin[2] = toDouble(originZ->text());
  
  if (delegate) delegate->updateLights();
}

void LightView::cancelSlot() {
  close();
}
