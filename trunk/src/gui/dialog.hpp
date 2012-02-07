#ifndef DIALOG_HPP_INCLUDED
#define DIALOG_HPP_INCLUDED

// File:    dialog.hpp
// Purpose: Dialog class
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
A Dialog object represents a generic Microsoft Windows dialog box.

The Dialog class is an extension of the Window class.
*/

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>
#include "window.hpp"

class Dialog: public Window {
public:
	// lifecycle
	Dialog(void) { ASSERT(false); };
	Dialog(char const *templateName, Window const &parent);
    Dialog(Dialog const &) { ASSERT(false); };
    // ~Dialog(void);  implicitly declared destructor is OK
	void Initialize(char const *templateName, Window const &parent, DLGPROC);

	// operator
    Dialog &operator=(Dialog const &) { ASSERT(false); };
	
	// misc
	INT_PTR HandleMessage(UINT message, WPARAM, LPARAM);

	// access
	INT_PTR Result(void) const;

private:
	INT_PTR mResult;
};

extern Dialog *gpNewlyCreatedDialog;

INT_PTR CALLBACK dialogMessageHandler(HWND, UINT message, WPARAM, LPARAM);

#endif
#endif