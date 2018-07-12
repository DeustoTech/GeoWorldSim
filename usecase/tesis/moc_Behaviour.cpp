/****************************************************************************
** Meta object code from reading C++ file 'Behaviour.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../behaviour/Behaviour.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Behaviour.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_GWSBehaviour_t {
    QByteArrayData data[6];
    char stringdata0[51];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GWSBehaviour_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GWSBehaviour_t qt_meta_stringdata_GWSBehaviour = {
    {
QT_MOC_LITERAL(0, 0, 12), // "GWSBehaviour"
QT_MOC_LITERAL(1, 13, 4), // "tick"
QT_MOC_LITERAL(2, 18, 0), // ""
QT_MOC_LITERAL(3, 19, 6), // "behave"
QT_MOC_LITERAL(4, 26, 9), // "GWSAgent*"
QT_MOC_LITERAL(5, 36, 14) // "behaving_agent"

    },
    "GWSBehaviour\0tick\0\0behave\0GWSAgent*\0"
    "behaving_agent"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GWSBehaviour[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       1,   29, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x08 /* Private */,
       3,    0,   25,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Bool,

 // constructors: parameters
    0x80000000 | 2, 0x80000000 | 4,    5,

 // constructors: name, argc, parameters, tag, flags
       0,    1,   26,    2, 0x0e /* Public */,

       0        // eod
};

void GWSBehaviour::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::CreateInstance) {
        switch (_id) {
        case 0: { GWSBehaviour *_r = new GWSBehaviour((*reinterpret_cast< GWSAgent*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        default: break;
        }
    } else if (_c == QMetaObject::InvokeMetaMethod) {
        GWSBehaviour *_t = static_cast<GWSBehaviour *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->tick(); break;
        case 1: { bool _r = _t->behave();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
}

const QMetaObject GWSBehaviour::staticMetaObject = {
    { &GWSObject::staticMetaObject, qt_meta_stringdata_GWSBehaviour.data,
      qt_meta_data_GWSBehaviour,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *GWSBehaviour::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GWSBehaviour::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GWSBehaviour.stringdata0))
        return static_cast<void*>(this);
    return GWSObject::qt_metacast(_clname);
}

int GWSBehaviour::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GWSObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
