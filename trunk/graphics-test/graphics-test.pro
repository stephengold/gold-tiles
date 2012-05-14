SVN_DIR = ../..
SRC_DIR = $${SVN_DIR}/gold-tiles/src
CONFIG += debug qt
DEFINES += _DEBUG _GRAPHICS_TEST _QT
INCLUDEPATH += $${SRC_DIR}
QT += core gui
TARGET = graphics-test
TEMPLATE = app

SOURCES += \
 $${SRC_DIR}/fraction.cpp \
 $${SRC_DIR}/gui/area.cpp \
 $${SRC_DIR}/gui/fractionpair.cpp \
 $${SRC_DIR}/gui/graphics.cpp \
 $${SRC_DIR}/gui/point.cpp \
 $${SRC_DIR}/gui/poly.cpp \
 $${SRC_DIR}/gui/rect.cpp \
 $${SRC_DIR}/gui/window.cpp \
 $${SRC_DIR}/project.cpp \
 $${SRC_DIR}/string.cpp \
 $${SRC_DIR}/strings.cpp
