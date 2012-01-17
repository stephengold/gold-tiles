// File:    project.cpp
// Purpose: project-wide utility functions for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "project.hpp"
#include <iostream>
#include <sstream>

void assertionFailed(const char *file, unsigned line) {
    cout << "Assertion failed at line " << line << " in " << file << endl;
    pause();
    exit(EXIT_FAILURE);
}

void pause(void) {
    system("PAUSE");
}

const char *plural(unsigned n) {
    char *result = "s";
    if (n == 1) {
        result = "";
    }
    
    return result; 
}

string plural(unsigned n, const char *noun) {
    ostringstream sout;

    sout << n << " " << noun << plural(n);
    string result = sout.str();

    return result;
}
