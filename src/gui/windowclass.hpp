#ifndef WINDOW_CLASS_HPP_INCLUDED
#define WINDOW_CLASS_HPP_INCLUDED

// File:    windowclass.hpp
// Purpose: WindowClass class
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

/*
A WindowClass object represents a Microsoft Windows "window class".

The WindowClass class extends the Win::WNDCLASSEX structure.
*/

#ifdef _WINDOWS
#include "gui/win.hpp"  // ISA Win::WNDCLASSEX

class WindowClass: public Win::WNDCLASSEX {
public:
	// public lifecycle
    WindowClass(Win::HINSTANCE, Win::WNDPROC, char const *className, 
		char const *menuName);
	// no default constructor
    // ~WindowClass(void);  compiler-generated destructor is fine

	// misc public methods
    Win::HINSTANCE Module(void) const;
	char const *   Name(void) const;
    void           RegisterClass(void);

private:
	// private lifecycle
	WindowClass(WindowClass const &); // not copyable

	// private operators
    WindowClass &operator=(WindowClass const &); // not assignable
};
#endif // defined(_WINDOWS)
#endif // !defined(WINDOWCLASS_HPP_INCLUDED)


