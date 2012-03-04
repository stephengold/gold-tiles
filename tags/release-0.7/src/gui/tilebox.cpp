// File:    tilebox.cpp
// Purpose: TileBox class
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

#include "gui/tilebox.hpp"
#include "gui/resource.hpp"
#include "gui/win_types.hpp"

// message handler (callback) for this dialog
static INT_PTR CALLBACK message_handler(
	HWND windowHandle,
	MessageType message,
	WPARAM wParameter, 
	LPARAM lParameter)
{
	ASSERT(windowHandle != NULL);
    TileBox *p_box = (TileBox *)Window::Lookup(windowHandle);
    ASSERT(HWND(*p_box) == windowHandle);
	INT_PTR result = p_box->HandleMessage(message, wParameter, lParameter);

	return result;
}


// lifecycle

TileBox::TileBox(char const *templateName, ACountType attributeCnt, AValueType *pValueMax):
    Dialog(templateName, &message_handler)
{
    mAttributeCnt = attributeCnt;
	for (unsigned i = 0; i < attributeCnt; i++) {
		mpNumValues[i] = pValueMax[i] + 1;
	}
}


// misc methods

IdType TileBox::EditboxId(IdType sliderId) const {
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
}

INT_PTR TileBox::HandleMessage(MessageType message, WPARAM wParam, LPARAM lParam) {
	INT_PTR result = FALSE;

    switch (message) {
        case WM_INITDIALOG: {
		    Dialog::HandleMessage(message, wParam);
		    
			InitControl(IDC_SLIDER1, mpNumValues[0], 4, 9);
		    InitControl(IDC_SLIDER2, mpNumValues[1], 4, 9);
			if (mAttributeCnt > 2) {
		        InitControl(IDC_SLIDER3, mpNumValues[2], 4, 9);
			}
			if (mAttributeCnt > 3) {
		        InitControl(IDC_SLIDER4, mpNumValues[3], 4, 9);
			}
			if (mAttributeCnt > 4) {
		        InitControl(IDC_SLIDER5, mpNumValues[4], 4, 9);
			}
            result = TRUE;
			break;
        }

        case WM_COMMAND: {
            IdType id = LOWORD(wParam);
            switch (id) {
                case IDC_EDIT1:
                case IDC_EDIT2:
                case IDC_EDIT3:
                case IDC_EDIT4:
                case IDC_EDIT5: {
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
		result = Dialog::HandleMessage(message, wParam);
	}

    return result;
}

void TileBox::InitControl(
	IdType sliderId,
	ValueType value,
	ValueType minValue,
	ValueType maxValue)
{
    ASSERT(value <= maxValue);
    ASSERT(value >= minValue);
	
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

IdType TileBox::MaxId(IdType sliderId) const {
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

IdType TileBox::MinId(IdType sliderId) const {
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

TileBox::ValueType *TileBox::NumValues(void) {
    return mpNumValues;
}

IdType TileBox::SliderId(IdType editboxId) const {
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

IdType TileBox::SliderId(HWND handle) const {
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

void TileBox::UpdateValue(IdType sliderId, ValueType value) {
    switch (sliderId) {
        case IDC_SLIDER1:
            mpNumValues[0] = value;
            break;
        case IDC_SLIDER2:
            mpNumValues[1] = value;
            break;
        case IDC_SLIDER3:
            mpNumValues[2] = value;
            break;
        case IDC_SLIDER4:
            mpNumValues[3] = value;
            break;
        case IDC_SLIDER5:
            mpNumValues[4] = value;
            break;
        default:
            FAIL();
    }
}

