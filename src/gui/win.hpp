#ifndef WIN_HPP_INCLUDED
#define WIN_HPP_INCLUDED

// File:     win.hpp
// Location: src/gui
// Purpose:  Win namespace
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

#ifndef WIN32
# error  win.hpp should be included only when the target platform is Microsoft Windows
#endif // !defined(WIN32)

namespace Win {
#define _M_CEE_SAFE
#include <Windows.h>
};

typedef Win::UINT IdType;  // resource ID

#endif // !defined(WIN_HPP_INCLUDED)
