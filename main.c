#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const unsigned int WIDTH = 20;
const unsigned int HEIGHT = 20;
const unsigned int mineCount = 25;

struct Game
{
	//unsigned int width;
	//unsigned int height;
	char game_board[HEIGHT][WIDTH] = {0};
};

void initBoard(struct Game *game)
{
	for(int i = 0; i < HEIGHT; i++)
	{
		for(int j = 0; j < WIDTH; j++)
		{
			game->game_board[i][j] = '0';
		}
	}
}

void increment_cell(struct Game *game, int inc_h, int inc_w)
{
	if(inc_h > 0 && inc_h < HEIGHT && inc_w > 0 && inc_w < WIDTH && game->game_board[inc_h][inc_w] != '*')
	{
		game->game_board[inc_h][inc_w] += 1;
	}
}

void populateBoard(struct Game *game, unsigned int mines)
{
	unsigned int r_width;
	unsigned int r_height;

	while(mines--)
	{
		r_width = rand() % WIDTH;
		r_height = rand() % HEIGHT;
		game->game_board[r_height][r_width] = '*';	

		for(int i = -1; i <= 1; i++)
		{
			for(int j = -1; j <= 1; j++)
			{
				if(i == 0 && j == 0) continue;
				increment_cell(game, r_height + i, r_width + j);
			}
		}
	}

}

void printBoard(struct Game game)
{
	for(int i = 0; i < HEIGHT; i++)
	{
		for(int j = 0; j < WIDTH; j++)
		{
			printf("%c", game.game_board[i][j]);
		}
		printf("\n");
	}
}

int main()
{
	srand(time(NULL));

	Game newGame;
	initBoard(&newGame);
	populateBoard(&newGame, mineCount);

	printBoard(newGame);

	return 0;
}
