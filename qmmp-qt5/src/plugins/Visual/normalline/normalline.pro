include($$PWD/../../plugins.pri)
include($$PWD/../common/common.pri)

DESTDIR = $$PLUGINS_PREFIX/Visual
TARGET = normalline

HEADERS += normalline.h \
           visualnormallinefactory.h
           
SOURCES += normalline.cpp \
           visualnormallinefactory.cpp

unix{
    QMAKE_CLEAN = $$DESTDIR/libnormalline.so
}
