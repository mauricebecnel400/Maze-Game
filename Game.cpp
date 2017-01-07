// File: Game.cpp
// Author: 
// Project: CS215 Project 3 Fall 2016
// Description of file contents:

#include "Game.h" 
#include <time.h>
#include <iostream>

static CBitmap images [NUMIMAGES];

using namespace std;

Game::GameSquare::GameSquare ()
{
	// This function will:

	what = PATH;
	imageid = PATHimg;
}

void Game::GameSquare::Display (CDC * deviceContextP)
{
	// This function will:

	CDC memDC;
	int res = memDC.CreateCompatibleDC(deviceContextP);
	memDC.SelectObject(&images[imageid]);
	deviceContextP->TransparentBlt (where.left, where.top, where.Width()+1,where.Height()+1,&memDC, 0, 0, 120, 90, SRCCOPY); 
	DeleteDC (memDC);	
}

Game::Game ()
{
	// This function will:

	numRows = numCols = 1;
	grid = new GameSquare * [numRows];
	for (int r = 0; r < numRows; r++)
		grid[r] = new GameSquare [numCols];
	int res = bgImage.LoadBitmap(CString("BACKGROUND_BMP"));
	res = images[PATHimg].LoadBitmap(CString("PATH_BMP"));		
	res = images[WALLimg].LoadBitmap(CString("WALL_BMP"));
	res = images[MEimg+0].LoadBitmap(CString("CATL_BMP"));
	res = images[MEimg+1].LoadBitmap(CString("CATR_BMP"));
	res = images[ENDGOALimg].LoadBitmap(CString("ENDGOAL_BMP"));
	res = images[FINALGOALimg].LoadBitmap(CString("FINALGOAL_BMP"));

}

Game::~Game ()
{
	// This function will:
	for (int r = 0; r < numRows; r++)
		delete[]grid[r];
	delete[] grid;

}

void Game::Init (int R, int C, CFrameWnd * windowP)
{

	// This function will: Intialize variables and call all functions needed to create game.
	// Generates 4 random goals. 
	for (int r = 0; r < numRows; r++)
		delete [] grid[r];
	delete [] grid;
	numRows = R;
	numCols = C;
	currentRow = currentCol = 1;
	endRow = numRows - 2;
	endCol = numCols - 2;
	finishrow = currentRow;
	finishcol = currentCol;
	score = 0;
	timeLeft = MIN_TIME;
	grid = new GameSquare * [numRows];
	for (int r = 0; r < numRows; r++)
		grid[r] = new GameSquare [numCols];
	Boarder();
	Divide(0, 0, numRows, numCols);
	grid[currentRow][currentCol].what = ME;
	grid[currentRow][currentCol].imageid = MEimg+1;	
	int goal = 0;
	while (goal != 4)
	{
		int randRow = rand() % numRows;
		int randCol = rand() % numCols;
		if (grid[randRow][randCol].what != WALL && grid[randRow][randCol].what != ENDGOAL && grid[randRow][randCol].what != TIMEGOAL)
		{
			grid[randRow][randCol].what = ENDGOAL;
			grid[randRow][randCol].imageid = ENDGOALimg;
			goal++;
		}
	}
}

void Game::Instructions (CFrameWnd * windowP)
{
	// This function will: Display the instrustions at the start of the game. 

	CString message = "You must Hurry, collect the money and escape the cops!\nUse the arrows keys to move up, down, left and right\n"
		"Gather all the money to win the game!\nThe money gives you a little time boost too.";
		;
	CString title = "Instructions";
	windowP->MessageBox (message, title);
}


void Game::Display (CFrameWnd * windowP)
{
	// This function will:

	CPaintDC dc (windowP);
	CRect rect;
	windowP->GetClientRect (&rect);

	CDC memDC;
	int res = memDC.CreateCompatibleDC(&dc);
	memDC.SelectObject(&bgImage);
	dc.TransparentBlt (0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, 2000, 1000, SRCCOPY); 
	DeleteDC (memDC);

	SetUp (rect);
	DisplayGameRect (&dc);
	dc.SetBkMode(TRANSPARENT);	
	CFont font;
	CString fontName = "Tekton Pro";
	font.CreatePointFont(rect.Height() / 4, fontName, &dc);
	CFont* def_font = dc.SelectObject(&font);
	COLORREF def_color = dc.SetTextColor (RGB (0, 0, 0));
	DisplayScoreRect (&dc);
	DisplaySpecialRect (&dc);
	dc.SelectObject(def_font);
	dc.SetTextColor(def_color);
}

void Game::SetUp (CRect rect)
{
	// This function will: Set up the windows on the game.
	int fouthH = rect.Height() / 4;
	int thirdW = rect.Width() / 3;
	int twelfthW = rect.Width() / 12;
	int twelfthH = rect.Height() / 12;

	gameRect = CRect (thirdW+twelfthW/2, twelfthH, rect.Width()-twelfthW, rect.Height()-twelfthH);
	specialRect = CRect (twelfthW, rect.Height() / 2, thirdW-twelfthW/2, 3 * rect.Height() / 4);
	scoreRect = CRect (twelfthW, 3 * rect.Height() / 4, thirdW-twelfthW/2, rect.Height()-twelfthH);

	int sqHeight = gameRect.Height() / numRows;
	int sqWidth = gameRect.Width() / numCols;
	int top = gameRect.top + (gameRect.Height() - sqHeight * numRows) / 2;
	int left = gameRect.left + (gameRect.Width() - sqWidth * numCols) / 2;
	for (int r = 0; r < numRows; r++)
		for (int c = 0; c < numCols; c++)
			grid[r][c].where = CRect(left + c*sqWidth, top + r*sqHeight, left + (c+1)*sqWidth, top + (r+1)*sqHeight);

	int thirdH = specialRect.Height() / 3;
	int halfW = specialRect.Width() / 2;
	timeRect = CRect (specialRect.left + halfW + 2, specialRect.top, specialRect.right - halfW/2 + 2, specialRect.top+thirdH);

	thirdH = scoreRect.Height() / 3;
	gamescoreRect = CRect (scoreRect.left, scoreRect.top, scoreRect.right, scoreRect.top+thirdH);
}

void Game::DisplayGameRect (CDC * deviceContextP)
{
	// This function will: display the maze game

	deviceContextP->Rectangle(gameRect);
	for (int r = 0; r < numRows; r++)
		for (int c = 0; c < numCols; c++)
			grid [r][c].Display (deviceContextP);
}

void Game::DisplayScoreRect (CDC * deviceContextP)
{
	// This function will: displays the score.

	deviceContextP->Rectangle(scoreRect);
	deviceContextP->Rectangle(gamescoreRect);
	char buffer [50];
	sprintf_s (buffer, "Score: %d", score);
	deviceContextP->DrawText(CString(buffer), gamescoreRect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
}

void Game::DisplaySpecialRect (CDC * deviceContextP)
{
	// This function will:

	deviceContextP->Rectangle(specialRect);
	deviceContextP->Rectangle(timeRect);
	char buffer [20];
	sprintf_s (buffer, "%d:%02d", timeLeft/60, (timeLeft < 0 ? 0 : timeLeft%60));
	deviceContextP->DrawText("Time:", CRect(specialRect.left, specialRect.top, timeRect.left, timeRect.bottom), DT_SINGLELINE | DT_VCENTER | DT_RIGHT);
	deviceContextP->DrawText(CString (buffer), timeRect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
}

void Game::Click (int y, int x, CFrameWnd * windowP)
{
	// This function will:

}


void Game::Move (char direction, CFrameWnd * windowP)
{
	// This function will: move your character and checks to see if you are collecting goals.

	CRect oldWhere = grid[currentRow][currentCol].where | grid[currentRow-1][currentCol].where;
	switch (direction)
	{
		case 'a': // moving left
			if (grid[currentRow][currentCol - 1].what == PATH)
			{
				grid[currentRow][currentCol].what = ME;
				grid[currentRow][currentCol].imageid = MEimg + 0;
				grid[currentRow][currentCol - 1].what = ME;
				grid[currentRow][currentCol - 1].imageid = MEimg + 0;
				grid[currentRow][currentCol].what = PATH;
				grid[currentRow][currentCol].imageid = PATHimg;
				currentCol = currentCol - 1;
			}
			else if (grid[currentRow][currentCol - 1].what == ENDGOAL)
			{
				grid[currentRow][currentCol].what = ME;
				grid[currentRow][currentCol].imageid = MEimg + 0;
				grid[currentRow][currentCol - 1].what = ME;
				grid[currentRow][currentCol - 1].imageid = MEimg + 0;
				grid[currentRow][currentCol].what = PATH;
				grid[currentRow][currentCol].imageid = PATHimg;
				currentCol = currentCol - 1;
				score++;
				windowP->InvalidateRect(scoreRect);
			}
			else if (grid[currentRow][currentCol - 1].what == TIMEGOAL)
			{
				grid[currentRow][currentCol].what = ME;
				grid[currentRow][currentCol].imageid = MEimg + 0;
				grid[currentRow][currentCol - 1].what = ME;
				grid[currentRow][currentCol - 1].imageid = MEimg + 0;
				grid[currentRow][currentCol].what = TIMEGOAL;
				grid[currentRow][currentCol].imageid = TIMEGOALimg;
				currentCol = currentCol - 1;
				timeLeft += 5;

			}
			break;

		case 'w': // moving up
			if (grid[currentRow - 1][currentCol].what == PATH)
			{
				grid[currentRow][currentCol].what = ME;
				grid[currentRow][currentCol].imageid = MEimg + 0;
				grid[currentRow - 1][currentCol].what = ME;
				grid[currentRow - 1][currentCol].imageid = MEimg + 1;
				grid[currentRow][currentCol].what = PATH;
				grid[currentRow][currentCol].imageid = PATHimg;
				currentRow = currentRow - 1;
			}
			else if (grid[currentRow - 1][currentCol].what == ENDGOAL)
			{
				grid[currentRow][currentCol].what = ME;
				grid[currentRow][currentCol].imageid = MEimg + 0;
				grid[currentRow - 1][currentCol].what = ME;
				grid[currentRow - 1][currentCol].imageid = MEimg + 1;
				grid[currentRow][currentCol].what = PATH;
				grid[currentRow][currentCol].imageid = PATHimg;
				currentRow = currentRow - 1;
				score++;
				windowP->InvalidateRect(scoreRect);
			}
			break;

		case 'd': // moving right
			if (grid[currentRow][currentCol + 1].what == PATH)
			{
				grid[currentRow][currentCol].what = ME;
				grid[currentRow][currentCol].imageid = MEimg + 1;
				grid[currentRow][currentCol + 1].what = ME;
				grid[currentRow][currentCol + 1].imageid = MEimg + 1;
				grid[currentRow][currentCol].what = PATH;
				grid[currentRow][currentCol].imageid = PATHimg;
				currentCol = currentCol + 1;
			}
			else if (grid[currentRow][currentCol + 1].what == ENDGOAL)
			{
				grid[currentRow][currentCol].what = ME;
				grid[currentRow][currentCol].imageid = MEimg + 1;
				grid[currentRow][currentCol + 1].what = ME;
				grid[currentRow][currentCol + 1].imageid = MEimg + 1;
				grid[currentRow][currentCol].what = PATH;
				grid[currentRow][currentCol].imageid = PATHimg;
				currentCol = currentCol + 1;
				score++;
				windowP->InvalidateRect(scoreRect);
			}
			break;

		case 's': // moving down
			if (grid[currentRow + 1][currentCol].what == PATH)
			{
				grid[currentRow][currentCol].what = ME;
				grid[currentRow][currentCol].imageid = MEimg + 1;
				grid[currentRow + 1][currentCol].what = ME;
				grid[currentRow + 1][currentCol].imageid = MEimg + 1;
				grid[currentRow][currentCol].what = PATH;
				grid[currentRow][currentCol].imageid = PATHimg;
				currentRow = currentRow + 1;
			}
			else if (grid[currentRow + 1][currentCol].what == ENDGOAL)
			{
				grid[currentRow][currentCol].what = ME;
				grid[currentRow][currentCol].imageid = MEimg + 1;
				grid[currentRow + 1][currentCol].what = ME;
				grid[currentRow + 1][currentCol].imageid = MEimg + 1;
				grid[currentRow][currentCol].what = PATH;
				grid[currentRow][currentCol].imageid = PATHimg;
				currentRow = currentRow + 1;
				score++;
				windowP->InvalidateRect(scoreRect);

			}
			break;
	}
	CRect newWhere = grid[currentRow][currentCol].where | grid[currentRow-1][currentCol].where;
	windowP->InvalidateRect (oldWhere | newWhere);
}

void Game::Message (CFrameWnd * windowP)
{
	// This function will:

	CString message, title;
	if (timeLeft > 0)
	{
		
		message = "Congrads You collected all the money here!\nLets move on.";
		title = "Congratulations!";
	}
	else
	{
		message = "Cops caught up to you!";
		title = "Sorry...";
	}
	windowP->KillTimer(1);
	windowP->MessageBox (message, title);
}

bool Game::Done(CFrameWnd * windowP)
{
	// This function will:
	if (timeLeft < 1 || score == 4)
	{
		return true;
	}

	return false;
}

void Game::UpdateTime (CFrameWnd * windowP)
{
	// This function will: Update the elapsed time each time it is called by OnTimer.

	timeLeft--;
	if (timeLeft <= 0)
		windowP->KillTimer(1);
	windowP->InvalidateRect (timeRect);
}
void Game::UpdateScore(CFrameWnd * windowP)
{
	windowP->InvalidateRect(scoreRect);
}

void Game::Boarder()
{
		// Creates a boarder of Xs around the grid.
	for (int r = 0; r < numRows; r++)
	{
		grid[r][numCols - 1].what = WALL;
		grid[r][numCols - 1].imageid = WALLimg;
		grid[r][0].what = WALL;
		grid[r][0].imageid = WALLimg;
	}
	for (int c = 0; c < numCols; c++)
	{
		grid[numRows - 1][c].what = WALL;
		grid[numRows - 1][c].imageid = WALLimg;
		grid[0][c].what = WALL;
		grid[0][c].imageid = WALLimg;
	}

}

int Game::RD(int top, int bottom, int left, int right)
{
	//This funcion will: generate a randow row. 
	int possibleRows = (bottom - top) / 2 - 1;
	int R = 1 + rand() % possibleRows;
	int row = top + 2 * R;
	return row;
}

int Game::CD(int top, int bottom, int left, int right)
{
	//This function will: generate a randow column.
	int possibleCols = (right - left) / 2 - 1;
	int R = 1 + rand() % possibleCols;
	int col = left + 2 * R;
	return col;
}


void Game::Divide(int top, int left, int bottom, int right)
{
	// This function does a recursive division algorithm
	// to create the maze, it also randomly generates 3 doors in order
	// to make a maze that has exactly one solution.
	srand(time(NULL));

	if (bottom - top < 4 || right - left < 4) {
		return;
	}

	// row and col will become the positions of two randomly generated
	int row = RD(top, bottom, left, right);
	int col = CD(top, bottom, left, right);

	// Add the walls based on the random row and col
	for (int c = left; c < right; c++) {
		grid[row][c].what = WALL;
		grid[row][c].imageid = WALLimg;
	}
	for (int r = top; r < bottom; r++) {
		grid[r][col].what = WALL;
		grid[r][col].imageid = WALLimg;
	}


	// Generates 4 random doors
	int odd1 = 1 + (2 * (rand() % ((col - left) / 2)));
	int odd2 = 1 + (2 * (rand() % ((right - col) / 2)));
	int odd3 = 1 + (2 * (rand() % ((row - top) / 2)));
	int odd4 = 1 + (2 * (rand() % ((bottom - row) / 2)));
	int door1 = left + odd1;
	int door2 = col + odd2;
	int door3 = top + odd3;
	int door4 = row + odd4;


	// Randomly chooses one door to skip
	// this creates only one path to create the maze.
	int skip = 1 + rand() % 4;

	if (skip != 1) {
		grid[row][door1].what = PATH;
		grid[row][door1].imageid = PATHimg;
	}
	if (skip != 2) {
		grid[row][door2].what = PATH;
		grid[row][door2].imageid = PATHimg;
	}
	if (skip != 3) {
		grid[door3][col].what = PATH;
		grid[door3][col].imageid = PATHimg;
	}
	if (skip != 4) {
		grid[door4][col].what = PATH;
		grid[door4][col].imageid = PATHimg;
	}


	// Recursively calls. 
	Divide(top, left, row, col); // UL
	Divide(top, col, row, right); // UR
	Divide(row, left, bottom, col); // LL
	Divide(row, col, bottom, right); // LR
}
