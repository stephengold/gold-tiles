SVN_DIR = ../..
SRC_DIR = $${SVN_DIR}/gold-tiles/src
CONFIG += debug qt
DEFINES += _DEBUG _QT _GRAPHICS_TEST
INCLUDEPATH += $${SRC_DIR}
QT += core gui
TARGET = graphics-test
TEMPLATE = app

SOURCES += \
 $${SRC_DIR}/gui/fractionpair.cpp \
 $${SRC_DIR}/gui/graphics.cpp \
 $${SRC_DIR}/gui/point.cpp \
 $${SRC_DIR}/gui/poly.cpp \
 $${SRC_DIR}/gui/rect.cpp \
 $${SRC_DIR}/gui/window.cpp \
 $${SRC_DIR}/project.cpp \
 $${SRC_DIR}/string.cpp \
 $${SRC_DIR}/strings.cpp

HEADERS += \
 $${SRC_DIR}/gui/color.hpp \
 $${SRC_DIR}/gui/fractionpair.hpp \
 $${SRC_DIR}/gui/graphics.hpp \
 $${SRC_DIR}/gui/point.hpp \
 $${SRC_DIR}/gui/poly.hpp \
 $${SRC_DIR}/gui/rect.hpp \
 $${SRC_DIR}/gui/window.hpp \
 $${SRC_DIR}/project.hpp \
 $${SRC_DIR}/string.hpp \
 $${SRC_DIR}/strings.hpp
