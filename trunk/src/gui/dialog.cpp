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

#ifdef _WINDOWS
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
    Dialog *p_dialog = (Dialog *)BaseWindow::Lookup(windowHandle);

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

int Dialog::Run(BaseWindow *pParent) {
	ASSERT(pParent != NULL);
	mspNewlyCreatedWindow = this;

	// create a modal dialog box
	HINSTANCE const module = CopyModule(*pParent);
	HWND const parent_handle = HWND(*pParent);
	INT_PTR const result = Win::DialogBox(module, mTemplateName, parent_handle, mpMessageHandler);

	ASSERT(result > 0);
	return int(result);
}


// misc methods

Dialog::ValueType Dialog::AddListboxItem(IdType listboxId, char const *text) {
	HWND const listbox_handle = GetControlHandle(listboxId);

	WPARAM const unused = 0;
	LRESULT const success = Win::SendMessage(listbox_handle, LB_ADDSTRING, unused, LPARAM(text));
	ASSERT(success != LB_ERR);
	ValueType result = success;

	return result;
}

void Dialog::Close(int result) {
	HWND const window = HWND(*this);
    Win::EndDialog(window, INT_PTR(result));
}

void Dialog::EnableControl(IdType controlId, bool enable) {
    HWND const control_handle = GetControlHandle(controlId);
	BOOL const enable_flag = enable ? TRUE : FALSE;
	Win::EnableWindow(control_handle, enable_flag);
}

// get the window handle for a particular control
HWND Dialog::GetControlHandle(IdType controlId) const {
	HWND const window_handle = HWND(*this);
    HWND const result = Win::GetDlgItem(window_handle, controlId);
    ASSERT(result != NULL);

	return result;
}

// fetch the numeric value of a listbox control
Dialog::ValueType Dialog::GetListboxSelection(IdType sliderId) {
    HWND const listbox_handle = GetControlHandle(sliderId);

    // Get selected index.
	WPARAM const unused_w = 0;
	LPARAM const unused_l = 0;
    ValueType const result = Win::SendMessage(listbox_handle, LB_GETCURSEL, unused_w, unused_l);
	ASSERT(result != LB_ERR);
    
    return result;
}

// fetch the numeric value of a slider control
Dialog::ValueType Dialog::GetSliderValue(IdType sliderId) {
    HWND const slider_handle = GetControlHandle(sliderId);
	WPARAM const unused_w = 0;
	LPARAM const unused_l = 0;
    ValueType const result = Win::SendMessage(slider_handle, TBM_GETPOS, unused_w, unused_l);
    
    return result;
}

// get the text value of a label or editbox control
String Dialog::GetTextString(IdType controlId) {
	HWND const window_handle = HWND(*this);
	char buffer[256];
	int const buffer_size = 256;
    UINT success = Win::GetDlgItemText(window_handle, controlId, buffer, buffer_size);
	ASSERT(success >= 0);

    String const result = String(buffer);

    return result;
}

// get the numeric value of a label or editbox control
Dialog::ValueType Dialog::GetTextValue(IdType controlId) {
	HWND window_handle = HWND(*this);

    BOOL success = FALSE;
    BOOL const sign = FALSE;
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
			IdType const id = LOWORD(wParam);
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
	HWND const button_handle = GetControlHandle(buttonId);

	WPARAM const wparam = value ? BST_CHECKED : BST_UNCHECKED;
	LPARAM const unused = 0;
    Win::SendMessage(button_handle, BM_SETCHECK, wparam, unused);
}

void Dialog::SetListboxSelection(IdType listboxId, ValueType position) {
	HWND const listbox_handle = GetControlHandle(listboxId);

	LPARAM unused = 0;
	WPARAM wparam = WPARAM(-1);
	if (position != VALUE_INVALID) {
		wparam = WPARAM(position);
	}
	LRESULT const success = Win::SendMessage(listbox_handle, LB_SETCURSEL, wparam, unused);
	ASSERT(success != LB_ERR || wparam == -1);
}

void Dialog::SetSliderRange(IdType sliderId, ValueType minValue, ValueType maxValue) {
    ASSERT(minValue < maxValue);
	ASSERT(maxValue < VALUE_INVALID);

    HWND const slider_handle = GetControlHandle(sliderId);
    
    WPARAM redraw = WPARAM(FALSE);
    Win::SendMessage(slider_handle, TBM_SETRANGEMIN, redraw, LPARAM(minValue));

    redraw = WPARAM(TRUE);
    Win::SendMessage(slider_handle, TBM_SETRANGEMAX, redraw, LPARAM(maxValue));
}

Dialog::ValueType Dialog::SetSliderValue(IdType id, ValueType value) {
    HWND const slider_handle = GetControlHandle(id);
    
    // set slider value
    WPARAM const redraw = WPARAM(TRUE);
    Win::SendMessage(slider_handle, TBM_SETPOS, redraw, LPARAM(value));
    
    // read it back
    ValueType result = GetSliderValue(id);

	return result;
}

// set the text value of a label or editbox control
void Dialog::SetTextString(IdType controlId, String const &rString) {
	HWND const window_handle = HWND(*this);

	unsigned const length = rString.Length();
    char *copy_text = new char[length + 1];
    ::strcpy_s(copy_text, length + 1, rString.c_str());

    UINT const success = Win::SetDlgItemText(window_handle, controlId, copy_text);
	ASSERT(success != 0);

	delete[] copy_text;
}

// change the numeric value displayed by an editbox or static label
void Dialog::SetTextValue(IdType id, ValueType value) {
	HWND const window_handle = HWND(*this);
    BOOL const sign = FALSE;
    BOOL const success = Win::SetDlgItemInt(window_handle, id, value, sign);
    ASSERT(success);

    ASSERT(GetTextValue(id) == value);
}
#endif // defined(_WINDOWS)
