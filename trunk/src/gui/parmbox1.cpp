// File:     parmbox1.cpp
// Location: src/gui
// Purpose:  implement ParmBox1 class
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
    ParmBox1* const p_box = (ParmBox1*)BaseWindow::Lookup(windowHandle);
    ASSERT(HWND(*p_box) == windowHandle);
    INT_PTR const result = p_box->HandleMessage(message, wParameter, lParameter);

    return result;
}


// lifecycle

ParmBox1::ParmBox1(GameOpt& rGameOpt):
Dialog("PARMBOX1", &message_handler),
    mrGameOpt(rGameOpt)
{
}


// misc methods

INT_PTR ParmBox1::HandleMessage(MessageType message, WPARAM wParam, LPARAM lParam) {
    INT_PTR result = FALSE;

    switch (message) {
    case WM_INITDIALOG: {
        Dialog::HandleMessage(message, wParam);

        SetStyle();

        IdType const slider_id = IDC_SLIDERMINUTES;
        ValueType const min_value = GameOpt::MINUTES_PER_HAND_MIN;
        ValueType const max_value = MINUTES_PER_HAND_MAX;
        SetSliderRange(slider_id, min_value, max_value);

        ValueType const minutes = mrGameOpt.MinutesPerHand();
        ValueType const slider_value = SetSliderValue(slider_id, minutes);
        ASSERT(slider_value == minutes);

        SetTextValue(IDC_EDITMINUTES, slider_value);
        EnableControl(IDC_EDITMINUTES, mrGameOpt.HasTimeLimit());

        RulesType const rules = RulesType(mrGameOpt);
        unsigned const standard_index = AddListboxItem(IDC_LIST1, 
            "Standard Rules - 108 square tiles on an endless grid, 2 hands of 6");
        ASSERT(standard_index == RULES_STANDARD);
        unsigned const custom_index = AddListboxItem(IDC_LIST1, 
            "Custom Rules - you decide what the rules will be");
        ASSERT(custom_index == RULES_CUSTOM);
        if (rules == RULES_REPLAY) {
            unsigned const replay_index = AddListboxItem(IDC_LIST1, 
                "Replay - use the same rules as last time");
            ASSERT(replay_index == RULES_REPLAY);
        }
        SetListboxSelection(IDC_LIST1, rules);

        result = TRUE;
        break;
                        }

    case WM_COMMAND: {
        IdType const control_id = LOWORD(wParam);
        int const notification_code = HIWORD(wParam);
        switch (control_id) {
        case IDC_EDITMINUTES: {
            ValueType const value = GetTextValue(control_id);
            if (value != VALUE_INVALID) {
                IdType const slider_id = IDC_SLIDERMINUTES;
                ValueType const slider_value = SetSliderValue(slider_id, value);
                if (slider_value != value) {
                    SetTextValue(control_id, slider_value);
                }
                mrGameOpt.SetMinutesPerHand(slider_value);
            }
            break;
                              }
        case IDC_LIST1:
            if (notification_code == LBN_SELCHANGE) {
                ValueType const value = GetListboxSelection(control_id);
                RulesType rules = RulesType(value);
                mrGameOpt.SetRules(rules); 
            }
            break;
        case IDC_RADIODEBUG:
        case IDC_RADIOPRACTICE:
        case IDC_RADIOFRIENDLY:
        case IDC_RADIOCHALLENGE: {
            if (notification_code == BN_CLICKED) {
                SetStyle(control_id);
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
        mrGameOpt.SetMinutesPerHand(value);
        break;
                     }
    }

    if (result == FALSE) {
        result = Dialog::HandleMessage(message, wParam);
    }

    return result;
}

void ParmBox1::SetStyle(void) {
    SetButton(IDC_RADIODEBUG, mrGameOpt.IsDebug());
    SetButton(IDC_RADIOPRACTICE, mrGameOpt.IsPractice());
    SetButton(IDC_RADIOFRIENDLY, mrGameOpt.IsFriendly());
    SetButton(IDC_RADIOCHALLENGE, mrGameOpt.IsChallenge());
}

void ParmBox1::SetStyle(IdType buttonId) {
    switch (buttonId) {
    case IDC_RADIODEBUG:
        mrGameOpt.SetDebug();
        break;
    case IDC_RADIOPRACTICE:
        mrGameOpt.SetPractice();
        break;
    case IDC_RADIOFRIENDLY:
        mrGameOpt.SetFriendly();
        break;
    case IDC_RADIOCHALLENGE:
        mrGameOpt.SetChallenge();
        break;
    default:
        FAIL();
    }
    EnableControl(IDC_EDITMINUTES, mrGameOpt.IsChallenge());
}
#endif // defined(_WINDOWS)
