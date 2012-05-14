SVN_DIR = ../..
SRC_DIR = $${SVN_DIR}/gold-tiles/src
CONFIG += console debug
DEFINES +=  _CLIENT _DEBUG _NETWORK_TEST _QT
INCLUDEPATH += $${SRC_DIR}
QT += core network
TARGET = client-test
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
 $${SRC_DIR}/hand.cpp \
 $${SRC_DIR}/handopt.cpp \
 $${SRC_DIR}/handopts.cpp \
 $${SRC_DIR}/hands.cpp \
 $${SRC_DIR}/indices.cpp \
 $${SRC_DIR}/move.cpp \
 $${SRC_DIR}/network.cpp \
 $${SRC_DIR}/partial.cpp \
 $${SRC_DIR}/project.cpp \
 $${SRC_DIR}/tile.cpp \
 $${SRC_DIR}/tilecell.cpp \
 $${SRC_DIR}/tileopt.cpp \
 $${SRC_DIR}/tiles.cpp \
 $${SRC_DIR}/turn.cpp \
 $${SRC_DIR}/turns.cpp \
 $${SRC_DIR}/socket.cpp \
 $${SRC_DIR}/string.cpp \
 $${SRC_DIR}/strings.cpp
