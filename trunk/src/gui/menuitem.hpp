#ifndef MENUITEM_HPP_INCLUDED
#define MENUITEM_HPP_INCLUDED

// File:     menuitem.hpp
// Location: src/gui
// Purpose:  declare MenuItem class
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
A MenuItem object represents a command item in a SubMenu.

In the Qt version, the MenuItem class encapsulates a pointer to a QAction.
In the native version, the MenuItem class encapsulates a handle and a resource id.
*/

#include "project.hpp"

#ifdef _QT
# include <QAction>
#elif defined(_WINDOWS)
# include "gui/win.hpp"  // HASA IdType
#endif // defined(_WINDOWS)

#ifdef _QT
class MenuItem: public QObject {
    Q_OBJECT
#elif defined(_WINDOWS)
class MenuItem {
#endif // defined(_WINDOWS)
public:
    // public lifecycle
    // no default constructor
#ifdef _QT
    MenuItem(QObject* parent, QString const& label);
#elif defined(_WINDOWS)
    MenuItem(Menu const&, IdType);
    MenuItem(Menu const&, IdType first, IdType last, IdType item);
#endif // defined(_WINDOWS)
    // ~MenuItem(void);  compiler-generated destructor is OK

    // misc public methods
#ifdef _QT
    QAction* pAction(void);
#endif // defined(_QT)
    void      Check(bool);
    void      CheckRadio(bool);
    void      Enable(bool);
#ifdef _QT
    void      SetShortcut(QString const&);
#endif // defined(_QT)

private:
    // private data
#ifdef _QT
    QAction* mpAction;
#elif defined(_WINDOWS)
    IdType    mFirstItemId;
    IdType    mItemId;
    IdType    mLastItemId;
    Menu const&
             mrMenu;
#endif // defined(_WINDOWS)

    // private lifecycle
    MenuItem(MenuItem const&);   // not copyable

    // private operators
    MenuItem& operator=(MenuItem const&);   // not assignable
};
#endif // !defined(MENUITEM_HPP_INCLUDED)
