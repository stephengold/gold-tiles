// File:    main.cpp
// Purpose: main entry points for the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include <time.h>
#include "game.hpp"

#ifdef _CONSOLE
// console interface

int main(int argc, char *argv[]) {
	// initialize random seed
	unsigned seed = (unsigned)::time(NULL);
    ::srand(seed);
    
    // game parameters: hard-coded for now
	unsigned numPlayers = 2;
	string playerNames[] = { "Stephen", "Paul" }; // oldest first!
	ACount numAttributes = 2;
	AValue maxAttribute[] = { 6, 6 };
	unsigned tileRedundancy = 2;
	unsigned handSize = 7;
	
	Game game(numPlayers, playerNames, numAttributes, maxAttribute,
        tileRedundancy, handSize);
	game.play();

    pause();
    return EXIT_SUCCESS;
}
#endif

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

Game *game = NULL;
TopWindow *topWindow = NULL;

// Windows version
int CALLBACK WinMain(
	HINSTANCE applicationInstance, 
	HINSTANCE previousInstance, // always NULL and ignored
	LPSTR commandLine,
	int showHow) 
{
	// initialize random seed
	unsigned seed = (unsigned)::time(NULL);
    ::srand(seed);

    // game parameters: hard-coded for now
	unsigned numPlayers = 2;
	string playerNames[] = { "Stephen", "Paul" }; // oldest first!
	ACount numAttributes = 2;
	AValue maxAttribute[] = { 6, 6 };
	unsigned tileRedundancy = 3;
	unsigned handSize = 7;

    // Instantiate the game.
	game = new Game(numPlayers, playerNames, numAttributes, maxAttribute,
        tileRedundancy, handSize);

	// Instantiate top window and display it.
	topWindow = new TopWindow(applicationInstance);
	topWindow->show(showHow);

    // Retrieve and dispatch messages for this application. 
	int exitCode = messageDispatchLoop();

	return exitCode;
}

#endif
