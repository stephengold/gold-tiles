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

#include "network.hpp"
#include "tile.hpp"

#ifdef _CONSOLE
# include <iostream>
# include "game.hpp"
# include "handopts.hpp"
# include "strings.hpp"
#elif defined(_QT)
# include <QtGui/QApplication>
#elif defined(_WINDOWS)
# include "gui/win_types.hpp"
#endif // defined(_WINDOWS)

#ifdef _GUI
# include "gui/gamewindow.hpp"
#endif // defined(_GUI)

#ifdef _WINDOWS

// Microsoft Windows native application main entry point
int CALLBACK Win::WinMain(
	HINSTANCE applicationInstance, 
	HINSTANCE previousInstance, // always NULL and ignored
	char* commandLine, // TODO
	int showHow) 
{
	previousInstance;
	commandLine;

	GameWindow* p_window = NULL;

#else // !defined(_WINDOWS)

// standard C++ main entry point
int main(int argCnt, char** argValues) {

#endif // !defined(_WINDOWS)

	// Seed the pseudo-random number generator.
	unsigned const seed = ::milliseconds();
    ::srand(seed);

	// Initialize the networking support.
	Network network;

	int exit_code = EXIT_SUCCESS;

#ifdef _CONSOLE

	argCnt;
	argValues;

	Game::ConsoleGame();

#elif defined(_QT)

	// Instantiate top window and display it.
    QApplication application(argCnt, argValues);

    p_window = new GameWindow(NULL);
    p_window->show();
    
    exit_code = application.exec();

#elif defined(_WINDOWS)

	// Instantiate top window and display it.
	p_window = new GameWindow(applicationInstance, NULL);
	p_window->Show(showHow);

    // Retrieve and dispatch messages for this application. 
	exit_code = p_window->MessageDispatchLoop();

#endif // defined(_WINDOWS)

	return exit_code;
}
