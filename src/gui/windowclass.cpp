// File:    windowclass.cpp
// Purpose: WindowClass class
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

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>
#include "gui/windowclass.hpp"

// lifecycle

WindowClass::WindowClass(
    HINSTANCE applicationInstance,
    WNDPROC messageHandler,
    char const *name)
{
	cbClsExtra = 0;
	cbWndExtra = 0;
    cbSize = sizeof(WNDCLASSEX);
	hbrBackground = 0; // using double buffering, so no background brush
    HINSTANCE nullInstance = NULL;
	hCursor = ::LoadCursor(nullInstance, IDC_ARROW);
	hIcon = 0;
	hIconSm = 0;
	hInstance = applicationInstance;
	lpfnWndProc = messageHandler;  // callback

	// The menu and the window class will share the same name.
	lpszClassName = name;
	lpszMenuName = name;

	style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
}


// misc methods

HINSTANCE WindowClass::Module(void) const {
    HINSTANCE result = hInstance;
    
	return result;
}

char const *WindowClass::Name(void) const {
    char const *result = lpszClassName;
    
	return result;
}

void WindowClass::RegisterClass(void) {
    ATOM classAtom = ::RegisterClassEx(this);
	ASSERT(classAtom != 0);
};

#endif
