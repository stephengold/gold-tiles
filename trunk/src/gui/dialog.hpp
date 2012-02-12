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
    typedef int      IdType;    // id of a control
    typedef unsigned ValueType; // value of a control

	static const ValueType VALUE_INVALID = 9099;
	static const INT_PTR   RESULT_OK = 1;
	static const INT_PTR   RESULT_CANCEL = 2;

	// lifecycle
	Dialog(char const *templateName);
	Dialog(char const *templateName, DLGPROC);
	// no default constructor
	// no copy constructor
    Dialog(Dialog const &) { ASSERT(false); };
    // ~Dialog(void);  compiler-generated destructor is OK
	INT_PTR Run(Window *pParent);

	// operators
    Dialog &operator=(Dialog const &) { ASSERT(false); };
	
	// misc public methods
	INT_PTR HandleMessage(UINT message, WPARAM, LPARAM);

protected:
	// misc protected methods
    void      Close(INT_PTR);
	void      EnableButton(IdType, bool);
	HWND      GetControlHandle(IdType) const;
    ValueType GetSliderValue(IdType);
	String    GetTextString(IdType);
    ValueType GetTextValue(IdType);
	void      SetButton(IdType, bool);
    ValueType SetSliderValue(IdType, ValueType);
	void      SetSliderRange(IdType, ValueType min, ValueType max);
	void      SetTextString(IdType, String const &);
    void      SetTextValue(IdType, ValueType);

private:
	DLGPROC     mpMessageHandler;
	char const * mTemplateName;
};

// globals

extern Dialog *gpNewlyCreatedDialog;

#endif
#endif
