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
QMAKE_CXXFLAGS += -std=c++14

TARGET      = gwswastesimulation
CONFIG      += console
CONFIG      -= app_bundle

TEMPLATE = app


HEADERS += \
        # BASICS
    ../../app/App.h \
    ../../object/Object.h \
    ../../object/ObjectFactory.h \
    ../../entity/Entity.h \
    ../../entity/EntitySource.h \
        # SKILLS
    ../../skill/Skill.h \
        # BEHAVIOURS
    ../../behaviour/Behaviour.h \
    ../../behaviour/transaction/TransactionBehaviour.h \
    ../../behaviour/property/CompareEntityPropertyBehaviour.h \
    ../../behaviour/random/GenerateRandomValueBehaviour.h \
    ../../behaviour/property/AccumulatePropertyBehaviour.h \
    ../../behaviour/time/WaitUntilTimeBehaviour.h \
    ../../behaviour/property/MathEntityPropertyBehaviour.h \
        # UTILS
    ../../util/parallelism/ParallelismController.h \
    ../../util/io/log/Logger.h \
    ../../util/storage/ObjectStorage.h \
    ../../util/units/Units.h \
            ## GEOMETRIES
    ../../util/geometry/Coordinate.h \
    ../../util/geometry/Geometry.h \
    ../../util/geometry/Quadtree.h \
    ../../util/geometry/GeometryTransformators.h  \
    ../../util/geometry/GeometryComparators.h  \
    ../../util/geometry/GeometryGetters.h  \
    ../../util/geometry/GeometryToGeoJSON.h \
    ../../util/routing/Routing.h \
    ../../util/api/APIDriver.h \
    ../../util/datasource/DatasourceReader.h \
    ../../util/datasource/EntityGeneratorDatasource.h \
    ../../util/routing/EdgeVisitor.hpp \
            ## GRID
            ## RANDOM
    ../../util/random/UniformDistribution.h \
    ../../util/distributed/ExternalListener.h \
    ../../util/distributed/ExternalCommunicator.h \
    ../../util/distributed/ExternalPublisher.h \
    ../../util/graph/Edge.h \
        # ENVIRONMENT
    ../../environment/Environment.h \
    ../../environment/EnvironmentsGroup.h \
    ../../environment/entity_environment/EntityEnvironment.h \
    ../../environment/physical_environment/PhysicalEnvironment.h \
    ../../environment/time_environment/TimeEnvironment.h \
    ../../environment/network_environment/NetworkEnvironment.h \
    ../../environment/execution_environment/ExecutionEnvironment.h \
    ../../environment/communication_environment/CommunicationEnvironment.h \
    ../../environment/social_environment/SocialEnvironment.h \
    ../../environment/social_environment/SocialRelation.h \
    ../../environment/network_environment/NetworkEdge.h
            ## TRANSPORT LINE



SOURCES += mainlauncher.cpp \
        # BASICS
    ../../app/App.cpp \
    ../../object/Object.cpp \
    ../../object/ObjectFactory.cpp \
    ../../entity/Entity.cpp \
    ../../entity/EntitySource.cpp \
        # SKILLS
    ../../skill/Skill.cpp \
        # BEHAVIOUR
    ../../behaviour/Behaviour.cpp \
    ../../behaviour/transaction/TransactionBehaviour.cpp \
    ../../behaviour/property/CompareEntityPropertyBehaviour.cpp \
    ../../behaviour/random/GenerateRandomValueBehaviour.cpp \
    ../../behaviour/property/AccumulatePropertyBehaviour.cpp \
    ../../behaviour/property/MathEntityPropertyBehaviour.cpp \
    ../../behaviour/time/WaitUntilTimeBehaviour.cpp \
        # UTILS
    ../../util/parallelism/ParallelismController.cpp \
    ../../util/io/log/Logger.cpp \
    ../../util/storage/ObjectStorage.cpp \
    ../../util/api/APIDriver.cpp \
    ../../util/datasource/DatasourceReader.cpp \
            ## DATASOURCES
    ../../util/datasource/EntityGeneratorDatasource.cpp \
            ## GEOMETRIES
    ../../util/geometry/Coordinate.cpp \
    ../../util/geometry/Geometry.cpp \
    ../../util/geometry/Quadtree.cpp \
    ../../util/geometry/GeometryTransformators.cpp  \
    ../../util/geometry/GeometryComparators.cpp  \
    ../../util/geometry/GeometryGetters.cpp  \
    ../../util/geometry/GeometryToGeoJSON.cpp \
    ../../util/routing/Routing.cpp \
            ## GRID
            ## RANDOM
    ../../util/random/UniformDistribution.cpp \
    ../../util/distributed/ExternalListener.cpp \
    ../../util/distributed/ExternalCommunicator.cpp \
    ../../util/distributed/ExternalPublisher.cpp \
    ../../util/graph/Edge.cpp \
        # ENVIRONMENT
    ../../environment/Environment.cpp \
    ../../environment/EnvironmentsGroup.cpp \
    ../../environment/entity_environment/EntityEnvironment.cpp \
    ../../environment/physical_environment/PhysicalEnvironment.cpp \
    #../../environment/social_environment/Message.cpp \
    ../../environment/time_environment/TimeEnvironment.cpp \
    ../../environment/network_environment/NetworkEnvironment.cpp \
    ../../environment/execution_environment/ExecutionEnvironment.cpp \
    ../../environment/communication_environment/CommunicationEnvironment.cpp \
    ../../environment/social_environment/SocialEnvironment.cpp \
    ../../environment/social_environment/SocialRelation.cpp \
    ../../environment/network_environment/NetworkEdge.cpp



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
