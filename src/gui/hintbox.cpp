// File:    hintbox.cpp
// Purpose: HintBox class
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
#include "gui/hintbox.hpp"
#include "gui/resource.hpp"
#include "gui/win_types.hpp"


// message handler (callback) for this dialog
static INT_PTR CALLBACK message_handler(
	HWND windowHandle,
	MessageType message,
	WPARAM wParameter, 
	LPARAM lParameter)
{
	lParameter;  // unused parameter
	ASSERT(windowHandle != NULL);
    HintBox * const p_box = (HintBox *)Window::Lookup(windowHandle);
    ASSERT(HWND(*p_box) == windowHandle);
	INT_PTR const result = p_box->HandleMessage(message, wParameter);

	return result;
}


// lifecycle

HintBox::HintBox(HintType hintStrength, GameStyleType gameStyle)
:
    Dialog("HINTBOX", &message_handler)
{
	mGameStyle = gameStyle;
	mStrength = hintStrength;
}

// operators

HintBox::operator HintType(void) const {
	return mStrength;
}


// misc methods

INT_PTR HintBox::HandleMessage(MessageType message, WPARAM wParam) {
	INT_PTR result = FALSE;

    switch (message) {
        case WM_INITDIALOG: {
		    Dialog::HandleMessage(message, wParam);

			if (mGameStyle == GAME_STYLE_CHALLENGE) {
	            // disable the stronger hints
	            EnableControl(IDC_RADIO_CONNECTED, false);
	            EnableControl(IDC_RADIO_USABLE_ANY, false);
	            EnableControl(IDC_RADIO_USABLE_SELECTED, false);
			}
			SetHintStrength();
			result = TRUE;
			break;
        }

        case WM_COMMAND: {
            IdType const id = LOWORD(wParam);
			int const notification_code = HIWORD(wParam);
            switch (id) {
			    case IDC_RADIO_NONE:
			    case IDC_RADIO_EMPTY:
			    case IDC_RADIO_CONNECTED:
			    case IDC_RADIO_USABLE_ANY:
			    case IDC_RADIO_USABLE_SELECTED:
				    if (notification_code == BN_CLICKED) {
				        SetHintStrength(id);
				    }
                    break;
				default:
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

char const *HintBox::Name(void) const {
	return "Hint Controls - Gold Tile";
}

void HintBox::SetHintStrength(void) {
	SetButton(IDC_RADIO_NONE, mStrength == HINT_NONE);
	SetButton(IDC_RADIO_EMPTY, mStrength == HINT_EMPTY);
	SetButton(IDC_RADIO_CONNECTED, mStrength == HINT_CONNECTED);
	SetButton(IDC_RADIO_USABLE_ANY, mStrength == HINT_USABLE_ANY);
	SetButton(IDC_RADIO_USABLE_SELECTED, mStrength == HINT_USABLE_SELECTED);
}

void HintBox::SetHintStrength(IdType buttonId) {
	switch (buttonId) {
		case IDC_RADIO_NONE:
			mStrength = HINT_NONE;
			break;
		case IDC_RADIO_EMPTY:
			mStrength = HINT_EMPTY;
			break;
		case IDC_RADIO_CONNECTED:
			mStrength = HINT_CONNECTED;
			break;
		case IDC_RADIO_USABLE_ANY:
			mStrength = HINT_USABLE_ANY;
			break;
		case IDC_RADIO_USABLE_SELECTED:
			mStrength = HINT_USABLE_SELECTED;
			break;
		default:
			FAIL();
	}
}
#endif // defined(_WINDOWS)
