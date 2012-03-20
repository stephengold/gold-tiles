// File:    project.cpp
// Purpose: project-wide utility functions for the Gold Tile Game
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU General Public License

/*
This file is part of the Gold Tile Game.

The Gold Tile Game is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The Gold Tile Game is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
for more details.

You should have received a copy of the GNU General Public License
along with the Gold Tile Game.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include "project.hpp"
#include "string.hpp"

#ifdef WIN32
#include "gui/win_types.hpp"
#else // !defined(WIN32)
#include <time.h>
#endif // !defined(WIN32)

void assertion_failed(const char *file, unsigned line) {
    std::cout << "Assertion failed at line " << line << " in " << file << std::endl;
    ::pause();
    ::exit(EXIT_FAILURE);
}

bool is_even(long number) {
	bool result = ((number & 0x1) == 0);

	return result;
}

bool is_odd(long number) {
	bool result = ((number & 0x1) == 0x1);

	return result;
}

long milliseconds(void) {
	long result;
#ifdef WIN32
	Win::DWORD ticks = Win::GetTickCount();
    result = long(ticks);
#else // !defined(WIN32)
	time_t seconds = ::time(NULL);
	result = MSECS_PER_SECOND*long(seconds);
#endif // !defined(WIN32)

	return result;
}

String ordinal(unsigned n) {
    String result(n);

	unsigned ones_place = n % 10;
	unsigned tens_place = (n / 10) % 10;

	if (tens_place == 1) {
		result += "th";
	} else if (ones_place == 1) {
		result += "st";
	} else if (ones_place == 2) {
		result += "nd";
	} else if (ones_place == 3) {
		result += "rd";
	} else {
		result += "th";
	}

	return result;
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
    String result = String(n);
	result += " ";
	result += noun;
	result += plural(n);

    return result;
}

bool str_eq(char const *string1, char const *string2) {
	bool result = (::strcmp(string1, string2) == 0);

	return result;
}