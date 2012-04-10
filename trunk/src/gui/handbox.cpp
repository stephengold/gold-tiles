// File:    handbox.cpp
// Purpose: HandBox class
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
#include "gui/handbox.hpp"
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
    HandBox * p_box = (HandBox *)BaseWindow::Lookup(windowHandle);
    ASSERT(HWND(*p_box) == windowHandle);
	INT_PTR const result = p_box->HandleMessage(message, wParameter);

	return result;
}


// lifecycle

HandBox::HandBox(unsigned handIndex, unsigned handCnt, HandOpt const &rOptions):
    Dialog("HANDBOX", &message_handler),
	mOptions(rOptions)
{
	mAreMoreHands = (handIndex < handCnt);
	mHandCnt = handCnt;
	mHandIndex = handIndex;
}


// operators

HandBox::operator HandOpt(void) const {
	return mOptions;
}


// misc methods

void HandBox::HandleButtonClick(IdType buttonId) {
	switch (buttonId) {
		case IDC_RADIOAUTO:
            mOptions.SetAutomatic();
			UpdateNameBox("Computer");
            break;

		case IDC_RADIOLOCAL:
			mOptions.SetLocalUser();
		    if (mOptions.PlayerName() == "Computer") {
				UpdateNameBox("User");
			}
            break;
				
		case IDC_RADIOREMOTE:
            mOptions.SetRemote();
		    if (mOptions.PlayerName() == "Computer") {
				UpdateNameBox("Remote Player");
			}
            break;
	}

	UpdateButtons();
}
				
INT_PTR HandBox::HandleMessage(MessageType message, WPARAM wParam) {
	INT_PTR result = FALSE;

    switch (message) {
        case WM_INITDIALOG: {
		    Dialog::HandleMessage(message, wParam);

			String message = "Who will play the ";
			if (mAreMoreHands) {
				message += ::ordinal(mHandIndex) + " of the ";
				message += String(mHandCnt) + " hands?";
			} else if (mHandIndex == 1) {
				message += "hand?";
			} else {
				message += "last of the ";
				message += String(mHandCnt) + " hands?";
			}
			SetTextString(IDC_WHO, message);

			if (!mAreMoreHands) {
			    SetTextString(IDOK, "Start Game");
			}

	        EnableControl(IDC_RADIOAUTO, true);
			EnableControl(IDC_RADIOLOCAL, true);
#if 1
			// TODO
			ASSERT(!mOptions.IsRemote());
	        EnableControl(IDC_RADIOREMOTE, false);
#endif
            SetSliderRange(IDC_SLIDER1, 0, LEVEL_MAX);
			UpdateButtons();
			String const name = mOptions.PlayerName();
			UpdateNameBox(name);
			UpdateSlider();

			result = TRUE;
			break;
        }

        case WM_COMMAND: {
            IdType const id = LOWORD(wParam);
			int const notification_code = HIWORD(wParam);
            switch (id) {
                case IDC_EDITNAME:
					if (notification_code == EN_CHANGE) {
                        String const name = GetTextString(id);
					    mOptions.SetPlayerName(name);
					    bool const good_name = mOptions.HasValidName();
	                    EnableControl(IDOK, good_name);
					}
 					break;

				case IDC_RADIOAUTO:
				case IDC_RADIOLOCAL:
				case IDC_RADIOREMOTE:
					if (notification_code == BN_CLICKED) {
					    HandleButtonClick(id);
					}
					break;
            }
			break;
		}

		case WM_HSCROLL:
		case WM_VSCROLL: {
            ValueType const value = GetSliderValue(IDC_SLIDER1);
			double const prob = double(LEVEL_MAX - value)/10.0;
			mOptions.SetSkipProbability(prob);
			break;
		}
    }

	if (result == FALSE) {
		result = Dialog::HandleMessage(message, wParam);
	}

    return result;
}

void HandBox::UpdateButtons(void) {
    SetButton(IDC_RADIOAUTO, mOptions.IsAutomatic());
    SetButton(IDC_RADIOLOCAL, mOptions.IsLocalUser());
    SetButton(IDC_RADIOREMOTE, mOptions.IsRemote());

	EnableControl(IDC_IPADDRESS1, mOptions.IsRemote());
	EnableControl(IDC_SLIDER1, mOptions.IsAutomatic());
}

void HandBox::UpdateNameBox(String const &rName) {
	mOptions.SetPlayerName(rName);
    SetTextString(IDC_EDITNAME, rName);

    bool const good_name = mOptions.HasValidName();
    EnableControl(IDOK, good_name);
}

void HandBox::UpdateSlider(void) {
	double const prob = mOptions.SkipProbability();
	ValueType const level = LEVEL_MAX - ValueType(0.5 + 10.0*prob);
	ValueType const new_level = SetSliderValue(IDC_SLIDER1, level);
	double const new_prob = double(LEVEL_MAX - new_level)/10.0;
	mOptions.SetSkipProbability(new_prob);
}
#endif // defined(_WINDOWS)
