// File:    dialog.cpp
// Purpose: Dialog class
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

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>
#include "gui/dialog.hpp"

Dialog *gpNewlyCreatedDialog = NULL;

// message handler (callback) for generic dialog
static INT_PTR CALLBACK dialogMessageHandler(
	HWND windowHandle,
	UINT message,
	WPARAM wParameter, 
	LPARAM lParameter)
{
    Dialog *p_dialog;
	if (gpNewlyCreatedDialog != NULL) {
		p_dialog = gpNewlyCreatedDialog;
		gpNewlyCreatedDialog = NULL;
		p_dialog->SetHandle(windowHandle);
	} else {
       p_dialog = (Dialog *)Window::Lookup(windowHandle);
	}

	INT_PTR result;
    ASSERT(p_dialog->Handle() == windowHandle);
    result = p_dialog->HandleMessage(message, wParameter, lParameter);

	return result;
}

// lifecycle

Dialog::Dialog(char const *templateName, Window const &parent) {
	gpNewlyCreatedDialog = this;
	Initialize(templateName, parent, &dialogMessageHandler);
}

Dialog::Dialog(char const *templateName, Window const &parent, DLGPROC messageHandler) {
	gpNewlyCreatedDialog = this;
	Initialize(templateName, parent, messageHandler);
}

void Dialog::Initialize(
	char const *templateName,
	Window const &rParent,
	DLGPROC messageHandler)
{
	// create a modal dialog box
	HINSTANCE module = CopyModule(rParent);
	HWND parent = rParent.Handle();
	mResult = ::DialogBox(module, templateName, parent, messageHandler);
}

// misc methods

INT_PTR Dialog::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
	INT_PTR result = FALSE;
	HWND window = Handle();

    switch (message) {
        case WM_INITDIALOG:
		    Center();
            result = TRUE;
			break;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK:
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

INT_PTR Dialog::Result(void) const {
	INT_PTR result = mResult;

	return result;
}

#endif
