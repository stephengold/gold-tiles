// File:    menu.cpp
// Purpose: implement the MenuItem and SubMenu classes.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>
#include "gui/resource.hpp"
#include "gui/menu.hpp"

// MenuItem

MenuItem::MenuItem(HMENU handle, UINT itemID):
    _handle(handle),
	_itemID(itemID) {}

void MenuItem::check(bool checkedFlag) {
	if (checkedFlag) {
	    ::CheckMenuItem(_handle, _itemID, MF_CHECKED | MF_BYCOMMAND);
	} else {
	    ::CheckMenuItem(_handle, _itemID, MF_UNCHECKED | MF_BYCOMMAND);
	}
}

void MenuItem::enable(bool enabledFlag) {
	if (enabledFlag) {
    	::EnableMenuItem(_handle, _itemID, MF_ENABLED | MF_BYCOMMAND);
	} else {
    	::EnableMenuItem(_handle, _itemID, MF_GRAYED | MF_BYCOMMAND);
	}
}

// SubMenu

SubMenu::SubMenu(HMENU parent, UINT position):
	_parent(parent),
    _position(position)
 {}

void SubMenu::enable(bool enabledFlag) {
	if (enabledFlag) {
  	    ::EnableMenuItem(_parent, _position, MF_ENABLED | MF_BYPOSITION);
	} else {
		::EnableMenuItem(_parent, _position, MF_GRAYED | MF_BYPOSITION);
	}
}

// the "Play" submenu
PlayMenu::PlayMenu(HMENU menu, UINT position):	
	SubMenu(menu, position),
    _pause(menu, IDM_PAUSE),
    _accept(menu, IDM_ACCEPT),
    _play(menu, IDM_PLAY_PLAY),
    _pass(menu, IDM_PASS),
    _resign(menu, IDM_RESIGN),
    _restart(menu, IDM_RESTART),
    _undo(menu, IDM_UNDO),
    _redo(menu, IDM_REDO),
    _autopause(menu, IDM_AUTOPAUSE)
 {}

void PlayMenu::autopause(bool active) {
	_autopause.check(active);
}
void PlayMenu::pause(bool paused) {
	_pause.check(paused);
}
void PlayMenu::enableItems(bool paused, bool play) {
    _pause.enable(true);
    _accept.enable(false);
    _play.enable(!paused && play);
    _pass.enable(!paused && !play);
    _resign.enable(false);
    _restart.enable(false);
    _undo.enable(false);
    _redo.enable(false);
    _autopause.enable(!paused);
}

// the "View" submenu
ViewMenu::ViewMenu(HMENU menu, UINT position):
	SubMenu(menu, position),
    _smallTiles(menu, IDM_SMALL_TILES),
    _mediumTiles(menu, IDM_MEDIUM_TILES),
    _largeTiles(menu, IDM_LARGE_TILES),
    _recenter(menu, IDM_RECENTER),
    _attributes(menu, IDM_ATTRIBUTES),
    _showClocks(menu, IDM_SHOW_CLOCKS),
    _showGrid(menu, IDM_SHOW_GRID),
    _showHints(menu, IDM_SHOW_HINTS),
    _showScores(menu, IDM_SHOW_SCORES),
    _showTiles(menu, IDM_SHOW_TILES),
    _animation(menu, IDM_ANIMATION)
{}

void ViewMenu::showClocks(bool shown) {
	_showClocks.check(shown);
}
void ViewMenu::showGrid(bool shown) {
	_showGrid.check(shown);
}
void ViewMenu::showHints(bool shown) {
	_showHints.check(shown);
}
void ViewMenu::showScores(bool shown) {
	_showScores.check(shown);
}
void ViewMenu::showTiles(bool shown) {
	_showTiles.check(shown);
}
void ViewMenu::tileSize(UINT position) {
	uncheckAllSizes();
	switch (position) {
 	    case IDM_SMALL_TILES:
	        _smallTiles.check(true);
			break;
		case IDM_MEDIUM_TILES:
            _mediumTiles.check(true);
			break;
		case IDM_LARGE_TILES:
        	_largeTiles.check(true);
			break;
		default:
			assert(false);
	}
}
void ViewMenu::uncheckAllSizes(void) {
	_smallTiles.check(false);
    _mediumTiles.check(false);
    _largeTiles.check(false);
}
#endif
