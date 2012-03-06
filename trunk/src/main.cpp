// File:    main.cpp
// Purpose: main entry points for the Gold Tile Game.
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

#include "tile.hpp"

#ifdef _CONSOLE
#include <iostream>
#include "game.hpp"
#include "strings.hpp"
#endif // defined(_CONSOLE)

#ifdef _QT
#include <QtGui/QApplication>
#endif // defined(_QT)

#ifdef _WINDOWS
#include "gui/win_types.hpp"
#endif // defined(_WINDOWS)

#ifdef _GUI
#include "gui/topwindow.hpp"
TopWindow *gTopWindow = NULL;
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
	unsigned seed = ::milliseconds();
    ::srand(seed);

	ACountType attribute_cnt = 2;
	AValueType max_attribute[] = { Tile::VALUE_CNT_DEFAULT - 1, Tile::VALUE_CNT_DEFAULT - 1 };
	Tile::SetStatic(attribute_cnt, max_attribute);

#ifdef _CONSOLE
	argCnt;
	argValues;

	// legal notice
	std::cout
		<< "Gold Tile Game (c) Copyright 2012 Stephen Gold" << std::endl
        << "This program comes with ABSOLUTELY NO WARRANTY." << std::endl
        << "This is free software, and you are welcome to redistribute" << std::endl
        << "it under certain conditions; see LICENSE.txt for details." << std::endl
        << std::endl;

	unsigned hand_cnt = 0;
	while (hand_cnt == 0) {
	    std::cout << "How many hands? ";
	    std::cin >> hand_cnt;
	}

	Strings player_names;
	Indices auto_flags;

	for (unsigned i_hand = 0; i_hand < hand_cnt; i_hand++) {
	    std::cout << "Name of player for the " << ::ordinal(i_hand + 1)
			      << " hand? (or else 'computer') ";
	    String name;
	    std::cin >> name;
	    player_names.Append(name);
	    if (name == "computer") {
			auto_flags.Add(i_hand);
	    }
	}

	unsigned hand_size = 0;
	while (hand_size == 0) {
	    std::cout << "How many tiles per hand? ";
	    std::cin >> hand_size;
	}
	std::cout << std::endl;
	
	// Clone tiles until there are enough to fill each hand at least three times.
	unsigned tiles_needed = 3 * hand_size * hand_cnt;
	long combo_cnt = Tile::CombinationCnt();
	unsigned clones_per_tile = unsigned(tiles_needed / combo_cnt);

    // Instantiate the game.
	Game game(player_names, auto_flags, GAME_STYLE_PRACTICE, clones_per_tile + 1, hand_size);

	game.PlayGame();
	std::cout << "The game is over." << std::endl;
    ::pause();
#endif // defined(_CONSOLE)

#ifdef _QT
	// Instantiate top window and display it.
    QApplication application(argCnt, argValues);

    gTopWindow = new TopWindow(NULL);
    gTopWindow->show();
    
    exit_code = application.exec();
#endif // defined(_QT)

#ifdef _WINDOWS
	// Instantiate top window and display it.
	gTopWindow = new TopWindow(applicationInstance, NULL);
	gTopWindow->Show(showHow);

    // Retrieve and dispatch messages for this application. 
	exit_code = gTopWindow->MessageDispatchLoop();
#endif // defined(_WINDOWS)

	return exit_code;
}

