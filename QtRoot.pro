#-------------------------------------------------
#
# Project created by QtCreator 2019-03-20T11:30:37
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtRoot
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

CONFIG += c++11

SOURCES += \
        main.cpp \
    MainWindow.cpp \
    qrootcanvas.cpp \
    reconstruction.cpp \
    setup.cpp \
    worker.cpp

HEADERS += \
    MainWindow.h \
    qrootcanvas.h \
    readerwriterqueue/atomicops.h \
    readerwriterqueue/readerwritercircularbuffer.h \
    readerwriterqueue/readerwriterqueue.h \
    reconstruction.h \
    setup.h \
    worker.h

FORMS += \
    MainWindow.ui

QMAKE_LFLAGS += -fopenmp
QMAKE_CXXFLAGS += -fopenmp
LIBS += -fopenmp

INCLUDEPATH += \
    $(ROOTSYS)/include
LIBS += \
    -L$(ROOTSYS)/lib -lCore -lImt -lRIO -lNet -lHist -lGraf \
    -lGraf3d -lGpad -lTree -lTreePlayer -lRint -lPostscript -lMatrix \
    -lPhysics -lMathCore -lThread -lMultiProc -pthread -lm -ldl -rdynamic

#QMAKE_PRE_LINK += /usr/local/opt/root6/bin/root-config --cflags --libs

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    README.md \
    readerwriterqueue/LICENSE.md \
    readerwriterqueue/README.md

RESOURCES += \
    resources.qrc

