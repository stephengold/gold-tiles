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

#include <iostream>
#include <time.h>
#include "game.hpp"
#include "strings.hpp"

#ifdef _GUI
#include "gui/topwindow.hpp"
#include "gui/win_types.hpp"

TopWindow *gTopWindow = NULL;

// Windows main entry point
int CALLBACK Win::WinMain(
	HINSTANCE applicationInstance, 
	HINSTANCE previousInstance, // always NULL and ignored
	char *commandLine, // TODO
	int showHow) 
{
	previousInstance;
	commandLine;
#endif  // defined(_GUI)

#ifdef _CONSOLE
// console main entry point
int main(int argCnt, char *argValues[]) {
#endif // defined(_CONSOLE)

	// seed the random number generator
	unsigned seed = (unsigned)::time(NULL);
    ::srand(seed);

	ACountType attribute_cnt = 2;
	AValueType max_attribute[] = { 3, 3 };
	Tile::SetStatic(attribute_cnt, max_attribute);

#ifdef _CONSOLE
	// legal notice
	std::cout
		<< "Gold Tile Game (c) Copyright 2012 Stephen Gold" << std::endl
        << "This program comes with ABSOLUTELY NO WARRANTY." << std::endl
        << "This is free software, and you are welcome to redistribute it" << std::endl
        << "under certain conditions; see LICENSE.txt for details." << std::endl
        << std::endl;

    // game parameters: hard-coded for now
	Strings hand_names;
	hand_names.Append("Stephen"); // oldest first
	hand_names.Append("Paul");
	hand_names.Append("Gale");

    // Instantiate the game.
	Game game(hand_names, GAME_STYLE_PRACTICE);

	game.PlayGame();
    ::pause();
	int exitCode = EXIT_SUCCESS;
#endif // defined(_CONSOLE)

#ifdef _GUI

	// Instantiate top window and display it.
	gTopWindow = new TopWindow(applicationInstance, NULL);
	gTopWindow->Show(showHow);

    // Retrieve and dispatch messages for this application. 
	int exitCode = gTopWindow->MessageDispatchLoop();
#endif // defined(GUI)

	return exitCode;
}

