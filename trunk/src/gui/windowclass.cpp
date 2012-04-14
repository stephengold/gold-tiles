// File:     windowclass.cpp
// Location: src/gui
// Purpose:  implement WindowClass class
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

#ifdef _WINDOWS
#include "gui/win_types.hpp"
#include "gui/windowclass.hpp"
#include "project.hpp"  // ASSERT

// lifecycle

WindowClass::WindowClass(
    HINSTANCE applicationInstance,
    WNDPROC messageHandler,
    TextType className,
	TextType menuName)
{
	cbClsExtra = 0;
	cbWndExtra = 0;
    cbSize = sizeof(WNDCLASSEX);
	hbrBackground = 0; // using double buffering, so no background brush
    HINSTANCE nullInstance = NULL;
	hCursor = Win::LoadCursor(nullInstance, IDC_ARROW);
	hIcon = 0;
	hIconSm = 0;
	hInstance = applicationInstance;
	lpfnWndProc = messageHandler;  // callback

	lpszClassName = className;
	lpszMenuName = menuName;

	style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
}


// misc methods

HINSTANCE WindowClass::Module(void) const {
    return hInstance;
}

TextType WindowClass::Name(void) const {
    return lpszClassName;
}

void WindowClass::RegisterClass(void) {
    ATOM const classAtom = Win::RegisterClassEx(this);
	ASSERT(classAtom != 0);
};

#endif // defined(_WINDOWS)
