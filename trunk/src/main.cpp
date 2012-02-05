// File:    main.cpp
// Purpose: main entry points for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU Lesser General Public License

#include <time.h>
#include "game.hpp"
#include "string.hpp"
#include "strings.hpp"

#ifdef _WINDOWS
// Microsoft Windows(tm) GUI interface

#include <windows.h>
#include "gui/topwindow.hpp"

static int messageDispatchLoop(void) {
    int exitCode;
    
	while (true) {
        MSG msg;
	    HWND anyWindow = NULL;
        BOOL success = ::GetMessage(&msg, anyWindow, 0, 0);
        if (success == 0) {   // retrieved a WM_QUIT message
			exitCode = msg.wParam;
			break;
		} else if (success == -1) { // error in GetMessage()
            exitCode = -1;
			break;
		}
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }

	return exitCode;
}

TopWindow *gTopWindow = NULL;

// Windows main entry point
int CALLBACK WinMain(
	HINSTANCE applicationInstance, 
	HINSTANCE previousInstance, // always NULL and ignored
	LPSTR commandLine,
	int showHow) 
{
#endif

#ifdef _CONSOLE
// console main entry point
int main(int argc, char *argv[]) {
#endif

	// seed the random number generator
	unsigned seed = (unsigned)::time(NULL);
    ::srand(seed);

    // game parameters: hard-coded for now
	Strings player_names;
	player_names.Append("Stephen"); // oldest first
	player_names.Append("Paul");
	player_names.Append("Gale");
	ACountType attribute_cnt = 2;
	AValueType max_attribute[] = { 5, 5 };
	unsigned tile_redundancy = 3;
	unsigned hand_size = 7;

    // Instantiate the game.
	Game game(player_names, attribute_cnt, max_attribute,
        tile_redundancy, hand_size);

#ifdef _CONSOLE
	game.PlayGame();

    ::pause();

	int exitCode = EXIT_SUCCESS;
#endif

#ifdef _WINDOWS
	// Instantiate top window and display it.
	gTopWindow = new TopWindow(applicationInstance, &game);
	gTopWindow->Show(showHow);

    // Retrieve and dispatch messages for this application. 
	int exitCode = messageDispatchLoop();
#endif

	return exitCode;
}

