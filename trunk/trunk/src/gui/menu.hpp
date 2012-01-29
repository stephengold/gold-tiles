#ifndef MENU_HPP_INCLUDED
#define MENU_HPP_INCLUDED

// File:    menu.hpp
// Purpose: menu classes for the Gold Tile game
// Author:  Stephen Gold sgold@sonic.net

/*
 A MenuItem object represents 
 A SubMenu object represents
 A PlayMenu object represents
 A ViewMenu object represents
 
*/

#include <windows.h>
#include "project.hpp"

class MenuItem {
    UINT _itemID;
    HMENU _handle;

    public:
        MenuItem(HMENU, UINT);

		void check(bool);
        void enable(bool);
};

class SubMenu {
    HMENU _parent;
    UINT _position;

    public:
        SubMenu(HMENU parent, UINT position);

        void enable(bool);
};

class PlayMenu: public SubMenu {
    MenuItem _pause, _accept, _play, _pass, _resign;
    MenuItem _restart, _undo, _redo, _autopause;

    public:
        PlayMenu(HMENU menu, UINT position);

		void autopause(bool);
		void pause(bool);
};

class ViewMenu: public SubMenu {
    MenuItem _recenter, _animation, _attributes;
    MenuItem _showClocks, _showGrid, _showHints, _showScores, _showTiles;
    MenuItem _smallTiles, _mediumTiles, _largeTiles;

	void uncheckAllSizes(void);

    public:
        ViewMenu(HMENU menu, UINT position);
		void animation(bool);
		void showClocks(bool);
		void showGrid(bool);
		void showHints(bool);
		void showScores(bool);
		void showTiles(bool);
        void tileSize(UINT);
};

#endif
