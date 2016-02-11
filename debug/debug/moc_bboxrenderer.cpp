/****************************************************************************
** Meta object code from reading C++ file 'bboxrenderer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../bboxrenderer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bboxrenderer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_BBoxRenderer_t {
    QByteArrayData data[22];
    char stringdata[204];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_BBoxRenderer_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_BBoxRenderer_t qt_meta_stringdata_BBoxRenderer = {
    {
QT_MOC_LITERAL(0, 0, 12),
QT_MOC_LITERAL(1, 13, 6),
QT_MOC_LITERAL(2, 20, 0),
QT_MOC_LITERAL(3, 21, 6),
QT_MOC_LITERAL(4, 28, 6),
QT_MOC_LITERAL(5, 35, 6),
QT_MOC_LITERAL(6, 42, 6),
QT_MOC_LITERAL(7, 49, 6),
QT_MOC_LITERAL(8, 56, 6),
QT_MOC_LITERAL(9, 63, 6),
QT_MOC_LITERAL(10, 70, 6),
QT_MOC_LITERAL(11, 77, 6),
QT_MOC_LITERAL(12, 84, 6),
QT_MOC_LITERAL(13, 91, 6),
QT_MOC_LITERAL(14, 98, 13),
QT_MOC_LITERAL(15, 112, 10),
QT_MOC_LITERAL(16, 123, 18),
QT_MOC_LITERAL(17, 142, 11),
QT_MOC_LITERAL(18, 154, 11),
QT_MOC_LITERAL(19, 166, 13),
QT_MOC_LITERAL(20, 180, 10),
QT_MOC_LITERAL(21, 191, 12)
    },
    "BBoxRenderer\0slotFW\0\0slotLW\0slotBW\0"
    "slotRW\0slotUW\0slotDW\0slotTL\0slotTR\0"
    "slotTU\0slotTD\0slotSL\0slotSR\0keyPressEvent\0"
    "QKeyEvent*\0drawingButtonCheck\0renderFrame\0"
    "resizeEvent\0QResizeEvent*\0paintEvent\0"
    "QPaintEvent*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BBoxRenderer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   99,    2, 0x08 /* Private */,
       3,    0,  100,    2, 0x08 /* Private */,
       4,    0,  101,    2, 0x08 /* Private */,
       5,    0,  102,    2, 0x08 /* Private */,
       6,    0,  103,    2, 0x08 /* Private */,
       7,    0,  104,    2, 0x08 /* Private */,
       8,    0,  105,    2, 0x08 /* Private */,
       9,    0,  106,    2, 0x08 /* Private */,
      10,    0,  107,    2, 0x08 /* Private */,
      11,    0,  108,    2, 0x08 /* Private */,
      12,    0,  109,    2, 0x08 /* Private */,
      13,    0,  110,    2, 0x08 /* Private */,
      14,    1,  111,    2, 0x08 /* Private */,
      16,    0,  114,    2, 0x08 /* Private */,
      17,    0,  115,    2, 0x08 /* Private */,
      18,    1,  116,    2, 0x08 /* Private */,
      20,    1,  119,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 15,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 19,    2,
    QMetaType::Void, 0x80000000 | 21,    2,

       0        // eod
};

void BBoxRenderer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BBoxRenderer *_t = static_cast<BBoxRenderer *>(_o);
        switch (_id) {
        case 0: _t->slotFW(); break;
        case 1: _t->slotLW(); break;
        case 2: _t->slotBW(); break;
        case 3: _t->slotRW(); break;
        case 4: _t->slotUW(); break;
        case 5: _t->slotDW(); break;
        case 6: _t->slotTL(); break;
        case 7: _t->slotTR(); break;
        case 8: _t->slotTU(); break;
        case 9: _t->slotTD(); break;
        case 10: _t->slotSL(); break;
        case 11: _t->slotSR(); break;
        case 12: _t->keyPressEvent((*reinterpret_cast< QKeyEvent*(*)>(_a[1]))); break;
        case 13: _t->drawingButtonCheck(); break;
        case 14: _t->renderFrame(); break;
        case 15: _t->resizeEvent((*reinterpret_cast< QResizeEvent*(*)>(_a[1]))); break;
        case 16: _t->paintEvent((*reinterpret_cast< QPaintEvent*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject BBoxRenderer::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_BBoxRenderer.data,
      qt_meta_data_BBoxRenderer,  qt_static_metacall, 0, 0}
};


const QMetaObject *BBoxRenderer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BBoxRenderer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BBoxRenderer.stringdata))
        return static_cast<void*>(const_cast< BBoxRenderer*>(this));
    return QWidget::qt_metacast(_clname);
}

int BBoxRenderer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 17;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
