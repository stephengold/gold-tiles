// File:    dialog.cpp
// Purpose: Dialog class
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
#include <commctrl.h>
#include "gui/dialog.hpp"

Dialog *gpNewlyCreatedDialog = NULL;

// message handler (callback) for generic dialog
static INT_PTR CALLBACK dialogMessageHandler(
	HWND windowHandle,
	UINT message,
	WPARAM wParameter, 
	LPARAM lParameter)
{
    Dialog *p_dialog;
	if (gpNewlyCreatedDialog != NULL) {
		p_dialog = gpNewlyCreatedDialog;
		gpNewlyCreatedDialog = NULL;
		p_dialog->SetHandle(windowHandle);
	} else {
       p_dialog = (Dialog *)Window::Lookup(windowHandle);
	}

	INT_PTR result;
    ASSERT(p_dialog->Handle() == windowHandle);
    result = p_dialog->HandleMessage(message, wParameter, lParameter);

	return result;
}

// lifecycle

Dialog::Dialog(char const *templateName, Window *pParent) {
	gpNewlyCreatedDialog = this;
	Initialize(templateName, pParent, &dialogMessageHandler);
}

Dialog::Dialog(char const *templateName, Window *pParent, DLGPROC messageHandler) {
	gpNewlyCreatedDialog = this;
	Initialize(templateName, pParent, messageHandler);
}

void Dialog::Initialize(
	char const *templateName,
	Window *pParent,
	DLGPROC messageHandler)
{
	// create a modal dialog box
	HINSTANCE module = CopyModule(*pParent);
	HWND parentHandle = pParent->Handle();
	mResult = ::DialogBox(module, templateName, parentHandle, messageHandler);
	ASSERT(mResult > 0);
}

// misc methods

void Dialog::Close(INT_PTR result) {
	HWND window = Handle();
    ::EndDialog(window, result);
}

// get the window handle for a particular control
HWND Dialog::GetControlHandle(IdType controlId) const {
	HWND window_handle = Handle();
    HWND result = ::GetDlgItem(window_handle, controlId);
    ASSERT(result != NULL);

	return result;
}

// fetch the numeric value of a slider control
Dialog::ValueType Dialog::GetSliderValue(IdType sliderId) {
    HWND sliderHandle = GetControlHandle(sliderId);
    ValueType result = ::SendMessage(sliderHandle, TBM_GETPOS, (WPARAM)0, (LPARAM)0);
    
    return result;
}

// get the value of a numeric label or editbox control
Dialog::ValueType Dialog::GetTextValue(IdType controlId) {
	HWND windowHandle = Handle();

    BOOL success;
    BOOL sign = FALSE;
    ValueType result = ::GetDlgItemInt(windowHandle, controlId, &success, sign);
    if (!success) {
		result = VALUE_INVALID;
	}
    
    return result;
}

INT_PTR Dialog::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
	INT_PTR result = FALSE;
	HWND window = Handle();

    switch (message) {
        case WM_INITDIALOG:
		    Center();
            result = TRUE;
			break;

        case WM_COMMAND: {
			IdType id = LOWORD(wParam);
            switch (id) {
                case IDCANCEL:
                    Close(RESULT_CANCEL);
                    result = TRUE;
					break;
                case IDOK:
                    Close(RESULT_OK);
                    result = TRUE;
					break;
            }
            break;
		}

	    default:
			break;
    }

    return result;
}

int Dialog::Result(void) const {
	int result = mResult;

	return result;
}

void Dialog::SetSliderRange(IdType sliderId, ValueType minValue, ValueType maxValue) {
    ASSERT(minValue < maxValue);
	ASSERT(maxValue < VALUE_INVALID);

    HWND slider_handle = GetControlHandle(sliderId);
    
    WPARAM redraw = WPARAM(FALSE);
    ::SendMessage(slider_handle, TBM_SETRANGEMIN, redraw, LPARAM(minValue));

    redraw = WPARAM(TRUE);
    ::SendMessage(slider_handle, TBM_SETRANGEMAX, redraw, LPARAM(maxValue));
}

Dialog::ValueType Dialog::SetSliderValue(IdType id, ValueType value) {
    HWND sliderHandle = GetControlHandle(id);
    
    // set slider value
    WPARAM redraw = (WPARAM)TRUE;
    ::SendMessage(sliderHandle, TBM_SETPOS, redraw, (LPARAM)value);
    
    // read it back
    ValueType result = GetSliderValue(id);

	return result;
}

// change the numeric value displayed by an editbox or static label
void Dialog::SetTextValue(IdType id, ValueType value) {
	HWND windowHandle = Handle();

    BOOL sign = FALSE;
    BOOL success = ::SetDlgItemInt(windowHandle, id, value, sign);
    ASSERT(success);

    ASSERT(GetTextValue(id) == value);
}

#endif
