#ifndef ABOUTDIALOG_HPP_INCLUDED
#define ABOUTDIALOG_HPP_INCLUDED

// File:    aboutdialog.hpp
// Purpose: AboutDialog class for the Gold Tile game
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

/*
 An AboutDialog object represents a the dialog box for 
 the "About" menu item.

 The class is implemented by extending the Dialog class.
*/

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>
#include "gui/dialog.hpp"

class AboutDialog: public Dialog {
public:
	// lifecycle
	AboutDialog(void) { ASSERT(false); };
	AboutDialog(Window const &parent);
    AboutDialog(AboutDialog const &) { ASSERT(false); };
    // ~AboutDialog(void);  implicitly declared destructor is OK

	// operator
    AboutDialog &operator=(AboutDialog const &) { ASSERT(false); };

	// misc
	INT_PTR HandleMessage(UINT message, WPARAM, LPARAM);
};

#endif
#endif