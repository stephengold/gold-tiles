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
#else // !defined(_WINDOWS)
#define _CONSOLE
#endif // !defined(_WINDOWS)

// debugging macros
#ifdef _DEBUG
#ifdef _WINDOWS
#include <assert.h>
#define ASSERT assert
#else // !defined(_WINDOWS)
#define ASSERT(expression) ((expression) ? (void)0 : ::assertion_failed(__FILE__, __LINE__))
#endif // !defined(_WINDOWS)
#define D(debug_only_code) (debug_only_code)
#else  // !defined(_DEBUG)
#define ASSERT(expression) ((void)0)
#define D(debug_only_code) ((void)0)
#endif // !defined(_DEBUG)

#define FAIL() ASSERT(false)

// forward declarations of project classes
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
class Partial;
class String;
class Strings;
class Tile;
class TileCell;
class Tiles;

#ifdef _GUI
class Canvas;
class Dialog;
class FileMenu;
class Graphics;
class HandBox;
class HintBox;
class Menu;
class MenuBar;
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
#endif  // defined(_GUI)

// project-wide utility functions
void        assertion_failed(const char *, unsigned);
bool        is_even(long);
bool        is_odd(long);
String      ordinal(unsigned);
void        pause(void);
const char *plural(unsigned);
String      plural(unsigned, const char *);
bool        str_eq(char const *, char const *);

// project-wide constants
#ifndef M_PI
const double   M_PI = 3.14159265358979323846;
#endif // !defined(M_PI)
const double   SQRT_3 = 1.732050807568877;
const unsigned SECONDS_PER_MINUTE = 60;

#endif // !defined(PROJECT_HPP_INCLUDED)
