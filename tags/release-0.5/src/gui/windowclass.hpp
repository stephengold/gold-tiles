#ifndef WINDOW_CLASS_HPP_INCLUDED
#define WINDOW_CLASS_HPP_INCLUDED

// File:    windowclass.hpp
// Purpose: WindowClass class
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU General Public License

/*
This file is part of the Gold Tile game.

The Gold Tile game is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The Gold Tile game is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
for more details.

You should have received a copy of the GNU General Public License
along with the Gold Tile game.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
A WindowClass object represents a Microsoft Windows "window class".
The WindowClass class extends the WNDCLASSEX struct.
*/

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>

class WindowClass: public WNDCLASSEX {
public:
	// lifecycle
    WindowClass(void) { ASSERT(false); };
    WindowClass(HINSTANCE, WNDPROC, char const *);
    WindowClass(WindowClass const &) { ASSERT(false); };
    // ~WindowClass(void);  implicitly declared destructor is fine

	// operator
    WindowClass &operator=(WindowClass const &) { ASSERT(false); };

	// misc
    void RegisterClass(void);

	// access
    HINSTANCE   Module(void) const;
	char const *Name(void) const;
};

#endif
#endif
