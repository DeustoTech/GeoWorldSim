#-------------------------------------------------
#
# Project created by QtCreator 2014-09-04T11:58:49
#
#-------------------------------------------------

QT          += core
QT          += gui
QT          += network
QT          += websockets # sudo apt-get install libqt5websockets5-dev
QT          += sql # sudo apt-get install libqt5sql5 libqt5sql5-psql
QT          += xml
QT          += multimedia # For video exporter sudo apt-get install qtmultimedia5-dev

# ACTIVATE QT-COMPILER OPTIMIZATION
# march=native
# 02 -> Optimizations level 2
# pipe
QMAKE_CXXFLAGS += -march=native -O2 -pipe
QMAKE_CFLAGS += -march=native -O2 -pipe

# FORCE C++11
QMAKE_CXXFLAGS += -std=c++14

TARGET      = tutorial
CONFIG      += console
CONFIG      -= app_bundle

TEMPLATE = app


HEADERS += \
        # BASICS
    ../../app/App.h \
    ../../object/Object.h \
    ../../object/ObjectFactory.h \
    ../../entity/Entity.h \
        # SKILLS
    ../../skill/Skill.h \
        # BEHAVIOUR
    ../../behaviour/Behaviour.h \
        # ENVIRONMENT
    ../../environment/Environment.h 



SOURCES += main.cpp \
        # BASICS
    ../../app/App.cpp \
    ../../object/Object.cpp \
    ../../object/ObjectFactory.cpp \
    ../../entity/Entity.cpp \
        # SKILLS
    ../../skill/Skill.cpp \
        # BEHAVIOURS
    ../../behaviour/Behaviour.cpp \
        # ENVIRONMENT
    ../../environment/Environment.cpp




#INCLUDE LEMON SYSTEM LIBRARY (sudo apt-get install liblemon-dev)
unix|win32: LIBS += -llemon

#INCLUDE LIBPQ SYSTEM LIBRARY (sudo apt-get install libpq-dev)
unix|win32: LIBS += -lpq

#INCLUDE LIBGEOS SYSTEM LIBRARY (sudo apt-get install libgeos-dev libgeos++-dev)
unix|win32: LIBS += -lgeos

#INCLUDE SPATIALINDEX SYSTEM LIBRARY (sudo apt-get install libspatialindex-dev)
unix|win32: LIBS += -lspatialindex

#INCLUDE LIBPROJ SYSTEM LIBRARY (sudo apt-get install libproj-dev)
unix|win32: LIBS += -lproj

#INCLUDE LIBPROJ SYSTEM LIBRARY (sudo apt-get install libgdal-dev)
unix|win32: LIBS += -lgdal

#INCLUDE LISVM SYSTEM LIBRARY (sudo apt-get install libsvm-dev)
unix|win32: LIBS += -lsvm

#INCLUDE GRASS SYSTEM LIBRARY (sudo apt-get install grass-dev)

#INCLUDE LIBFUZZYLITE SYSTEM LIBRARY (sudo apt-get install libfuzzylite-dev) USE DFL_CPP11 FLAG
unix|win32: LIBS += -lfuzzylite
QMAKE_CXXFLAGS += -DFL_CPP11=ON

#INCLUDE EIGEN3 SYSTEM LIBRARY (sudo apt-get install libeigen3-dev)

#INCLUDE SHAPELIB SYSTEM LIBRARY (sudo apt-get install libshp-dev)
#unix|win32: LIBS += -lshp

#INCLUDE FANN SYSTEM LIBRARY (sudo apt-get install libfann-dev)
unix|win32: LIBS += -lfann

#INCLUDE OPENBEAGLE REALATED LIBRARIES
#unix|win32: LIBS += -lpacc
#unix|win32: LIBS += -lopenbeagle
#unix|win32: LIBS += -lopenbeagle-GA
