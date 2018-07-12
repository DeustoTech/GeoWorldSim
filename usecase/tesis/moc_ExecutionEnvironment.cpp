/****************************************************************************
** Meta object code from reading C++ file 'ExecutionEnvironment.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../environment/execution_environment/ExecutionEnvironment.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ExecutionEnvironment.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_GWSExecutionEnvironment_t {
    QByteArrayData data[9];
    char stringdata0[116];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GWSExecutionEnvironment_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GWSExecutionEnvironment_t qt_meta_stringdata_GWSExecutionEnvironment = {
    {
QT_MOC_LITERAL(0, 0, 23), // "GWSExecutionEnvironment"
QT_MOC_LITERAL(1, 24, 15), // "tickEndedSignal"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 13), // "executed_tick"
QT_MOC_LITERAL(4, 55, 22), // "runningExecutionSignal"
QT_MOC_LITERAL(5, 78, 23), // "stoppingExecutionSignal"
QT_MOC_LITERAL(6, 102, 3), // "run"
QT_MOC_LITERAL(7, 106, 4), // "stop"
QT_MOC_LITERAL(8, 111, 4) // "tick"

    },
    "GWSExecutionEnvironment\0tickEndedSignal\0"
    "\0executed_tick\0runningExecutionSignal\0"
    "stoppingExecutionSignal\0run\0stop\0tick"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GWSExecutionEnvironment[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,
       4,    0,   47,    2, 0x06 /* Public */,
       5,    0,   48,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   49,    2, 0x0a /* Public */,
       7,    0,   50,    2, 0x0a /* Public */,
       8,    0,   51,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void GWSExecutionEnvironment::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GWSExecutionEnvironment *_t = static_cast<GWSExecutionEnvironment *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->tickEndedSignal((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->runningExecutionSignal(); break;
        case 2: _t->stoppingExecutionSignal(); break;
        case 3: _t->run(); break;
        case 4: _t->stop(); break;
        case 5: _t->tick(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (GWSExecutionEnvironment::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GWSExecutionEnvironment::tickEndedSignal)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (GWSExecutionEnvironment::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GWSExecutionEnvironment::runningExecutionSignal)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (GWSExecutionEnvironment::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GWSExecutionEnvironment::stoppingExecutionSignal)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject GWSExecutionEnvironment::staticMetaObject = {
    { &GWSEnvironment::staticMetaObject, qt_meta_stringdata_GWSExecutionEnvironment.data,
      qt_meta_data_GWSExecutionEnvironment,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *GWSExecutionEnvironment::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GWSExecutionEnvironment::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GWSExecutionEnvironment.stringdata0))
        return static_cast<void*>(this);
    return GWSEnvironment::qt_metacast(_clname);
}

int GWSExecutionEnvironment::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GWSEnvironment::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void GWSExecutionEnvironment::tickEndedSignal(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GWSExecutionEnvironment::runningExecutionSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void GWSExecutionEnvironment::stoppingExecutionSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
