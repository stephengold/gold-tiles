#ifndef WINDOW_HPP_INCLUDED
#define WINDOW_HPP_INCLUDED

// File:    window.hpp
// Purpose: Window class
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
A Window object represents a generic Microsoft Windows window.

The Window class encapsulates an HWND.
*/

#include "project.hpp"

#ifdef _WINDOWS
#include <map>
#include <windows.h>

class Window {
public:
	// lifecycle
	Window(void);
    Window(Window const &) { ASSERT(false); };
    // ~WindowClass(void);  implicitly declared destructor is OK
    HDC Initialize(CREATESTRUCT const *);

	// operator
    Window &operator=(Window const &) { ASSERT(false); };
    operator Rect(void) const;

	// misc
	void            CaptureMouse(void);
	void            CenterWindow(void);
	unsigned        ClientAreaHeight(void) const;
	unsigned        ClientAreaWidth(void) const;
	HINSTANCE       CopyModule(Window const &);
	void            ForceRepaint(void);
	HACCEL          GetAcceleratorTable(char const *resourceName);
	HWND            Handle(void) const;
	LRESULT         HandleMessage(UINT message, WPARAM, LPARAM); 
    static Window * Lookup(HWND);
	void            SelfDestruct(void);
	void            SetClientArea(unsigned width, unsigned height);
	void            SetHandle(HWND);
	void            SetIcons(char const *resourceName);
	void            Show(int showHow);

	// inquiry
	bool IsMouseCaptured(void) const;

private:
    typedef std::map<unsigned long, Window*> Map;
    typedef std::pair<unsigned long, Window*> Pair;

	static Map msMap;
	
    unsigned  mClientAreaWidth, mClientAreaHeight;
    HWND      mHandle;
    HINSTANCE mModule; // the module which owns this window
};

#endif
#endif
