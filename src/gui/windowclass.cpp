// File:    windowclass.cpp
// Purpose: WindowClass class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>
#include "gui/windowclass.hpp"

WindowClass::WindowClass(
    HINSTANCE applicationInstance,
    WNDPROC messageHandler,
    char const *name)
{
    HINSTANCE nullInstance = NULL;
	cbClsExtra = 0;
	cbWndExtra = 0;
    cbSize = sizeof(WNDCLASSEX);
	hbrBackground = 0; // using double buffering, so no background brush
	hCursor = LoadCursor(nullInstance, IDC_ARROW);
	hIcon = 0;
	hIconSm = 0;
	hInstance = applicationInstance;
	lpfnWndProc = messageHandler;  // callback

	// The menu and the window class will share the same name.
	lpszClassName = name;
	lpszMenuName = name;

	style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
}

HINSTANCE WindowClass::getModule(void) const {
    HINSTANCE result = hInstance;
    
	return result;
}

char const *WindowClass::getName(void) const {
    char const *result = lpszClassName;
    
	return result;
}

void WindowClass::registerClass(void) {
    ATOM classAtom = ::RegisterClassEx(this);
	ASSERT(classAtom != 0);
};

#endif
