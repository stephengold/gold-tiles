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

#include "gui/handbox.hpp"
#include "gui/resource.hpp"

// message handler (callback) for this dialog
static INT_PTR CALLBACK handbox_message_handler(
	HWND windowHandle,
	UINT message,
	WPARAM wParameter, 
	LPARAM lParameter)
{
	ASSERT(windowHandle != NULL);
    HandBox *p_box = (HandBox *)Window::Lookup(windowHandle);
    ASSERT(p_box->Handle() == windowHandle);
	INT_PTR result = p_box->HandleMessage(message, wParameter, lParameter);

	return result;
}


// lifecycle

HandBox::HandBox(
	unsigned handIndex,
	bool areMoreHands,
	String const &playerName,
	bool isAutomatic,
	bool isRemote,
	LPARAM ipAddress)
:
    Dialog("HANDBOX", &handbox_message_handler)
{
	mAreMoreHands = areMoreHands;
	mHandIndex = handIndex;
	mIsAutomatic = isAutomatic;
	mIsRemote = isRemote;
	mPlayerName = playerName;
}


// misc methods

INT_PTR HandBox::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
	INT_PTR result = FALSE;

    switch (message) {
        case WM_INITDIALOG: {
		    Dialog::HandleMessage(message, wParam, lParam);

			String message = "Who will play the ";
			if (mAreMoreHands) {
				message += ::ordinal(mHandIndex) + " hand?";
			} else if (mHandIndex == 1) {
				message += "hand?";
			} else {
				message += "last hand?";
			}
			SetTextString(IDC_WHO, message);
			SetTextString(IDC_EDITNAME, mPlayerName);
			SetButton(IDC_RADIOLOCAL, !mIsAutomatic && !mIsRemote);
	        SetButton(IDC_RADIOAUTO, mIsAutomatic && !mIsRemote);
	        SetButton(IDC_RADIOREMOTE, mIsRemote);

			if (!mAreMoreHands) {
			    SetTextString(IDOK, "Start Game");
			}

			result = TRUE;
			break;
        }

        case WM_COMMAND: {
            IdType id = LOWORD(wParam);
			int notification_code = HIWORD(wParam);
            switch (id) {
                case IDC_EDITNAME: {
                    mPlayerName = GetTextString(id);
					bool good_name = !mPlayerName.IsEmpty();
	                EnableButton(IDOK, good_name);
                    break;
                }
				case IDC_RADIOAUTO: {
					if (notification_code == BN_CLICKED) {
					    SetButton(IDC_RADIOLOCAL, false);
					    SetButton(IDC_RADIOREMOTE, false);
                        mIsAutomatic = true;
                        mIsRemote = false;
					}
                    break;
				}
				case IDC_RADIOLOCAL: {
					if (notification_code == BN_CLICKED) {
					    SetButton(IDC_RADIOAUTO, false);
					    SetButton(IDC_RADIOREMOTE, false);
                        mIsAutomatic = false;
                        mIsRemote = false;
					}
                    break;
				}
				case IDC_RADIOREMOTE: {
					if (notification_code == BN_CLICKED) {
					    SetButton(IDC_RADIOAUTO, false);
					    SetButton(IDC_RADIOLOCAL, false);
                        mIsRemote = true;
					}
                    break;
				}
            }
            break;
        }
    }

	if (result == FALSE) {
		result = Dialog::HandleMessage(message, wParam, lParam);
	}

    return result;
}

LPARAM HandBox::IpAddress(void) const {
	return mIpAddress;
}

String HandBox::PlayerName(void) const {
	return mPlayerName;
}


// inquiry methods

bool HandBox::IsAutomatic(void) const {
	return mIsAutomatic;
}
bool HandBox::IsRemote(void) const {
	return mIsRemote;
}
