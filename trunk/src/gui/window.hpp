#ifndef WINDOW_HPP_INCLUDED
#define WINDOW_HPP_INCLUDED

// File:    window.hpp
// Purpose: window classes for the Gold Tile game
// Author:  Stephen Gold sgold@sonic.net

/*
 A Window object represents a generic window.

 The Window class is implemented by encapsulating an HWND.
 */

#include <map>
#include <windows.h>
#include "project.hpp"

typedef std::map<unsigned long, Window*> WMap;
typedef std::pair<unsigned long, Window*> WPair;

class Window {
	static WMap _map;
	
    HWND _handle;

    protected:
        unsigned _clientAreaWidth, _clientAreaHeight;
        HINSTANCE _module; // the module which owns this window

	    virtual char const *getClassName(void) const = 0;
        virtual WNDPROC getMessageHandler(void) const = 0;

    public:
		Window(void);
        Window(Window const &) { assert(false); };
        Window &operator=(Window const &) { assert(false); };
        // ~WindowClass(void);  implicitly declared destructor is fine

		static Window *lookup(HWND);

		void destroy(void);
		void forceRepaint(void);
		HWND getHandle(void) const;
	    char const *getName(void) const;
		LRESULT handleMessage(UINT message, WPARAM, LPARAM);
        HDC initialize(CREATESTRUCT const *);
	    void setClientArea(unsigned width, unsigned height);
	    void setHandle(HWND);
	    void setIcons(char const *iconResourceName);
		void show(int showHow);
};

#endif
