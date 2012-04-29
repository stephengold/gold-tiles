// File:     parmbox2.cpp
// Location: src/gui
// Purpose:  implement ParmBox2 class
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
#include "gui/parmbox2.hpp"
#include "gui/resource.hpp"
#include "gui/win_types.hpp"

// message handler (callback) for this dialog box

static INT_PTR CALLBACK message_handler(
    HWND windowHandle,
    MessageType message,
    WPARAM wParameter, 
    LPARAM lParameter)
{
    lParameter;  // unused parameter
    ASSERT(windowHandle != NULL);
    ParmBox2* const p_box = (ParmBox2*)BaseWindow::Lookup(windowHandle);
    ASSERT(HWND(*p_box) == windowHandle);
    INT_PTR const result = p_box->HandleMessage(message, wParameter);

    return result;
}


// lifecycle

ParmBox2::ParmBox2(GameOpt& rGameOpt):
Dialog("PARMBOX2", &message_handler),
    mrGameOpt(rGameOpt)
{
}


// misc methods

IndexType ParmBox2::GetTextIndex(IdType controlId) {
    String const string = GetTextString(controlId);

    IndexType result = Cell::HEIGHT_MAX;
    if (string != "endless") {
        result = IndexType(string);
    }

    return result;
}

INT_PTR ParmBox2::HandleMessage(MessageType message, WPARAM wParam) {
    INT_PTR result = FALSE;

    switch (message) {
    case WM_INITDIALOG: {
        Dialog::HandleMessage(message, wParam);

        SetGrid();
        SetTopology();

        SetTextIndex(IDC_EDITHEIGHT, mrGameOpt.BoardHeight());
        SetTextIndex(IDC_EDITWIDTH, mrGameOpt.BoardWidth());
        UpdateCellCnt();

        result = TRUE;
        break;
                        }

    case WM_COMMAND: {
        IdType const id = LOWORD(wParam);
        int const notification_code = HIWORD(wParam);
        switch (id) {
        case IDC_EDITHEIGHT: {
            RowType height = RowType(GetTextIndex(id));
            if (::is_odd(height)) { // make even
                height++;
            }
            if (height >= Cell::HEIGHT_MIN
                && height <= Cell::HEIGHT_MAX) {
                    mrGameOpt.SetBoardHeight(height);
            }
            SetTopology();
            UpdateCellCnt();
            break;
                             }
        case IDC_EDITWIDTH: {
            ColumnType width = GetTextIndex(id);
            if (::is_odd(width)) { // make even
                width++;
            }
            if (width >= Cell::WIDTH_MIN
                && width <= Cell::WIDTH_MAX) {
                    mrGameOpt.SetBoardWidth(width);
            }
            SetTopology();
            UpdateCellCnt();
            break;
                            }
        case IDC_RECALC:
            if (notification_code == BN_CLICKED) {
                SetTopology();
                SetTextIndex(IDC_EDITHEIGHT, mrGameOpt.BoardHeight());
                SetTextIndex(IDC_EDITWIDTH, mrGameOpt.BoardWidth());
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
                SetTextIndex(IDC_EDITHEIGHT, mrGameOpt.BoardHeight());
                SetTextIndex(IDC_EDITWIDTH, mrGameOpt.BoardWidth());
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
                SetTextIndex(IDC_EDITHEIGHT, mrGameOpt.BoardHeight());
                SetTextIndex(IDC_EDITWIDTH, mrGameOpt.BoardWidth());
                UpdateCellCnt();
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

void ParmBox2::SetGrid(void) {
    EnableControl(IDC_TRIANGLE, false); // TODO
    EnableControl(IDC_4WAY, true);
    EnableControl(IDC_HEX, true);
    EnableControl(IDC_8WAY, true);

    GridType const grid = GridType(mrGameOpt);
    SetButton(IDC_TRIANGLE, grid == GRID_TRIANGLE);
    SetButton(IDC_4WAY, grid == GRID_4WAY);
    SetButton(IDC_HEX, grid == GRID_HEX);
    SetButton(IDC_8WAY, grid == GRID_8WAY);    
}

void ParmBox2::SetGrid(IdType buttonId) {
    switch (buttonId) {
    case IDC_TRIANGLE:
        mrGameOpt.SetGrid(GRID_TRIANGLE);
        break;
    case IDC_4WAY:
        mrGameOpt.SetGrid(GRID_4WAY);
        break;
    case IDC_HEX:
        mrGameOpt.SetGrid(GRID_HEX);
        break;
    case IDC_8WAY:
        mrGameOpt.SetGrid(GRID_8WAY);
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
    EnableControl(IDC_EDITHEIGHT, true);
    EnableControl(IDC_EDITWIDTH, true);
    EnableControl(IDC_ENDLESS, true);
    EnableControl(IDC_RECT, true);
    EnableControl(IDC_HSTRIP, true);
    EnableControl(IDC_VSTRIP, true);
    EnableControl(IDC_TORUS, true);
    EnableControl(IDC_VCYLINDER, true);
    EnableControl(IDC_HCYLINDER, true);

    bool const wrap = mrGameOpt.DoesBoardWrap();
    bool const finite_height = mrGameOpt.HasFiniteHeight();
    bool const finite_width = mrGameOpt.HasFiniteWidth();
    bool const inf_height = !finite_height;
    bool const inf_width = !finite_width;

    SetButton(IDC_ENDLESS, inf_height && inf_width);
    SetButton(IDC_RECT, !wrap && finite_height && finite_width);
    SetButton(IDC_TORUS, wrap && finite_height && finite_width);
    SetButton(IDC_VSTRIP, !wrap && inf_height && finite_width);
    SetButton(IDC_HSTRIP, !wrap && finite_height && inf_width);
    SetButton(IDC_VCYLINDER, wrap && inf_height && finite_width);
    SetButton(IDC_HCYLINDER, wrap && finite_height && inf_width);
}

void ParmBox2::SetTopology(IdType buttonId) {
    // update board height and width
    RowType height = mrGameOpt.BoardHeight();
    ColumnType width = mrGameOpt.BoardWidth();

    switch (buttonId) {
    case IDC_ENDLESS:
        height = Cell::HEIGHT_MAX;
        width = Cell::WIDTH_MAX;
        break;

    case IDC_RECT:
    case IDC_TORUS:
        if (height == Cell::HEIGHT_MAX) {
            height = HEIGHT_DEFAULT;
        }
        if (width == Cell::WIDTH_MAX) {
            width = WIDTH_DEFAULT;
        }
        break;

    case IDC_VSTRIP:
    case IDC_VCYLINDER:
        height = Cell::HEIGHT_MAX;
        if (width == Cell::WIDTH_MAX) {
            width = WIDTH_DEFAULT;
        }
        break;

    case IDC_HSTRIP:
    case IDC_HCYLINDER:
        if (height == Cell::HEIGHT_MAX) {
            height = HEIGHT_DEFAULT;
        }
        width = Cell::WIDTH_MAX;
        break;

    default:
        FAIL();
    }

    mrGameOpt.SetBoardHeight(height);
    mrGameOpt.SetBoardWidth(width);

    // update wrap flag
    bool does_wrap = mrGameOpt.DoesBoardWrap();
    switch (buttonId) {
    case IDC_ENDLESS:
        // mWrapFlag doesn't matter
        break;

    case IDC_HSTRIP:
    case IDC_RECT:
    case IDC_VSTRIP:
        does_wrap = false;
        break;

    case IDC_HCYLINDER:
    case IDC_TORUS:
    case IDC_VCYLINDER:
        does_wrap = true;
        break;

    default:
        FAIL();
    }
    mrGameOpt.SetDoesBoardWrap(does_wrap);
}

void ParmBox2::UpdateCellCnt(void) {
    IndexType cell_cnt = Cell::HEIGHT_MAX;

    if (mrGameOpt.HasFiniteHeight() && mrGameOpt.HasFiniteWidth()) {
        RowType const height = mrGameOpt.BoardHeight();
        ColumnType const width = mrGameOpt.BoardWidth();
        if (GridType(mrGameOpt) == GRID_HEX) { // hex grids have half as many cells
            cell_cnt = (height/2) * width;
        } else {
            cell_cnt = height * width;
        }
    }
    SetTextIndex(IDC_CELLCNT, cell_cnt);
}
#endif // defined(_WINDOWS)
