#ifndef WINDOW_HPP_INCLUDED
#define WINDOW_HPP_INCLUDED

// File:    window.hpp
// Purpose: Window class for the Gold Tile game
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

/*
 A Window object represents a generic Microsoft Windows window.

 The Window class is implemented by encapsulating an HWND.
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
	HINSTANCE CopyModule(Window const &);
	void      ForceRepaint(void);
	LRESULT   HandleMessage(UINT message, WPARAM, LPARAM);
	void      SelfDestruct(void);
	void      Show(int showHow);

	// access
	unsigned       ClientAreaHeight(void) const;
	unsigned       ClientAreaWidth(void) const;
	HWND           Handle(void) const;
    static Window *Lookup(HWND);
	void           SetClientArea(unsigned width, unsigned height);
	void           SetHandle(HWND);
	void           SetIcons(char const *iconResourceName);

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
