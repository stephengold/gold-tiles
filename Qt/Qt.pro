SVN_DIR = ../..
SRC_DIR = $${SVN_DIR}/gold-tiles/src
CONFIG += debug qt
DEFINES += _CLIENT _DEBUG _SERVER _QT
FORMS +=
INCLUDEPATH += $${SRC_DIR}
QT += core gui network
TARGET = Qt
TEMPLATE = app

SOURCES += \
 $${SRC_DIR}/address.cpp \
 $${SRC_DIR}/baseboard.cpp \
 $${SRC_DIR}/board.cpp \
 $${SRC_DIR}/cell.cpp \
 $${SRC_DIR}/cells.cpp \
 $${SRC_DIR}/combo.cpp \
 $${SRC_DIR}/direction.cpp \
 $${SRC_DIR}/fraction.cpp \
 $${SRC_DIR}/game.cpp \
 $${SRC_DIR}/gameopt.cpp \
 $${SRC_DIR}/gui/area.cpp \
 $${SRC_DIR}/gui/canvas.cpp \
 $${SRC_DIR}/gui/displaymodes.cpp \
 $${SRC_DIR}/gui/filemenu.cpp \
 $${SRC_DIR}/gui/fractionpair.cpp \
 $${SRC_DIR}/gui/gameview.cpp \
 $${SRC_DIR}/gui/gamewindow.cpp \
 $${SRC_DIR}/gui/graphics.cpp \
 $${SRC_DIR}/gui/main.cpp \
 $${SRC_DIR}/gui/markings.cpp \
 $${SRC_DIR}/gui/menubar.cpp \
 $${SRC_DIR}/gui/menuitem.cpp \
 $${SRC_DIR}/gui/playmenu.cpp \
 $${SRC_DIR}/gui/point.cpp \
 $${SRC_DIR}/gui/poly.cpp \
 $${SRC_DIR}/gui/rect.cpp \
 $${SRC_DIR}/gui/sizemenu.cpp \
 $${SRC_DIR}/gui/submenu.cpp \
 $${SRC_DIR}/gui/user.cpp \
 $${SRC_DIR}/gui/viewmenu.cpp \
 $${SRC_DIR}/gui/window.cpp \
 $${SRC_DIR}/hand.cpp \
 $${SRC_DIR}/handopt.cpp \
 $${SRC_DIR}/handopts.cpp \
 $${SRC_DIR}/hands.cpp \
 $${SRC_DIR}/indices.cpp \
 $${SRC_DIR}/move.cpp \
 $${SRC_DIR}/network.cpp \
 $${SRC_DIR}/partial.cpp \
 $${SRC_DIR}/project.cpp \
 $${SRC_DIR}/socket.cpp \
 $${SRC_DIR}/string.cpp \
 $${SRC_DIR}/strings.cpp \
 $${SRC_DIR}/tile.cpp \
 $${SRC_DIR}/tilecell.cpp \
 $${SRC_DIR}/tileopt.cpp \
 $${SRC_DIR}/tiles.cpp \
 $${SRC_DIR}/turn.cpp \
 $${SRC_DIR}/turns.cpp
