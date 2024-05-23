#include <stdio.h>

const unsigned int WIDTH = 20;
const unsigned int HEIGHT = 20;
const unsigned int mineCount = 25;

struct Game
{
	//unsigned int width;
	//unsigned int height;
	char game_board[HEIGHT][WIDTH] = {0};
};

void populateBoard(struct Game *game, unsigned int mines)
{
	while(mines--)
	{
		//TODO: Generate random coordinates, place mine there, and increment neighbouring cells
		
	}

}

int main()
{
	Game newGame;
	populateBoard(&newGame, mineCount);

	return 0;
}
