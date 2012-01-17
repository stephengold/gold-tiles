#ifndef PROJECT_HPP
#define PROJECT_HPP

// File:    project.hpp
// Purpose: Project-wide definitions for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

using namespace std;

#define ASSERT(e) ((e) ? (void)0 : assertionFailed(__FILE__, __LINE__))

#include <string>

void assertionFailed(const char *, unsigned);
void pause(void);
const char *plural(unsigned);
string plural(unsigned n, const char *);

#endif
