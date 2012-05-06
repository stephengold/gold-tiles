// File:     window.cpp
// Location: src/gui
// Purpose:  implement Window class
// Author:   Stephen Gold sgold@sonic.net
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
#ifdef _WINDOWS
# include "gui/win_types.hpp"
#endif // defined(_WINDOWS)
#include "string.hpp"


// lifecycle

Window::Window(void) {
#ifdef _WINDOWS
    mAcceleratorTable = 0;
    mPaintDevice = 0;
#endif // defined(_WINDOWS)
}

#ifdef _WINDOWS
void Window::Initialize(CREATESTRUCT const& rCreateStruct) {
    BaseWindow::Initialize(rCreateStruct);

    HWND const this_window = *this;
    HDC const private_dc = Win::GetDC(this_window);
    ASSERT(private_dc != NULL);
    /* It's a private DC because CS_OWNDC is hard-coded into the
    WindowClass constructor. */

    // record size of client area
    SIZE client_area_size;
    BOOL const success = Win::GetWindowExtEx(private_dc, &client_area_size);
    ASSERT(success != 0);
    SetClientArea(client_area_size.cx, client_area_size.cy);

    TextType const icon_resource_name = "GAMEICON";
    SetIcons(icon_resource_name);
}
#endif // defined(_WINDOWS)


// operators

Window::operator Rect(void) const {
    Point const origin(0,0);
    Rect const result(origin, ClientAreaWidth(), ClientAreaHeight());

    return result;
}


// misc methods

#ifdef _WINDOWS
void* Window::AddFiber(void (__stdcall & rStartRoutine)(void *)) {
    Win::SIZE_T const default_stack_size = 0;
    void* const result = Win::CreateFiber(default_stack_size, &rStartRoutine, LPVOID(this));
    ASSERT(result != NULL);

    return result;
}
#endif // defined(_WINDOWS)

void Window::BeginPaint(void) {
#ifdef _WINDOWS
    ASSERT(mPaintDevice == NULL);

    HWND const this_window = *this;
    ASSERT(this_window != NULL);
    mPaintDevice = Win::BeginPaint(this_window, &mPaintStruct);

    ASSERT(mPaintDevice != NULL);
#endif // defined(_WINDOWS)
}

Point Window::Brc(void) const {
    LogicalXType const x = ClientAreaWidth() - 1;
    LogicalYType const y = ClientAreaHeight() - 1;
    Point const result(x, y);

    return result;
}

void Window::CaptureMouse(void) {
#ifdef _WINDOWS
    HWND const this_window = *this;
    Win::SetCapture(this_window);

    ASSERT(IsMouseCaptured());
#endif // defined(_WINDOWS)
}

PixelCntType Window::ClientAreaHeight(void) const {
#ifdef _QT
    QSize const area = size();
    PixelCntType const result = area.height();
    return result;
#elif defined(_WINDOWS)
    return mClientAreaHeight;
#endif // defined(_WINDOWS)
}

PixelCntType Window::ClientAreaWidth(void) const {
#ifdef _QT
    QSize const area = size();
    PixelCntType const result = area.width();
    return result;
#elif defined(_WINDOWS)
    return mClientAreaWidth;
#endif // defined(_WINDOWS)
}

void Window::Close(void) {
#ifdef _WINDOWS
    int const ignored = 0;
    HWND const this_window = *this;

    Win::SendMessage(this_window, WM_CLOSE, WPARAM(ignored), LPARAM(ignored));
#endif // defined(_WINDOWS)
}

#ifdef _WINDOWS
void Window::Create(
    String const& rClassName,
    Rect const& rRect,
    BaseWindow* pParent,
    HINSTANCE applicationInstance)
{
    BaseWindow::Create();

    LPCTSTR const name = Name();
    ASSERT(name != NULL);

    LPCTSTR const class_name = rClassName.c_str();
    ASSERT(class_name != NULL);

    DWORD const window_style = WS_OVERLAPPEDWINDOW;
    PixelCntType const height = rRect.Height();
    PixelCntType const width = rRect.Width();
    LogicalXType const x = rRect.LeftX();
    LogicalYType const y = rRect.TopY();

    HWND parent_handle = NULL;
    if (pParent != NULL) {
        parent_handle = *pParent;
    }

    HMENU const menu_handle = NULL;
    LPVOID const parameters = NULL;

    HWND const handle = Win::CreateWindow(class_name, name, window_style, x, y, 
        width, height, parent_handle, menu_handle, 
        applicationInstance, parameters);
    ASSERT(handle == HWND(*this));
}
#endif // defined(_WINDOWS)

void Window::EndPaint(void) {
#ifdef _WINDOWS
    HWND const this_window = *this;

    ASSERT(this_window != NULL);
    ASSERT(mPaintDevice != NULL);

    Win::EndPaint(this_window, &mPaintStruct);
    mPaintDevice = NULL;
#endif // defined(_WINDOWS)
}

// Display a simple dialog box with a pilot error message and an OK button.
void Window::ErrorBox(TextType message, TextType title) {
#ifdef _WINDOWS
    UINT const options = MB_OK | MB_ICONERROR | MB_DEFBUTTON1 | MB_APPLMODAL;
    int const success = Win::MessageBox(HWND(*this), message, title, options);
    ASSERT(success == IDOK);
#endif // defined(_WINDOWS)
}

void Window::ForceRepaint(void) {
#ifdef _WINDOWS
    HWND const this_window = *this;
    ASSERT(this_window != 0);

    RECT* const entire_client_area = NULL;
    BOOL const erase = TRUE;
    BOOL const success = Win::InvalidateRect(this_window, entire_client_area, erase);
    ASSERT(success != 0);
#endif // defined(_WINDOWS)
}

#ifdef _WINDOWS
bool Window::GetAMessage(MSG& rMessage, int& rExitCode) {
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
    HWND const this_window = *this;
    ASSERT(this_window != 0);
    LRESULT result = 0;

    switch (message) {
    case WM_CLOSE: {
        BOOL const success = Win::DestroyWindow(this_window);
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
        PixelCntType const width = LOWORD(lParameter);
        PixelCntType const height = HIWORD(lParameter);
        SetClientArea(width, height);
        break;
                  }

    default:  // invoke default message handler
        result = Win::DefWindowProc(this_window, message, wParameter, lParameter);
        break;
    }

    return result;
}
#endif // defined(_WINDOWS)

// display a simple dialog box with a informational message and an OK button
void Window::InfoBox(TextType message, TextType title) {
#ifdef _WINDOWS
    HWND const this_window = *this;
    UINT const options = MB_OK | MB_ICONINFORMATION | MB_DEFBUTTON1 | MB_APPLMODAL;
    int const success = Win::MessageBox(this_window, message, title, options);
    ASSERT(success == IDOK);
#endif // defined(_WINDOWS)
}

#ifdef _WINDOWS
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
#endif // defined(_WINDOWS)

// Display a simple dialog box with a question and buttons for Yes and No.
int Window::QuestionBox(TextType message, TextType title) {
#ifdef _QT
    return true;
#elif defined(_WINDOWS)
    HWND const this_window = *this;
    UINT const options = MB_YESNO | MB_ICONASTERISK | MB_DEFBUTTON1 | MB_APPLMODAL;
    int const result = Win::MessageBox(this_window, message, title, options);
    ASSERT(result == IDCANCEL || result == IDYES || result == IDNO);

    return result;
#endif // defined(_WINDOWS)
}

void Window::ReleaseMouse(void) {
#ifdef _WINDOWS
    Win::ReleaseCapture();
#endif // defined(_WINDOWS)
}

// Display a simple dialog box with a retry button.
bool Window::RetryBox(TextType message, TextType title) {
#ifdef _QT
    return true;
#elif defined(_WINDOWS)
    UINT const options = MB_RETRYCANCEL | MB_ICONWARNING | MB_DEFBUTTON1 | MB_APPLMODAL;
    int const success = Win::MessageBox(HWND(*this), message, title, options);
    ASSERT(success == IDRETRY || success == IDCANCEL);
    bool const result = (success == IDRETRY);

    return result;
#endif // defined(_WINDOWS)
}

void Window::SelfDestruct(void) {
#ifdef _WINDOWS
    int const application_exit_code = 0;
    Win::PostQuitMessage(application_exit_code);
#endif // defined(_WINDOWS)
}

void Window::SetAcceleratorTable(TextType resourceName) {
#ifdef _WINDOWS
    HINSTANCE const module = CopyModule(*this);
    mAcceleratorTable = Win::LoadAccelerators(module, resourceName);
    ASSERT(mAcceleratorTable != NULL);
#endif // defined(_WINDOWS)
}

void Window::SetClientArea(PixelCntType width, PixelCntType height) {
    ASSERT(width < 4000);
    ASSERT(height < 4000);
#ifdef _WINDOWS
    mClientAreaWidth = width;
    mClientAreaHeight = height;
#endif // defined(_WINDOWS)
}

#ifdef _WINDOWS
void Window::SetCursor(LPSTR resource) {
    HINSTANCE const instance = NULL;
    PixelCntType const desired_width = 0;
    PixelCntType const desired_height = 0;
    UINT const options = LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_SHARED;
    HCURSOR const handle = (HCURSOR)Win::LoadImage(instance, resource, IMAGE_CURSOR,
        desired_width, desired_height, options);
    ASSERT(handle != NULL);

    Win::SetCursor(handle);
}
#endif // defined(_WINDOWS)

void Window::SetCursorBusy(void) {
#ifdef _WINDOWS
    SetCursor(IDC_WAIT);
#endif // defined(_WINDOWS)
}

void Window::SetCursorDrag(void) {
#ifdef _WINDOWS
    SetCursor(IDC_HAND);
#endif // defined(_WINDOWS)
}

void Window::SetCursorSelect(void) {
#ifdef _WINDOWS
    SetCursor(IDC_ARROW);
#endif // defined(_WINDOWS)
}

// set large and small icons for a window
void Window::SetIcons(TextType resourceName) {
#ifdef _WINDOWS
    HWND const this_window = *this;
    ASSERT(this_window != 0);

    HINSTANCE const module = CopyModule(*this);

    PixelCntType desired_width;
    PixelCntType desired_height;
    UINT const options = LR_DEFAULTCOLOR;

    // small icon for title bar
    desired_width = Win::GetSystemMetrics(SM_CXSMICON);
    desired_height = Win::GetSystemMetrics(SM_CYSMICON);
    HICON const small_icon = HICON(Win::LoadImage(module, resourceName, IMAGE_ICON,
        desired_width, desired_height, options));
    ASSERT(small_icon != NULL);

    Win::SendMessage(this_window, WM_SETICON, WPARAM(ICON_SMALL), LPARAM(small_icon));

    // large icon for ALT+TAB dialog box
    desired_width = Win::GetSystemMetrics(SM_CXICON);
    desired_height = Win::GetSystemMetrics(SM_CYICON);
    HICON const large_icon = HICON(Win::LoadImage(module, resourceName, IMAGE_ICON,
        desired_width, desired_height, options));
    ASSERT(large_icon != NULL);

    Win::SendMessage(this_window, WM_SETICON, WPARAM(ICON_BIG), LPARAM(large_icon));
#endif // defined(_WINDOWS)
}

void Window::SetTimer(unsigned interval_msecs, unsigned event_id) {
#ifdef _WINDOWS
    HWND const this_window = *this;
    TIMERPROC const callback = NULL;
    UINT_PTR const success = Win::SetTimer(this_window, event_id, interval_msecs, callback);
    ASSERT(success != 0);
#endif // defined(_WINDOWS)
}

void Window::Show(int how) {
#ifdef _WINDOWS
    HWND const this_window = *this;
    ASSERT(this_window != 0);

    Win::ShowWindow(this_window, how);
#endif // defined(_WINDOWS)
}

#ifdef _WINDOWS
void Window::TranslateAndDispatch(MSG& rMessage) {
    HWND const this_window = *this;
    if (mAcceleratorTable != 0) {
        int const translated = Win::TranslateAccelerator(this_window, mAcceleratorTable, &rMessage);
        if (translated != 0) {
            return;
        }
    }
    Win::TranslateMessage(&rMessage); 
    Win::DispatchMessage(&rMessage); 
}
#endif // defined(_WINDOWS)

// redraw all menus
void Window::UpdateMenuBar(void) {
#ifdef _WINDOWS
    HWND const this_window = *this;
    BOOL const success = Win::DrawMenuBar(this_window);
    ASSERT(success);
#endif // defined(_WINDOWS)
}

void Window::UseFibers(void) {
#ifdef _WINDOWS
    // create the first fiber for this thread
    mMainFiber = Win::ConvertThreadToFiber(this);
    ASSERT(mMainFiber != NULL);
#endif // defined(_WINDOWS)
}

// display a simple dialog box with a warning message and buttons for Cancel, Try Again, and Continue
int Window::WarnBox(TextType message, TextType title) {
#ifdef _QT
    return 0;
#elif defined(_WINDOWS)
    HWND const this_window = *this;
    UINT const options = MB_CANCELTRYCONTINUE | MB_ICONERROR | MB_DEFBUTTON2 | MB_APPLMODAL;
    int const result = Win::MessageBox(this_window, message, title, options);
    ASSERT(result == IDCANCEL || result == IDTRYAGAIN || result == IDCONTINUE);

    return result;
#endif // defined(_WINDOWS)
}

void Window::WarpCursor(Point const& rDestination) {
#ifdef _WINDOWS
    // convert to screen coordinates
    HWND const this_window = *this;
    POINT point = POINT(rDestination);
    BOOL success = Win::ClientToScreen(this_window, &point);
    ASSERT(success);

    // warp the cursor to new coordinates
    success = Win::SetCursorPos(point.x, point.y);
    ASSERT(success);
#endif // defined(_WINDOWS)
}

void Window::Yields(void) {
#ifdef _WINDOWS
    // switch to main fiber if there's a message ready to dispatch
    if (HasAMessage()) {
        ASSERT(mMainFiber != NULL);
        Win::SwitchToFiber(mMainFiber);
    }
#endif // defined(_WINDOWS)
}


// inquiry methods

bool Window::HasAMessage(void) const {
#ifdef _QT
    return false;
#elif defined(_WINDOWS)
    Win::MSG message;
    HWND const any_window = NULL;
    UINT const no_filtering = 0;
    UINT const options = PM_NOREMOVE;
    BOOL const found = Win::PeekMessage(&message, any_window,  no_filtering, no_filtering, options);
    bool const result = (found != 0);

    return result;
#endif // defined(_WINDOWS)
}

bool Window::IsMouseCaptured(void) const {
#ifdef _QT
    return false;
#elif defined(_WINDOWS)
    HWND const captor = Win::GetCapture();
    HWND const this_window = *this;
    bool const result = (captor == this_window);

    return result;
#endif // defined(_WINDOWS)
}
