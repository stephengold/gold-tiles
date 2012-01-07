#include <iostream>
#include "game.hpp"

int main(int argc, char *argv[])
{
	unsigned numPlayers = 2;
	string playerNames[] = { "Stephen", "Paul" };
	unsigned numAttributes = 2;
	unsigned maxAttribute[] = { 6, 6 };
	unsigned tileRedundancy = 3;
	unsigned numBlankTiles = 0;
	unsigned handSize = 7;
	
	Game game(
        numPlayers,
        playerNames,
        numAttributes,
        maxAttribute,
        tileRedundancy,
        numBlankTiles,
        handSize);

    system("PAUSE");
    return EXIT_SUCCESS;
}
