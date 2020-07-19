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
QT          += network # sudo apt-get install openssl
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
CONFIG      += resources_big

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
    ../../skill/move/MoveSkill.h \
    ../../skill/move/MoveThroughNetworkSkill.h \
        # BEHAVIOURS
    ../../behaviour/Behaviour.h \
    ../../behaviour/information/SendEntitySnapshotBehaviour.h \
    ../../behaviour/property/SetPropertyBehaviour.h \
    ../../behaviour/move/MoveBehaviour.h \
    ../../behaviour/move/MoveThroughNetworkBehaviour.h \
    ../../behaviour/emissions/VehicleNoiseBehaviour.h \
    ../../behaviour/property/SendPropertyStatisticsBehaviour.h \
        # UTILS
    ../../util/parallelism/ParallelismController.h \
    ../../util/io/log/Logger.h \
    ../../util/storage/ObjectStorage.h \
    ../../util/storage/GlobalObjectStorage.h \
    ../../util/units/Units.h \
    ../../util/svm/Svm.h \
    ../../util/polyline_encoder/polylineencoder.h \
            ## GEOMETRIES
    ../../util/geometry/Geometry.h \
    ../../util/geometry/Coordinate.h \
    ../../util/geometry/Quadtree.h \
    ../../util/geometry/GeometryToGeoJSON.h \
    ../../util/geometry/GeometryComparators.h \
    ../../util/geometry/GeometryTransformators.h \
    ../../util/geometry/GeometryGetters.h \
    ../../util/network/WebSocketDriver.h \
    ../../util/network/ListenerWebSocket.h \
    ../../util/network/PublisherWebSocket.h \
    ../../util/network/HttpDriver.h \
    ../../util/datasource/DatasourceReader.h \
    ../../util/datasource/EntityGeneratorDatasource.h \
    ../../util/routing/EdgeVisitor.hpp \
    ../../util/routing/Routing.h \
    ../../util/routing/TSPRouting.h \
            ## GRAPH
    ../../util/graph/Edge.h \
            ## GRID
    ../../util/grid/Grid.h \
    ../../util/grid/GridCoordinatesConversor.h \
    ../../util/ai/Intelligence.h \
            ## RANDOM
    ../../util/random/UniformDistribution.h \
    ../../util/random/RouletteWheel.h \
            ## DATASOURCES
        # ENVIRONMENT
    ../../environment/Environment.h \
    ../../environment/EnvironmentsGroup.h \
    ../../environment/entity_environment/EntityEnvironment.h \
    ../../environment/communication_environment/CommunicationEnvironment.h \
    ../../environment/physical_environment/PhysicalEnvironment.h \
    ../../environment/time_environment/TimeEnvironment.h \
    ../../environment/network_environment/NetworkEnvironment.h \
    ../../environment/network_environment/NetworkEdge.h \
    ../../environment/execution_environment/ExecutionEnvironment.h \
    ../../environment/grid_environment/GridEnvironment.h \
        # AGENTS`
            ## TRANSPORT LINES
    ../../skill/pollute/PolluteSkill.h \
    ../../behaviour/emissions/PolluteBehaviour.h \
    ../../behaviour/move/SetNextRouteDestinationBehaviour.h \
    ../../behaviour/electricTravelling/CalculateGTAlgRouteBehaviour.h \
    ../../behaviour/electricTravelling/CalculateETPlannerRouteBehaviour.h \
    ../../skill/move/StoreMultiRouteSkill.h \
    ../../behaviour/execution/StopEntityBehaviour.h \
    ../../behaviour/execution/CheckIfEntitiesRunningBehaviour.h \
    ../../behaviour/electricTravelling/AssignVehicleSubtypeBehaviour.h \
    ../../behaviour/move/MoveToPointBehaviour.h


SOURCES += main.cpp \
        # BASICS
    ../../app/App.cpp \
    ../../object/Object.cpp \
    ../../object/ObjectFactory.cpp \
    ../../entity/Entity.cpp \
    ../../entity/EntitySource.cpp \
        # SKILLS
    ../../skill/Skill.cpp \
    ../../skill/move/MoveSkill.cpp \
    ../../skill/move/MoveThroughNetworkSkill.cpp \
        # BEHAVIOUR
    ../../behaviour/Behaviour.cpp \
    ../../behaviour/information/SendEntitySnapshotBehaviour.cpp \
    ../../behaviour/property/SetPropertyBehaviour.cpp \
    ../../behaviour/move/MoveBehaviour.cpp \
    ../../behaviour/move/MoveThroughNetworkBehaviour.cpp \
    ../../behaviour/emissions/VehicleNoiseBehaviour.cpp \
    ../../behaviour/property/SendPropertyStatisticsBehaviour.cpp \
        # UTILS
    ../../util/parallelism/ParallelismController.cpp \
    ../../util/io/log/Logger.cpp \
    ../../util/storage/ObjectStorage.cpp \
    ../../util/storage/GlobalObjectStorage.cpp \
    ../../util/network/WebSocketDriver.cpp \
    ../../util/network/ListenerWebSocket.cpp \
    ../../util/network/PublisherWebSocket.cpp \
    ../../util/network/HttpDriver.cpp \
    ../../util/datasource/DatasourceReader.cpp \
    ../../util/datasource/EntityGeneratorDatasource.cpp \
    ../../util/routing/Routing.cpp \
    ../../util/routing/TSPRouting.cpp \
    ../../util/polyline_encoder/polylineencoder.cpp \
            ## DATASOURCES
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
    ../../util/grid/GridCoordinatesConversor.cpp \
    ../../util/ai/Intelligence.cpp \
    ../../util/svm/Svm.cpp \
            ## RANDOM
    ../../util/random/UniformDistribution.cpp \
    ../../util/random/RouletteWheel.cpp \
        # ENVIRONMENT
    ../../environment/Environment.cpp \
    ../../environment/EnvironmentsGroup.cpp \
    ../../environment/entity_environment/EntityEnvironment.cpp \
    ../../environment/communication_environment/CommunicationEnvironment.cpp \
    ../../environment/physical_environment/PhysicalEnvironment.cpp \
    ../../environment/time_environment/TimeEnvironment.cpp \
    ../../environment/network_environment/NetworkEnvironment.cpp \
    ../../environment/network_environment/NetworkEdge.cpp \
    ../../environment/execution_environment/ExecutionEnvironment.cpp \
    ../../environment/grid_environment/GridEnvironment.cpp \
        # AGENTS
    ../../skill/pollute/PolluteSkill.cpp \
    ../../behaviour/emissions/PolluteBehaviour.cpp \
    ../../behaviour/move/SetNextRouteDestinationBehaviour.cpp \
    ../../behaviour/electricTravelling/CalculateGTAlgRouteBehaviour.cpp \
    ../../behaviour/electricTravelling/CalculateETPlannerRouteBehaviour.cpp \
    ../../skill/move/StoreMultiRouteSkill.cpp \
    ../../behaviour/execution/StopEntityBehaviour.cpp \
    ../../behaviour/execution/CheckIfEntitiesRunningBehaviour.cpp \
    ../../behaviour/electricTravelling/AssignVehicleSubtypeBehaviour.cpp \
    ../../behaviour/move/MoveToPointBehaviour.cpp


#INCLUDE LEMON SYSTEM LIBRARY (sudo apt-get install liblemon-dev)
#unix|win32: LIBS += -llemon
unix|win32: LIBS += -lemon

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
#unix|win32: LIBS += -lfuzzylite
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
