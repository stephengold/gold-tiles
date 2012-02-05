#ifndef WINDOW_CLASS_HPP_INCLUDED
#define WINDOW_CLASS_HPP_INCLUDED

// File:    windowclass.hpp
// Purpose: WindowClass class for the Gold Tile game
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

/*
 A WindowClass object represents a Microsoft Windows "window class".
 Implemented by extending WNDCLASSEX.
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
