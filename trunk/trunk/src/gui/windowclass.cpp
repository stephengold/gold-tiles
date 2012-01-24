// File:    windowclass.cpp
// Purpose: WindowClass class for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <windows.h>
#include "gui/windowclass.hpp"
#include "project.hpp"

WindowClass::WindowClass(
    HINSTANCE applicationInstance,
    WNDPROC messageHandler,
    char const *name)
{
    HINSTANCE nullInstance = NULL;
	cbClsExtra = 0;
	cbWndExtra = 0;
    cbSize = sizeof(WNDCLASSEX);
	hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
	hCursor = LoadCursor(nullInstance, IDC_ARROW);
	hIcon = 0;
	hIconSm = 0;
	hInstance = applicationInstance;
	lpfnWndProc = messageHandler;  // callback

	// The menu and the window class will share the same name.
	lpszClassName = name;
	lpszMenuName = name;

	style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
}

HINSTANCE WindowClass::getModule(void) const {
	return hInstance;
}

char const *WindowClass::getName(void) const {
	return lpszClassName;
}

void WindowClass::registerClass(void) {
    ATOM classAtom = ::RegisterClassEx(this);
	ASSERT(classAtom != 0);
};
