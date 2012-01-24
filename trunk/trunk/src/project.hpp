#ifndef PROJECT_HPP_INCLUDED
#define PROJECT_HPP_INCLUDED

// File:    project.hpp
// Purpose: project-wide definitions for the Gold Tile game
// Author:  Stephen Gold sgold@sonic.net

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
//#define ASSERT(expression) ((void)0)
#endif

//#define D(debug_only_code) (debug_only_code)
#define D(debug_only_code) ((void)0)

#include <string>
using namespace std;

// forward declarations of project-wide classes
class BaseBoard;
class Board;
class Game;
class GridRef;
class Locus;
class Play;
class Player;
class Tile;
class Tiles;
class TileSquare;

#ifdef _GUI
class Canvas;
class MenuItem;
class PlayMenu;
class SubMenu;
class TopWindow;
class ViewMenu;
class Window;
class WindowClass;
#endif

// project-wide utility functions
void assertionFailed(const char *, unsigned);
int atoi(string const &);
string itoa(int);
void pause(void);
const char *plural(unsigned);
string plural(unsigned n, const char *);

// global pointers
#ifdef _GUI
extern Game *game;
extern TopWindow *topWindow;
#endif

#endif
