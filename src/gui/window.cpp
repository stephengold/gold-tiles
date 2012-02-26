// File:    window.cpp
// Purpose: Window class
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

#include "gui/window.hpp"
#include "gui/win_types.hpp"

// static data

/* static */ Window::Map Window::msMap;
/* static */ Window *Window::mspNewlyCreatedWindow = NULL;

// lifecycle

Window::Window(void) {
	mHandle = 0;
	mModule = 0;
}

void Window::Initialize(CREATESTRUCT const &rCreateStruct) {
	ASSERT(mHandle != 0);
	ASSERT(mModule == 0);

	mModule = rCreateStruct.hInstance;
	ASSERT(mModule != 0);

    HDC private_dc = Win::GetDC(mHandle);
    ASSERT(private_dc != NULL);
    // It's a private DC because CS_OWNDC is hard-coded into the
    // WindowClass constructor.

	// record size of client area
	SIZE client_area_size;
	BOOL success = Win::GetWindowExtEx(private_dc, &client_area_size);
    ASSERT(success != 0);
    SetClientArea(client_area_size.cx, client_area_size.cy);

	char const *icon_resource_name = "TOPWINDOWICON";
	SetIcons(icon_resource_name);
}


// operators

Window::operator Rect(void) const {
	Point origin(0,0);
    Rect result(origin, mClientAreaWidth, mClientAreaHeight);

	return result;
}


// misc methods

void Window::CaptureMouse(void) {
	HWND this_window = Handle();
    Win::SetCapture(this_window);

	ASSERT(IsMouseCaptured());
}

// Center a window on its parent.
// If it has no parent, center it on the desktop.
void Window::Center(void) {
	HWND this_window = Handle();
    HWND owner = Win::GetParent(this_window);
    if (owner == NULL) {
        owner = Win::GetDesktopWindow();
	}

	RECT bounds;
	Win::GetWindowRect(owner, &bounds);
	Rect owner_bounds(bounds);

    Win::GetWindowRect(this_window, &bounds);
    Rect window_bounds(bounds);

	PCntType pad_left = (owner_bounds.Width() - window_bounds.Width())/2;
	PCntType pad_top = (owner_bounds.Height() - window_bounds.Height())/2;

    LogicalXType x = owner_bounds.LeftX() + pad_left;
    LogicalYType y = owner_bounds.TopY() + pad_top;
    Win::SetWindowPos(this_window, HWND_TOP, x, y, 0, 0, SWP_NOSIZE); 
}

PCntType Window::ClientAreaHeight(void) const {
	PCntType result = mClientAreaHeight;

	return result;
}

PCntType Window::ClientAreaWidth(void) const {
	PCntType result = mClientAreaWidth;

	return result;
}

void Window::Close(void) {
	int ignored = 0;
    Win::SendMessage(mHandle, WM_CLOSE, WPARAM(ignored), LPARAM(ignored));
}

HINSTANCE Window::CopyModule(Window const &rOther) {
	mModule = rOther.mModule;
    HINSTANCE result = mModule;

	return result;
}

void Window::ForceRepaint(void) {
	ASSERT(mHandle != 0);
    RECT *entire_client_area = NULL;
    BOOL erase = TRUE;
    BOOL success = Win::InvalidateRect(mHandle, entire_client_area, erase);
    ASSERT(success != 0);
}

HACCEL Window::GetAcceleratorTable(char const *resourceName) {
	HACCEL result = Win::LoadAccelerators(mModule, resourceName);
	ASSERT(result != NULL);

	return result;
}

HMENU Window::GetMenu(char const *resourceName) {
	HMENU result = Win::LoadMenu(mModule, resourceName);
	ASSERT(result != NULL);

	return result;
}

HWND Window::Handle(void) const {
	HWND result = mHandle;
	
	return result;
}

LRESULT Window::HandleMessage(MessageType message, WPARAM wParameter, LPARAM lParameter) {
	ASSERT(mHandle != 0);
	LRESULT result = 0;

    switch (message) {
	    case WM_CLOSE: {
			BOOL success = Win::DestroyWindow(mHandle);
			ASSERT(success);
			break;
		}

        case WM_CREATE: { // initialize window
			CREATESTRUCT *p_create_struct = (CREATESTRUCT *)lParameter;
			ASSERT(p_create_struct != NULL);
            Initialize(*p_create_struct);
            break;
		}

        case WM_DESTROY: // destroy window
			SelfDestruct();
            break;

        case WM_SIZE: { // resize window
			PCntType width = LOWORD(lParameter);
			PCntType height = HIWORD(lParameter);
            SetClientArea(width, height);
            break;
		}

		default:  // invoke default message handler
			result = Win::DefWindowProc(Handle(), message, wParameter, lParameter);
			break;
    }

    return result;
}

// display a simple dialog box with a message and an OK button
void Window::InfoBox(char const *message, char const *title) {
	UINT options = MB_OK | MB_ICONINFORMATION | MB_DEFBUTTON1 | MB_APPLMODAL;
    int success = Win::MessageBox(Handle(), message, title, options);
	ASSERT(success != 0);
}

/* static */ Window *Window::Lookup(HWND handle) {
    Key key = Key(handle);
    ConstIterator i_window = msMap.find(key);

	Window *result = NULL;
    if (i_window != msMap.end()) {
        result = i_window->second;
        ASSERT(result != NULL);
    } else if (mspNewlyCreatedWindow != NULL) {
		result = mspNewlyCreatedWindow;
		mspNewlyCreatedWindow = NULL;
		result->SetHandle(handle);
	}

	return result;
}

void Window::SelfDestruct(void) {
	int application_exit_code = 0;
    Win::PostQuitMessage(application_exit_code);
}

void Window::SetClientArea(PCntType width, PCntType height) {
    ASSERT(width < 4000);
    ASSERT(height < 4000);
    mClientAreaWidth = width;
    mClientAreaHeight = height;
}

void Window::SetCursor(LPSTR resource) {
	HINSTANCE instance = NULL;
	PCntType desired_width = 0;
    PCntType desired_height = 0;
	UINT options = (LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_SHARED);
	HCURSOR handle = (HCURSOR)Win::LoadImage(instance, resource, IMAGE_CURSOR,
		desired_width, desired_height, options);
	ASSERT(handle != NULL);
    
	Win::SetCursor(handle);
}

void Window::SetCursorBusy(void) {
	SetCursor(IDC_WAIT);
}
void Window::SetCursorDrag(void) {
	SetCursor(IDC_HAND);
}
void Window::SetCursorSelect(void) {
	SetCursor(IDC_ARROW);
}

void Window::SetHandle(HWND handle) {
	mHandle = handle;

	Key key = Key(handle);
	Pair new_mapping(key, this); 
	InsertResult ins_result = msMap.insert(new_mapping);
	bool success = ins_result.second;
	ASSERT(success);
}

// set large and small icons for a window
void Window::SetIcons(char const *resourceName) {
	ASSERT(mHandle != 0);

	PCntType desired_width;
    PCntType desired_height;
	UINT options = LR_DEFAULTCOLOR;

	// small icon for title bar
	desired_width = Win::GetSystemMetrics(SM_CXSMICON);
    desired_height = Win::GetSystemMetrics(SM_CYSMICON);
	HICON small_icon = (HICON)Win::LoadImage(mModule, resourceName, IMAGE_ICON,
		desired_width, desired_height, options);
	ASSERT(small_icon != NULL);

    Win::SendMessage(mHandle, WM_SETICON, WPARAM(ICON_SMALL), LPARAM(small_icon));

	// large icon for ALT+TAB dialog box
	desired_width = Win::GetSystemMetrics(SM_CXICON);
    desired_height = Win::GetSystemMetrics(SM_CYICON);
	HICON large_icon = (HICON)Win::LoadImage(mModule, resourceName, IMAGE_ICON,
		desired_width, desired_height, options);
	ASSERT(large_icon != NULL);

    Win::SendMessage(mHandle, WM_SETICON, WPARAM(ICON_BIG), LPARAM(large_icon));
}

void Window::SetTimer(unsigned interval_msecs, unsigned event_id) {
	TIMERPROC callback = NULL;
	UINT_PTR success = Win::SetTimer(mHandle, event_id, interval_msecs, callback);
    ASSERT(success != 0);
}

void Window::Show(int how) {
	ASSERT(mHandle != 0);

    Win::ShowWindow(mHandle, how);
    BOOL success = Win::UpdateWindow(mHandle);
    ASSERT(success);
}

void Window::UpdateMenuBar(void) {
	// redraw all menus
	BOOL success = Win::DrawMenuBar(mHandle);
	ASSERT(success);
}

void Window::WarpCursor(Point const &rDestination) {
    // convert to screen coordinates
    POINT point = POINT(rDestination);
    BOOL success = Win::ClientToScreen(mHandle, &point);
    ASSERT(success);
    
    // warp the cursor to new coordinates
    success = Win::SetCursorPos(point.x, point.y);
    ASSERT(success);
}

// inquiry methods

bool Window::IsMouseCaptured(void) const {
	HWND captor = Win::GetCapture();
    bool result = (captor == mHandle);
	
	return result;
}
