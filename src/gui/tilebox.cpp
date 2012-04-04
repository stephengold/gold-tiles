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

#ifdef _WINDOWS
#include "string.hpp"
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
    TileBox * const p_box = (TileBox *)Window::Lookup(windowHandle);
    ASSERT(HWND(*p_box) == windowHandle);
	INT_PTR const result = p_box->HandleMessage(message, wParameter, lParameter);

	return result;
}


// lifecycle

TileBox::TileBox(GameOpt &rGameOpt):
    Dialog("TILEBOX", &message_handler),
	mrGameOpt(rGameOpt)
{
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
		    
			AttrType const attr_cnt = mrGameOpt.AttrCnt();
			InitControl(IDC_SLIDER1, mrGameOpt.CountAttrValues(0), attr_cnt > 0);
		    InitControl(IDC_SLIDER2, mrGameOpt.CountAttrValues(1), attr_cnt > 1);
	        InitControl(IDC_SLIDER3, mrGameOpt.CountAttrValues(2), attr_cnt > 2);
	        InitControl(IDC_SLIDER4, mrGameOpt.CountAttrValues(3), attr_cnt > 3);
	        InitControl(IDC_SLIDER5, mrGameOpt.CountAttrValues(4), attr_cnt > 4);

			UpdateTileCnt();
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
                    ValueType value = GetTextValue(id);
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

void TileBox::InitControl(
	IdType sliderId,
	ValueType value,
	bool enableFlag)
{
	AttrType const min_value = Tile::VALUE_CNT_MIN;
	AttrType const max_value = Tile::VALUE_CNT_MAX;
    ASSERT(value <= max_value);
    ASSERT(value >= min_value);
	
	SetSliderRange(sliderId, min_value, max_value);
	ValueType const slider_value = SetSliderValue(sliderId, value);
	ASSERT(slider_value == value);
	EnableControl(sliderId, enableFlag);

	IdType const min_id = MinId(sliderId);
	SetTextValue(min_id, min_value);
	EnableControl(min_id, enableFlag);

	IdType const max_id = MaxId(sliderId);
	SetTextValue(max_id, max_value);
	EnableControl(max_id, enableFlag);

	IdType const editbox_id = EditboxId(sliderId);
    SetTextValue(editbox_id, slider_value);
	EnableControl(editbox_id, enableFlag);
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

void TileBox::UpdateTileCnt(void) {
	ValueType const combo_cnt = mrGameOpt.ComboCnt();
	ValueType const clone_cnt = mrGameOpt.CloneCnt();
	ValueType const total_tile_cnt = mrGameOpt.TotalTileCnt();

	String const combo_cnt_string = ::plural(combo_cnt, "combination");
	String const clone_cnt_string = ::plural(clone_cnt, "clone");
	String const total_tile_cnt_string = ::plural(total_tile_cnt, "total tile");

	SetTextString(IDC_COMBINATIONS, combo_cnt_string);
    SetTextString(IDC_CLONES, clone_cnt_string);
    SetTextString(IDC_TOTAL_TILES, total_tile_cnt_string);
}

void TileBox::UpdateValue(IdType sliderId, ValueType value) {
	AttrType const num_values = AttrType(value);
    switch (sliderId) {
        case IDC_SLIDER1:
            mrGameOpt.SetNumAttrValues(0, num_values);
            break;
        case IDC_SLIDER2:
            mrGameOpt.SetNumAttrValues(1, num_values);
            break;
        case IDC_SLIDER3:
            mrGameOpt.SetNumAttrValues(2, num_values);
            break;
        case IDC_SLIDER4:
            mrGameOpt.SetNumAttrValues(3, num_values);
            break;
        case IDC_SLIDER5:
            mrGameOpt.SetNumAttrValues(4, num_values);
            break;
        default:
            FAIL();
    }
	UpdateTileCnt();
}
#endif // defined(_WINDOWS)
