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
QT          += network
QT          += multimedia # For video exporter sudo apt-get install qtmultimedia5-dev

# ACTIVATE QT-COMPILER OPTIMIZATION
# march=native
# 02 -> Optimizations level 2
# pipe
QMAKE_CXXFLAGS += -march=native -O2 -pipe
QMAKE_CFLAGS += -march=native -O2 -pipe

# FORCE C++11
QMAKE_CXXFLAGS += -std=c++11

TARGET      = gws-tesis
CONFIG      += console
CONFIG      -= app_bundle

TEMPLATE = app


HEADERS += \
        # BASICS
    ../../app/App.h \
    ../../object/Object.h \
    ../../object/ObjectFactory.h \
    ../../agent/Agent.h \
    ../../skill/Skill.h \
    ../../behaviour/Behaviour.h \
        # UTILS
    ../../util/parallelism/ParallelismController.h \
    ../../util/io/log/Logger.h \
    ../../util/storage/ObjectStorage.h \
    ../../util/units/Units.h \
    ../../util/uistyle/UiStyle.h \
            ## GEOMETRIES
    ../../util/geometry/GeometryFactory.h \
    ../../util/geometry/Coordinate.h \
    ../../util/geometry/Envelope.h \
    ../../util/geometry/Geometry.h \
    ../../util/geometry/GeometryUtils.h \
    ../../util/geometry/LineString.h \
    ../../util/geometry/Polygon.h \
    ../../util/geometry/Point.h \
    ../../util/geometry/Quadtree.h \
            ## GRAPH
    ../../util/graph/GraphNode.h \
    ../../util/graph/GraphEdge.h \
    ../../util/graph/Graph.h \
    ../../util/graph/GraphUtils.h \
            ## GRID
    ../../ util/grid/Grid.h \
    ../../util/grid/GridCoordinatesConversor.h \
            ## RANDOM
    ../../util/random/UniformDistribution.h \
        # ENVIRONMENT
    ../../environment/Environment.h \
    ../../environment/agent_environment/AgentEnvironment.h \
    ../../environment/physical_environment/PhysicalEnvironment.h \
    ../../environment/social_environment/SocialEnvironment.h \
    ../../environment/social_environment/Message.h \
    ../../environment/time_environment/TimeEnvironment.h \
    ../../environment/grid_environment/GridEnvironment.h \
    ../../environment/network_environment/NetworkEnvironment.h \
    ../../environment/execution_environment/ExecutionEnvironment.h
        # AGENTS
    #agent/person/Person.h


SOURCES += main.cpp \
        # BASICS
    ../../app/App.cpp \
    ../../object/Object.cpp \
    ../../object/ObjectFactory.cpp \
    ../../agent/Agent.cpp \
    ../../skill/Skill.cpp \
    ../../behaviour/Behaviour.cpp \
        # UTILS
    ../../util/parallelism/ParallelismController.cpp \
    ../../util/io/log/Logger.cpp \
    ../../util/storage/ObjectStorage.cpp \
    ../../util/uistyle/UiStyle.cpp \
            ## GEOMETRIES
    ../../util/geometry/GeometryFactory.cpp \
    ../../util/geometry/Coordinate.cpp \
    ../../util/geometry/Envelope.cpp \
    ../../util/geometry/Geometry.cpp \
    ../../util/geometry/GeometryUtils.cpp \
    ../../util/geometry/LineString.cpp \
    ../../util/geometry/Polygon.cpp \
    ../../util/geometry/Point.cpp \
    ../../util/geometry/Quadtree.cpp \
            ## GRAPH
    ../../util/graph/GraphNode.cpp \
    ../../util/graph/GraphEdge.cpp \
    ../../util/graph/Graph.cpp \
    ../../util/graph/GraphUtils.cpp \
            ## GRID
    ../../util/grid/Grid.cpp \
    ../../util/grid/GridCoordinatesConversor.cpp \
            ## RANDOM
    ../../util/random/UniformDistribution.cpp \
        # ENVIRONMENT
    ../../environment/Environment.cpp \
    ../../environment/agent_environment/AgentEnvironment.cpp \
    ../../environment/physical_environment/PhysicalEnvironment.cpp \
    ../../environment/social_environment/SocialEnvironment.cpp \
    ../../environment/social_environment/Message.cpp \
    ../../environment/time_environment/TimeEnvironment.cpp \
    ../../environment/network_environment/NetworkEnvironment.cpp \
    ../../environment/execution_environment/ExecutionEnvironment.cpp \
    ../../environment/grid_environment/GridEnvironment.cpp
        # AGENTS
    #agent/person/Person.cpp



#INCLUDE LEMON COMPILED LIBRARY
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../lib/lemon-1.3.1/build/lemon/release/ -lemon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../lib/lemon-1.3.1/build/lemon/debug/ -lemon
else:unix: LIBS += -L$$PWD/../../lib/lemon-1.3.1/build/lemon/ -lemon

INCLUDEPATH += $$PWD/../../lib/lemon-1.3.1/build/lemon
DEPENDPATH += $$PWD/../../lib/lemon-1.3.1/build/lemon

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../lib/lemon-1.3.1/build/lemon/release/libemon.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../lib/lemon-1.3.1/build/lemon/debug/libemon.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../lib/lemon-1.3.1/build/lemon/release/emon.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../lib/lemon-1.3.1/build/lemon/debug/emon.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../lib/lemon-1.3.1/build/lemon/libemon.a

#INCLUDE LEMON SYSTEM LIBRARY
#unix|win32: LIBS += -lemon

#INCLUDE LIBPQ SYSTEM LIBRARY (sudo apt-get install libpq-dev)
unix|win32: LIBS += -lpq

#INCLUDE LIBGEOS SYSTEM LIBRARY (sudo apt-get install libgeos-dev libgeos++-dev)
unix|win32: LIBS += -lgeos

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
