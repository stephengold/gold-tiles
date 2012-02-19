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

#include "project.hpp"

#ifdef _WINDOWS
#include <map>
#include <windows.h>
#include "gui/resource.hpp"
#include "gui/window.hpp"
#include "gui/windowclass.hpp"
#include "rect.hpp"

// static data

/* static */ Window::Map Window::msMap;
/* static */ Window *Window::mspNewlyCreatedWindow = NULL;

// lifecycle

Window::Window(void) {
	mHandle = 0;
	mModule = 0;
}

HDC Window::Initialize(CREATESTRUCT const *pCreateStruct) {
	ASSERT(mHandle != 0);
	ASSERT(mModule == 0);
   	ASSERT(pCreateStruct != NULL);
	mModule = pCreateStruct->hInstance;

    HDC private_dc = ::GetDC(mHandle);
    ASSERT(private_dc != NULL);
    // It's a private DC because CS_OWNDC is hard-coded into the
    // WindowClass constructor.

	// record size of client area
	SIZE client_area_size;
	BOOL success = ::GetWindowExtEx(private_dc, &client_area_size);
    ASSERT(success != 0);
    SetClientArea(client_area_size.cx, client_area_size.cy);

	char const *icon_resource_name = "TOPWINDOWICON";
	SetIcons(icon_resource_name);
	
	return private_dc;
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
    ::SetCapture(this_window);

	ASSERT(IsMouseCaptured());
}

// Center a window on its parent.
// If it has no parent, center it on the desktop.
void Window::Center(void) {
	HWND this_window = Handle();
    HWND owner = ::GetParent(this_window);
    if (owner == NULL) {
        owner = ::GetDesktopWindow();
	}

	RECT bounds;
	::GetWindowRect(owner, &bounds);
	Rect owner_bounds(bounds);

    ::GetWindowRect(this_window, &bounds);
    Rect window_bounds(bounds);

	PCntType pad_left = (owner_bounds.Width() - window_bounds.Width())/2;
	PCntType pad_top = (owner_bounds.Height() - window_bounds.Height())/2;

    LogicalXType x = owner_bounds.LeftX() + pad_left;
    LogicalYType y = owner_bounds.TopY() + pad_top;
    ::SetWindowPos(this_window, HWND_TOP, x, y, 0, 0, SWP_NOSIZE); 
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
    ::SendMessage(mHandle, WM_CLOSE, WPARAM(ignored), LPARAM(ignored));
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
    BOOL success = ::InvalidateRect(mHandle, entire_client_area, erase);
    ASSERT(success != 0);
}

HACCEL Window::GetAcceleratorTable(char const *resourceName) {
	HACCEL result = ::LoadAccelerators(mModule, resourceName);
	ASSERT(result != NULL);

	return result;
}

HWND Window::Handle(void) const {
	HWND result = mHandle;
	
	return result;
}

LRESULT Window::HandleMessage(UINT message, WPARAM wParameter, LPARAM lParameter) {
	ASSERT(mHandle != 0);
	LRESULT result = 0;

    switch (message) {
	    case WM_CLOSE: {
			BOOL success = ::DestroyWindow(mHandle);
			ASSERT(success);
			break;
		}

        case WM_CREATE: { // initialize window
			CREATESTRUCT *create_struct = (CREATESTRUCT *)lParameter;
            Initialize(create_struct);
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
			result = ::DefWindowProc(Handle(), message, wParameter, lParameter);
			break;
    }

    return result;
}

/* static */ Window *Window::Lookup(HWND handle) {
    Key key = Key(handle);
    ConstIteratorType i_window = msMap.find(key);

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
	int applicationExitCode = 0;
    ::PostQuitMessage(applicationExitCode);
}

void Window::SetClientArea(PCntType width, PCntType height) {
    ASSERT(width < 4000);
    ASSERT(height < 4000);
    mClientAreaWidth = width;
    mClientAreaHeight = height;
}

void Window::SetHandle(HWND handle) {
	mHandle = handle;

	Key key = Key(handle);
	Window *value = this;
	Pair new_mapping(key, value); 
	InsertResultType ins_result = msMap.insert(new_mapping);
	bool success = ins_result.second;
	ASSERT(success);
}

// set large and small icons for a window
void Window::SetIcons(char const *iconResourceName) {
	ASSERT(mHandle != 0);

	HINSTANCE module_instance = mModule;
    HWND window_handle = mHandle;
	UINT message = WM_SETICON;
	UINT image_type = IMAGE_ICON;
	UINT option = LR_DEFAULTCOLOR;
	PCntType desired_width;
    PCntType desired_height;
	WPARAM which_icon;

	// small icon for title bar
	which_icon = ICON_SMALL;
	desired_width = ::GetSystemMetrics(SM_CXSMICON);
    desired_height = ::GetSystemMetrics(SM_CYSMICON);
	HICON smallIconHandle = (HICON)::LoadImage(module_instance, iconResourceName, image_type,
		desired_width, desired_width, option);
    ::SendMessage(window_handle, message, which_icon, LPARAM(smallIconHandle));

	// large icon for ALT+TAB dialog box
	which_icon = ICON_BIG;
	desired_width = ::GetSystemMetrics(SM_CXICON);
    desired_height = ::GetSystemMetrics(SM_CYICON);
	HICON largeIconHandle = (HICON)::LoadImage(module_instance, iconResourceName, image_type,
		desired_width, desired_width, option);
    ::SendMessage(window_handle, message, which_icon, LPARAM(largeIconHandle));
}

void Window::SetTimer(unsigned interval_msecs, unsigned event_id) {
	TIMERPROC callback = NULL;
	int success = ::SetTimer(mHandle, event_id, interval_msecs, callback);
    ASSERT(success != 0);
}

void Window::Show(int how) {
	ASSERT(mHandle != 0);

    ::ShowWindow(mHandle, how);
    BOOL success = ::UpdateWindow(mHandle);
    ASSERT(success != 0);
}


// inquiry methods

bool Window::IsMouseCaptured(void) const {
	HWND captor = ::GetCapture();
    bool result = (captor == mHandle);
	
	return result;
}

#endif
