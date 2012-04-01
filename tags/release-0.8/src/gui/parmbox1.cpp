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

#ifdef _WINDOWS
#include "game.hpp"
#include "gui/parmbox1.hpp"
#include "gui/resource.hpp"
#include "gui/win_types.hpp"

// message handler (callback) for this dialog box

static INT_PTR CALLBACK message_handler(
	HWND windowHandle,
	MessageType message,
	WPARAM wParameter, 
	LPARAM lParameter)
{
	ASSERT(windowHandle != NULL);
	ParmBox1 * const p_box = (ParmBox1 *)Window::Lookup(windowHandle);
    ASSERT(HWND(*p_box) == windowHandle);
	INT_PTR const result = p_box->HandleMessage(message, wParameter, lParameter);

	return result;
}


// lifecycle

ParmBox1::ParmBox1(GameStyleType gameStyle, unsigned secondsPerHand):
    Dialog("PARMBOX1", &message_handler)
{
	mGameStyle = gameStyle;
	if (mGameStyle == GAME_STYLE_NONE) {
		// default style
#ifdef _DEBUG
		mGameStyle = GAME_STYLE_DEBUG;
#else // !defined(_DEBUG)
		mGameStyle = GAME_STYLE_PRACTICE;
#endif // !defined(_DEBUG)
	}

	if (secondsPerHand == Game::TIME_UNLIMITED) {
		mPlayerMinutes = PLAYER_MINUTES_DEFAULT;

	} else { // limited time

    	// convert to minutes so slider doesn't have to deal with small change
	    mPlayerMinutes = (secondsPerHand + SECONDS_PER_MINUTE - 1)/SECONDS_PER_MINUTE;

		// limit range so value will fit on the slider
		if (mPlayerMinutes < PLAYER_MINUTES_MIN) {
			mPlayerMinutes = PLAYER_MINUTES_MIN;
		}
		if (mPlayerMinutes > PLAYER_MINUTES_MAX) {
			mPlayerMinutes = PLAYER_MINUTES_MAX;
		}

	}
	ASSERT(mPlayerMinutes >= PLAYER_MINUTES_MIN);
	ASSERT(mPlayerMinutes <= PLAYER_MINUTES_MAX);
}


// operators

ParmBox1::operator GameStyleType(void) const {
    return mGameStyle;
}


// misc methods

INT_PTR ParmBox1::HandleMessage(MessageType message, WPARAM wParam, LPARAM lParam) {
	INT_PTR result = FALSE;

    switch (message) {
        case WM_INITDIALOG: {
		    Dialog::HandleMessage(message, wParam);

			SetStyle();

			IdType const slider_id = IDC_SLIDERMINUTES;
			ValueType const min_value = PLAYER_MINUTES_MIN;
			ValueType const max_value = PLAYER_MINUTES_MAX;
	        SetSliderRange(slider_id, min_value, max_value);
	        ValueType const slider_value = SetSliderValue(slider_id, mPlayerMinutes);
	        ASSERT(slider_value == mPlayerMinutes);

            SetTextValue(IDC_EDITMINUTES, slider_value);
	        EnableControl(IDC_EDITMINUTES, mGameStyle == GAME_STYLE_CHALLENGE);

			result = TRUE;
			break;
        }

        case WM_COMMAND: {
            IdType const id = LOWORD(wParam);
			int const notification_code = HIWORD(wParam);
            switch (id) {
                case IDC_EDITMINUTES: {
                    ValueType const value = GetTextValue(id);
					if (value != VALUE_INVALID) {
                        IdType const slider_id = IDC_SLIDERMINUTES;
                        ValueType const slider_value = SetSliderValue(slider_id, value);
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
			HWND const control_handle = HWND(lParam);

            IdType const slider_id = IDC_SLIDERMINUTES;
			ASSERT(control_handle == GetControlHandle(slider_id));
            ValueType const value = GetSliderValue(slider_id);
            IdType editbox_id = IDC_EDITMINUTES;
            SetTextValue(editbox_id, value);
		    mPlayerMinutes = value;
			break;
		}
    }

	if (result == FALSE) {
		result = Dialog::HandleMessage(message, wParam);
	}

    return result;
}

unsigned ParmBox1::PlayerSeconds(void) const {
	unsigned result = Game::TIME_UNLIMITED;

	if (mGameStyle == GAME_STYLE_CHALLENGE) {
		result = mPlayerMinutes * SECONDS_PER_MINUTE;
	}

	return result;
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
			FAIL();
	}
	EnableControl(IDC_EDITMINUTES, mGameStyle == GAME_STYLE_CHALLENGE);
}
#endif // defined(_WINDOWS)
