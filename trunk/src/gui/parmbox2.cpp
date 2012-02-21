// File:    parmbox2.cpp
// Purpose: ParmBox2 class
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

#include "gui/parmbox2.hpp"
#include "gui/resource.hpp"


// message handler (callback) for this dialog box

static INT_PTR CALLBACK message_handler(
	HWND windowHandle,
	UINT message,
	WPARAM wParameter, 
	LPARAM lParameter)
{
	ASSERT(windowHandle != NULL);
    ParmBox2 *p_box = (ParmBox2 *)Window::Lookup(windowHandle);
    ASSERT(p_box->Handle() == windowHandle);
	INT_PTR result = p_box->HandleMessage(message, wParameter, lParameter);

	return result;
}


// lifecycle

ParmBox2::ParmBox2(
	bool wrapFlag,
	IndexType height,
	IndexType width,
	GridType grid)
:
    Dialog("PARMBOX2", &message_handler)
{
	mGrid =grid;
	mHeight = height;
	mWidth = width;
	mWrapFlag = wrapFlag;
}

// operators

ParmBox2::operator GridType(void) const {
	return mGrid;
}

// misc methods

IndexType ParmBox2::GetTextIndex(IdType controlId) {
	String string = GetTextString(controlId);

	IndexType result = Cell::HEIGHT_MAX;
	if (string != "endless") {
		result = unsigned(string);
	}

    return result;
}

INT_PTR ParmBox2::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
	INT_PTR result = FALSE;

    switch (message) {
        case WM_INITDIALOG: {
		    Dialog::HandleMessage(message, wParam, lParam);
		    
			SetGrid();
			SetTopology();

			SetTextIndex(IDC_EDITHEIGHT, mHeight);
			SetTextIndex(IDC_EDITWIDTH, mWidth);
			UpdateCellCnt();

            result = TRUE;
			break;
        }

        case WM_COMMAND: {
            IdType id = LOWORD(wParam);
			int notification_code = HIWORD(wParam);
            switch (id) {
                case IDC_EDITHEIGHT: {
                    ValueType value = GetTextIndex(id);
					if (value >= Cell::HEIGHT_MIN
					 && value <= Cell::HEIGHT_MAX) {
						if (::is_odd(value)) { // make even
							mHeight = value + 1;
						} else {
							mHeight = value;
						}
			            UpdateCellCnt();
					}
                    break;
                }
                case IDC_EDITWIDTH: {
                    ValueType value = GetTextIndex(id);
					if (value >= Cell::WIDTH_MIN
					 && value <= Cell::WIDTH_MAX) {
						if (::is_odd(value)) { // make even
							mWidth = value + 1;
						} else {
							mWidth = value;
						}
						UpdateCellCnt();
					}
                    break;
                }
				case IDC_RECALC:
					if (notification_code == BN_CLICKED) {
                        SetTopology();
            			SetTextIndex(IDC_EDITHEIGHT, mHeight);
			            SetTextIndex(IDC_EDITWIDTH, mWidth);
						UpdateCellCnt();
					}
					break;
				case IDC_TRIANGLE:
				case IDC_4WAY:
				case IDC_HEX:
				case IDC_8WAY:
					if (notification_code == BN_CLICKED) {
                        SetGrid(id);
                        SetTopology();
            			SetTextIndex(IDC_EDITHEIGHT, mHeight);
			            SetTextIndex(IDC_EDITWIDTH, mWidth);
						UpdateCellCnt();
					}
                    break;
				
				case IDC_ENDLESS:
				case IDC_RECT:
				case IDC_TORUS:
				case IDC_VSTRIP:
				case IDC_HSTRIP:
				case IDC_VCYLINDER:
				case IDC_HCYLINDER:
					if (notification_code == BN_CLICKED) {
                        SetTopology(id);
                        SetGrid();
            			SetTextIndex(IDC_EDITHEIGHT, mHeight);
			            SetTextIndex(IDC_EDITWIDTH, mWidth);
						UpdateCellCnt();
					}
                    break;
            }
            break;
        }
    }

	if (result == FALSE) {
		result = Dialog::HandleMessage(message, wParam, lParam);
	}

    return result;
}

IndexType ParmBox2::Height(void) const {
	return mHeight;
}

void ParmBox2::SetGrid(void) {
	SetButton(IDC_TRIANGLE, mGrid == GRID_TRIANGLE);
	SetButton(IDC_4WAY, mGrid == GRID_4WAY);
	SetButton(IDC_HEX, mGrid == GRID_HEX);
	SetButton(IDC_8WAY, mGrid == GRID_8WAY);    
}

void ParmBox2::SetGrid(IdType buttonId) {
	switch (buttonId) {
		case IDC_TRIANGLE:
			mGrid = GRID_TRIANGLE;
			break;
		case IDC_4WAY:
			mGrid = GRID_4WAY;
			break;
		case IDC_HEX:
			mGrid = GRID_HEX;
			break;
		case IDC_8WAY:
			mGrid = GRID_8WAY;
			break;
		default:
			FAIL();
	}
}

void ParmBox2::SetTextIndex(IdType controlId, ValueType value) {
	if (value == Cell::HEIGHT_MAX) {
		SetTextString(controlId, "endless");
	} else {
		SetTextValue(controlId, value);
	}
}

void ParmBox2::SetTopology(void) {
#if 1
	// TODO
	EnableButton(IDC_ENDLESS, true);
	EnableButton(IDC_RECT, false);
	EnableButton(IDC_TORUS, false);
	EnableButton(IDC_VSTRIP, false);
	EnableButton(IDC_HSTRIP, false);
	EnableButton(IDC_VCYLINDER, false);
	EnableButton(IDC_HCYLINDER, false);
	mHeight = Cell::HEIGHT_MAX;
	mWidth = Cell::WIDTH_MAX;
	EnableEditBox(IDC_EDITHEIGHT, false);
	EnableEditBox(IDC_EDITWIDTH, false);
#endif

	SetButton(IDC_ENDLESS, mHeight == Cell::HEIGHT_MAX 
	                    && mWidth == Cell::WIDTH_MAX);
	SetButton(IDC_RECT, !mWrapFlag 
		             && mHeight < Cell::HEIGHT_MAX 
					 && mWidth < Cell::WIDTH_MAX);
	SetButton(IDC_TORUS, mWrapFlag
		              && mHeight < Cell::HEIGHT_MAX 
					  && mWidth < Cell::WIDTH_MAX);
	SetButton(IDC_VSTRIP, !mWrapFlag
		              && mHeight == Cell::HEIGHT_MAX 
					  && mWidth < Cell::WIDTH_MAX);
	SetButton(IDC_HSTRIP, !mWrapFlag
		              && mHeight < Cell::HEIGHT_MAX 
					  && mWidth == Cell::WIDTH_MAX);
	SetButton(IDC_VCYLINDER, mWrapFlag
		              && mHeight == Cell::HEIGHT_MAX 
					  && mWidth < Cell::WIDTH_MAX);
	SetButton(IDC_HCYLINDER, mWrapFlag
		              && mHeight < Cell::HEIGHT_MAX 
					  && mWidth == Cell::WIDTH_MAX);
}

void ParmBox2::SetTopology(IdType buttonId) {
	switch (buttonId) {
		case IDC_ENDLESS:
			mHeight = Cell::HEIGHT_MAX;
			mWidth = Cell::WIDTH_MAX;
			break;

		case IDC_RECT:
		case IDC_TORUS:
			if (mHeight == Cell::HEIGHT_MAX) {
				mHeight = HEIGHT_DEFAULT;
			}
			if (mWidth == Cell::WIDTH_MAX) {
				mWidth = WIDTH_DEFAULT;
			}
			break;

		case IDC_VSTRIP:
		case IDC_VCYLINDER:
			mHeight = Cell::HEIGHT_MAX;
			if (mWidth == Cell::WIDTH_MAX) {
				mWidth = WIDTH_DEFAULT;
			}
			break;

		case IDC_HSTRIP:
		case IDC_HCYLINDER:
			if (mHeight == Cell::HEIGHT_MAX) {
				mHeight = HEIGHT_DEFAULT;
			}
			mWidth = Cell::WIDTH_MAX;
			break;

		default:
			FAIL();
	}

	// set mWrapFlag
	switch (buttonId) {
		case IDC_ENDLESS:
			// mWrapFlag doesn't matter
			break;

		case IDC_HSTRIP:
		case IDC_RECT:
		case IDC_VSTRIP:
			mWrapFlag = false;
			break;

		case IDC_HCYLINDER:
		case IDC_TORUS:
		case IDC_VCYLINDER:
			mWrapFlag = true;
			break;

		default:
			FAIL();
	}
}

void ParmBox2::UpdateCellCnt(void) {
	IndexType cell_cnt = Cell::HEIGHT_MAX;
	if (mHeight < Cell::HEIGHT_MAX && mWidth < Cell::WIDTH_MAX) {
        IndexType max = (mHeight < mWidth) ? mWidth : mHeight;
        mHeight = max;
        mWidth = max;
		if (mGrid == GRID_HEX) { // hex grids have half as many cells
		    cell_cnt = (mHeight/2) * mWidth;
		} else {
		    cell_cnt = mHeight * mWidth;
		}
	}
    SetTextIndex(IDC_CELLCNT, cell_cnt);
}

IndexType ParmBox2::Width(void) const {
	return mWidth;
}

// inquiry methods

bool ParmBox2::DoesWrap(void) const {
	return mWrapFlag;
}
