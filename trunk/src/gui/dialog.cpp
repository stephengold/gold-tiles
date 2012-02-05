// File:    dialog.cpp
// Purpose: Dialog class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>
#include "gui/dialog.hpp"

// lifecycle

Dialog::Dialog(void) {
}

void Dialog::Initialize(
	char const *templateName,
	Window const &rParent,
	DLGPROC messageHandler)
{
	HINSTANCE module = CopyModule(rParent);
	HWND parent = rParent.Handle();
	mResult = ::DialogBox(module, templateName, parent, messageHandler);
}

// access

INT_PTR Dialog::Result(void) const {
	INT_PTR result = mResult;

	return result;
}

#endif