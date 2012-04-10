#ifndef SUBMENU_HPP_INCLUDED
#define SUBMENU_HPP_INCLUDED

// File:    submenu.hpp
// Purpose: SubMenu class
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
A SubMenu object represents a popup menu such as the "Play" menu, the "Help" menu,
or the "Tile Size" submenu.
*/

#include "project.hpp"  // HASA Menu&

enum ThinkModeType {
	THINK_IDLE,
	THINK_SUGGEST,
	THINK_AUTOPLAY,
	THINK_CANCEL
};

#ifdef _QT
# include <QMenu>
class SubMenu: public QObject {
    Q_OBJECT
#elif defined(_WINDOWS)
class SubMenu {
#endif // defined(_WINDOWS)

public:
    // public lifecycle
#ifdef _QT
    SubMenu(QString const &label);
#elif defined(_WINDOWS)
    SubMenu(Menu const &, unsigned position);
#endif // defined(_WINDOWS)
    // no default constructor
    // ~SubMenu(void);  compiler-generated destructor is OK

    // misc public methods
#ifdef _QT
    void   Add(MenuItem &);
    void   AddSeparator(void);
#endif // !defined(_QT)
    void   Enable(bool);
#ifdef _QT
    QMenu *Qt(void);
#endif // !defined(_QT)

private:
    // private data
#ifdef _QT
    QMenu *mpQMenu;
#elif defined(_WINDOWS)
    Menu const &mrMenu;
    unsigned     mPosition;
#endif

    // private lifecycle
    SubMenu(SubMenu const &);  // not copyable

    // private operators
    SubMenu &operator=(SubMenu const &);  // not assignable
};
#endif // !defined(SUBMENU_HPP_INCLUDED)
