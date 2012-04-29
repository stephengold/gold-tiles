#ifndef PROJECT_HPP_INCLUDED
#define PROJECT_HPP_INCLUDED

// File:     project.hpp
// Location: src
// Purpose:  project-wide definitions for the Gold Tile Game
// Author:   Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU General Public License


// platform, UI, and network configuration macros
#ifdef WIN32
# define _WINSOCK2
#endif // defined(WIN32)

#ifdef _QT
# define _GUI

#elif defined(_WINDOWS)
# define _GUI
# define Q_OBJECT
# define _SERVER
# define _CLIENT

#elif !defined(_CONSOLE)
# define _CONSOLE

# if defined(_CLIENT) && defined(_SERVER)
#  error console build cannot act as both a client and a server
# endif

#endif // !defined(_WINDOWS) && !defined(_QT) && !defined(_CONSOLE)

#if !defined(_CLIENT) && !defined(_SERVER)
#  error build should include either client or server support
#endif

// debugging macros: D(), ASSERT(), and FAIL()
#ifdef _DEBUG
#define D(debug_only_code) (debug_only_code)
#else  // !defined(_DEBUG)
#define D(debug_only_code) (void(0))
#endif // !defined(_DEBUG)

#ifdef _GUI
#include <assert.h>
#define ASSERT(expression) D(assert(expression))
#else // !defined(_GUI)
#define ASSERT(expression) D((expression) ? void(0) : ::assertion_failed(__FILE__, __LINE__))
#endif // !defined(_GUI)

#define FAIL() ASSERT(false)

// forward declarations of project classes
class Address;
class BaseBoard;
class Board;
class Cell;
class Cells;
class Combo;
class Direction;
class Fifo;
class Game;
class GameOpt;
class Hand;
class HandOpt;
class HandOpts;
class Hands;
class Indices;
class Move;
class Network;
class Partial;
class Socket;
class String;
class Strings;
class Tile;
class TileCell;
class Tiles;
class Turn;
class Turns;

#ifdef _GUI
// forward declarations of GUI classes
class AttrBox;
class BaseWindow;
class Canvas;
class Dialog;
class DisplayModes;
class FileMenu;
class FractionPair;
class GameView;
class GameWindow;
class Graphics;
class HandBox;
class HintBox;
class Markings;
class Menu;
class MenuBar;
class MenuItem;
class ParmBox1;
class ParmBox2;
class ParmBox3;
class Player;
class PlayMenu;
class Point;
class Poly;
class Rect;
class SizeMenu;
class SubMenu;
class TileBox;
class ViewMenu;
class Window;
class WindowClass;
#endif // defined(_GUI)

// project-wide typedefs
typedef unsigned long MsecIntervalType;
typedef unsigned      ScoreType;
typedef char const*   TextType;   // read-only, NUL-terminated string of characters

// project-wide constants
#ifndef NULL
#define NULL 0
#endif // !defined(NULL)
#ifndef M_PI
const double   M_PI = 3.14159265358979323846;
#endif // !defined(M_PI)
const MsecIntervalType 
               MSECS_PER_SECOND = 1000;
const unsigned SECONDS_PER_MINUTE = 60;
const double   SQRT_3 = 1.732050807568877;

// project-wide utility functions
void     assertion_failed(TextType, unsigned);
bool     is_even(long);
bool     is_odd(long);
MsecIntervalType
         milliseconds(void);  // read real-time clock
String   ordinal(unsigned);
void     pause(void);
TextType plural(unsigned);
String   plural(unsigned, TextType);
bool     random_bool(double trueProb);
bool     str_eq(TextType, TextType);  // compare text strings

#endif // !defined(PROJECT_HPP_INCLUDED)
