#-------------------------------------------------
#
# Project created by QtCreator 2022-04-20T15:41:54
#
#-------------------------------------------------

QT       -= gui

TARGET = QtBZip2Lib
TEMPLATE = lib

DEFINES += QTBZIP2LIB_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        qbzip2device.cpp

HEADERS += \
        bzlib.h \
        qbzip2device.h \
        qtbzip2lib_global.h 


win32 {
    LIBS += -L $$PWD/libs/windows -lzlib
    LIBS += -lbz2
}

unix {
    target.path = /usr/lib
    INSTALLS += target
    LIBS += -L $$PWD/libs/Unix -lz -lbz
}
