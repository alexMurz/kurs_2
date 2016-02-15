/****************************************************************************
** Meta object code from reading C++ file 'raycastrenderer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../raycastrenderer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'raycastrenderer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_RaycastRenderer_t {
    QByteArrayData data[11];
    char stringdata[132];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RaycastRenderer_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RaycastRenderer_t qt_meta_stringdata_RaycastRenderer = {
    {
QT_MOC_LITERAL(0, 0, 15), // "RaycastRenderer"
QT_MOC_LITERAL(1, 16, 10), // "prepareObj"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 16), // "paintButtonCheck"
QT_MOC_LITERAL(4, 45, 18), // "drawingButtonCheck"
QT_MOC_LITERAL(5, 64, 11), // "renderFrame"
QT_MOC_LITERAL(6, 76, 5), // "force"
QT_MOC_LITERAL(7, 82, 11), // "resizeEvent"
QT_MOC_LITERAL(8, 94, 13), // "QResizeEvent*"
QT_MOC_LITERAL(9, 108, 10), // "paintEvent"
QT_MOC_LITERAL(10, 119, 12) // "QPaintEvent*"

    },
    "RaycastRenderer\0prepareObj\0\0"
    "paintButtonCheck\0drawingButtonCheck\0"
    "renderFrame\0force\0resizeEvent\0"
    "QResizeEvent*\0paintEvent\0QPaintEvent*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RaycastRenderer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x0a /* Public */,
       3,    0,   50,    2, 0x0a /* Public */,
       4,    0,   51,    2, 0x0a /* Public */,
       5,    1,   52,    2, 0x0a /* Public */,
       5,    0,   55,    2, 0x2a /* Public | MethodCloned */,
       7,    1,   56,    2, 0x0a /* Public */,
       9,    1,   59,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    2,
    QMetaType::Void, 0x80000000 | 10,    2,

       0        // eod
};

void RaycastRenderer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RaycastRenderer *_t = static_cast<RaycastRenderer *>(_o);
        switch (_id) {
        case 0: _t->prepareObj(); break;
        case 1: _t->paintButtonCheck(); break;
        case 2: _t->drawingButtonCheck(); break;
        case 3: _t->renderFrame((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->renderFrame(); break;
        case 5: _t->resizeEvent((*reinterpret_cast< QResizeEvent*(*)>(_a[1]))); break;
        case 6: _t->paintEvent((*reinterpret_cast< QPaintEvent*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject RaycastRenderer::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_RaycastRenderer.data,
      qt_meta_data_RaycastRenderer,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *RaycastRenderer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RaycastRenderer::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_RaycastRenderer.stringdata))
        return static_cast<void*>(const_cast< RaycastRenderer*>(this));
    return QWidget::qt_metacast(_clname);
}

int RaycastRenderer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
