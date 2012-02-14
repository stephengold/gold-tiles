#ifndef PROJECT_HPP_INCLUDED
#define PROJECT_HPP_INCLUDED

// File:    project.hpp
// Purpose: project-wide definitions for the Gold Tile Game
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU General Public License

// project-wide macros
#ifdef _WINDOWS
#define _GUI
#else
#define _CONSOLE
#endif

// debugging macros
#ifdef _WINDOWS
#include <assert.h>
#define ASSERT assert
#else
#define ASSERT(expression) ((expression) ? (void)0 : ::assertion_failed(__FILE__, __LINE__))
#endif
//#define ASSERT(expression) ((void)0)

//#define D(debug_only_code) (debug_only_code)
#define D(debug_only_code) ((void)0)

// forward declarations of project-wide classes
class BaseBoard;
class Board;
class Cell;
class Cells;
class FractionPair;
class Game;
class Hand;
class Hands;
class Indices;
class Move;
class String;
class Strings;
class Tile;
class Tiles;
class TileCell;

#ifdef _GUI
class Canvas;
class Dialog;
class Graphics;
class HandBox;
class MenuItem;
class ParmBox1;
class ParmBox2;
class ParmBox3;
class PlayMenu;
class Point;
class Poly;
class Rect;
class SubMenu;
class TopWindow;
class ViewMenu;
class Window;
class WindowClass;
class YesNo;
#endif

// project-wide utility functions
void        assertion_failed(const char *, unsigned);
String      ordinal(unsigned n);
void        pause(void);
const char *plural(unsigned);
String      plural(unsigned n, const char *);

#endif
