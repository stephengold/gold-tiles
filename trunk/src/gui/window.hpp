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

The Window class encapsulates an HWND (window handle) and implements
a static map for translating handles to Window objects. 
*/

#include <map>
#include "gui/rect.hpp"    // PCntType

typedef Win::UINT MessageType;

class Window {
public:
	// lifecycle
	Window(void);
    Window(Window const &) { FAIL(); };
    // ~WindowClass(void);  compiler-generated destructor is OK

	// public operators
    Window &operator=(Window const &) { FAIL(); };

	// misc public methods
	Win::HWND       Handle(void) const;
	Win::LRESULT    HandleMessage(MessageType, Win::WPARAM, Win::LPARAM); 
    static Window * Lookup(Win::HWND);
	void            Show(int showHow);

protected:
	static Window *mspNewlyCreatedWindow;

	// protected lifecycle
    void Initialize(Win::CREATESTRUCT const &);

	// protected operators
    operator       Rect(void) const;

	// protected misc methods
	void           CaptureMouse(void);
	void           Center(void);
	PCntType       ClientAreaHeight(void) const;
	PCntType       ClientAreaWidth(void) const;
	void           Close(void);
	Win::HINSTANCE CopyModule(Window const &);
	void           ForceRepaint(void);
	Win::HACCEL    GetAcceleratorTable(char const *resourceName);
	Win::HMENU     GetMenu(char const *resourceName);
	bool           IsMouseCaptured(void) const;
	void           SelfDestruct(void);
	void           SetClientArea(PCntType width, PCntType height);
	void           SetCursor(Win::LPSTR);
	void           SetHandle(Win::HWND);
	void           SetIcons(char const *resourceName);
	void           SetTimer(unsigned msecs, unsigned id);
	void           UpdateMenuBar(void);

private:
    typedef unsigned long                 Key;
    typedef std::map<Key, Window*>        Map;
    typedef std::pair<Key, Window*>       Pair;
	typedef Map::const_iterator           ConstIteratorType;
    typedef Map::iterator                 IteratorType;
    typedef std::pair<IteratorType, bool> InsertResultType;

	static Map msMap;
	
    PCntType       mClientAreaWidth, mClientAreaHeight;
    Win::HWND      mHandle;
    Win::HINSTANCE mModule; // the module/instance which owns this window  TODO static?
};

#endif
