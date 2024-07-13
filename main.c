#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//TODO: Implement customization?
//TODO: Icons?
//TODO: Refactor?

//Board constants
const unsigned int WIDTH = 20;
const unsigned int HEIGHT = 20;
const unsigned int MineCount = 50;

//UI
const unsigned int CellSize = 30;
const unsigned int CellGap = 3;
const unsigned int CellCenterX = CellSize / 3;
const unsigned int CellCenterY = CellSize / 4;
const unsigned int WindowWidth = WIDTH * (CellSize + CellGap) - CellGap;
const unsigned int WindowHeight = HEIGHT * (CellSize + CellGap) - CellGap;
const unsigned int HudHeight = 70;
const unsigned int HudWidth = WindowWidth;
const unsigned int RestartButtonX1 = (HudWidth - HudHeight) / 2;
const unsigned int RestartButtonX2 = RestartButtonX1 + HudHeight;

typedef enum
{
	Closed,
	Open,
	Flagged
} Cell_state;

typedef struct 
{
	char gameBoard[HEIGHT][WIDTH];
	Cell_state playerBoard[HEIGHT][WIDTH];
	bool gameBoardInitialized;
	bool gameLost;
	bool gameWon;
	bool firstClick;
	unsigned int mineDisplay;
	float timeElapsed;
	Texture2D textures[5];
} Game;

typedef struct
{
	int x;
	int y;
} Pos;

Pos cellLocation(int x, int y)
{
	Pos cell = {0, 0};
	y -= HudHeight;
	cell.x += (x / (CellSize + CellGap)); 
	cell.y += (y / (CellSize + CellGap)); 

	return cell;
}

void initBoard(Game *game)
{
	game->gameBoardInitialized = false;
	game->gameLost = false;
	game->gameWon = false;
	game->firstClick = true;
	game->mineDisplay = MineCount;	
	game->timeElapsed = 0.0;

	for(int i = 0; i < HEIGHT; i++)
	{
		for(int j = 0; j < WIDTH; j++)
		{
			game->gameBoard[i][j] = '0';
			game->playerBoard[i][j] = Closed;
		}
	}
}

void incrementCell(Game *game, int incW, int incH)
{
	if(incH >= 0 && incH < HEIGHT && incW >= 0 && incW < WIDTH && game->gameBoard[incH][incW] != '*')
	{
		game->gameBoard[incH][incW] += 1;
	}
}

bool neighborOfFirstClick(int rWidth, int rHeight, int x, int y)
{
	for(int i = -1; i <= 1; i++)
	{
		for(int j = -1; j <= 1; j++)
		{
			if(rWidth + j == x && rHeight + i == y) return true;
		}
	}

	return false;
}

void populateBoard(Game *game, int x, int y)
{
	unsigned int rWidth;
	unsigned int rHeight;
	unsigned int mines = MineCount;

	while(mines > 0)
	{
		rWidth = rand() % WIDTH;
		rHeight = rand() % HEIGHT;

		char* cell = &(game->gameBoard[rHeight][rWidth]);
		Cell_state state = game->playerBoard[rHeight][rWidth];
		if(*cell != '*' && !neighborOfFirstClick(rWidth, rHeight, x, y))
		{
			*cell = '*';
			mines--;
			for(int i = -1; i <= 1; i++)
			{
				for(int j = -1; j <= 1; j++)
				{
					if(i == 0 && j == 0) continue;
					incrementCell(game, rWidth + j, rHeight + i);
				}
			}
		}			
	}
}

void openEmptyCells(Game *game, int x, int y)
{
	if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
	if(game->playerBoard[y][x] == Open && !(game->firstClick)) return;
	game->firstClick = false;

	game->playerBoard[y][x] = Open;

	if(game->gameBoard[y][x] != '0') return;

	for(int i = -1; i <= 1; i++)
	{
		for(int j = -1; j <= 1; j++)
		{
			if(i == 0 && j == 0) continue;
			openEmptyCells(game, x + j, y + i);	
		}
	}
}

void openNeighbors(Game *game, int x, int y)
{
	int flaggedNeighbors = 0;
	int cellValue = game->gameBoard[y][x] - '0';
	for(int i = -1; i <= 1; i++)
	{
		for(int j = -1; j <= 1; j++)
		{
			if(x + j < 0 || x + j >= WIDTH || y + i < 0 || y + i >= HEIGHT) continue;	
			if(game->playerBoard[y + i][x + j] == Flagged) flaggedNeighbors++;
		}
	}

	if(cellValue != flaggedNeighbors) return;	
	
	for(int i = -1; i <= 1; i++)
	{
		for(int j = -1; j <= 1; j++)
		{
			if(x + j < 0 || x + j >= WIDTH || y + i < 0 || y + i >= HEIGHT) continue;			
			if(game->playerBoard[y + i][x + j] == Flagged || game->playerBoard[y + i][x + j] == Open) continue;

			if(game->gameBoard[y + i][x + j] == '0')
			{
				openEmptyCells(game, x + j, y + i);
			}
			else if(game->gameBoard[y + i][x + j] == '*')
			{
				game->gameLost = true;
			}

			game->playerBoard[y + i][x + j] = Open;
		}
	}	
}

void leftClick(Game *game)
{
	Pos clickPos;
	Pos clickedCell;

	clickPos.x = GetMouseX();		
	clickPos.y = GetMouseY();
	if(clickPos.y <= HudHeight)
	{
		//Restart button
		if(clickPos.x >= RestartButtonX1 && clickPos.x <= RestartButtonX2)
		{
			initBoard(game);
		}
		return;
	}

	if(game->gameLost) return;

	clickedCell = cellLocation(clickPos.x, clickPos.y);

	//First click is always safe
	if(!(game->gameBoardInitialized))
	{
		game->playerBoard[clickedCell.y][clickedCell.x] = Open;
		populateBoard(game, clickedCell.x, clickedCell.y);
		game->gameBoardInitialized = true;
	}
			
	if(game->gameBoard[clickedCell.y][clickedCell.x] == '0')
	{
		openEmptyCells(game, clickedCell.x, clickedCell.y);
	}
	else if(game->gameBoard[clickedCell.y][clickedCell.x] == '*')
	{
		game->gameLost = true;
		game->playerBoard[clickedCell.y][clickedCell.x] = Open;	
	}
	else
	{
		game->playerBoard[clickedCell.y][clickedCell.x] = Open;
	}			
}

void rightClick(Game *game)
{	
	if(game->gameLost) return;

	Pos clickPos;
	Pos clickedCell;

	clickPos.x = GetMouseX();		
	clickPos.y = GetMouseY();
	if(clickPos.y <= HudHeight) return;

	clickedCell = cellLocation(clickPos.x, clickPos.y);

	Cell_state *cell = &(game->playerBoard[clickedCell.y][clickedCell.x]);

	if(*cell == Open)
	{
		openNeighbors(game, clickedCell.x, clickedCell.y);
	}
	else if(*cell == Flagged)
	{
		*cell = Closed;
		game->mineDisplay += 1;
	}
	else if(*cell == Closed)
	{
		*cell = Flagged;
		game->mineDisplay -= 1;
	}
}

void checkIfGameWon(Game *game)
{
	for(int y = 0; y < HEIGHT; y++)
	{
		for(int x = 0; x < WIDTH; x++)
		{
			if(game->playerBoard[y][x] == Closed) return;
		}
	}
	game->gameWon = true;
}

void printBoard(Game game)
{
	ClearBackground(LIGHTGRAY);
	Color cellColor;
	for(int y = 0; y < HEIGHT; y++)
	{
		for(int x = 0; x < WIDTH; x++)
		{
			int cellX = x * (CellSize + CellGap);
			int cellY = y * (CellSize + CellGap);

			if(game.playerBoard[y][x] == Open)
			{
				char cellState = game.gameBoard[y][x];
				if(cellState == '*')
				{
					DrawTexture(game.textures[4], cellX, cellY +HudHeight, WHITE);
					continue;
				}
				else if(cellState == '0') cellColor = GRAY;
			   	else if(cellState == '1') cellColor = BLUE;	
				else if(cellState == '2') cellColor = GREEN;
				else if(cellState == '3') cellColor = ORANGE;
				else if(cellState == '4') cellColor = DARKPURPLE;
				else if(cellState == '5') cellColor = MAROON;
				else if(cellState == '6') cellColor = SKYBLUE;
				else if(cellState == '7') cellColor = BEIGE;
				else if(cellState == '8') cellColor = DARKBROWN;

				char cellText[2] = {cellState, '\0'};
				DrawText(cellText, cellX + CellCenterX, cellY + CellCenterY + HudHeight, 24, cellColor);
			}	
			else if(game.playerBoard[y][x] == Flagged)
			{
				DrawTexture(game.textures[3], cellX, cellY + HudHeight, WHITE);	
			}
			else
			{	
				cellColor = GRAY;
				DrawRectangle(cellX, cellY + HudHeight, CellSize, CellSize, cellColor);
			}	
		}
	}	
}

void printHud(Game game)
{
	//Mines remaining
	DrawText("Mines left: ", 5, HudHeight / 3, 24, BLACK);
	char minesLeftText[3];
	sprintf(minesLeftText, "%d", game.mineDisplay);
	DrawText(minesLeftText, 135, HudHeight / 3 + 2, 24, BLACK);

	//Restart button
	Texture2D smiley;
	if(game.gameLost) smiley = game.textures[1];
	else if(game.gameWon) smiley = game.textures[2];
	else smiley = game.textures[0];
	DrawTexture(smiley, RestartButtonX1, 10, WHITE);
	Rectangle rec = {RestartButtonX1, 10, HudHeight - 20, HudHeight - 20};
	DrawRectangleLinesEx(rec, 2, BLACK);

	//Timer
	DrawText("Time:", HudWidth - 120, HudHeight / 3, 24, BLACK);
	char timeElapsedStr[4];
	sprintf(timeElapsedStr, "%d", (int)game.timeElapsed); 
	DrawText(timeElapsedStr, HudWidth - 55, HudHeight / 3 + 2, 24, BLACK);
}

void LoadTextures(Game *game)
{
	game->textures[0] = LoadTexture("./resources/smiley.png");
	game->textures[1] = LoadTexture("./resources/smiley_lose.png");
	game->textures[2] = LoadTexture("./resources/smiley_win.png");
	game->textures[3] = LoadTexture("./resources/flag.png");
	game->textures[4] = LoadTexture("./resources/mine.png");
}	

int main()
{
	srand(time(NULL));

	Game game;
	initBoard(&game);

	InitWindow(WindowWidth, WindowHeight + HudHeight, "CSweeper");
	SetTargetFPS(30);
	LoadTextures(&game);
	Image icon = LoadImage("./resources/mine.png");
	SetWindowIcon(icon);
	while(!WindowShouldClose())
	{
		BeginDrawing();

		printBoard(game);
		printHud(game);

		EndDrawing();

		if(game.mineDisplay == 0) checkIfGameWon(&game);

		if(!game.firstClick && !game.gameLost && !game.gameWon)
		{	
			game.timeElapsed += GetFrameTime();
		}

		if(IsMouseButtonReleased(0)) 
		{
			leftClick(&game); 	
		}	
		//TODO: Space button scan code is different for different OSes
		else if(IsMouseButtonPressed(1) || IsKeyPressed(32))
		{
			rightClick(&game);	
		}
	}

	CloseWindow();

	return 0;
}
