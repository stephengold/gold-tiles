// File:    parmbox.cpp
// Purpose: ParmBox class
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
#include "gui/parmbox.hpp"

// message handler (callback) for this dialog
static INT_PTR CALLBACK parmBoxMessageHandler(
	HWND windowHandle,
	UINT message,
	WPARAM wParameter, 
	LPARAM lParameter)
{
    ParmBox *p_box;
	if (gpNewlyCreatedDialog != NULL) {
		p_box = (ParmBox *)gpNewlyCreatedDialog;
		gpNewlyCreatedDialog = NULL;
		p_box->SetHandle(windowHandle);
	} else {
       p_box = (ParmBox *)Window::Lookup(windowHandle);
	}

	INT_PTR result;
    ASSERT(p_box->Handle() == windowHandle);
    result = p_box->HandleMessage(message, wParameter, lParameter);

	return result;
}

ParmBox::ParmBox(Window const &parent):
    Dialog("PARMBOX", parent, &parmBoxMessageHandler) 
{
}


// misc methods

INT_PTR ParmBox::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
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


#endif