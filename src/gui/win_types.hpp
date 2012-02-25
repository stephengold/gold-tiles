#ifndef WIN_TYPES_HPP_INCLUDED
#define WIN_TYPES_HPP_INCLUDED

// File:    win_types.hpp
// Purpose: types needed to access Microsoft Windows APIs
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

// This header file is meant to be #included only in .cpp files, not in
// other header files.

using Win::ATOM;
using Win::BOOL;
using Win::CREATESTRUCT;
using Win::DLGPROC;
using Win::DWORD;
using Win::DWORD_PTR;
using Win::HACCEL;
using Win::HBRUSH;
using Win::HCURSOR;
using Win::HDC;
using Win::HGDIOBJ;
using Win::HICON;
using Win::HINSTANCE;
using Win::HMENU;
using Win::HPEN;
using Win::HWND;
using Win::INT_PTR;
using Win::LPCTSTR;
using Win::LPARAM;
using Win::LPSTR;
using Win::LPVOID;
using Win::LRESULT;
using Win::MSG;
using Win::PAINTSTRUCT;
using Win::POINT;
using Win::POINTS;
using Win::RECT;
using Win::SHORT;
using Win::SIZE;
using Win::TIMERPROC;
using Win::UINT;
using Win::ULONG_PTR;
using Win::WNDCLASSEX;
using Win::WNDPROC;
using Win::WORD;
using Win::WPARAM;

#endif
