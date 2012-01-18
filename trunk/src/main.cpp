// File:    main.cpp
// Purpose: main() for console version of the Gold Tile game.
// Author:  Stephen Gold sgold@sonic.net

#include "game.hpp"

int main(int argc, char *argv[])
{
	// initialize random seed
    srand(time(NULL));
    
    // game parameters: hard-coded for now
	unsigned numPlayers = 2;
	string playerNames[] = { "Stephen", "Paul" }; // oldest first!
	ACount numAttributes = 2;
	AValue maxAttribute[] = { 5, 5 };
	unsigned tileRedundancy = 3;
	unsigned handSize = 7;
	
	Game game(
        numPlayers,
        playerNames,
        numAttributes,
        maxAttribute,
        tileRedundancy,
        handSize);

    pause();
    return EXIT_SUCCESS;
}
