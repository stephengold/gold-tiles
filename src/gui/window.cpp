// File:    window.cpp
// Purpose: Window class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include "project.hpp"

#ifdef _WINDOWS
#include <map>
#include <windows.h>
#include "gui/resource.hpp"
#include "gui/window.hpp"
#include "gui/windowclass.hpp"

// message handler (callback)
static Window *newlyCreatedWindow = NULL;
static LRESULT CALLBACK messageHandler(
	HWND windowHandle,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
    Window *window;
	if (message == WM_CREATE && newlyCreatedWindow != NULL) {
		window = newlyCreatedWindow;
		newlyCreatedWindow = NULL;
		window->SetHandle(windowHandle);
	} else {
       window = Window::Lookup(windowHandle);
	}

	LRESULT result;
	if (window == NULL) { // unknown window handle
		// invoke default message handler
		result = ::DefWindowProc(windowHandle, message, wParam, lParam);
	} else {
     	ASSERT(window->Handle() == windowHandle);
        result = window->HandleMessage(message, wParam, lParam);
	}
	return result;
}

// static members
Window::Map Window::msMap;

Window *Window::Lookup(HWND handle) {
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

Window::Window(void) {
	mHandle = 0;
}

void Window::ForceRepaint(void) {
     RECT *entire_client_area = NULL;
     BOOL erase = TRUE;
     BOOL success = ::InvalidateRect(mHandle, entire_client_area, erase);
     ASSERT(success != 0);
}

HWND Window::Handle(void) const {
	HWND result = mHandle;
	
	return result;
}

LRESULT Window::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;
    switch (message) {
        case WM_CREATE: { // initialize window
			CREATESTRUCT *create_struct = (CREATESTRUCT *)lParam;
            Initialize(create_struct);
            break;
		}

        case WM_DESTROY: // destroy window
			SelfDestruct();
            break;

        case WM_SIZE: { // resize window
			unsigned width = LOWORD(lParam);
			unsigned height = HIWORD(lParam);
            SetClientArea(width, height);
            break;
		}

		default:  // invoke default message handler
			result = ::DefWindowProc(Handle(), message, wParam, lParam);
			break;
    }

    return result;
}

HDC Window::Initialize(CREATESTRUCT const *pCreateStruct) {
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

	char const *icon_resource_name = ClassName(); // use same name
	SetIcons(icon_resource_name);
	
	return private_dc;
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
