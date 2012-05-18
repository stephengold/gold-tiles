// File:     handbox.cpp
// Location: src/gui
// Purpose:  implement HandBox class
// Author:   Stephen Gold sgold@sonic.net
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
    HandBox* p_box = (HandBox*)BaseWindow::Lookup(windowHandle);
    ASSERT(HWND(*p_box) == windowHandle);
    INT_PTR const result = p_box->HandleMessage(message, wParameter);

    return result;
}


// lifecycle

HandBox::HandBox(unsigned handIndex, unsigned handCnt, HandOpt& rOptions):
Dialog("HANDBOX", &message_handler),
    mrOptions(rOptions)
{
    mAreMoreHands = (handIndex < handCnt);
    mHandCnt = handCnt;
    mHandIndex = handIndex;
}


// misc methods

void HandBox::HandleButtonClick(IdType buttonId) {
    String const name = mrOptions.PlayerName();

    switch (buttonId) {
    case IDC_RADIOAUTO:
        mrOptions.SetAutomatic();
        UpdateNameBox("Computer");
        break;

    case IDC_RADIOLOCAL:
        mrOptions.SetLocalUser();
        if (name == "Computer") {
            UpdateNameBox("User");
        }
        break;

    case IDC_RADIOREMOTE:
        mrOptions.SetRemote();
        if (name == "Computer") {
            UpdateNameBox("User");
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

        // Fill in the dialog box label.
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
        EnableControl(IDC_RADIOREMOTE, true);

        SetSliderRange(IDC_SLIDER1, HandOpt::LEVEL_MIN, HandOpt::LEVEL_MAX);
        Address const address = mrOptions;
        SetTextString(IDC_IPADDRESS1, address);
        UpdateButtons();
        String const name = mrOptions.PlayerName();
        UpdateNameBox(name);
        UpdateSlider();

        result = TRUE;
        break;
                        }

    case WM_COMMAND: {
        IdType const control_id = LOWORD(wParam);
        int const notification_code = HIWORD(wParam);
        switch (control_id) {
        case IDC_IPADDRESS1:
            if (notification_code == EN_CHANGE) {
                String const address_string = GetTextString(control_id);
                Address const address(address_string);
                mrOptions.SetAddress(address);
                bool const valid = mrOptions.IsValid();
                EnableControl(IDOK, valid);
            }
            break;

        case IDC_EDITNAME:
            if (notification_code == EN_CHANGE) {
                String const name = GetTextString(control_id);
                mrOptions.SetPlayerName(name);
                bool const valid = mrOptions.IsValid();
                EnableControl(IDOK, valid);
            }
            break;

        case IDC_RADIOAUTO:
        case IDC_RADIOLOCAL:
        case IDC_RADIOREMOTE:
            if (notification_code == BN_CLICKED) {
                HandleButtonClick(control_id);
            }
            break;
        }
        break;
                     }

    case WM_HSCROLL:
    case WM_VSCROLL: {
        ValueType const level = GetSliderValue(IDC_SLIDER1);
        mrOptions.SetLevel(level);
        break;
                     }
    }

    if (result == FALSE) {
        result = Dialog::HandleMessage(message, wParam);
    }

    return result;
}

void HandBox::UpdateButtons(void) {
    SetButton(IDC_RADIOAUTO, mrOptions.IsAutomatic());
    SetButton(IDC_RADIOLOCAL, mrOptions.IsLocalUser());
    SetButton(IDC_RADIOREMOTE, mrOptions.IsRemote());

    EnableControl(IDC_IPADDRESS1, mrOptions.IsRemote());
    EnableControl(IDC_SLIDER1, mrOptions.IsAutomatic());
}

void HandBox::UpdateNameBox(String const& rName) {
    mrOptions.SetPlayerName(rName);
    SetTextString(IDC_EDITNAME, rName);

    bool const good_name = mrOptions.IsValid();
    EnableControl(IDOK, good_name);
}

void HandBox::UpdateSlider(void) {
    ValueType const level = mrOptions.Level();
    ValueType const new_level = SetSliderValue(IDC_SLIDER1, level);
    mrOptions.SetLevel(new_level);
}
#endif // defined(_WINDOWS)
