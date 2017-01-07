// File: Game.h
// Author: 
// Project: CS215 Project 3 Fall 2016
// Description of file contents:

#ifndef GAME_H
#define GAME_H

#include <afxwin.h>

#define NUMIMAGES 10
#define MIN_TIME 30

enum square_types {PATH, WALL, ME, GOAL, TIMEGOAL, ENDGOAL, FINALGOAL};
enum image_type { MEimg = 0, PATHimg = 3, WALLimg, TIMEGOALimg, ENDGOALimg, FINALGOALimg };

class Game
{
	public:
		Game ();
		Game (const Game & other);
		~Game ();
		Game & operator = (const Game & other);
		void Init (int R, int C, CFrameWnd * windowP);
		void GenerateMaze(int numrows, int numcols);
		void Instructions (CFrameWnd * windowP);
		void Display (CFrameWnd * windowP);
		void Click (int x, int y, CFrameWnd * windowP);
		void Move (char dir, CFrameWnd * windowP);
		void Message (CFrameWnd * windowP);
		void UpdateTime (CFrameWnd * windowP);		
		bool Done (CFrameWnd * windowP);
		void UpdateScore(CFrameWnd * windowP);
		int RD (int top, int bottom, int left, int right);
		int CD(int top, int bottom, int left, int right);
		void Boarder();
		void Divide(int top, int left, int bottom, int right);
	private:
		void SetUp (CRect window);
		void DisplayGameRect (CDC * deviceContextP);
		void DisplayScoreRect (CDC * deviceContextP);
		void DisplaySpecialRect (CDC * deviceContextP);

		struct GameSquare
		{
			GameSquare ();
			void Display (CDC * deviceContextP);
			square_types what;
			int points;	// for goal
			int imageid;
			CRect where;
		};

		GameSquare ** grid;
		int numRows;
		int numCols;
		int score;
		int timeLeft;
		int currentRow, currentCol;
		int endRow, endCol;
		int finishrow, finishcol;
		CRect gameRect;
		CRect specialRect;
		CRect scoreRect;
		CRect timeRect;
		CRect gamescoreRect;
		CBitmap bgImage;
};

#endif
