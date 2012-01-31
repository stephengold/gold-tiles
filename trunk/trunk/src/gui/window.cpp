// File:    window.cpp
// Purpose: Window class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

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
		window->setHandle(windowHandle);
	} else {
       window = Window::lookup(windowHandle);
	}

	LRESULT result;
	if (window == NULL) { // unknown window handle
		// invoke default message handler
		result = ::DefWindowProc(windowHandle, message, wParam, lParam);
	} else {
     	ASSERT(window->getHandle() == windowHandle);
        result = window->handleMessage(message, wParam, lParam);
	}
	return result;
}

// static members
WMap Window::_map;

Window *Window::lookup(HWND handle) {
	unsigned long key = (unsigned long)handle;
	WMap::const_iterator it;
	it = _map.find(key);

	Window *result = NULL;
	if (it != _map.end()) {
	    result = it->second;
	    ASSERT(result != NULL);
	}

	return result;
}

// non-static members

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
     ASSERT(success != 0);
}

HWND Window::getHandle(void) const {
	HWND result = _handle;
	
	return result;
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
   	ASSERT(createStruct != NULL);
	_module = createStruct->hInstance;

    HDC privateDC = ::GetDC(_handle);
    ASSERT(privateDC != NULL);
    // It's a private DC because CS_OWNDC is hard-coded into the
    // WindowClass constructor.

	// record size of client area
	SIZE clientAreaSize;
	BOOL success = ::GetWindowExtEx(privateDC, &clientAreaSize);
    ASSERT(success != 0);
    setClientArea(clientAreaSize.cx, clientAreaSize.cy);

	char const *iconResourceName = getClassName(); // use same name
	setIcons(iconResourceName);
	
	return privateDC;
}

void Window::setClientArea(unsigned width, unsigned height) {
    ASSERT(width < 4000);
    ASSERT(height < 4000);
    _clientAreaWidth = width;
    _clientAreaHeight = height;
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

void Window::show(int how) {
	ASSERT(_handle != 0);

    ::ShowWindow(_handle, how);
    BOOL success = ::UpdateWindow(_handle);
    ASSERT(success != 0);
}

#endif
