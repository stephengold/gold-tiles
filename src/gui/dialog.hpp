#ifndef DIALOG_HPP_INCLUDED
#define DIALOG_HPP_INCLUDED

// File:    dialog.hpp
// Purpose: Dialog class for the Gold Tile game
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

/*
 A Dialog object represents a generic Microsoft Windows dialog box.

 The class is implemented by extending the Window class.
*/

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>
#include "window.hpp"

class Dialog: public Window {
public:
	// lifecycle
	Dialog(void);
    Dialog(Dialog const &) { ASSERT(false); };
    // ~Dialog(void);  implicitly declared destructor is OK
	void Initialize(char const *templateName, Window const &parent, DLGPROC);

	// operator
    Dialog &operator=(Dialog const &) { ASSERT(false); };

	// access
	INT_PTR Result(void) const;

private:
	INT_PTR mResult;
};

#endif
#endif