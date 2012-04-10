// File:    attrbox.cpp
// Purpose: AttrBox class
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
#include "gui/attrbox.hpp"
#include "gui/resource.hpp"
#include "gui/markings.hpp"
#include "gui/win_types.hpp"

// message handler (callback) for this dialog
static INT_PTR CALLBACK message_handler(
	HWND windowHandle,
	MessageType message,
	WPARAM wParameter, 
	LPARAM lParameter)
{
	ASSERT(windowHandle != NULL);
    AttrBox * const p_box = (AttrBox *)BaseWindow::Lookup(windowHandle);
    ASSERT(HWND(*p_box) == windowHandle);
	INT_PTR const result = p_box->HandleMessage(message, wParameter, lParameter);

	return result;
}


// lifecycle

AttrBox::AttrBox(DisplayModes const &rDisplayModes):
    Dialog("ATTRBOX", &message_handler),
	mDisplayModes(rDisplayModes)
{}


// operators

AttrBox::operator DisplayModes(void) const {
	return mDisplayModes;
}


// misc methods

/* static */ AttrIndexType AttrBox::AttrIndex(IdType listboxId) {
	AttrIndexType result = 0;

    switch (listboxId) {
        case IDC_LIST1:
            result = 0;
            break;
        case IDC_LIST2:
            result = 1;
            break;
        case IDC_LIST3:
            result = 2;
            break;
        case IDC_LIST4:
            result = 3;
            break;
        case IDC_LIST5:
            result = 4;
            break;
        default:
            FAIL();
    }

	return result;
}

void AttrBox::BumpDisplayMode(AttrIndexType ind) {
	ASSERT(ind < Tile::AttributeCnt());

	// list all available modes
	Indices unused_modes;
	for (IndexType i_mode = ATTR_MODE_MIN; i_mode <= ATTR_MODE_MAX; i_mode++) {
		unused_modes.Add(i_mode);
	}

	// remove modes used for other attributes
	for (AttrIndexType i_attr = 0; i_attr < Tile::AttributeCnt(); i_attr++) {
		if (i_attr != ind) {
			ValueType mode = mDisplayModes.Mode(i_attr);
			if (unused_modes.Contains(mode)) {
			    unused_modes.Remove(mode);
			}
		}
	}
	IndexType const i_mode = unused_modes.First();
	AttrModeType const mode = AttrModeType(i_mode);
	mDisplayModes.SetMode(ind, mode);

	IdType const listbox_id = ListboxId(ind);
	SetListboxSelection(listbox_id, mode); // relies on assertion in HandleMessage()
}

/* static */ char const * AttrBox::DisplayModeText(AttrModeType disp) {
	char const * result = "";

	switch (disp) {
	    case ATTR_MODE_COLOR:
			result = "colors";
			break;
		case ATTR_MODE_SHAPE:
			result = "shapes";
			break;
		case ATTR_MODE_ABC:
			result = "ABC...";
			break;
		case ATTR_MODE_RST:
			result = "RST...";
			break;
		case ATTR_MODE_123:
			result = "123...";
			break;
        default:
            FAIL();
	}

	return result;
}

INT_PTR AttrBox::HandleMessage(MessageType message, WPARAM wParam, LPARAM lParam) {
	lParam;
	INT_PTR result = FALSE;

    switch (message) {
        case WM_INITDIALOG: {
		    Dialog::HandleMessage(message, wParam);

			for (AttrIndexType i_attr = 0; i_attr < Tile::ATTRIBUTE_CNT_MAX; i_attr++) {
				bool const enable_flag = (i_attr < Tile::AttributeCnt());
				IdType const listbox_id = ListboxId(i_attr);
				EnableControl(listbox_id, enable_flag);

				for (unsigned i_mode = ATTR_MODE_MIN; i_mode <= ATTR_MODE_MAX; i_mode++) {
					AttrModeType const mode = AttrModeType(i_mode);
				    char const * const text = DisplayModeText(mode);
					unsigned const item_index = AddListboxItem(listbox_id, text);
					ASSERT(item_index == i_mode); // BumpDisplayMode() relies on this
				}

			    int selected_value = -1;
				if (enable_flag) {
				    selected_value = mDisplayModes.Mode(i_attr);
				}
                SetListboxSelection(listbox_id, selected_value);
			}

            result = TRUE;
			break;
        }

        case WM_COMMAND: {
            IdType const id = LOWORD(wParam);
			int const notification_code = HIWORD(wParam);
            switch (id) {
                case IDC_LIST1:
                case IDC_LIST2:
                case IDC_LIST3:
                case IDC_LIST4:
                case IDC_LIST5:
                    if (notification_code == LBN_SELCHANGE) {
						ValueType const value = GetListboxSelection(id);
						UpdateValue(id, AttrModeType(value)); 
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

/* static */ IdType AttrBox::ListboxId(AttrIndexType ind) {
	IdType result = 0;

	switch (ind) {
	    case 0:
		    result = IDC_LIST1;
		    break;
	    case 1:
		    result = IDC_LIST2;
		    break;
	    case 2:
		    result = IDC_LIST3;
		    break;
	    case 3:
		    result = IDC_LIST4;
		    break;
	    case 4:
		    result = IDC_LIST5;
		    break;
        default:
            FAIL();
	}

	return result;
}

void AttrBox::UpdateValue(IdType listboxId, AttrModeType value) {
	ASSERT(value != VALUE_INVALID);

	AttrIndexType const ind = AttrIndex(listboxId);
    mDisplayModes.SetMode(ind, value);

	// only one attribute can use color
	if (value == ValueType(ATTR_MODE_COLOR)) {
		// check whether another attribute is already using color
		for (AttrIndexType i_attr = 0; i_attr < Tile::AttributeCnt(); i_attr++) {
			if (i_attr != ind && mDisplayModes.IsColor(i_attr)) {
				BumpDisplayMode(i_attr);
				break;
			}
		}
	}

	// can't display more than 4 markings per tile
	AttrCntType marking_cnt = mDisplayModes.MarkingCnt();
	if (marking_cnt > Markings::MARKING_CNT_MAX) {
		AttrIndexType new_color_index = 0;
		if (ind == new_color_index) {
			new_color_index++;
		}
		AttrModeType const mode = ATTR_MODE_COLOR;
		mDisplayModes.SetMode(new_color_index, mode);

	    IdType const listbox_id = ListboxId(new_color_index);
	    SetListboxSelection(listbox_id, mode); // relies on assertion in HandleMessage()

		marking_cnt = mDisplayModes.MarkingCnt();
	}
	ASSERT(marking_cnt <= Markings::MARKING_CNT_MAX);
}
#endif // defined(_WINDOWS)
