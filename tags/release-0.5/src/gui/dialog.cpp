// File:    dialog.cpp
// Purpose: Dialog class
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU General Public License

/*
This file is part of the Gold Tile game.

The Gold Tile game is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The Gold Tile game is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
for more details.

You should have received a copy of the GNU General Public License
along with the Gold Tile game.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>
#include "gui/dialog.hpp"
#include "gui/rect.hpp"

Dialog *gpNewlyCreatedDialog = NULL;

// lifecycle

Dialog::Dialog(char const *templateName, Window const &parent) {
	gpNewlyCreatedDialog = this;
	Initialize(templateName, parent, &dialogMessageHandler);
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

// misc

// methods
INT_PTR Dialog::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
	INT_PTR result = FALSE;
	HWND window = Handle();

    switch (message) {
        case WM_INITDIALOG:
		    CenterWindow();
            result = TRUE;
			break;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK:
                case IDCANCEL:
                    ::EndDialog(window, 0);
                    result = TRUE;
					break;
            }
            break;

	    default:
			break;
    }

    return result;
}

// access

INT_PTR Dialog::Result(void) const {
	INT_PTR result = mResult;

	return result;
}

// message handler (callback) for generic dialog

INT_PTR CALLBACK dialogMessageHandler(
	HWND window, 
	UINT message, 
	WPARAM wParameter, 
	LPARAM lParameter)
{
    Dialog *p_dialog;
	if (gpNewlyCreatedDialog != NULL) {
		p_dialog = gpNewlyCreatedDialog;
		gpNewlyCreatedDialog = NULL;
		p_dialog->SetHandle(window);
	} else {
       p_dialog = (Dialog *)Window::Lookup(window);
	}

	INT_PTR result;
    ASSERT(p_dialog->Handle() == window);
    result = p_dialog->HandleMessage(message, wParameter, lParameter);

	return result;
}

#endif