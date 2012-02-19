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

The Window class encapsulates an HWND (window handle).
It provides a static map for translating handles to Window objects. 
*/

#include "project.hpp"

#ifdef _WINDOWS
#include <map>
#include <windows.h>
#include "gui/rect.hpp"

class Window {
public:
	// lifecycle
	Window(void);
    Window(Window const &) { ASSERT(false); };
    // ~WindowClass(void);  compiler-generated destructor is OK
    HDC Initialize(CREATESTRUCT const *);

	// public operators
    Window &operator=(Window const &) { ASSERT(false); };

	// misc public methods
	HWND            Handle(void) const;
	LRESULT         HandleMessage(UINT message, WPARAM, LPARAM); 
    static Window * Lookup(HWND);
	void            SetHandle(HWND);
	void            Show(int showHow);

	// public inquiry methods
	bool IsMouseCaptured(void) const;

protected:
	static Window *mspNewlyCreatedWindow;

	// protected operators
    operator Rect(void) const;

	// protected misc methods
	void      CaptureMouse(void);
	void      Center(void);
	PCntType  ClientAreaHeight(void) const;
	PCntType  ClientAreaWidth(void) const;
	void      Close(void);
	HINSTANCE CopyModule(Window const &);
	void      ForceRepaint(void);
	HACCEL    GetAcceleratorTable(char const *resourceName);
	void      SelfDestruct(void);
	void      SetClientArea(PCntType width, PCntType height);
	void      SetIcons(char const *resourceName);
	void      SetTimer(unsigned msecs, unsigned id=1);

private:
    typedef unsigned long                 Key;
    typedef std::map<Key, Window*>        Map;
    typedef std::pair<Key, Window*>       Pair;
    typedef Map::iterator                 IteratorType;
	typedef Map::const_iterator           ConstIteratorType;
    typedef std::pair<IteratorType, bool> InsertResultType;

	static Map msMap;
	
    PCntType  mClientAreaWidth, mClientAreaHeight;
    HWND      mHandle;
    HINSTANCE mModule; // the module which owns this window  TODO static?
};

#endif
#endif
