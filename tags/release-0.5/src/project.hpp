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
#define ASSERT(expression) ((expression) ? (void)0 : ::assertionFailed(__FILE__, __LINE__))
#endif
//#define ASSERT(expression) ((void)0)

//#define D(debug_only_code) (debug_only_code)
#define D(debug_only_code) ((void)0)

// forward declarations of project-wide classes and enums
class BaseBoard;
class Board;
class Cell;
class Cells;
enum DirectionType;
class FractionPair;
class Game;
class Indices;
class Move;
class Player;
class Players;
class String;
class Strings;
class Tile;
class Tiles;
class TileCell;

#ifdef _GUI
class Canvas;
enum ColorType;
class Dialog;
class Graphics;
class MenuItem;
class PlayMenu;
class Poly;
class Rect;
class SubMenu;
class TopWindow;
class ViewMenu;
class Window;
class WindowClass;
#endif

// project-wide utility functions
void assertionFailed(const char *, unsigned);
void pause(void);
const char *plural(unsigned);
String plural(unsigned n, const char *);

// global pointers
#ifdef _GUI
extern TopWindow *gpTopWindow;
#endif

#endif
