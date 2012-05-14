# File:     makefile
# Purpose:  makefile for the Gold Tile Game.
# Author:   Stephen Gold sgold@sonic.net
# (c) Copyright 2012 Stephen Gold
# Distributed under the terms of the GNU General Public License

# This file is part of the Gold Tile Game.
#
# The Gold Tile Game is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by the 
# Free Software Foundation, either version 3 of the License, or (at your 
# option) any later version.
#
# The Gold Tile Game is distributed in the hope that it will be useful, but 
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
# for more details.
#
# You should have received a copy of the GNU General Public License
# along with the Gold Tile Game.  If not, see <http://www.gnu.org/licenses/>.

CC = g++
DIRS = client server
LDFLAGS =
SRCDIR = src
TARGETS = gold-tile-client gold-tile-server

CFLAGS = -c -D_DEBUG -D_POSIX -g -I$(SRCDIR) -Wall
SOURCES = \
 $(SRCDIR)/address.cpp \
 $(SRCDIR)/baseboard.cpp \
 $(SRCDIR)/board.cpp \
 $(SRCDIR)/cell.cpp \
 $(SRCDIR)/cells.cpp \
 $(SRCDIR)/combo.cpp \
 $(SRCDIR)/direction.cpp \
 $(SRCDIR)/fifo.cpp \
 $(SRCDIR)/fraction.cpp \
 $(SRCDIR)/game.cpp \
 $(SRCDIR)/gameopt.cpp \
 $(SRCDIR)/hand.cpp \
 $(SRCDIR)/handopt.cpp \
 $(SRCDIR)/handopts.cpp \
 $(SRCDIR)/hands.cpp \
 $(SRCDIR)/indices.cpp \
 $(SRCDIR)/main.cpp \
 $(SRCDIR)/move.cpp \
 $(SRCDIR)/network.cpp \
 $(SRCDIR)/partial.cpp \
 $(SRCDIR)/project.cpp \
 $(SRCDIR)/socket.cpp \
 $(SRCDIR)/string.cpp \
 $(SRCDIR)/strings.cpp \
 $(SRCDIR)/tilecell.cpp \
 $(SRCDIR)/tile.cpp \
 $(SRCDIR)/tileopt.cpp \
 $(SRCDIR)/tiles.cpp \
 $(SRCDIR)/turn.cpp \
 $(SRCDIR)/turns.cpp

CLIENT_OBJECTS = $(SOURCES:src/%.cpp=client/%.o)
SERVER_OBJECTS = $(SOURCES:src/%.cpp=server/%.o)

all: $(TARGETS)

clean:
	rm -rf $(DIRS) $(TARGETS)

client:
	mkdir client

client/%.o: src/%.cpp client
	$(CC) $(CFLAGS) -D_CLIENT -o $@ $<

gold-tile-client: $(CLIENT_OBJECTS)
	$(CC) $(LDFLAGS) $(CLIENT_OBJECTS) -o $@

gold-tile-server: $(SERVER_OBJECTS)
	$(CC) $(LDFLAGS) $(SERVER_OBJECTS) -o $@

server:
	mkdir server

server/%.o: src/%.cpp server
	$(CC) $(CFLAGS) -D_SERVER -o $@ $<

sloc:
	sloccount src | grep '(SLOC'
