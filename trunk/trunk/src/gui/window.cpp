// File:    window.cpp
// Purpose: Window class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <map>
#include <windows.h>
//#include "about.h"
#include "gui/canvas.hpp"
#include "gui/resource.hpp"
#include "gui/window.hpp"
#include "gui/windowclass.hpp"
#include "project.hpp"

// message handler (callback) for window
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
		window->setHandle(windowHandle);
	} else {
       window = Window::lookup(windowHandle);
	}

	LRESULT result;
	if (window == NULL) { // unknown window handle
		// invoke default message handler
		result = ::DefWindowProc(windowHandle, message, wParam, lParam);
	} else {
     	assert(window->getHandle() == windowHandle);
        result = window->handleMessage(message, wParam, lParam);
	}
	return result;
}

// static members of Window
WMap Window::_map;

Window *Window::lookup(HWND handle) {
	unsigned long key = (unsigned long)handle;
	WMap::const_iterator it;
	it = _map.find(key);

	Window *result = NULL;
	if (it != _map.end()) {
	    result = it->second;
	    assert(result != NULL);
	}

	return result;
}

// non-static members of Window

Window::Window(void) {
	_handle = 0;
}

void Window::destroy(void) {
	int applicationExitCode = 0;
    ::PostQuitMessage(applicationExitCode);
}

void Window::forceRepaint(void) {
     RECT *entireClientArea = NULL;
     BOOL eraseFlag = TRUE;
     BOOL success = ::InvalidateRect(_handle, entireClientArea, eraseFlag);
     assert(success != 0);
}

HWND Window::getHandle(void) const {
	return _handle;
}

LRESULT Window::handleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;
    switch (message) {
        case WM_CREATE: { // initialize window
			CREATESTRUCT *createStruct = (CREATESTRUCT *)lParam;
            initialize(createStruct);
            break;
		}

        case WM_DESTROY: // destroy window
			destroy();
            break;

        case WM_SIZE: { // resize window
			unsigned width = LOWORD(lParam);
			unsigned height = HIWORD(lParam);
            setClientArea(width, height);
            break;
		}

		default:  // invoke default message handler
			result = ::DefWindowProc(this->getHandle(), message, wParam, lParam);
			break;
    }

    return result;
}

HDC Window::initialize(CREATESTRUCT const *createStruct) {
   	assert(createStruct != NULL);
	_module = createStruct->hInstance;

    HDC privateDC = ::GetDC(_handle);
    assert(privateDC != NULL);
    // It's a private DC because CS_OWNDC is hard-coded into the
    // WindowClass constructor.

	// record actual size of client area
	SIZE clientAreaSize;
	BOOL success = ::GetWindowExtEx(privateDC, &clientAreaSize);
    assert(success != 0);
    setClientArea(clientAreaSize.cx, clientAreaSize.cy);

	char const *iconResourceName = getClassName(); // use same name
	setIcons(iconResourceName);
	
	return privateDC;
}

void Window::setHandle(HWND handle) {
	_handle = handle;

	unsigned long key = (unsigned long)handle;
	Window *value = this;
	WPair newMapping(key, value); 
	_map.insert(newMapping);
}

// set large and small icons for a window
void Window::setIcons(char const *iconResourceName) {
    HINSTANCE moduleInstance = _module;
    HWND windowHandle = _handle;
	UINT message = WM_SETICON;
	UINT imageType = IMAGE_ICON;
	UINT option = LR_DEFAULTCOLOR;
	int desiredWidth;
    int desiredHeight;
	WPARAM whichIcon;

	// small icon for title bar
	whichIcon = ICON_SMALL;
	desiredWidth = ::GetSystemMetrics(SM_CXSMICON);
    desiredHeight = ::GetSystemMetrics(SM_CYSMICON);
	HICON smallIconHandle = (HICON)::LoadImage(moduleInstance, iconResourceName, imageType,
		desiredWidth, desiredWidth, option);
    ::SendMessage(windowHandle, message, whichIcon, LPARAM(smallIconHandle));

	// large icon for ALT+TAB dialog box
	whichIcon = ICON_BIG;
	desiredWidth = ::GetSystemMetrics(SM_CXICON);
    desiredHeight = ::GetSystemMetrics(SM_CYICON);
	HICON largeIconHandle = (HICON)::LoadImage(moduleInstance, iconResourceName, imageType,
		desiredWidth, desiredWidth, option);
    ::SendMessage(windowHandle, message, whichIcon, LPARAM(largeIconHandle));
}

void Window::setClientArea(unsigned width, unsigned height) {
    assert(width < 4000);
    assert(height < 4000);
    _clientAreaWidth = width;
    _clientAreaHeight = height;
}

void Window::show(int how) {
	assert(_handle != 0);

    ::ShowWindow(_handle, how);
    BOOL success = ::UpdateWindow(_handle);
    assert(success != 0);
}
