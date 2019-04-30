  #-------------------------------------------------
#
# Project created by QtCreator 2019-02-20T11:44:26
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

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

TARGET = gwspopulation
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# ACTIVATE QT-COMPILER OPTIMIZATION
# march=native
# 02 -> Optimizations level 2
# pipe
QMAKE_CXXFLAGS += -march=native -O2 -pipe
QMAKE_CFLAGS += -march=native -O2 -pipe

# FORCE C++11
QMAKE_CXXFLAGS += -std=c++14

SOURCES += \
        main.cpp \
       # functions.cpp \
        #utils.cpp \
        ../../app/App.cpp \
        PopulationGeneratorAgent.cpp \
        ../../object/ObjectFactory.cpp \
        ../../object/Object.cpp \
        ../../util/storage/ObjectStorage.cpp \
        ../../agent/Agent.cpp \
        ../../behaviour/Behaviour.cpp \
        # SKILLS
        ../../skill/Skill.cpp \
        ../../skill/move/MoveSkill.cpp \
        # UTILS
        ../../util/parallelism/ParallelismController.cpp \
        ../../util/io/log/Logger.cpp \
        ../../util/routing/TSPRouting.cpp \
        ../../util/distributed/ExternalListener.cpp \
        ../../util/distributed/ExternalCommunicator.cpp \
        ../../util/distributed/ExternalPublisher.cpp \
        ../../util/api/APIDriver.cpp \
        # GEOMETRIES
        ../../util/geometry/Coordinate.cpp \
        ../../util/geometry/Geometry.cpp \
        ../../util/geometry/Quadtree.cpp \
        ../../util/geometry/GeometryTransformators.cpp \
        ../../util/geometry/GeometryGetters.cpp \
        ../../util/geometry/GeometryToGeoJSON.cpp \
        ../../util/graph/Edge.cpp \
        ../../util/grid/Grid.cpp \
        ../../util/grid/GridCoordinatesConversor.cpp \
        ../../util/datasource/DatasourceReader.cpp \
        ../../util/datasource/AgentGeneratorDatasource.cpp \
        # BEHAVIOURS
        ../../behaviour/population/GeneratePopulationBehaviour.cpp \
        ../../behaviour/execution/StopAgentBehaviour.cpp \
        ../../behaviour/time/WaitUntilTimeBehaviour.cpp \
        ../../behaviour/geometry/GenerateAgentGeometryBehaviour.cpp \
        ../../behaviour/information/SendAgentSnapshotBehaviour.cpp \
        ../../behaviour/population/CreateChildBehaviour.cpp \
        ../../behaviour/move/MoveBehaviour.cpp \
        ../../behaviour/execution/DeleteAgentBehaviour.cpp \
        ../../behaviour/property/PropertyStatisticsBehaviour.cpp \
        # ENVIRONMENTS
        ../../environment/Environment.cpp \
        ../../environment/EnvironmentsGroup.cpp \
        ../../environment/agent_environment/AgentEnvironment.cpp \
        ../../environment/physical_environment/PhysicalEnvironment.cpp \
        ../../environment/time_environment/TimeEnvironment.cpp \
        ../../environment/grid_environment/GridEnvironment.cpp\
        ../../environment/network_environment/NetworkEnvironment.cpp \
        ../../environment/social_environment/SocialEnvironment.cpp \
        ../../environment/execution_environment/ExecutionEnvironment.cpp \
        ../../environment/communication_environment/CommunicationEnvironment.cpp \
        ../../environment/network_environment/NetworkEdge.cpp \
        ../../environment/social_environment/SocialRelation.cpp





HEADERS += \
        #header.h \
        ../../app/App.h \
        PopulationGeneratorAgent.h \
        ../../object/ObjectFactory.h \
        ../../object/Object.h \
        ../../util/storage/ObjectStorage.h \
        ../../agent/Agent.h \
        ../../behaviour/Behaviour.h \
        # SKILLS
        ../../skill/Skill.h \
        ../../skill/move/MoveSkill.h \
        # UTILS
        ../../util/parallelism/ParallelismController.h \
        ../../util/io/log/Logger.h \
        ../../util/routing/Routing.hpp \
        ../../util/routing/EdgeVisitor.hpp \
        ../../util/routing/TSPRouting.h \
        ../../util/distributed/ExternalListener.h \
        ../../util/distributed/ExternalCommunicator.h \
        ../../util/distributed/ExternalPublisher.h \
        ../../util/api/APIDriver.h \
        # GEOMETRIES
        ../../util/geometry/Coordinate.h \
        ../../util/geometry/Geometry.h \
        ../../util/geometry/GeometryGetters.h \
        ../../util/geometry/GeometryToGeoJSON.h \
        ../../util/geometry/Quadtree.h \
        ../../util/graph/Edge.h \
        ../../util/grid/Grid.h \
        ../../util/grid/GridCoordinatesConversor.h\
        ../../util/datasource/DatasourceReader.h \
        ../../util/datasource/AgentGeneratorDatasource.h \
        # BEHAVIOURS
        ../../behaviour/population/GeneratePopulationBehaviour.h \
        ../../behaviour/execution/StopAgentBehaviour.h \
        ../../behaviour/time/WaitUntilTimeBehaviour.h \
        ../../behaviour/geometry/GenerateAgentGeometryBehaviour.h \
        ../../behaviour/information/SendAgentSnapshotBehaviour.h \
        ../../behaviour/population/CreateChildBehaviour.h \
        ../../behaviour/move/MoveBehaviour.h \
        ../../behaviour/execution/DeleteAgentBehaviour.h \
        ../../behaviour/property/PropertyStatisticsBehaviour.h \
        # ENVIRONMENTS
        ../../environment/Environment.h \
        ../../environment/EnvironmentsGroup.h \
        ../../environment/agent_environment/AgentEnvironment.h \
        ../../environment/physical_environment/PhysicalEnvironment.h \
        ../../environment/time_environment/TimeEnvironment.h \
        ../../environment/network_environment/NetworkEnvironment.h \
        ../../environment/social_environment/SocialEnvironment.h \
        ../../environment/execution_environment/ExecutionEnvironment.h \
        ../../environment/communication_environment/CommunicationEnvironment.h \
        ../../environment/grid_environment/GridEnvironment.h \
        ../../environment/network_environment/NetworkEdge.h \
    ../../environment/social_environment/SocialRelation.h




# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



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
