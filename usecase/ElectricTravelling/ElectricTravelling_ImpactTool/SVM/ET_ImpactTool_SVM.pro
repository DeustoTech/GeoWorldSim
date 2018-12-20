TEMPLATE = app
QT = core
QT += network

SOURCES += main.cpp \
    ../../../../util/ai/Intelligence.cpp \
    ../../../../util/svm/Svm.cpp

HEADERS += \
    ../../../../util/ai/Intelligence.h \
    ../../../../util/svm/Svm.h



#INCLUDE LISVM SYSTEM LIBRARY (sudo apt-get install libsvm-dev)

unix|win32: LIBS += -lsvm
