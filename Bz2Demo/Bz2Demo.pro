QT -= gui
QT += xml

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_BZIP2_DEMO

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/../QtBZip2Lib

HEADERS += \
         $$PWD/../QtBZip2Lib/bzlib.h \
         $$PWD/../QtBZip2Lib/qbzip2device.h \
         $$PWD/../QtBZip2Lib/qtbzip2lib_global.h

SOURCES += \
        $$PWD/../QtBZip2Lib/qbzip2device.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32 {
    LIBS += -L $$PWD/../QtBZip2Lib/libs/windows -lzlib
    LIBS += -lbz2
}

unix {
    INSTALLS += target
    LIBS += -L $$PWD/../QtBZip2Lib/libs/unix -lz -lbz
}




