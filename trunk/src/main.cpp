// File:     main.cpp
// Purpose:  main entry points for the Gold Tile Game.
// Location: src
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

#include "tile.hpp"

#ifdef _CONSOLE
#include <iostream>
#include "game.hpp"
#include "handopts.hpp"
#endif // defined(_CONSOLE)

#ifdef _QT
#include <QtGui/QApplication>
#endif // defined(_QT)

#ifdef _WINDOWS
#include "gui/win_types.hpp"
#endif // defined(_WINDOWS)

#ifdef _GUI
#include "gui/gamewindow.hpp"
GameWindow *gGameWindow = NULL;
#endif // defined(_GUI)

#ifdef _WINDOWS

// Microsoft Windows native application main entry point
int CALLBACK Win::WinMain(
	HINSTANCE applicationInstance, 
	HINSTANCE previousInstance, // always NULL and ignored
	char *commandLine, // TODO
	int showHow) 
{
	previousInstance;
	commandLine;

#else // !defined(_WINDOWS)

// standard C++ main entry point
int main(int argCnt, char *argValues[]) {

#endif // !defined(_WINDOWS)

	int exit_code = EXIT_SUCCESS;

	// seed the pseudo-random number generator
	unsigned const seed = ::milliseconds();
    ::srand(seed);

#ifdef _CONSOLE
	argCnt;
	argValues;

	// display legal notice
	std::cout
		<< "Gold Tile Game (c) Copyright 2012 Stephen Gold" << std::endl
        << "This program comes with ABSOLUTELY NO WARRANTY." << std::endl
        << "This is free software, and you are welcome to redistribute" << std::endl
        << "it under certain conditions; see LICENSE.txt for details." << std::endl
        << std::endl;

	GameOpt game_opt;
	game_opt.GetUserChoice();

	unsigned const hand_cnt = game_opt.HandsDealt();
	HandOpts hand_opts;
	hand_opts.GetUserChoice(hand_cnt);

    // Instantiate the game.
	Game game(game_opt, hand_opts);

	game.PlayGame();
	std::cout << "The game is over." << std::endl;

	// A pause is needed when running in a console window because the window will
	// be destroyed soon after this function returns. 
    ::pause();
#endif // defined(_CONSOLE)

#ifdef _QT
	// Instantiate top window and display it.
    QApplication application(argCnt, argValues);

    gGameWindow = new GameWindow(NULL);
    gGameWindow->show();
    
    exit_code = application.exec();
#endif // defined(_QT)

#ifdef _WINDOWS
	// Instantiate top window and display it.
	gGameWindow = new GameWindow(applicationInstance, NULL);
	gGameWindow->Show(showHow);

    // Retrieve and dispatch messages for this application. 
	exit_code = gGameWindow->MessageDispatchLoop();
#endif // defined(_WINDOWS)

	return exit_code;
}

