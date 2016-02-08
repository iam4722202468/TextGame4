#include <string>
#include "gameController.h"

int main()
{
	GameController game;
	game.parseFile("game.txt");
	game.storyline = ";setup;";
	game.start(); //put me in a thread!
	return 0;
}
