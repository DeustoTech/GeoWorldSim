TEMPLATE = app
QT = core
QT += network

SOURCES += main.cpp \
    ../../../util/datasource/DatasourceReader.cpp \
    ../../../util/api/APIDriver.cpp \
    ../../../util/neural_network/NeuralNetwork.cpp

HEADERS += \
    ../../../util/datasource/DatasourceReader.h \
    ../../../util/api/APIDriver.h \
    ../../../util/neural_network/NeuralNetwork.h

#INCLUDE SHARKSYSTEM LIBRARY

unix|win32: LIBS += -lfann
