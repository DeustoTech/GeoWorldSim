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

TARGET      = gwsaccessibility
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
        # BEHAVIOUR
    ../../behaviour/Behaviour.h \
    ../../behaviour/geometry/GenerateAgentGeometryBehaviour.h \
    ../../behaviour/information/SendAgentSnapshotBehaviour.h \
    ../../behaviour/property/CompareAgentPropertyBehaviour.h \
    ../../behaviour/execution/StopAgentBehaviour.h \
    ../../behaviour/property/SetAgentPropertyBehaviour.h \
    ../../behaviour/information/ListenToMessagesBehaviour.h \
    ../../behaviour/accessibility/DetermineAccessibilityBehaviour.h \
    ../../behaviour/accessibility/FindDirectClosestBehaviour.h \
    ../../behaviour/accessibility/FindRoutingClosestBehaviour.h \
    ../../behaviour/random/ChooseRandomValueFromSetBehaviour.h \
        # UTILS
    ../../util/io/log/Logger.h \
    ../../util/io/csv/CsvImporter.h \
    ../../util/io/ImporterExporter.h \
    ../../util/storage/ObjectStorage.h \
    ../../util/units/Units.h \
            ## GEOMETRIES
    ../../util/geometry/Coordinate.h \
    ../../util/geometry/Geometry.h \
    ../../util/geometry/Quadtree.h \
    ../../util/api/APIDriver.h \
    ../../util/routing/Routing.hpp \
    ../../util/routing/EdgeVisitor.hpp \
    ../../util/routing/TSPRouting.h \
            ## DATASOURCES
    ../../util/datasource/DatasourceReader.h \
    ../../util/datasource/AgentGeneratorDatasource.h \
            ## GRID
    ../../util/grid/Grid.h \
    ../../util/grid/AgentGrid.h \
    ../../util/grid/GridCoordinatesConversor.h \
            ## RANDOM
    ../../util/random/UniformDistribution.h \
    ../../util/distributed/ExternalListener.h \
    ../../util/distributed/ExternalCommunicator.h \
    ../../util/distributed/ExternalPublisher.h \
    ../../util/ai/Intelligence.h \
    ../../util/svm/Svm.h \
    ../../util/graph/Edge.h \
    ../../util/graph/NewEdge.h \
        # ENVIRONMENT
    ../../environment/Environment.h \
    ../../environment/EnvironmentsGroup.h \
    ../../environment/agent_environment/AgentEnvironment.h \
    ../../environment/physical_environment/PhysicalEnvironment.h \
    ../../environment/time_environment/TimeEnvironment.h \
    ../../environment/grid_environment/GridEnvironment.h \
    ../../environment/network_environment/NetworkEnvironment.h \
    ../../environment/network_environment/NewNetworkEdge.h \
    ../../environment/execution_environment/ExecutionEnvironment.h \
    ../../environment/communication_environment/CommunicationEnvironment.h \
    ../../environment/social_environment/SocialEnvironment.h \
    ../../environment/network_environment/NetworkEdge.h \
        # AGENTS
    #agent/person/Person.h \
    HumanAgent.h \
    ../../behaviour/property/AddGWSGroupPropertyBehaviour.h



SOURCES += mainAccesibility.cpp \
        # BASICS
    ../../app/App.cpp \
    ../../object/Object.cpp \
    ../../object/ObjectFactory.cpp \
    ../../agent/Agent.cpp \
    ../../agent/AgentSource.cpp \
        # SKILLS
    ../../skill/Skill.cpp \
        # BEHAVIOURS
    ../../behaviour/Behaviour.cpp \
    ../../behaviour/geometry/GenerateAgentGeometryBehaviour.cpp \
    ../../behaviour/information/SendAgentSnapshotBehaviour.cpp \
    ../../behaviour/property/CompareAgentPropertyBehaviour.cpp \
    ../../behaviour/execution/StopAgentBehaviour.cpp \
    ../../behaviour/property/SetAgentPropertyBehaviour.cpp \
    ../../behaviour/information/ListenToMessagesBehaviour.cpp \
    ../../behaviour/accessibility/DetermineAccessibilityBehaviour.cpp \
    ../../behaviour/accessibility/FindDirectClosestBehaviour.cpp \
    ../../behaviour/accessibility/FindRoutingClosestBehaviour.cpp \
    ../../behaviour/random/ChooseRandomValueFromSetBehaviour.cpp \
        # UTILS
    ../../util/io/log/Logger.cpp \
    ../../util/storage/ObjectStorage.cpp \
    ../../util/api/APIDriver.cpp \
    ../../util/routing/TSPRouting.cpp \
    ../../util/io/csv/CsvImporter.cpp \
    ../../util/io/ImporterExporter.cpp \
            ## DATASOURCES
    ../../util/datasource/DatasourceReader.cpp \
    ../../util/datasource/AgentGeneratorDatasource.cpp \
            ## GEOMETRIES
    ../../util/geometry/Coordinate.cpp \
    ../../util/geometry/Geometry.cpp \
    ../../util/geometry/Quadtree.cpp \
    #../../util/graph/Graph.cpp \
    #../../util/graph/GraphUtils.cpp \
            ## GRID
    ../../util/grid/Grid.cpp \
    #../../util/grid/NumberGrid.cpp \
    ../../util/grid/AgentGrid.cpp \
    ../../util/grid/GridCoordinatesConversor.cpp \
            ## RANDOM
    ../../util/random/UniformDistribution.cpp \
    ../../util/distributed/ExternalListener.cpp \
    ../../util/distributed/ExternalCommunicator.cpp \
    ../../util/distributed/ExternalPublisher.cpp \
    ../../util/ai/Intelligence.cpp \
    ../../util/svm/Svm.cpp \
    ../../util/graph/Edge.cpp \
    ../../util/graph/NewEdge.cpp \
        # ENVIRONMENT
    ../../environment/Environment.cpp \
    ../../environment/EnvironmentsGroup.cpp \
    ../../environment/agent_environment/AgentEnvironment.cpp \
    ../../environment/physical_environment/PhysicalEnvironment.cpp \
    ../../environment/time_environment/TimeEnvironment.cpp \
    ../../environment/network_environment/NetworkEnvironment.cpp \
    ../../environment/network_environment/NewNetworkEdge.cpp \
    ../../environment/execution_environment/ExecutionEnvironment.cpp \
    ../../environment/grid_environment/GridEnvironment.cpp \
    ../../environment/communication_environment/CommunicationEnvironment.cpp \
    ../../environment/social_environment/SocialEnvironment.cpp \
    ../../environment/network_environment/NetworkEdge.cpp \
        # AGENTS
    HumanAgent.cpp \
    ../../behaviour/property/AddGWSGroupPropertyBehaviour.cpp


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
