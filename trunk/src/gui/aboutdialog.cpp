// File:    aboutdialog.cpp
// Purpose: AboutDialog class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include "project.hpp"

#ifdef _WINDOWS
#include <windows.h>
#include "gui/aboutdialog.hpp"

void CenterWindow(HWND hwnd) {
    // Center the window of the desktop if it has no parent
    RECT    ownerBounds;
    {
        HWND hwndOwner = GetParent(hwnd);
        if (hwndOwner == NULL) 
            hwndOwner = GetDesktopWindow(); 
        GetWindowRect(hwndOwner, &ownerBounds); 
    }

    RECT	dialogBounds;
    // Place the dialog at 0,0
    ::GetWindowRect(hwnd, &dialogBounds);
    ::OffsetRect(&dialogBounds, -dialogBounds.left, -dialogBounds.top);

    // set the dialog position centered
    {
        int diffWidth   = ownerBounds.right - ownerBounds.left - dialogBounds.right,
            diffHeight  = ownerBounds.bottom - ownerBounds.top - dialogBounds.bottom;

        ::OffsetRect(&dialogBounds, ownerBounds.left + diffWidth/2, ownerBounds.top + diffHeight/2);
    }

    ::SetWindowPos(hwnd, 
        HWND_TOP, 
        dialogBounds.left,
        dialogBounds.top,
        0, 0,
        SWP_NOSIZE); 
}

// message handler (callback) for "About" dialog
static AboutDialog *spNewlyCreatedAboutDialog = NULL;
static INT_PTR CALLBACK aboutDialogMessageHandler(
	HWND window, 
	UINT message, 
	WPARAM wParameter, 
	LPARAM lParameter)
{
    AboutDialog *p_dialog;
	if (spNewlyCreatedAboutDialog != NULL) {
		p_dialog = spNewlyCreatedAboutDialog;
		spNewlyCreatedAboutDialog = NULL;
		p_dialog->SetHandle(window);
	} else {
       p_dialog = (AboutDialog *)Dialog::Lookup(window);
	}

	INT_PTR result;
    ASSERT(p_dialog->Handle() == window);
    result = p_dialog->HandleMessage(message, wParameter, lParameter);

	return result;
}

// lifecycle
AboutDialog::AboutDialog(Window const &parent) {
	spNewlyCreatedAboutDialog = this;
	Dialog::Initialize("ABOUT", parent, &aboutDialogMessageHandler);
}

// methods
INT_PTR AboutDialog::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
	INT_PTR result = FALSE;
	HWND window = Handle();

    switch (message) {
        case WM_INITDIALOG:
		    ::CenterWindow(window);
            result = TRUE;
			break;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK:
                case IDCANCEL:
                    ::EndDialog(window, 0);
                    result = TRUE;
					break;
            }
            break;

	    default:
			break;
    }

    return result;
}

#endif

