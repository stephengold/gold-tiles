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
#include <windowsx.h>
#include <commctrl.h>
#include "gui/dialog.hpp"
#include "gui/resource.hpp"
#include "string.hpp"

// message handler (callback) for generic dialog
static INT_PTR CALLBACK message_handler(
	HWND windowHandle,
	UINT message,
	WPARAM wParameter, 
	LPARAM lParameter)
{
	ASSERT(windowHandle != NULL);
    Dialog *p_dialog = (Dialog *)Window::Lookup(windowHandle);

	INT_PTR result;
    ASSERT(p_dialog->Handle() == windowHandle);
    result = p_dialog->HandleMessage(message, wParameter, lParameter);

	return result;
}

// lifecycle

Dialog::Dialog(char const *templateName) {
	mTemplateName = templateName;
	mpMessageHandler = &message_handler;
}

Dialog::Dialog(char const *templateName, DLGPROC messageHandler) {
	mTemplateName = templateName;
	mpMessageHandler = messageHandler;
}

INT_PTR Dialog::Run(Window *pParent) {
	mspNewlyCreatedWindow = this;

	// create a modal dialog box
	HINSTANCE module = CopyModule(*pParent);
	HWND parentHandle = pParent->Handle();
	INT_PTR result = ::DialogBox(module, mTemplateName, parentHandle, mpMessageHandler);

	ASSERT(result > 0);
	return result;
}

// misc methods

void Dialog::Close(INT_PTR result) {
	HWND window = Handle();
    ::EndDialog(window, result);
}

void Dialog::EnableButton(IdType buttonId, bool enable) {
    HWND button_handle = GetControlHandle(buttonId);
	BOOL enable_flag = enable ? TRUE : FALSE;
	Button_Enable(button_handle, enable_flag);
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
    HWND slider_handle = GetControlHandle(sliderId);
    ValueType result = ::SendMessage(slider_handle, TBM_GETPOS, (WPARAM)0, (LPARAM)0);
    
    return result;
}

// get the text value of a label or editbox control
String Dialog::GetTextString(IdType controlId) {
	HWND window_handle = Handle();
	char buffer[256];
	int buffer_size = 256;
    UINT success = ::GetDlgItemText(window_handle, controlId, buffer, buffer_size);

    String result = String(buffer);

    return result;
}

// get the numeric value of a label or editbox control
Dialog::ValueType Dialog::GetTextValue(IdType controlId) {
	HWND window_handle = Handle();

    BOOL success;
    BOOL sign = FALSE;
    ValueType result = ::GetDlgItemInt(window_handle, controlId, &success, sign);
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
                case IDC_BACK:
                    Close(RESULT_BACK);
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

void Dialog::SetButton(IdType buttonId, bool value) {
	HWND button_handle = GetControlHandle(buttonId);

	WPARAM wparam = value ? BST_CHECKED : BST_UNCHECKED;
	LPARAM unused = 0;
    ::SendMessage(button_handle, BM_SETCHECK, wparam, unused);
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
    HWND slider_handle = GetControlHandle(id);
    
    // set slider value
    WPARAM redraw = (WPARAM)TRUE;
    ::SendMessage(slider_handle, TBM_SETPOS, redraw, (LPARAM)value);
    
    // read it back
    ValueType result = GetSliderValue(id);

	return result;
}

// set the text value of a label or editbox control
void Dialog::SetTextString(IdType controlId, String const &rString) {
	HWND window_handle = Handle();

	unsigned length = rString.Length();
    char *copy_text = new char[length + 1];
    ::strcpy_s(copy_text, length + 1, rString.c_str());

    UINT success = ::SetDlgItemText(window_handle, controlId, copy_text);
	ASSERT(success != 0);

	delete[] copy_text;
}

// change the numeric value displayed by an editbox or static label
void Dialog::SetTextValue(IdType id, ValueType value) {
	HWND window_handle = Handle();

    BOOL sign = FALSE;
    BOOL success = ::SetDlgItemInt(window_handle, id, value, sign);
    ASSERT(success);

    ASSERT(GetTextValue(id) == value);
}

#endif
