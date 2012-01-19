#ifndef PROJECT_HPP
#define PROJECT_HPP

// File:    project.hpp
// Purpose: project-wide definitions for the Gold Tile game
// Author:  Stephen Gold sgold@sonic.net

// project-wide macros

// debugging macros
#define ASSERT(expression) ((expression) ? (void)0 : assertionFailed(__FILE__, __LINE__))
//#define ASSERT(expression) ((void)0)

//#define D(debug_only_code) (debug_only_code)
#define D(debug_only_code) ((void)0)

using namespace std;

#include <string>

// project-wide utility functions
void assertionFailed(const char *, unsigned);
int atoi(string const &);
string itoa(int);
void pause(void);
const char *plural(unsigned);
string plural(unsigned n, const char *);

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

#endif
