// File:    yesno.cpp
// Purpose: YesNo class
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

#include "gui/resource.hpp"
#include "gui/yesno.hpp"

// message handler (callback) for this dialog
static INT_PTR CALLBACK yesNoMessageHandler(
	HWND windowHandle,
	UINT message,
	WPARAM wParameter, 
	LPARAM lParameter)
{
    YesNo *p_box;
	if (gpNewlyCreatedDialog != NULL) {
		p_box = (YesNo *)gpNewlyCreatedDialog;
		gpNewlyCreatedDialog = NULL;
		p_box->SetHandle(windowHandle);
	} else {
       p_box = (YesNo *)Window::Lookup(windowHandle);
	}

	INT_PTR result;
    ASSERT(p_box->Handle() == windowHandle);
    result = p_box->HandleMessage(message, wParameter, lParameter);

	return result;
}


// lifecycle

YesNo::YesNo(char const *templateName):
    Dialog(templateName, &yesNoMessageHandler)
{}


// misc methods

INT_PTR YesNo::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
	INT_PTR result = FALSE;

    switch (message) {
        case WM_INITDIALOG: {
		    Dialog::HandleMessage(message, wParam, lParam);
            result = TRUE;
			break;
        }

        case WM_COMMAND: {
			IdType id = LOWORD(wParam);
            switch (id) {
                case IDC_Y:
                    Close(RESULT_YES);
                    result = TRUE;
					break;
                case IDC_N:
                    Close(RESULT_NO);
                    result = TRUE;
					break;
            }
            break;

        }
    }

	if (result == FALSE) {
		result = Dialog::HandleMessage(message, wParam, lParam);
	}

    return result;
}

