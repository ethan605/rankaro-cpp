/**
	RanKaro.h
	CRanKaro class interface file
*/

#ifndef _RANKARO_H_
#define _RANKARO_H_

#include <iostream>
#include <fstream>
#include <stdint.h>
#include <windows.h>
#include <stdio.h>
#include <stack>

#define DEFAULT_VALUE 0
#define FILLED_COLOR 1
#define BINGO_COLOR 2

const int MATRIX_STREAM_SIZE = 9*9*4;
const int HSCORE_STREAM_SIZE = 10*(9+5);
const int FILE_STREAM_SIZE = MATRIX_STREAM_SIZE + HSCORE_STREAM_SIZE;
const uint32_t DEFAULT_KEY[4] = {0x12345678, 0x23456789, 0x3456789a, 0x456789ab};

typedef int** intPtr;
typedef char CHARTYPE;

class CGameMove {
	public:
		int number[3];							// Random numbers generated in move
		int xPos, yPos;							// Positions of random numbers in move
		CGameMove();							// Default constructor
		CGameMove(int, int, int, int, int);		// Constructor
		void operator=(const CGameMove&);		// Copy constructor
		void show();							// Show the last moves
};

class CHighScore {
	private:
		int* savedScore;						// Saved high scores
		char** savedName;						// Saved high score users
	public:
		CHighScore();							// Default constructor
		~CHighScore();							// Destructor
		char** getSavedName();					// Get the saved names list
		int* getSavedScore();					// Get the saved scores list
		void insertHighScore(char*, int);		// Insert a high score to high score record
		void loadHighScore(char*);				// Load high scores from file buffer
		int newHighScore(int);					// Check if a score whether a new high score or not
		void resetHighScore();					// Reset the high scores
		void showHighScore(HWND);				// Show high scores
};

class CRanKaro {
	private:
		int score;											// Game score
		intPtr numMap, colorMap;							// Numbers map & color map
		std::stack<CGameMove> undoStack;					// Undo & redo moves stack

		int updateColorRow(int);							// Color the map by rows
		int updateColorColumn(int);							// Color the map by columns
		int updateColorDiagon(int, int, bool);				// Color the map by diagonal lines

	public:
		CRanKaro();											// Default constructor
		CRanKaro(intPtr, intPtr);							// Constructor
		~CRanKaro();										// Destructor
		bool addRandNum(int, int, int, int, int, bool);		// Add numbers into map
		int getGameScore();									// Return the game score
		intPtr getNumMap();									// Get the number map
		bool isEmpty();										// Check if matrix is empty
		bool isFull();										// Check if matrix is full
		bool loadGame(char*);								// Load a game from file buffer
		void showMap(int);									// Show color map
		bool undoMove(CGameMove&);							// Undo a move
		intPtr updateMapColor();							// Color the map
};

class CFileBuffer {
	public:
		char* fileStream;								// Loaded buffer from save file

		uint32_t cypherToUInt(char*, int, int);			// Convert cypher string to uint32_t code
		bool streamValid();								// Check if the stream read is valid or not
		uint32_t sourceToUInt(char*, int, int);			// Convert source string to uint32_t code
		void TEAEncrypt(uint32_t*, const uint32_t*);	// Tiny Encryption Algorithm encrypt method
		void TEADecrypt(uint32_t*, const uint32_t*);	// Tiny Encryption Algorithm decrypt method
		char* UIntToCypher(uint32_t);					// Convert uint32_t code to cypher string
		char* UIntToSource(uint32_t);					// Convert uint32_t code to source string
	public:
		CFileBuffer();									// Default constructor
		CFileBuffer(char*);								// Constructor
		~CFileBuffer();									// Destructor
		char* getFileStream();							// Return the whole stream buffer
		char* getFileStream(int, int);					// Return parts of stream buffer
		void loadFile(char*);							// Load stream buffer from file
		void resetHighScore();							// Reset high score part of stream buffer
		void resetMap();								// Reset numbers & color map part of stream buffer
		void saveBuffer(CRanKaro*, CHighScore*);		// Save data from CRanKaro & CHighScore objects
		void saveHighScore(CHighScore*);				// Save data from CHighScore object
		void saveMap(CRanKaro*);						// Save data from CRanKaro object
		void saveFile(char*);							// Save stream buffer to file
};

#endif
