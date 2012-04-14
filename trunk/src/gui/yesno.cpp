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

#ifdef _WINDOWS
#include "gui/resource.hpp"
#include "gui/yesno.hpp"
#include "gui/win_types.hpp"

// message handler (callback) for this dialog
static INT_PTR CALLBACK yesNoMessageHandler(
	HWND windowHandle,
	MessageType message,
	WPARAM wParameter, 
	LPARAM lParameter)
{
	lParameter;  // unused parameter
	ASSERT(windowHandle != NULL);
    YesNo *p_box = (YesNo *)BaseWindow::Lookup(windowHandle);
    ASSERT(HWND(*p_box) == windowHandle);
	int result = p_box->HandleMessage(message, wParameter);

	return result;
}


// lifecycle

YesNo::YesNo(TextType templateName):
    Dialog(templateName,& yesNoMessageHandler)
{}


// misc methods

int YesNo::HandleMessage(MessageType message, WPARAM wParam) {
	int result = 0;

    switch (message) {
        case WM_INITDIALOG: {
		    Dialog::HandleMessage(message, wParam);
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
		result = Dialog::HandleMessage(message, wParam);
	}

    return result;
}

#endif // defined(_WINDOWS)