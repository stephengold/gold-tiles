#ifndef PROJECT_HPP
#define PROJECT_HPP

// File:    project.hpp
// Purpose: project-wide definitions for the Gold Tile game
// Author:  Stephen Gold sgold@sonic.net

// project-wide macros
#define ASSERT(e) ((e) ? (void)0 : assertionFailed(__FILE__, __LINE__))
//#define ASSERT(e) ((void)0)

//#define D(e) (e)
#define D(e) ((void)0)

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
