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
    HandBox *p_box = (HandBox *)Window::Lookup(windowHandle);
    ASSERT(HWND(*p_box) == windowHandle);
	INT_PTR const result = p_box->HandleMessage(message, wParameter);

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
    Dialog("HANDBOX", &message_handler)
{
	mAreMoreHands = areMoreHands;
	mHandIndex = handIndex;
	mIsAutomatic = isAutomatic;
	mIsRemote = isRemote;
	mIpAddress = ipAddress;
	mPlayerName = playerName;
	mPlayerName.Capitalize();
}


// operators

HandBox::operator IpAddressType(void) const {
	return mIpAddress;
}


// misc methods

INT_PTR HandBox::HandleMessage(MessageType message, WPARAM wParam) {
	INT_PTR result = FALSE;

    switch (message) {
        case WM_INITDIALOG: {
		    Dialog::HandleMessage(message, wParam);

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
			EnableControl(IDC_RADIOLOCAL, true);
	        EnableControl(IDC_RADIOAUTO, true);
#if 1
			// TODO
			mIsRemote = false;
	        EnableControl(IDC_RADIOREMOTE, false);
#endif
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
            IdType const id = LOWORD(wParam);
			int const notification_code = HIWORD(wParam);
            switch (id) {
                case IDC_EDITNAME:
					if (notification_code == EN_CHANGE) {
                        mPlayerName = GetTextString(id);
					    mPlayerName.Capitalize();
					    bool const good_name = !mPlayerName.IsEmpty();
	                    EnableControl(IDOK, good_name);
					}
 					break;

				case IDC_RADIOAUTO: 
					if (notification_code == BN_CLICKED) {
					    SetButton(IDC_RADIOLOCAL, false);
					    SetButton(IDC_RADIOREMOTE, false);
                        mIsAutomatic = true;
                        mIsRemote = false;
						mPlayerName = "Computer";
			            SetTextString(IDC_EDITNAME, mPlayerName);
					}
                    break;

				case IDC_RADIOLOCAL:
					if (notification_code == BN_CLICKED) {
					    SetButton(IDC_RADIOAUTO, false);
					    SetButton(IDC_RADIOREMOTE, false);
                        mIsAutomatic = false;
                        mIsRemote = false;
						if (mPlayerName == "Computer") {
							mPlayerName = "Player";
							SetTextString(IDC_EDITNAME, mPlayerName);
						}
					}
                    break;
				
				case IDC_RADIOREMOTE:
					if (notification_code == BN_CLICKED) {
					    SetButton(IDC_RADIOAUTO, false);
					    SetButton(IDC_RADIOLOCAL, false);
                        mIsRemote = true;
					}
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
#endif // defined(_WINDOWS)
