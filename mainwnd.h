/**
	mainwnd.h
	CMainWnd class interface file
*/

#ifndef _MAINWND_H_
#define _MAINWND_H_

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include "RanKaro.h"
#include "resource.h"

typedef char CHARTYPE;

/**
	Main window class
*/
class CMainWnd {
	private:
		CRanKaro *ranKaro;					// Core processing
		CHighScore *hScore;					// High score object
		CFileBuffer *fileBuffer;			// Loaded buffer from file

		WNDCLASSEX mainWndCl;				// Main window class
		HINSTANCE hMainInst;				// Main window instance
		HWND hMainWnd;						// Main window handle
		HWND hMatrixWnd[9][9];				// Matrix button handle
		HWND hRandShowWnd[3];				// Random number show handle
		HWND hRandButton;					// Random button handle
		HWND hScoreShow;					// Score show handle
		HMENU hMenu;						// The main window menu

		int randNum[3];						// Random numbers generated
		int gameMode;						// Game playing mode
		bool randButtonClicked;				// Button clicking state
		bool gameSaved;						// Save state of the game

		void createInterface();						// Main window interface creating method
		void displayRandNum();						// Random numbers showing method
		bool fillRandNum(LPARAM);					// Random numbers filling method
		void getMatrixPos(HWND, int&, int&);		// Numbers matrix position getting method
		void init();								// Initialize a game
		void loadRanKaro(CRanKaro*);				// Load game from specified CRanKaro object
		void mutexCheck();							// Main window duplication checking method
		void setHotKey();							// Set hotkeys to the program
		void setRandButton(bool);					// Random button state setting method
		void updateHighScore(int);					// Update high score
		void updateMatrix();						// Update the matrix of numbers

	public:
		// CMainWnd class methods
		CMainWnd(HINSTANCE);						// Default constructor
		virtual ~CMainWnd();						// Destructor
		void colorMatrix(WPARAM, LPARAM);			// Color the matrix of numbers
		void fill(LPARAM);							// Fill random numbers
		void insertHighScore(char*);				// Insert a new high score
		void resetHighScore();						// Reset high score
		void setGameMode(int);						// Set game mode
		void showHighScore(HWND);					// Show high score to high score dialog

		// Menu functions
		void newGame();								// Menu Game -> New game
		void newGame_Single();						// New game - single mode
		void newGame_Multi(int);					// New game - multiplayer mode
		void joinGame();							// Menu Game -> Join game
		void saveGame();							// Menu Game -> Save game
		void resumeGame();							// Menu Game -> Resume game
		void close();								// Menu Game -> Exit
		void random();								// Menu Game -> Random
		void undoMove();							// Menu Option -> Undo
		void highScore();							// Menu Option -> High score
		void configGame();							// Menu Option -> Game config
		void configMulti();							// Menu Option -> Multiplayer config
		void help();								// Menu Help -> Game instructions
		void about();								// Menu Help -> About
};

#endif

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK NewGameProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK JoinGameProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK GameConfigProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK CommonProc(HWND, UINT, WPARAM, LPARAM);
