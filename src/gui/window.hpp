#ifndef WINDOW_HPP_INCLUDED
#define WINDOW_HPP_INCLUDED

// File:    window.hpp
// Purpose: window classes for the Gold Tile game
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

	// operator
    Window &operator=(Window const &) { ASSERT(false); };

	// misc
	void           ForceRepaint(void);
	LRESULT        HandleMessage(UINT message, WPARAM, LPARAM);
    HDC            Initialize(CREATESTRUCT const *);
    static Window *Lookup(HWND);
	void           SelfDestruct(void);
	void           Show(int showHow);

	// access
	char const *Name(void) const;
	HWND        Handle(void) const;
	void        SetClientArea(unsigned width, unsigned height);
	void        SetHandle(HWND);
	void        SetIcons(char const *iconResourceName);

protected:
    unsigned mClientAreaWidth, mClientAreaHeight;
    HINSTANCE mModule; // the module which owns this window

	// access
    virtual char const *ClassName(void) const = 0;
    virtual WNDPROC     MessageHandler(void) const = 0;

private:
    typedef std::map<unsigned long, Window*> Map;
    typedef std::pair<unsigned long, Window*> Pair;

	static Map msMap;
	
    HWND mHandle;
};

#endif
#endif
