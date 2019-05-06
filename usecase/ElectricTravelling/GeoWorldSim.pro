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

TARGET      = gws-electric
CONFIG      += console
CONFIG      -= app_bundle

TEMPLATE = app


HEADERS += \
        # BASICS
    ../../app/App.h \
    ../../object/Object.h \
    ../../object/ObjectFactory.h \
    ../../agent/Agent.h \
    ../../agent/AgentSource.h \
        # SKILLS
    ../../skill/Skill.h \
    ../../skill/move/MoveSkill.h \
    ../../skill/move/MoveThroughRouteSkill.h \
        # BEHAVIOURS
    ../../behaviour/Behaviour.h \
    ../../behaviour/information/SendAgentSnapshotBehaviour.h \
    ../../behaviour/move/MoveBehaviour.h \
    ../../behaviour/move/MoveThroughRouteBehaviour.h \
    ../../behaviour/execution/StopAgentBehaviour.h \
    ../../behaviour/property/PropertyStatisticsBehaviour.h \
    ../../behaviour/emissions/VehicleNoiseBehaviour.h \
        # UTILS
    ../../util/parallelism/ParallelismController.h \
    ../../util/io/log/Logger.h \
    ../../util/storage/ObjectStorage.h \
    ../../util/units/Units.h \
    ../../util/ai/InAPItelligence.h \
    ../../util/svm/Svm.h \
            ## GEOMETRIES
    ../../util/geometry/Geometry.h \
    ../../util/geometry/Coordinate.h \
    ../../util/geometry/Quadtree.h \
    ../../util/geometry/GeometryToGeoJSON.h \
    ../../util/geometry/GeometryComparators.h \
    ../../util/geometry/GeometryTransformators.h \
    ../../util/geometry/GeometryGetters.h \
    ../../util/distributed/ExternalCommunicator.h \
    ../../util/distributed/ExternalListener.h \
    ../../util/distributed/ExternalPublisher.h \
    ../../util/api/APIDriver.h \
    ../../util/datasource/DatasourceReader.h \
    ../../util/routing/EdgeVisitor.hpp \
    ../../util/routing/Routing.h \
    ../../util/routing/TSPRouting.h \
            ## GRAPH
    ../../util/graph/Edge.h \
            ## GRID
    ../../util/grid/Grid.h \
    ../../util/ai/Intelligence.h \
            ## RANDOM
    ../../util/random/UniformDistribution.h \
            ## DATASOURCES
    ../../util/datasource/AgentGeneratorDatasource.h \
        # ENVIRONMENT
    ../../environment/Environment.h \
    ../../environment/EnvironmentsGroup.h \
    ../../environment/agent_environment/AgentEnvironment.h \
    ../../environment/communication_environment/CommunicationEnvironment.h \
    ../../environment/physical_environment/PhysicalEnvironment.h \
    ../../environment/time_environment/TimeEnvironment.h \
    ../../environment/network_environment/NetworkEnvironment.h \
    ../../environment/network_environment/NetworkEdge.h \
    ../../environment/execution_environment/ExecutionEnvironment.h \
        # AGENTS`
            ## TRANSPORT LINES
    ../../skill/pollute/PolluteSkill.h \
    ../../behaviour/emissions/PolluteBehaviour.h \
    ../../behaviour/move/SetNextRouteDestinationBehaviour.h \
    ../../behaviour/electricTravelling/CalculateGTAlgRouteBehaviour.h \
    ../../skill/move/StoreMultiRouteSkill.h \
    ../../behaviour/property/PropertyHeatmapBehaviour.h
    #../../skill/pollute/VehiclePolluteSkill.h


SOURCES += main.cpp \
        # BASICS
    ../../app/App.cpp \
    ../../object/Object.cpp \
    ../../object/ObjectFactory.cpp \
    ../../agent/Agent.cpp \
    ../../agent/AgentSource.cpp \
        # SKILLS
    ../../skill/Skill.cpp \
    ../../skill/move/MoveSkill.cpp \
    ../../skill/move/MoveThroughRouteSkill.cpp \
        # BEHAVIOUR
    ../../behaviour/Behaviour.cpp \
    ../../behaviour/information/SendAgentSnapshotBehaviour.cpp \
    ../../behaviour/move/MoveBehaviour.cpp \
    ../../behaviour/move/MoveThroughRouteBehaviour.cpp \
    ../../behaviour/execution/StopAgentBehaviour.cpp \
    ../../behaviour/property/PropertyStatisticsBehaviour.cpp \
    ../../behaviour/emissions/VehicleNoiseBehaviour.cpp \
        # UTILS
    ../../util/parallelism/ParallelismController.cpp \
    ../../util/io/log/Logger.cpp \
    ../../util/storage/ObjectStorage.cpp \
    ../../util/distributed/ExternalCommunicator.cpp \
    ../../util/distributed/ExternalListener.cpp \
    ../../util/distributed/ExternalPublisher.cpp \
    ../../util/api/APIDriver.cpp \
    ../../util/datasource/DatasourceReader.cpp \
    ../../util/routing/Routing.cpp \
    ../../util/routing/TSPRouting.cpp \
            ## DATASOURCES
    ../../util/datasource/AgentGeneratorDatasource.cpp \
            ## GEOMETRIES
    ../../util/geometry/Geometry.cpp \
    ../../util/geometry/Coordinate.cpp \
    ../../util/geometry/Quadtree.cpp \
    ../../util/geometry/GeometryToGeoJSON.cpp \
    ../../util/geometry/GeometryComparators.cpp \
    ../../util/geometry/GeometryTransformators.cpp \
    ../../util/geometry/GeometryGetters.cpp \
            ## GRAPH
    ../../util/graph/Edge.cpp \
            ## GRID
    ../../util/grid/Grid.cpp \
    ../../util/ai/Intelligence.cpp \
    ../../util/svm/Svm.cpp \
            ## RANDOM
    ../../util/random/UniformDistribution.cpp \
        # ENVIRONMENT
    ../../environment/Environment.cpp \
    ../../environment/EnvironmentsGroup.cpp \
    ../../environment/agent_environment/AgentEnvironment.cpp \
    ../../environment/communication_environment/CommunicationEnvironment.cpp \
    ../../environment/physical_environment/PhysicalEnvironment.cpp \
    ../../environment/time_environment/TimeEnvironment.cpp \
    ../../environment/network_environment/NetworkEnvironment.cpp \
    ../../environment/network_environment/NetworkEdge.cpp \
    ../../environment/execution_environment/ExecutionEnvironment.cpp \
        # AGENTS
    ../../skill/pollute/PolluteSkill.cpp \
    ../../behaviour/emissions/PolluteBehaviour.cpp \
    ../../behaviour/move/SetNextRouteDestinationBehaviour.cpp \
    ../../behaviour/electricTravelling/CalculateGTAlgRouteBehaviour.cpp \
    ../../skill/move/StoreMultiRouteSkill.cpp \
    ../../behaviour/property/PropertyHeatmapBehaviour.cpp
    #../../skill/pollute/VehiclePolluteSkill.cpp


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
