// File:    parmbox1.cpp
// Purpose: ParmBox1 class
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

#include "gui/parmbox1.hpp"
#include "gui/resource.hpp"

// message handler (callback) for this dialog
static INT_PTR CALLBACK message_handler(
	HWND windowHandle,
	UINT message,
	WPARAM wParameter, 
	LPARAM lParameter)
{
	ASSERT(windowHandle != NULL);

	ParmBox1 *p_box;
	if (gpNewlyCreatedDialog != NULL) {
		p_box = (ParmBox1 *)gpNewlyCreatedDialog;
		gpNewlyCreatedDialog = NULL;
		p_box->SetHandle(windowHandle);
	} else {
       p_box = (ParmBox1 *)Window::Lookup(windowHandle);
	}

	INT_PTR result;
    ASSERT(p_box->Handle() == windowHandle);
    result = p_box->HandleMessage(message, wParameter, lParameter);

	return result;
}


// lifecycle

ParmBox1::ParmBox1(void):
    Dialog("PARMBOX1", &message_handler)
{
	mGameStyle = GAME_STYLE_PRACTICE;
	mPlayerMinutes = 30;
}

// operators

ParmBox1::operator GameStyleType(void) const {
    return mGameStyle;
}

// misc methods

INT_PTR ParmBox1::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
	INT_PTR result = FALSE;

    switch (message) {
        case WM_INITDIALOG: {
		    Dialog::HandleMessage(message, wParam, lParam);

			SetStyle();

			IdType slider_id = IDC_SLIDERMINUTES;
			ValueType min_value = 2;
			ValueType max_value = 120;
	        SetSliderRange(slider_id, min_value, max_value);
	        ValueType slider_value = SetSliderValue(slider_id, mPlayerMinutes);
	        ASSERT(slider_value == mPlayerMinutes);

            SetTextValue(IDC_EDITMINUTES, slider_value);

			result = TRUE;
			break;
        }

        case WM_COMMAND: {
            IdType id = LOWORD(wParam);
			int notification_code = HIWORD(wParam);
            switch (id) {
                case IDC_EDITMINUTES: {
                    ValueType value = GetTextValue(id);
					if (value != VALUE_INVALID) {
                        IdType slider_id = IDC_SLIDERMINUTES;
                        ValueType slider_value = SetSliderValue(slider_id, value);
					    if (slider_value != value) {
                            SetTextValue(id, slider_value);
					    }
         			    mPlayerMinutes = slider_value;
					}
                    break;
                }
				case IDC_RADIODEBUG:
				case IDC_RADIOPRACTICE:
				case IDC_RADIOFRIENDLY:
				case IDC_RADIOCHALLENGE: {
					if (notification_code == BN_CLICKED) {
                        SetStyle(id);
					}
                    break;
				}

            }
            break;
        }

		case WM_VSCROLL:
        case WM_HSCROLL: {
			HWND control_handle = HWND(lParam);

            IdType slider_id = IDC_SLIDERMINUTES;
			ASSERT(control_handle == GetControlHandle(slider_id));
            ValueType value = GetSliderValue(slider_id);
            IdType editbox_id = IDC_EDITMINUTES;
            SetTextValue(editbox_id, value);
		    mPlayerMinutes = value;
			break;
		}
    }

	if (result == FALSE) {
		result = Dialog::HandleMessage(message, wParam, lParam);
	}

    return result;
}

unsigned ParmBox1::PlayerMinutes(void) const {
	return mPlayerMinutes;
}

void ParmBox1::SetStyle(void) {
	SetButton(IDC_RADIODEBUG, mGameStyle == GAME_STYLE_DEBUG);
	SetButton(IDC_RADIOPRACTICE, mGameStyle == GAME_STYLE_PRACTICE);
	SetButton(IDC_RADIOFRIENDLY, mGameStyle == GAME_STYLE_FRIENDLY);
	SetButton(IDC_RADIOCHALLENGE, mGameStyle == GAME_STYLE_CHALLENGE);
}

void ParmBox1::SetStyle(IdType buttonId) {
	switch (buttonId) {
		case IDC_RADIODEBUG:
			mGameStyle = GAME_STYLE_DEBUG;
			break;
		case IDC_RADIOPRACTICE:
			mGameStyle = GAME_STYLE_PRACTICE;
			break;
		case IDC_RADIOFRIENDLY:
			mGameStyle = GAME_STYLE_FRIENDLY;
			break;
		case IDC_RADIOCHALLENGE:
			mGameStyle = GAME_STYLE_CHALLENGE;
			break;
		default:
			ASSERT(false);
	}
}
