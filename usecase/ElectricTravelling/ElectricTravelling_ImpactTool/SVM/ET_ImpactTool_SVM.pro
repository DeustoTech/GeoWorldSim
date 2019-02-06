TEMPLATE = app
QT = core
QT += network
QT += sql


# ACTIVATE QT-COMPILER OPTIMIZATION
# march=native
# 02 -> Optimizations level 2
# pipe
QMAKE_CXXFLAGS += -march=native -O2 -pipe
QMAKE_CFLAGS += -march=native -O2 -pipe

# FORCE C++11
QMAKE_CXXFLAGS += -std=c++14

SOURCES += main.cpp \
    ../../../../util/ai/Intelligence.cpp \
    ../../../../util/svm/Svm.cpp

HEADERS += \
    ../../../../util/ai/Intelligence.h \
    ../../../../util/svm/Svm.h



#INCLUDE LISVM SYSTEM LIBRARY (sudo apt-get install libsvm-dev)

unix|win32: LIBS += -lsvm
