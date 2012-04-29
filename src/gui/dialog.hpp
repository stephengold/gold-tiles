#ifndef DIALOG_HPP_INCLUDED
#define DIALOG_HPP_INCLUDED

// File:     dialog.hpp
// Location: src/gui
// Purpose:  declare Dialog class
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

/*
A Dialog object represents a generic dialog box.
It mimics a message box when its Run() method is invoked.

The Dialog class is implemented as an extension of the BaseWindow class.
It meant to be futher extended by adding controls.  
*/

#include "address.hpp"
#include "gui/basewindow.hpp"  // ISA BaseWindow


class Dialog: public BaseWindow {
public:
	// public types
    typedef long ValueType; // value of a control

	// public constants
	static const int       RESULT_OK = 1;
	static const int       RESULT_CANCEL = 2;
    static const int       RESULT_BACK = 5;
	static const ValueType VALUE_INVALID = LONG_MAX;

	// public lifecycle
	// no default constructor
	explicit Dialog(TextType templateName);
	Dialog(TextType templateName, Win::DLGPROC);
    // ~Dialog(void);  compiler-generated destructor is OK

	// misc public methods
	int HandleMessage(MessageType, Win::WPARAM);
	int Run(BaseWindow* pParent);

protected:
	// misc protected methods
	ValueType AddListboxItem(IdType, TextType);
    void      Close(int);
	void      EnableControl(IdType, bool);
	Address   GetAddress(IdType);
	Win::HWND GetControlHandle(IdType) const;
	ValueType GetListboxSelection(IdType);
    ValueType GetSliderValue(IdType);
	String    GetTextString(IdType);
    ValueType GetTextValue(IdType);
	void      SetAddress(IdType, Address const&);
	void      SetButton(IdType, bool);
	void      SetListboxSelection(IdType, ValueType);
    ValueType SetSliderValue(IdType, ValueType);
	void      SetSliderRange(IdType, ValueType min, ValueType max);
	void      SetTextString(IdType, String const&);
    void      SetTextValue(IdType, ValueType);

private:
	// private data
	Win::DLGPROC mpMessageHandler;
	TextType      mTemplateName;

	// private lifecycle
    Dialog(Dialog const&);  // not copyable

	// private operators
    Dialog& operator=(Dialog const&);  // not assignable

	// private methods
	TextType Name(void) const;
};
#endif // !defined(DIALOG_HPP_INCLUDED)
