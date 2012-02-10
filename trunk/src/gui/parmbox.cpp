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

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>
#include <Commctrl.h>
#include "gui/parmbox.hpp"
#include "gui/resource.hpp"

// message handler (callback) for this dialog
static INT_PTR CALLBACK parmBoxMessageHandler(
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

ParmBox::ParmBox(Window const &parent):
    Dialog("PARMBOX", parent, &parmBoxMessageHandler)
{}


// misc methods

unsigned ParmBox::Editbox(int editboxId) {
	HWND windowHandle = Handle();

    BOOL success;
    BOOL sign = FALSE;
    unsigned result = ::GetDlgItemInt(windowHandle, editboxId, &success, sign);
    ASSERT(success);
    
    return result;
}

int ParmBox::EditboxId(int sliderId) const {
    int result;
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
        default:
            ASSERT(false);
    }
    
    return result;
}

INT_PTR ParmBox::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
	INT_PTR result = FALSE;
	HWND window = Handle();

    switch (message) {
        case WM_INITDIALOG: {
		    Center();
		    
            unsigned player_cnt = 3;
		    InitControl(IDC_SLIDER1, player_cnt, 1, 9);
            
            unsigned hand_size = 7;
		    InitControl(IDC_SLIDER2, hand_size, 1, 13);
            
            unsigned attribute_cnt = 2;
		    InitControl(IDC_SLIDER3, attribute_cnt, 2, 5);
		    
            result = TRUE;
			break;
        }

        case WM_COMMAND: {
            int id = LOWORD(wParam);
            switch (id) {
                case IDOK:
                    ::EndDialog(window, 0);
                    result = TRUE;
					break;
                case IDC_EDIT1:
                case IDC_EDIT2:
                case IDC_EDIT3: {
                     unsigned value = Editbox(id);
                     int slider_id = SliderId(id);
                     SetControl(slider_id, value);
                     break;
                }
                case IDC_SLIDER1:
                case IDC_SLIDER2:
                case IDC_SLIDER3: {
                     unsigned value = Slider(id);
                     SetControl(id, value);
                     break;
                }
                default:
                     ASSERT(false);
            }
            break;
        }

	    default:
			break;
    }

    return result;
}

void ParmBox::InitControl(int sliderId, unsigned value, unsigned min, unsigned max) {
    ASSERT(value <= max);
    ASSERT(value >= min);
	HWND windowHandle = Handle();
	
    HWND sliderHandle = ::GetDlgItem(windowHandle, sliderId);
    ASSERT(sliderHandle != NULL);
    
    WPARAM redraw = (WPARAM)FALSE;
    ::SendMessage(sliderHandle, TBM_SETRANGEMIN, redraw, (LPARAM)min);
    ::SendMessage(sliderHandle, TBM_SETRANGEMAX, redraw, (LPARAM)max);
    
    SetControl(sliderId, value);
}

void ParmBox::SetControl(int sliderId, unsigned value) {
	HWND windowHandle = Handle();

    HWND sliderHandle = ::GetDlgItem(windowHandle, sliderId);
    ASSERT(sliderHandle != NULL);
    
    // set slider value
    WPARAM redraw = (WPARAM)TRUE;
    ::SendMessage(sliderHandle, TBM_SETPOS, redraw, (LPARAM)value);
    
    // read it back
    unsigned slider_value = Slider(sliderId);

    // set editbox value
    int editboxId = EditboxId(sliderId);     
    BOOL sign = FALSE;
    BOOL success = ::SetDlgItemInt(windowHandle, editboxId, slider_value, sign);
    ASSERT(success);
}

unsigned ParmBox::Slider(int sliderId) {
	HWND windowHandle = Handle();

    HWND sliderHandle = ::GetDlgItem(windowHandle, sliderId);
    ASSERT(sliderHandle != NULL);
    unsigned result = ::SendMessage(sliderHandle, TBM_GETPOS, (WPARAM)0, (LPARAM)0);
    
    return result;
}

int ParmBox::SliderId(int editboxId) const {
    int result;
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
        default:
            ASSERT(false);
    }
    return result;
}

#endif
