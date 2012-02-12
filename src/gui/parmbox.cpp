// File:    parmbox.cpp
// Purpose: ParmBox class
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

#include "gui/parmbox.hpp"
#include "gui/resource.hpp"

// message handler (callback) for this dialog
static INT_PTR CALLBACK message_handler(
	HWND windowHandle,
	UINT message,
	WPARAM wParameter, 
	LPARAM lParameter)
{
    ParmBox *p_box;
	if (gpNewlyCreatedDialog != NULL) {
		p_box = (ParmBox *)gpNewlyCreatedDialog;
		gpNewlyCreatedDialog = NULL;
		p_box->SetHandle(windowHandle);
	} else {
       p_box = (ParmBox *)Window::Lookup(windowHandle);
	}

	INT_PTR result;
    ASSERT(p_box->Handle() == windowHandle);
    result = p_box->HandleMessage(message, wParameter, lParameter);

	return result;
}


// lifecycle

ParmBox::ParmBox(void):
    Dialog("PARMBOX", &message_handler)
{}


// misc methods

unsigned ParmBox::AttributeCnt(void) const {
    return mAttributeCnt;
}

unsigned ParmBox::ClonesPerTile(void) const {
    return mClonesPerTile;
}

ParmBox::IdType ParmBox::EditboxId(IdType sliderId) const {
    IdType result;
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
        default:
            ASSERT(false);
    }
    
    return result;
}

INT_PTR ParmBox::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
	INT_PTR result = FALSE;

    switch (message) {
        case WM_INITDIALOG: {
		    Dialog::HandleMessage(message, wParam, lParam);
		    
            mPlayerCnt = 2;
		    InitControl(IDC_SLIDER1, mPlayerCnt, 1, 10);
            
            mHandSize = 7;
		    InitControl(IDC_SLIDER2, mHandSize, 1, 12);
            
            mAttributeCnt = 2;
		    InitControl(IDC_SLIDER3, mAttributeCnt, 2, 5);
		    
            mClonesPerTile = 2;
		    InitControl(IDC_SLIDER4, mClonesPerTile, 0, 5);

            result = TRUE;
			break;
        }

        case WM_COMMAND: {
            IdType id = LOWORD(wParam);
            switch (id) {
                case IDC_EDIT1:
                case IDC_EDIT2:
                case IDC_EDIT3:
                case IDC_EDIT4: {
                    ValueType value = GetTextValue(id);
					if (value != VALUE_INVALID) {
                        IdType slider_id = SliderId(id);
                        ValueType slider_value = SetSliderValue(slider_id, value);
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
            IdType slider_id = SliderId((HWND)lParam);
			ASSERT(slider_id != 0);
            ValueType value = GetSliderValue(slider_id);
            IdType editbox_id = EditboxId(slider_id);
            SetTextValue(editbox_id, value);
			UpdateValue(slider_id, value);
			break;
		}
    }

	if (result == FALSE) {
		result = Dialog::HandleMessage(message, wParam, lParam);
	}

    return result;
}

unsigned ParmBox::HandSize(void) const {
    return mHandSize;
}

void ParmBox::InitControl(
	IdType sliderId,
	ValueType value,
	ValueType minValue,
	ValueType maxValue)
{
    ASSERT(value <= maxValue);
    ASSERT(value >= minValue);
	
    WPARAM redraw = WPARAM(FALSE);
	SetSliderRange(sliderId, minValue, maxValue);

	ValueType slider_value = SetSliderValue(sliderId, value);
	ASSERT(slider_value == value);

	IdType min_id = MinId(sliderId);
	SetTextValue(min_id, minValue);

	IdType max_id = MaxId(sliderId);
	SetTextValue(max_id, maxValue);

	IdType editbox_id = EditboxId(sliderId);
    SetTextValue(editbox_id, slider_value);
}

ParmBox::IdType ParmBox::MaxId(IdType sliderId) const {
    IdType result;
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
        default:
            ASSERT(false);
    }
    
    return result;
}

ParmBox::IdType ParmBox::MinId(IdType sliderId) const {
    IdType result;
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
        default:
            ASSERT(false);
    }
    
    return result;
}


unsigned ParmBox::PlayerCnt(void) const {
    return mPlayerCnt;
}

ParmBox::IdType ParmBox::SliderId(IdType editboxId) const {
    IdType result;
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
        default:
            ASSERT(false);
    }
    return result;
}

ParmBox::IdType ParmBox::SliderId(HWND handle) const {
    IdType result = 0;
	if (handle == GetControlHandle(IDC_SLIDER1)) {
		result = IDC_SLIDER1;
	} else if (handle == GetControlHandle(IDC_SLIDER2)) {
		result = IDC_SLIDER2;
	} else if (handle == GetControlHandle(IDC_SLIDER3)) {
		result = IDC_SLIDER3;
	} else if (handle == GetControlHandle(IDC_SLIDER4)) {
		result = IDC_SLIDER4;
	}

	return result;
}

void ParmBox::UpdateValue(IdType sliderId, ValueType value) {
    switch (sliderId) {
        case IDC_SLIDER1:
            mPlayerCnt = value;
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
        default:
            ASSERT(false);
    }
}