// File:     main.cpp
// Location: src
// Purpose:  main entry points for the Gold Tile Game.
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

#include "gui/gamewindow.hpp"
#include "network.hpp"
#ifdef _QT
# include <QtGui/QApplication>
#elif defined(_WINDOWS)
# include "gui/win_types.hpp"
#endif // defined(_WINDOWS)


#ifdef _QT

// standard C++ main entry point
int main(int argCnt, char** argValues) {

#elif defined(_WINDOWS)

// Microsoft Windows native application main entry point
int CALLBACK Win::WinMain(
    HINSTANCE applicationInstance, 
    HINSTANCE previousInstance, // always NULL and ignored
    char* commandLine, // TODO
    int showHow) 
{
    previousInstance;
    commandLine;

#endif // defined(_WINDOWS)

    GameWindow* p_window = NULL;

    // Seed the pseudo-random number generator.
    unsigned const seed = ::milliseconds();
    Fraction::ReseedGenerator(seed);

    // Initialize the networking support.
    Network network;

    int exit_code = EXIT_SUCCESS;

#ifdef _QT

    QApplication application(argCnt, argValues);

    // Instantiate a game window and display it.
    p_window = new GameWindow(NULL);
    p_window->show();

    exit_code = application.exec();

#elif defined(_WINDOWS)

    // Instantiate a game window and display it.
    p_window = new GameWindow(applicationInstance, NULL);
    p_window->Show(showHow);

    // Retrieve and dispatch messages for this application. 
    exit_code = p_window->MessageDispatchLoop();

#endif // defined(_WINDOWS)

    return exit_code;
}
