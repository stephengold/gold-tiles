// File:    parmbox3.cpp
// Purpose: ParmBox3 class
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
#include "gui/parmbox3.hpp"
#include "gui/resource.hpp"
#include "gui/win_types.hpp"

// message handler (callback) for this dialog
static int CALLBACK message_handler3(
	HWND windowHandle,
	MessageType message,
	WPARAM wParameter, 
	LPARAM lParameter)
{
	ASSERT(windowHandle != NULL);
    ParmBox3 * const p_box = (ParmBox3 *)Window::Lookup(windowHandle);
    ASSERT(HWND(*p_box) == windowHandle);
	int const result = p_box->HandleMessage(message, wParameter, lParameter);

	return result;
}


// lifecycle

ParmBox3::ParmBox3(unsigned attributeCnt, unsigned clonesPerTile, 
		     unsigned handSize, unsigned handCnt, unsigned bonusTilePercentage):
    Dialog("PARMBOX3", &message_handler3)
{
    mAttributeCnt = attributeCnt;
	mBonusTilePercentage = bonusTilePercentage;
    mClonesPerTile = clonesPerTile;
    mHandCnt = handCnt;
    mHandSize = handSize;            
}


// misc methods

ACountType ParmBox3::AttributeCnt(void) const {
    ACountType const result = ACountType(mAttributeCnt);

    return result;
}

unsigned ParmBox3::BonusTilePercentage(void) const {
    return mBonusTilePercentage;
}

unsigned ParmBox3::ClonesPerTile(void) const {
    return mClonesPerTile;
}

/* static */ IdType ParmBox3::EditboxId(IdType sliderId) {
    IdType result = 0;

    switch (sliderId) {
        case IDC_SLIDER1:
            result = IDC_EDIT1;
            break;
        case IDC_SLIDER2:
            result = IDC_EDIT2;
            break;
        case IDC_SLIDER3:
            result = IDC_EDIT3;
            break;
        case IDC_SLIDER4:
            result = IDC_EDIT4;
            break;
        case IDC_SLIDER5:
            result = IDC_EDIT5;
            break;
        default:
            FAIL();
    }
    
    return result;
};

INT_PTR ParmBox3::HandleMessage(MessageType message, WPARAM wParam, LPARAM lParam) {
	INT_PTR result = FALSE;

    switch (message) {
        case WM_INITDIALOG: {
		    Dialog::HandleMessage(message, wParam);
		    
			InitControl(IDC_SLIDER1, mHandCnt, 1, 10);
		    InitControl(IDC_SLIDER2, mHandSize, 1, 12);
		    InitControl(IDC_SLIDER3, mAttributeCnt, 2, 5);
		    InitControl(IDC_SLIDER4, mClonesPerTile, 0, 8);
		    InitControl(IDC_SLIDER5, mBonusTilePercentage, 0, 25);

            result = TRUE;
			break;
        }

        case WM_COMMAND: {
            IdType const id = LOWORD(wParam);
            switch (id) {
                case IDC_EDIT1:
                case IDC_EDIT2:
                case IDC_EDIT3:
                case IDC_EDIT4:
                case IDC_EDIT5: {
                    ValueType const value = GetTextValue(id);
					if (value != VALUE_INVALID) {
                        IdType const slider_id = SliderId(id);
                        ValueType const slider_value = SetSliderValue(slider_id, value);
					    if (slider_value != value) {
                            SetTextValue(id, slider_value);
					    }
         			    UpdateValue(slider_id, slider_value);
					}
                    break;
                }
            }
            break;
        }

		case WM_VSCROLL:
        case WM_HSCROLL: {
            IdType const slider_id = SliderId(HWND(lParam));
			ASSERT(slider_id != 0);
            ValueType const value = GetSliderValue(slider_id);
            IdType const editbox_id = EditboxId(slider_id);
            SetTextValue(editbox_id, value);
			UpdateValue(slider_id, value);
			break;
		}
    }

	if (result == FALSE) {
		result = Dialog::HandleMessage(message, wParam);
	}

    return result;
}

unsigned ParmBox3::HandSize(void) const {
    return mHandSize;
}

void ParmBox3::InitControl(
	IdType sliderId,
	ValueType value,
	ValueType minValue,
	ValueType maxValue)
{
    ASSERT(value >= minValue);
    ASSERT(value <= maxValue);
	
	SetSliderRange(sliderId, minValue, maxValue);

	ValueType const slider_value = SetSliderValue(sliderId, value);
	ASSERT(slider_value == value);

	IdType const min_id = MinId(sliderId);
	SetTextValue(min_id, minValue);

	IdType const max_id = MaxId(sliderId);
	SetTextValue(max_id, maxValue);

	IdType const editbox_id = EditboxId(sliderId);
    SetTextValue(editbox_id, slider_value);
}

/* static */ IdType ParmBox3::MaxId(IdType sliderId) {
    IdType result = 0;

    switch (sliderId) {
        case IDC_SLIDER1:
            result = IDC_SMAX1;
            break;
        case IDC_SLIDER2:
            result = IDC_SMAX2;
            break;
        case IDC_SLIDER3:
            result = IDC_SMAX3;
            break;
        case IDC_SLIDER4:
            result = IDC_SMAX4;
            break;
        case IDC_SLIDER5:
            result = IDC_SMAX5;
            break;
        default:
            FAIL();
    }
    
    return result;
}

/* static */ IdType ParmBox3::MinId(IdType sliderId) {
    IdType result = 0;

    switch (sliderId) {
        case IDC_SLIDER1:
            result = IDC_SMIN1;
            break;
        case IDC_SLIDER2:
            result = IDC_SMIN2;
            break;
        case IDC_SLIDER3:
            result = IDC_SMIN3;
            break;
        case IDC_SLIDER4:
            result = IDC_SMIN4;
            break;
        case IDC_SLIDER5:
            result = IDC_SMIN5;
            break;
        default:
            FAIL();
    }
    
    return result;
}


unsigned ParmBox3::HandCnt(void) const {
    return mHandCnt;
}

/* static */ IdType ParmBox3::SliderId(IdType editboxId) {
    IdType result = 0;

    switch (editboxId) {
        case IDC_EDIT1:
            result = IDC_SLIDER1;
            break;
        case IDC_EDIT2:
            result = IDC_SLIDER2;
            break;
        case IDC_EDIT3:
            result = IDC_SLIDER3;
            break;
        case IDC_EDIT4:
            result = IDC_SLIDER4;
            break;
        case IDC_EDIT5:
            result = IDC_SLIDER5;
            break;
        default:
            FAIL();
    }

    return result;
}

IdType ParmBox3::SliderId(HWND handle) const {
    IdType result = 0;

	if (handle == GetControlHandle(IDC_SLIDER1)) {
		result = IDC_SLIDER1;
	} else if (handle == GetControlHandle(IDC_SLIDER2)) {
		result = IDC_SLIDER2;
	} else if (handle == GetControlHandle(IDC_SLIDER3)) {
		result = IDC_SLIDER3;
	} else if (handle == GetControlHandle(IDC_SLIDER4)) {
		result = IDC_SLIDER4;
	} else if (handle == GetControlHandle(IDC_SLIDER5)) {
		result = IDC_SLIDER5;
	} else {
		FAIL();
	}

	return result;
}

void ParmBox3::UpdateValue(IdType sliderId, ValueType value) {
    switch (sliderId) {
        case IDC_SLIDER1:
            mHandCnt = value;
            break;
        case IDC_SLIDER2:
            mHandSize = value;
            break;
        case IDC_SLIDER3:
            mAttributeCnt = value;
            break;
        case IDC_SLIDER4:
            mClonesPerTile = value;
            break;
        case IDC_SLIDER5:
            mBonusTilePercentage = value;
            break;
        default:
            FAIL();
    }
}
#endif // defined(_WINDOWS)
