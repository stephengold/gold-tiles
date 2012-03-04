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

#include "gui/dialog.hpp"
#include "gui/resource.hpp"  // IDC_BACK
#include "gui/win_types.hpp"
#include "string.hpp"

namespace Win {
#include <WindowsX.h> // Edit_Enable
#include <CommCtrl.h> // TBM_GETPOS
};


// message handler (callback) for generic dialog
static INT_PTR CALLBACK message_handler(
	HWND windowHandle,
	MessageType message,
	WPARAM wParameter, 
	LPARAM lParameter)
{
	lParameter; // unused parameter
	ASSERT(windowHandle != NULL);
    Dialog *p_dialog = (Dialog *)Window::Lookup(windowHandle);

	INT_PTR result;
    ASSERT(HWND(*p_dialog) == windowHandle);
    result = p_dialog->HandleMessage(message, wParameter);

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

int Dialog::Run(Window *pParent) {
	ASSERT(pParent != NULL);
	mspNewlyCreatedWindow = this;

	// create a modal dialog box
	HINSTANCE module = CopyModule(*pParent);
	HWND parent_handle = HWND(*pParent);
	INT_PTR result = Win::DialogBox(module, mTemplateName, parent_handle, mpMessageHandler);

	ASSERT(result > 0);
	return int(result);
}

// misc methods

void Dialog::Close(int result) {
	HWND window = HWND(*this);
    Win::EndDialog(window, INT_PTR(result));
}

void Dialog::EnableButton(IdType buttonId, bool enable) {
    HWND button_handle = GetControlHandle(buttonId);
	BOOL enable_flag = enable ? TRUE : FALSE;
	Win::Button_Enable(button_handle, enable_flag);
}

void Dialog::EnableEditBox(IdType boxId, bool enable) {
    HWND box_handle = GetControlHandle(boxId);
	BOOL enable_flag = enable ? TRUE : FALSE;
	Win::Edit_Enable(box_handle, enable_flag);
}

// get the window handle for a particular control
HWND Dialog::GetControlHandle(IdType controlId) const {
	HWND window_handle = HWND(*this);
    HWND result = Win::GetDlgItem(window_handle, controlId);
    ASSERT(result != NULL);

	return result;
}

// fetch the numeric value of a slider control
Dialog::ValueType Dialog::GetSliderValue(IdType sliderId) {
    HWND slider_handle = GetControlHandle(sliderId);
    ValueType result = Win::SendMessage(slider_handle, TBM_GETPOS, WPARAM(0), LPARAM(0));
    
    return result;
}

// get the text value of a label or editbox control
String Dialog::GetTextString(IdType controlId) {
	HWND window_handle = HWND(*this);
	char buffer[256];
	int buffer_size = 256;
    UINT success = Win::GetDlgItemText(window_handle, controlId, buffer, buffer_size);
	ASSERT(success >= 0);

    String result = String(buffer);

    return result;
}

// get the numeric value of a label or editbox control
Dialog::ValueType Dialog::GetTextValue(IdType controlId) {
	HWND window_handle = HWND(*this);

    BOOL success;
    BOOL sign = FALSE;
    ValueType result = Win::GetDlgItemInt(window_handle, controlId, &success, sign);
    if (!success) {
		result = VALUE_INVALID;
	}
    
    return result;
}

INT_PTR Dialog::HandleMessage(UINT message, WPARAM wParam) {
	INT_PTR result = FALSE;

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

char const *Dialog::Name(void) const {
	return "Dialog Box - Gold Tile";
}

void Dialog::SetButton(IdType buttonId, bool value) {
	HWND button_handle = GetControlHandle(buttonId);

	WPARAM wparam = value ? BST_CHECKED : BST_UNCHECKED;
	LPARAM unused = 0;
    Win::SendMessage(button_handle, BM_SETCHECK, wparam, unused);
}

void Dialog::SetSliderRange(IdType sliderId, ValueType minValue, ValueType maxValue) {
    ASSERT(minValue < maxValue);
	ASSERT(maxValue < VALUE_INVALID);

    HWND slider_handle = GetControlHandle(sliderId);
    
    WPARAM redraw = WPARAM(FALSE);
    Win::SendMessage(slider_handle, TBM_SETRANGEMIN, redraw, LPARAM(minValue));

    redraw = WPARAM(TRUE);
    Win::SendMessage(slider_handle, TBM_SETRANGEMAX, redraw, LPARAM(maxValue));
}

Dialog::ValueType Dialog::SetSliderValue(IdType id, ValueType value) {
    HWND slider_handle = GetControlHandle(id);
    
    // set slider value
    WPARAM redraw = (WPARAM)TRUE;
    Win::SendMessage(slider_handle, TBM_SETPOS, redraw, (LPARAM)value);
    
    // read it back
    ValueType result = GetSliderValue(id);

	return result;
}

// set the text value of a label or editbox control
void Dialog::SetTextString(IdType controlId, String const &rString) {
	HWND window_handle = HWND(*this);

	unsigned length = rString.Length();
    char *copy_text = new char[length + 1];
    ::strcpy_s(copy_text, length + 1, rString.c_str());

    UINT success = Win::SetDlgItemText(window_handle, controlId, copy_text);
	ASSERT(success != 0);

	delete[] copy_text;
}

// change the numeric value displayed by an editbox or static label
void Dialog::SetTextValue(IdType id, ValueType value) {
	HWND window_handle = HWND(*this);

    BOOL sign = FALSE;
    BOOL success = Win::SetDlgItemInt(window_handle, id, value, sign);
    ASSERT(success);

    ASSERT(GetTextValue(id) == value);
}
