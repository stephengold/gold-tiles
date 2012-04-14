#ifndef BASEWINDOW_HPP_INCLUDED
#define BASEWINDOW_HPP_INCLUDED

// File:     basewindow.hpp
// Location: src/gui
// Purpose:  declare BaseWindow class
// Author:   Stephen Gold sgold@sonic.net
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
A BaseWindow object represents a generic GUI window.

The BaseWindow class encapsulates an HWND (window handle) and implements
a static map for translating handles to BaseWindow objects. 
*/

#include <map>          // HASA std::map
#include "project.hpp"
#include "gui/win.hpp"

typedef Win::UINT MessageType;

class BaseWindow {
public:
	// public lifecycle
	BaseWindow(void);
    // ~BaseWindow(void);  compiler-generated destructor is OK

	// public operators
	operator Win::HWND(void) const;

	// misc public methods
    static BaseWindow* Lookup(Win::HWND);

protected:
	// protected data
	static BaseWindow* mspNewlyCreatedWindow;

	// protected lifecycle
    void Initialize(Win::CREATESTRUCT const&);

	// misc protected methods
	void           Center(void);
	Win::HINSTANCE CopyModule(BaseWindow const&);
	void           Create(void);
	static Rect    DesktopBounds(void);
	virtual TextType  
		           Name(void) const = 0;
	void           SetHandle(Win::HWND);

private:
	// private types
    typedef unsigned long                   KeyType;
    typedef std::map<KeyType, BaseWindow*>  Map;
    typedef std::pair<KeyType, BaseWindow*> Pair;
	typedef Map::const_iterator             ConstIterator;
    typedef Map::iterator                   Iterator;
    typedef std::pair<Iterator, bool>       InsertResult;

	// private data
    Win::HWND      mHandle;
	static Map    msMap;
    Win::HINSTANCE mModule; // the module/instance which owns this window  TODO static?

	// private lifecycle
    BaseWindow(BaseWindow const&); // not copyable

	// private operators
    BaseWindow& operator=(BaseWindow const&); // not assignable
};
#endif // !defined(BASEWINDOW_HPP_INCLUDED)
