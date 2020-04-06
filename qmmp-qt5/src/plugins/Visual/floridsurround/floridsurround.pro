include($$PWD/../../plugins.pri)
include($$PWD/../common/common.pri)

DESTDIR = $$PLUGINS_PREFIX/Visual
TARGET = floridsurround

HEADERS += floridsurround.h \
           visualfloridsurroundfactory.h
           
SOURCES += floridsurround.cpp \
           visualfloridsurroundfactory.cpp

unix{
    QMAKE_CLEAN = $$DESTDIR/libfloridsurround.so
    LIBS += -L/usr/lib -I/usr/include
}
