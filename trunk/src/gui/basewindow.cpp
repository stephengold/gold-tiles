// File:     basewindow.cpp
// Location: src/gui
// Purpose:  implement BaseWindow class
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

#ifdef _WINDOWS
#include "gui/basewindow.hpp"
#include "gui/rect.hpp"
#include "gui/win_types.hpp"


// static data

BaseWindow::Map BaseWindow::msMap;
BaseWindow*     BaseWindow::mspNewlyCreatedWindow = NULL;


// lifecycle

BaseWindow::BaseWindow(void) {
    mHandle = 0;
    mModule = 0;
}

// The implicitly defined destructor is fine.

void BaseWindow::Initialize(CREATESTRUCT const& rCreateStruct) {
    ASSERT(mHandle != 0);
    ASSERT(mModule == 0);

    mModule = rCreateStruct.hInstance;
    ASSERT(mModule != 0);
}


// operators

BaseWindow::operator HWND(void) const {
    return mHandle;
}


// misc methods

// Center a window on its parent.
// If it has no parent, center it on the desktop.
void BaseWindow::Center(void) {
    HWND const this_window = *this;
    HWND owner = Win::GetParent(this_window);
    if (owner == NULL) {
        owner = Win::GetDesktopWindow();
    }

    RECT bounds;
    Win::GetWindowRect(owner, &bounds);
    Rect const owner_bounds(bounds);

    Win::GetWindowRect(this_window, &bounds);
    Rect const window_bounds(bounds);

    PixelCntType const pad_left = (owner_bounds.Width() - window_bounds.Width())/2;
    PixelCntType const pad_top = (owner_bounds.Height() - window_bounds.Height())/2;

    LogicalXType const x = owner_bounds.LeftX() + pad_left;
    LogicalYType const y = owner_bounds.TopY() + pad_top;
    Win::SetWindowPos(this_window, HWND_TOP, x, y, 0, 0, SWP_NOSIZE); 
}

HINSTANCE BaseWindow::CopyModule(BaseWindow const& rOther) {
    mModule = rOther.mModule;
    HINSTANCE const result = mModule;

    return result;
}

void BaseWindow::Create(void) {
    // Make this object accessable to its message handler before WM_CREATE.
    ASSERT(mspNewlyCreatedWindow == NULL);
    mspNewlyCreatedWindow = this;
}

/* static */ Rect BaseWindow::DesktopBounds(void) {
    HWND const desktop_handle = Win::GetDesktopWindow();
    RECT rect;
    Win::GetWindowRect(desktop_handle, &rect);
    Rect const result(rect);

    return result;
}

/* static */ BaseWindow* BaseWindow::Lookup(HWND handle) {
    KeyType const key = KeyType(handle);
    ConstIterator const i_window = msMap.find(key);

    BaseWindow* result = NULL;
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

void BaseWindow::SetHandle(HWND handle) {
    ASSERT(handle != NULL);
    mHandle = handle;

    KeyType const key = KeyType(handle);
    Pair const new_mapping(key, this); 
    InsertResult const ins_result = msMap.insert(new_mapping);
    bool const success = ins_result.second;
    ASSERT(success);
}
#endif // defined(_WINDOWS)
