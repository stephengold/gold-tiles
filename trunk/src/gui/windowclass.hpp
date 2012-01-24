#ifndef WINDOW_CLASS_HPP_INCLUDED
#define WINDOW_CLASS_HPP_INCLUDED

// File:    windowclass.hpp
// Purpose: WindowClass class for the Gold Tile game
// Author:  Stephen Gold sgold@sonic.net

/*
 A WindowClass object represents a Microsoft "window class".
 Implemented by encapsulating WNDCLASSEX.
 */

#include <windows.h>
#include "project.hpp"

class WindowClass: public WNDCLASSEX {
    public:
        WindowClass(void) { assert(false); };
        WindowClass(WindowClass const &) { assert(false); };
        WindowClass &operator=(WindowClass const &) { assert(false); };
        // ~WindowClass(void);  implicitly declared destructor is fine

		WindowClass(HINSTANCE, WNDPROC, char const *);

		HINSTANCE getModule(void) const;
		char const *getName(void) const;
        void registerClass(void);
};

#endif
