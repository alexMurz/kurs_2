#-------------------------------------------------
#
# Project created by QtCreator 2015-11-08T13:32:10
#
#-------------------------------------------------

QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kurs_2
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    bboxrenderer.cpp \
    geom.cpp \
    object3d.cpp \
    info.cpp \
    raytracerenderer.cpp \
    lightview.cpp \
    raycastrenderer.cpp

HEADERS  += mainwindow.h \
    bboxrenderer.h \
    geom.h \
    object3d.h \
    scene3d.h \
    info.h \
    cl.h \
    raytracerenderer.h \
    lightview.h \
    raycastrenderer.h

RESOURCES = resource.qrc

DISTFILES += bbox.cl \
    raytrace.cl


win32 {
    INCLUDEPATH += 'C:\INCLUDEPATH'
    INCLUDEPATH += 'C:\AMDSDK\AMD APP SDK\3.0\include\SDKUtil'
    LIBS += -L'C:\AMDSDK\AMD APP SDK\3.0\lib\x86_64\''
}

mac {
    QMAKE_LFLAGS += -framework OpenCL
}
