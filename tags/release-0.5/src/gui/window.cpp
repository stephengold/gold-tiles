// File:    window.cpp
// Purpose: Window class
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU General Public License

/*
This file is part of the Gold Tile game.

The Gold Tile game is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The Gold Tile game is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
for more details.

You should have received a copy of the GNU General Public License
along with the Gold Tile game.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "project.hpp"

#ifdef _WINDOWS
#include <map>
#include <windows.h>
#include "gui/resource.hpp"
#include "gui/window.hpp"
#include "gui/windowclass.hpp"
#include "rect.hpp"

// message handler (callback)
static Window *spNewlyCreatedWindow = NULL;
static LRESULT CALLBACK messageHandler(
	HWND windowHandle,
	UINT message,
	WPARAM wParameter,
	LPARAM lParameter)
{
    Window *window;
	if (message == WM_CREATE && spNewlyCreatedWindow != NULL) {
		window = spNewlyCreatedWindow;
		spNewlyCreatedWindow = NULL;
		window->SetHandle(windowHandle);
	} else {
       window = Window::Lookup(windowHandle);
	}

	LRESULT result;
	if (window == NULL) { // unknown window handle
		// invoke default message handler
		result = ::DefWindowProc(windowHandle, message, wParameter, lParameter);
	} else {
     	ASSERT(window->Handle() == windowHandle);
        result = window->HandleMessage(message, wParameter, lParameter);
	}
	return result;
}

// static data

Window::Map Window::msMap;

// lifecycle

Window::Window(void) {
	mHandle = 0;
	mModule = 0;
}

HDC Window::Initialize(CREATESTRUCT const *pCreateStruct) {
	ASSERT(mHandle != 0);
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

// methods

Window::operator Rect(void) const {
	int x = 0;
    int y = 0;
    Rect result(y, x, mClientAreaWidth, mClientAreaHeight);

	return result;
}

// Center a window on its parent.
// If it has no parent, center it on the desktop.
void Window::CenterWindow(void) {
	HWND window = Handle();
    HWND owner = ::GetParent(window);
    if (owner == NULL) {
        owner = ::GetDesktopWindow();
	}

	Rect owner_bounds(0, 0, 0, 0);
	::GetWindowRect(owner, &(RECT &)owner_bounds);

    Rect window_bounds(0, 0, 0, 0);
    ::GetWindowRect(window, &(RECT &)window_bounds);

	int pad_left = (owner_bounds.Width() - window_bounds.Width())/2;
	int pad_top = (owner_bounds.Height() - window_bounds.Height())/2;

    ::SetWindowPos(window, HWND_TOP, 
		owner_bounds.LeftX() + pad_left, owner_bounds.TopY() + pad_top, 0, 0, SWP_NOSIZE); 
}

unsigned Window::ClientAreaHeight(void) const {
	unsigned result = mClientAreaHeight;

	return result;
}

unsigned Window::ClientAreaWidth(void) const {
	unsigned result = mClientAreaWidth;

	return result;
}

HINSTANCE Window::CopyModule(Window const &other) {
	mModule = other.mModule;
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

HWND Window::Handle(void) const {
	HWND result = mHandle;
	
	return result;
}

LRESULT Window::HandleMessage(UINT message, WPARAM wParameter, LPARAM lParameter) {
	ASSERT(mHandle != 0);
	LRESULT result = 0;
    switch (message) {
        case WM_CREATE: { // initialize window
			CREATESTRUCT *create_struct = (CREATESTRUCT *)lParameter;
            Initialize(create_struct);
            break;
		}

        case WM_DESTROY: // destroy window
			SelfDestruct();
            break;

        case WM_SIZE: { // resize window
			unsigned width = LOWORD(lParameter);
			unsigned height = HIWORD(lParameter);
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
	unsigned long key = (unsigned long)handle;
	Map::const_iterator it;
	it = msMap.find(key);

	Window *result = NULL;
	if (it != msMap.end()) {
	    result = it->second;
	    ASSERT(result != NULL);
	}

	return result;
}

void Window::SelfDestruct(void) {
	int applicationExitCode = 0;
    ::PostQuitMessage(applicationExitCode);
}

void Window::SetClientArea(unsigned width, unsigned height) {
    ASSERT(width < 4000);
    ASSERT(height < 4000);
    mClientAreaWidth = width;
    mClientAreaHeight = height;
}

void Window::SetHandle(HWND handle) {
	mHandle = handle;

	unsigned long key = (unsigned long)handle;
	Window *value = this;
	Pair newMapping(key, value); 
	msMap.insert(newMapping);
}

// set large and small icons for a window
void Window::SetIcons(char const *iconResourceName) {
	ASSERT(mHandle != 0);

	HINSTANCE module_instance = mModule;
    HWND window_handle = mHandle;
	UINT message = WM_SETICON;
	UINT image_type = IMAGE_ICON;
	UINT option = LR_DEFAULTCOLOR;
	int desired_width;
    int desired_height;
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

void Window::Show(int how) {
	ASSERT(mHandle != 0);

    ::ShowWindow(mHandle, how);
    BOOL success = ::UpdateWindow(mHandle);
    ASSERT(success != 0);
}

#endif