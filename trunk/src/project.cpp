// File:    project.cpp
// Purpose: project-wide utility functions for the Gold Tile game
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include <iostream>
#include <sstream>
#include "project.hpp"
#include "string.hpp"

void assertionFailed(const char *file, unsigned line) {
    std::cout << "Assertion failed at line " << line << " in " << file << std::endl;
    pause();
    ::exit(EXIT_FAILURE);
}

void pause(void) {
    ::system("PAUSE");
}

const char *plural(unsigned n) {
    char *result = "s";
    if (n == 1) {
        result = "";
    }
    
    return result; 
}

String plural(unsigned n, const char *noun) {
    std::ostringstream sout;

    sout << n << " " << noun << plural(n);
    String result = sout.str();

    return result;
}

