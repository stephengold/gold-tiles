#ifndef YESNO_HPP_INCLUDED
#define YESNO_HPP_INCLUDED

// File:    yesno.hpp
// Purpose: YesNo class
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
A YesNo object represents a dialog box for answering a yes/no question.

The YesNo class is an extension of the Dialog class.
*/

#include "gui/dialog.hpp"

class YesNo: public Dialog {
public:
	static const INT_PTR RESULT_YES = 3;
	static const INT_PTR RESULT_NO = 4;

    // lifecycle
	YesNo(char const *templateName); 
	// no default constructor
	// no copy constructor
	// ~YesNo(void);  compiler-generated destructor is OK

	// public operators
    YesNo &operator=(YesNo const &) { ASSERT(false); };

	// misc public methods
	INT_PTR HandleMessage(UINT message, WPARAM, LPARAM);
};

#endif
