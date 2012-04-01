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

#ifdef _WINDOWS
#include "gui/window.hpp"
#include "gui/win_types.hpp"
#include "string.hpp"

// static data

/* static */ Window::Map Window::msMap;
/* static */ Window *Window::mspNewlyCreatedWindow = NULL;

// lifecycle

Window::Window(void) {
	mAcceleratorTable = 0;
	mHandle = 0;
	mModule = 0;
	mPaintDevice = 0;
}

void Window::Initialize(CREATESTRUCT const &rCreateStruct) {
	ASSERT(mHandle != 0);
	ASSERT(mModule == 0);

	mModule = rCreateStruct.hInstance;
	ASSERT(mModule != 0);

    HDC const private_dc = Win::GetDC(mHandle);
    ASSERT(private_dc != NULL);
    // It's a private DC because CS_OWNDC is hard-coded into the
    // WindowClass constructor.

	// record size of client area
	SIZE client_area_size;
	BOOL const success = Win::GetWindowExtEx(private_dc, &client_area_size);
    ASSERT(success != 0);
    SetClientArea(client_area_size.cx, client_area_size.cy);

	char const *const icon_resource_name = "TOPWINDOWICON";
	SetIcons(icon_resource_name);
}


// operators

Window::operator HWND(void) const {
	return mHandle;
}

Window::operator Rect(void) const {
	Point const origin(0,0);
    Rect const result(origin, mClientAreaWidth, mClientAreaHeight);

	return result;
}


// misc methods

void * Window::AddFiber(void (__stdcall &rStartRoutine)(void *)) {
	Win::SIZE_T const default_stack_size = 0;
	void * const result = Win::CreateFiber(default_stack_size, &rStartRoutine, LPVOID(this));
	ASSERT(result != NULL);

	return result;
}

void Window::BeginPaint(void) {
	ASSERT(mPaintDevice == NULL);
	ASSERT(mHandle != NULL);

    mPaintDevice = Win::BeginPaint(mHandle, &mPaintStruct);

    ASSERT(mPaintDevice != NULL);
}

void Window::CaptureMouse(void) {
	HWND const this_window = HWND(*this);
    Win::SetCapture(this_window);

	ASSERT(IsMouseCaptured());
}

// Center a window on its parent.
// If it has no parent, center it on the desktop.
void Window::Center(void) {
	HWND const this_window = HWND(*this);
    HWND owner = Win::GetParent(this_window);
    if (owner == NULL) {
        owner = Win::GetDesktopWindow();
	}

	RECT bounds;
	Win::GetWindowRect(owner, &bounds);
	Rect const owner_bounds(bounds);

    Win::GetWindowRect(this_window, &bounds);
    Rect const window_bounds(bounds);

	PCntType pad_left = (owner_bounds.Width() - window_bounds.Width())/2;
	PCntType pad_top = (owner_bounds.Height() - window_bounds.Height())/2;

    LogicalXType const x = owner_bounds.LeftX() + pad_left;
    LogicalYType const y = owner_bounds.TopY() + pad_top;
    Win::SetWindowPos(this_window, HWND_TOP, x, y, 0, 0, SWP_NOSIZE); 
}

PCntType Window::ClientAreaHeight(void) const {
	return mClientAreaHeight;
}

PCntType Window::ClientAreaWidth(void) const {
	return mClientAreaWidth;
}

void Window::Close(void) {
	int const ignored = 0;
    Win::SendMessage(mHandle, WM_CLOSE, WPARAM(ignored), LPARAM(ignored));
}

HINSTANCE Window::CopyModule(Window const &rOther) {
	mModule = rOther.mModule;
    HINSTANCE result = mModule;

	return result;
}

void Window::Create(
	String const &rClassName,
	Rect const &rRect,
	Window *pParent,
	HINSTANCE applicationInstance)
{
	// Make this object accessable to its message handler before WM_CREATE.
    ASSERT(mspNewlyCreatedWindow == NULL);
	mspNewlyCreatedWindow = this;

	LPCTSTR const name = Name();
	ASSERT(name != NULL);

	LPCTSTR const class_name = rClassName.c_str();
	ASSERT(class_name != NULL);

	DWORD const window_style = WS_OVERLAPPEDWINDOW;
	PCntType const height = rRect.Height();
	PCntType const width = rRect.Width();
	LogicalXType const x = rRect.LeftX();
	LogicalYType const y = rRect.TopY();

	HWND parent_handle = NULL;
	if (pParent != NULL) {
		parent_handle = HWND(*pParent);
	}

	HMENU const menu_handle = NULL;
	LPVOID const parameters = NULL;

    HWND const handle = Win::CreateWindow(class_name, name, window_style, x, y, 
                             width, height, parent_handle, menu_handle, 
							 applicationInstance, parameters);
    ASSERT(handle == mHandle);
}

/* static */ Rect Window::DesktopBounds(void) {
	HWND const desktop_handle = Win::GetDesktopWindow();
	RECT rect;
	Win::GetWindowRect(desktop_handle, &rect);
	Rect const result(rect);

	return result;
}

void Window::EndPaint(void) {
	ASSERT(mHandle != NULL);
	ASSERT(mPaintDevice != NULL);

    Win::EndPaint(mHandle, &mPaintStruct);
	mPaintDevice = NULL;
}

// display a simple dialog box with a pilot error message and an OK button
void Window::ErrorBox(char const *message, char const *title) {
	UINT const options = MB_OK | MB_ICONERROR | MB_DEFBUTTON1 | MB_APPLMODAL;
    int const success = Win::MessageBox(HWND(*this), message, title, options);
	ASSERT(success == IDOK);
}

void Window::ForceRepaint(void) {
	ASSERT(mHandle != 0);
    RECT * const entire_client_area = NULL;
    BOOL const erase = TRUE;
    BOOL const success = Win::InvalidateRect(mHandle, entire_client_area, erase);
    ASSERT(success != 0);
}

HMENU Window::GetMenu(char const *resourceName) {
	HMENU const result = Win::LoadMenu(mModule, resourceName);
	ASSERT(result != NULL);

	return result;
}

bool Window::GetAMessage(MSG &rMessage, int &rExitCode) {
    HWND const any_window = NULL;
	UINT const no_filtering = 0;
    BOOL const success = Win::GetMessage(&rMessage, any_window, no_filtering, no_filtering);

	bool result = false;
	if (success == 0) {   // retrieved a WM_QUIT message
		rExitCode = rMessage.wParam;
		result = true;
	} else if (success == -1) { // error in GetMessage()
        rExitCode = -1;
		result = true;
	}

	return result;
}

LRESULT Window::HandleMessage(MessageType message, WPARAM wParameter, LPARAM lParameter) {
	ASSERT(mHandle != 0);
	LRESULT result = 0;

    switch (message) {
	    case WM_CLOSE: {
			BOOL const success = Win::DestroyWindow(mHandle);
			ASSERT(success);
			break;
		}

        case WM_CREATE: { // initialize window
			CREATESTRUCT * const p_create_struct = (CREATESTRUCT *)lParameter;
			ASSERT(p_create_struct != NULL);
            Initialize(*p_create_struct);
            break;
		}

        case WM_DESTROY: // destroy window
			SelfDestruct();
            break;

        case WM_SIZE: { // resize window
			PCntType const width = LOWORD(lParameter);
			PCntType const height = HIWORD(lParameter);
            SetClientArea(width, height);
            break;
		}

		default:  // invoke default message handler
			result = Win::DefWindowProc(mHandle, message, wParameter, lParameter);
			break;
    }

    return result;
}

// display a simple dialog box with a informational message and an OK button
void Window::InfoBox(char const *message, char const *title) {
	UINT const options = MB_OK | MB_ICONINFORMATION | MB_DEFBUTTON1 | MB_APPLMODAL;
    int const success = Win::MessageBox(mHandle, message, title, options);
	ASSERT(success == IDOK);
}

/* static */ Window *Window::Lookup(HWND handle) {
    KeyType const key = KeyType(handle);
    ConstIterator const i_window = msMap.find(key);

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

int Window::MessageDispatchLoop(void) {
    int exit_code;

	for (;;) {
        MSG message;
		bool const done = GetAMessage(message, exit_code);
        if (done) {
			break;
		}
		TranslateAndDispatch(message);
    }

	return exit_code;
}

HDC Window::PaintDevice(void) const {
	ASSERT(mPaintDevice != NULL);

	return mPaintDevice;
}

void Window::ReleaseMouse(void) {
    Win::ReleaseCapture();
}

void Window::SelfDestruct(void) {
	int const application_exit_code = 0;
    Win::PostQuitMessage(application_exit_code);
}

void Window::SetAcceleratorTable(char const *resourceName) {
	mAcceleratorTable = Win::LoadAccelerators(mModule, resourceName);
	ASSERT(mAcceleratorTable != NULL);
}

void Window::SetClientArea(PCntType width, PCntType height) {
    ASSERT(width < 4000);
    ASSERT(height < 4000);
    mClientAreaWidth = width;
    mClientAreaHeight = height;
}

void Window::SetCursor(LPSTR resource) {
	HINSTANCE const instance = NULL;
	PCntType const desired_width = 0;
    PCntType const desired_height = 0;
	UINT const options = LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_SHARED;
	HCURSOR const handle = (HCURSOR)Win::LoadImage(instance, resource, IMAGE_CURSOR,
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

	KeyType const key = KeyType(handle);
	Pair const new_mapping(key, this); 
	InsertResult const ins_result = msMap.insert(new_mapping);
	bool const success = ins_result.second;
	ASSERT(success);
}

// set large and small icons for a window
void Window::SetIcons(char const *resourceName) {
	ASSERT(mHandle != 0);

	PCntType desired_width;
    PCntType desired_height;
	UINT const options = LR_DEFAULTCOLOR;

	// small icon for title bar
	desired_width = Win::GetSystemMetrics(SM_CXSMICON);
    desired_height = Win::GetSystemMetrics(SM_CYSMICON);
	HICON const small_icon = (HICON)Win::LoadImage(mModule, resourceName, IMAGE_ICON,
		desired_width, desired_height, options);
	ASSERT(small_icon != NULL);

    Win::SendMessage(mHandle, WM_SETICON, WPARAM(ICON_SMALL), LPARAM(small_icon));

	// large icon for ALT+TAB dialog box
	desired_width = Win::GetSystemMetrics(SM_CXICON);
    desired_height = Win::GetSystemMetrics(SM_CYICON);
	HICON const large_icon = (HICON)Win::LoadImage(mModule, resourceName, IMAGE_ICON,
		desired_width, desired_height, options);
	ASSERT(large_icon != NULL);

    Win::SendMessage(mHandle, WM_SETICON, WPARAM(ICON_BIG), LPARAM(large_icon));
}

void Window::SetTimer(unsigned interval_msecs, unsigned event_id) {
	TIMERPROC const callback = NULL;
	UINT_PTR const success = Win::SetTimer(mHandle, event_id, interval_msecs, callback);
    ASSERT(success != 0);
}

void Window::Show(int how) {
	ASSERT(mHandle != 0);

    Win::ShowWindow(mHandle, how);
}

void Window::TranslateAndDispatch(MSG &rMessage) {
	if (mAcceleratorTable != 0) {
	    int const translated = Win::TranslateAccelerator(mHandle, mAcceleratorTable, &rMessage);
	    if (translated != 0) {
			return;
		}
	}
    Win::TranslateMessage(&rMessage); 
    Win::DispatchMessage(&rMessage); 
}

void Window::UpdateMenuBar(void) {
	// redraw all menus
	BOOL const success = Win::DrawMenuBar(mHandle);
	ASSERT(success);
}

void Window::UseFibers(void) {
	// create the first fiber for this thread
	mMainFiber = Win::ConvertThreadToFiber(this);
	ASSERT(mMainFiber != NULL);
}

// display a simple dialog box with a warning message and buttons for Cancel, Try Again, and Continue
int Window::WarnBox(char const *message, char const *title) {
	UINT const options = MB_CANCELTRYCONTINUE | MB_ICONERROR | MB_DEFBUTTON2 | MB_APPLMODAL;
    int const result = Win::MessageBox(mHandle, message, title, options);
	ASSERT(result == IDCANCEL || result == IDTRYAGAIN || result == IDCONTINUE);

    return result;
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

void Window::Yields(void) {
	// switch to main fiber if there's a message ready to dispatch
	if (HasAMessage()) {
	    ASSERT(mMainFiber != NULL);
	    Win::SwitchToFiber(mMainFiber);
	}
}

// inquiry methods

bool Window::HasAMessage(void) const {
	Win::MSG message;
    HWND const any_window = NULL;
	UINT const no_filtering = 0;
	UINT const options = PM_NOREMOVE;
	BOOL const found = Win::PeekMessage(&message, any_window,  no_filtering, no_filtering, options);
	bool const result = (found != 0);

	return result;
}

bool Window::IsMouseCaptured(void) const {
	HWND const captor = Win::GetCapture();
    bool const result = (captor == mHandle);
	
	return result;
}
#endif // defined(_WINDOWS)
