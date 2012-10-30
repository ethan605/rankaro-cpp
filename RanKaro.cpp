/**
	RanKaro.cpp
	CRanKaro class implementation file
*/

#include "RanKaro.h"
#include "mainwnd.h"

/**************************************************/
inline uint32_t CFileBuffer::cypherToUInt(char* cypher, int start, int len) {
	uint32_t result = 0, temp;
	char buffer[3] = "";

	for (int i = 0; i < len; i += 2) {
		buffer[0] = cypher[start+i];
		buffer[1] = cypher[start+i+1];
		sscanf(buffer, "%x", &temp);
		result |= (temp << (i/2)*8);
	}
	return result;
}

inline bool CFileBuffer::streamValid() {
	int position;

	for (int i = 0; i < MATRIX_STREAM_SIZE; i++)
		if ((this->fileStream[i] > '9' || this->fileStream[i] < '0') && (this->fileStream[i] != ' '))
			return false;
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 5; j++) {
			position = MATRIX_STREAM_SIZE+i*14+9+j;
			if ((this->fileStream[position] > '9' || this->fileStream[position] < '0') && (this->fileStream[position] != ' '))
				return false;
		}
	return true;
}

inline uint32_t CFileBuffer::sourceToUInt(char* source, int start, int len) {
	uint32_t result = 0;
	for (int i = 0; i < len; i++)
		result |= (source[start+i] << i*8);
	return result;
}

inline void CFileBuffer::TEADecrypt(uint32_t* v, const uint32_t* k) {
	uint32_t v0 = v[0], v1 = v[1], sum = 0, i;
    uint32_t delta = 0x9e3779b9;
    uint32_t k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3];
    for (i = 0; i < 32; i++) {
		sum += delta;
		v0 += ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
		v1 += ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
    }
    v[0] = v0; v[1] = v1;
}

inline void CFileBuffer::TEAEncrypt(uint32_t* v, const uint32_t* k) {
	uint32_t v0 = v[0], v1 = v[1], sum = 0xC6EF3720, i;
    uint32_t delta = 0x9e3779b9;
    uint32_t k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3];
    for (i = 0; i < 32; i++) {
        v1 -= ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
        v0 -= ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
        sum -= delta;
    }
    v[0] = v0; v[1] = v1;
}

inline char* CFileBuffer::UIntToCypher(uint32_t code) {
	char* result = new char[9];
	unsigned char cypherBuffer[5] = "";
	char buffer[3] = "";
	for (int i = 0; i < 8; i += 2) {
		cypherBuffer[i/2] = (code >> ((i/2)*8));
		sprintf(buffer, "%x", (int)cypherBuffer[i/2]);
		result[i] = buffer[0];
		result[i+1] = buffer[1];
	}
	result[8] = NULL;
	return result;
}

inline char* CFileBuffer::UIntToSource(uint32_t code) {
	char* result = new char[5];
	for (int i = 0; i < 4; i++)
		result[i] = (code >> i*8);
	result[4] = NULL;
	return result;
}

CFileBuffer::CFileBuffer() {
	this->fileStream = new char[FILE_STREAM_SIZE+1];
	for (int i = 0; i < FILE_STREAM_SIZE; i++)
		this->fileStream[i] = ' ';
	this->fileStream[FILE_STREAM_SIZE] = NULL;
}

CFileBuffer::CFileBuffer(char* other) {
	this->fileStream = new char[FILE_STREAM_SIZE+1];
	for (int i = 0; i < FILE_STREAM_SIZE; i++)
		this->fileStream[i] = other[i];
	this->fileStream[FILE_STREAM_SIZE] = NULL;
}

CFileBuffer::~CFileBuffer() {
	delete this->fileStream;
	this->fileStream = NULL;
}

char* CFileBuffer::getFileStream() {
	char* result = new char[FILE_STREAM_SIZE+1];
	sprintf(result, "%s", this->fileStream);
	return result;
}

char* CFileBuffer::getFileStream(int start, int length) {
	char* result = new char[length];
	result[length] = NULL;
	for (int i = 0; i < length; i++)
		result[i] = this->fileStream[start+i];
	return result;
}

void CFileBuffer::loadFile(char* fileName) {
	FILE *pFile = fopen(fileName, "rb");
	uint32_t v[2];
	char cypherStream[FILE_STREAM_SIZE*2+1] = "", *buffer = NULL;

	if (pFile == NULL) {
		this->resetMap();
		this->resetHighScore();
		this->saveFile(fileName);
		return;
	}

	fread(cypherStream, sizeof(char), FILE_STREAM_SIZE*2, pFile);
	cypherStream[FILE_STREAM_SIZE*2] = NULL;

	for (int i = 0; i < FILE_STREAM_SIZE*2; i += 16) {
		v[0] = this->cypherToUInt(cypherStream, i, 8);
		v[1] = this->cypherToUInt(cypherStream, i+8, 8);
		this->TEADecrypt(v, DEFAULT_KEY);
		buffer = UIntToSource(v[0]);
		for (int j = 0; j < 4; j++)
			this->fileStream[i/2+j] = buffer[j];
		buffer = UIntToSource(v[1]);
		for (int j = 0; j < 4; j++)
			this->fileStream[i/2+4+j] = buffer[j];
	}

	if (!this->streamValid()) {
		MessageBox(NULL, NOTICE_DATAFILE_CORRUPT, TITLE_DATAFILE_CORRUPT, MB_OK|MB_ICONERROR);
		this->resetMap();
		this->resetHighScore();
		this->saveFile(fileName);
		return;
	}

	fclose(pFile);
}

void CFileBuffer::resetHighScore() {
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++)
			this->fileStream[MATRIX_STREAM_SIZE+i*14+j] = ' ';
		for (int j = 0; j < 5; j++)
			this->fileStream[MATRIX_STREAM_SIZE+i*14+9+j] = '0';
	}
}

void CFileBuffer::resetMap() {
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++) {
			this->fileStream[(i*9+j)*4] = ' ';
			this->fileStream[(i*9+j)*4+1] = '0';
			this->fileStream[(i*9+j)*4+2] = ' ';
			this->fileStream[(i*9+j)*4+3] = '0';
		}
}

void CFileBuffer::saveBuffer(CRanKaro* ranKaro, CHighScore* hScore) {
	this->saveMap(ranKaro);
	this->saveHighScore(hScore);
}

void CFileBuffer::saveHighScore(CHighScore* hScore) {
	char nameBuffer[10], scoreBuffer[6];
	int* savedScore;
	char** savedName;

	savedScore = hScore->getSavedScore();
	savedName = hScore->getSavedName();

	for (int i = 0; i < 10; i++) {
		sprintf(nameBuffer, "%9s", savedName[i]);
		sprintf(scoreBuffer, "%5d", savedScore[i]);

		for (int j = 0; j < 9; j++)
			this->fileStream[MATRIX_STREAM_SIZE+i*14+j] = nameBuffer[j];
		for (int j = 0; j < 5; j++)
			this->fileStream[MATRIX_STREAM_SIZE+i*14+9+j] = scoreBuffer[j];
	}
}

void CFileBuffer::saveMap(CRanKaro* ranKaro) {
	char mapBuffer[3] = "";
	intPtr numMap, colorMap;

	numMap = ranKaro->getNumMap();
	colorMap = ranKaro->updateMapColor();

	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++) {
			sprintf(mapBuffer, "%2d", numMap[i][j]);
			this->fileStream[(i*9+j)*4] = mapBuffer[0];
			this->fileStream[(i*9+j)*4+1] = mapBuffer[1];
			sprintf(mapBuffer, "%2d", colorMap[i][j]);
			this->fileStream[(i*9+j)*4+2] = mapBuffer[0];
			this->fileStream[(i*9+j)*4+3] = mapBuffer[1];
		}
}

void CFileBuffer::saveFile(char* fileName) {
	FILE *pFile = fopen(fileName, "wb");
	uint32_t v[2];
	char *buffer;

	for (int i = 0; i < FILE_STREAM_SIZE; i+= 8) {
		v[0] = sourceToUInt(this->fileStream, i, 4);
		v[1] = sourceToUInt(this->fileStream, i+4, 4);
		this->TEAEncrypt(v, DEFAULT_KEY);
		buffer = UIntToCypher(v[0]);
		fwrite(buffer, sizeof(char), 8, pFile);
		buffer = UIntToCypher(v[1]);
		fwrite(buffer, sizeof(char), 8, pFile);
	}

	fclose(pFile);
}

/**************************************************/
CGameMove::CGameMove() {
	number[0] = number[1] = number[2] = 0;
	xPos = yPos = 0;
}

CGameMove::CGameMove(int num1, int num2, int num3, int x, int y) {
	this->number[0] = num1;
	this->number[1] = num2;
	this->number[2] = num3;
	this->xPos = x;
	this->yPos = y;
}

void CGameMove::operator=(const CGameMove& other) {
	for (int i = 0; i < 3; i++)
		this->number[i] = other.number[i];
	this->xPos = other.xPos;
	this->yPos = other.yPos;
}

void CGameMove::show() {
	std::cout << this->number[0] << " " << this->number[1] << " " << this->number[2] << " "
				<< this->xPos << " " << this->yPos << "\n";
}

/**************************************************/
CHighScore::CHighScore() {
	this->savedScore = new int[10];
	this->savedName = new char*[10];
	for (int i = 0; i < 10; i++) {
		this->savedScore[i] = 0;
		this->savedName[i] = new char[10];
		sprintf(this->savedName[i], "%9s", TITLE_DEFAULT_SHOW);
	}
}

CHighScore::~CHighScore() {
	delete this->savedScore;
	this->savedScore = NULL;

	for (int i = 0; i < 10; i++) {
		delete this->savedName[i];
		this->savedName[i] = NULL;
	}
	delete this->savedName;
	this->savedName = NULL;
}

char** CHighScore::getSavedName() {
	char** result = new char*[10];
	for (int i = 0; i < 10; i++) {
		result[i] = new char[10];
		sprintf(result[i], "%s", this->savedName[i]);
	}
	return result;
}

int* CHighScore::getSavedScore() {
	int* result = new int[10];
	for (int i = 0; i < 10; i++) {
		result[i] = this->savedScore[i];
	}
	return result;
}

void CHighScore::insertHighScore(char* name, int score) {
	int insertPos = this->newHighScore(score);

	if (insertPos == -1)
		return;

	for (int i = 0; i < (int)strlen(name); i++)
		if (name[i] == ' ')
			name[i] = '\37';			// Unit separate code

	for (int i = 9; i > insertPos; i--) {
		this->savedScore[i] = this->savedScore[i-1];
		sprintf(this->savedName[i], "%s", this->savedName[i-1]);
	}
	this->savedScore[insertPos] = score;
	sprintf(this->savedName[insertPos], "%s", name);
}

void CHighScore::loadHighScore(char* fileBuffer) {
	char scoreBuffer[6], nameBuffer[10];
	scoreBuffer[5] = NULL;
	nameBuffer[9] = NULL;

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 9; j++)
			nameBuffer[j] = fileBuffer[MATRIX_STREAM_SIZE+i*14+j];
		sscanf(nameBuffer, "%s", savedName[i]);
		for (int j = 0; j < 5; j++)
			scoreBuffer[j] = fileBuffer[MATRIX_STREAM_SIZE+i*14+9+j];
		sscanf(scoreBuffer, "%d", &savedScore[i]);
	}
}

int CHighScore::newHighScore(int currentScore) {
	int insertPos = -1;

	if (currentScore < this->savedScore[9])
		return -1;

	if (currentScore > savedScore[0])
		return 0;

	for (int i = 0; i < 9; i++)
		if (currentScore <= savedScore[i] && currentScore > savedScore[i+1]) {
			insertPos = i+1;
			break;
		}

	return insertPos;
}

void CHighScore::resetHighScore() {
	for (int i = 0; i < 10; i++) {
		sprintf(this->savedName[i], "%s", TITLE_DEFAULT_SHOW);
		this->savedScore[i] = 0;
	}
}

void CHighScore::showHighScore(HWND hdlg) {
	char buffer[15];
	char nameBuffer[10] = "";
	for (int i = 0; i < 10; i++) {
		if (this->savedScore[i]) {
			sprintf(nameBuffer, "%s", savedName[i]);
			for (int j = 0; j < (int)strlen(nameBuffer); j++)
				if (nameBuffer[j] == '\37')				// Unit separate code
					nameBuffer[j] = ' ';
			sprintf(buffer, "%d. %s", i+1, nameBuffer);
		}
		else
			sprintf(buffer, "%d. %s", i+1, TITLE_DEF_SAVED_NAME);
		SetWindowTextA(GetDlgItem(hdlg, IDC_HS_NAME+i), buffer);
		sprintf(buffer, "%d", savedScore[i]);
		SetWindowTextA(GetDlgItem(hdlg, IDC_HS_SCORE+i), buffer);
	}
}

/**************************************************/
inline int CRanKaro::updateColorRow(int row) {
	int sequenceCount = 1, startPos = 0, endPos = 0, gameScore = 0;
	for (int i = 0; i < 8; i++) {
		if (numMap[row][i] != DEFAULT_VALUE && numMap[row][i+1] == numMap[row][i]) {
			sequenceCount++;
			endPos++;
			if (sequenceCount >= 3 && (i == 7 || numMap[row][i+2] != numMap[row][i+1]))
				for (int j = startPos; j <= endPos; j++) {
					colorMap[row][j] = BINGO_COLOR+numMap[row][j];
					//gameScore += numMap[row][j];
					gameScore += numMap[row][j]*(sequenceCount-2);
				}
		}
		else {
			endPos = startPos = i+1;
			sequenceCount = 1;
		}
	}
	return gameScore;
}

inline int CRanKaro::updateColorColumn(int column) {
	int sequenceCount = 1, startPos = 0, endPos = 0, gameScore = 0;
	for (int i = 0; i < 8; i++) {
		if (numMap[i][column] != DEFAULT_VALUE && numMap[i+1][column] == numMap[i][column]) {
			sequenceCount++;
			endPos++;
			if (sequenceCount >= 3 && (i == 7 || numMap[i+2][column] != numMap[i+1][column]))
				for (int j = startPos; j <= endPos; j++) {
					colorMap[j][column] = BINGO_COLOR+numMap[j][column];
					//gameScore += numMap[j][column];
					gameScore += numMap[j][column]*(sequenceCount-2);
				}
		}
		else {
			endPos = startPos = i+1;
			sequenceCount = 1;
		}
	}
	return gameScore;
}

inline int CRanKaro::updateColorDiagon(int row, int column, bool mode) {
	int sequenceCount, startXPos, startYPos, gameScore = 0, i, j;

	// Left-right diagonal line (i+1, j+1)
	if (mode) {
		i = row;
		j = column;
		sequenceCount = 1;
		startXPos = i;
		startYPos = j;
		while (i < 8 && j < 8) {
			if (numMap[i][j] != DEFAULT_VALUE && numMap[i][j] == numMap[i+1][j+1]) {
				sequenceCount++;
				if (sequenceCount >= 3 && (i == 7 || j == 7 || numMap[i+2][j+2] != numMap[i+1][j+1]))
					for (int k = 0; k < sequenceCount; k++) {
						colorMap[startXPos+k][startYPos+k] = BINGO_COLOR+numMap[startXPos+k][startYPos+k];
						//gameScore += numMap[startXPos+k][startYPos+k];
						gameScore += numMap[startXPos+k][startYPos+k]*(sequenceCount-2);
					}
			}
			else {
				startXPos = i+1;
				startYPos = j+1;
				sequenceCount = 1;
			}
			i++;
			j++;
		}
	}
	// Right-left diagon line (i+1, j-1)
	else {
		i = row;
		j = column;
		sequenceCount = 1;
		startXPos = i;
		startYPos = j;
		while (i < 8 && j > 0) {
			if (numMap[i][j] != DEFAULT_VALUE && numMap[i][j] == numMap[i+1][j-1]) {
				sequenceCount++;
				if (sequenceCount >= 3 && (i == 7 || j == 1 || numMap[i+2][j-2] != numMap[i+1][j-1]))
					for (int k = 0; k < sequenceCount; k++) {
						colorMap[startXPos+k][startYPos-k] = BINGO_COLOR+numMap[startXPos+k][startYPos-k];
						//gameScore += numMap[startXPos+k][startYPos-k];
						gameScore += numMap[startXPos+k][startYPos-k]*(sequenceCount-2);
					}
			}
			else {
				startXPos = i+1;
				startYPos = j-1;
				sequenceCount = 1;
			}
			i++;
			j--;
		}
	}

	return gameScore;
}

/**************************************************/
// Default constructor
CRanKaro::CRanKaro() {
	score = 0;
	numMap = new int*[9];
	colorMap = new int*[9];

	for (int i = 0; i < 9; i++) {
		numMap[i] = new int[9];
		colorMap[i] = new int[9];
		for (int j = 0; j < 9; j++)
			numMap[i][j] = colorMap[i][j] = DEFAULT_VALUE;
	}
}

// Constructor
CRanKaro::CRanKaro(intPtr _numMap, intPtr _colorMap) {
	this->score = 0;
	this->numMap = new int*[9];
	this->colorMap = new int*[9];
	for (int i = 0; i < 9; i++) {
		this->numMap[i] = new int[9];
		this->colorMap[i] = new int[9];
		for (int j = 0; j < 9; j++) {
			this->numMap[i][j] = _numMap[i][j];
			this->colorMap[i][j] = _colorMap[i][j];
		}
	}
	this->updateMapColor();
}

// Destructor
CRanKaro::~CRanKaro() {
	this->score = 0;
	for (int i = 0; i < 9; i++) {
		delete numMap[i];
		delete colorMap[i];
		numMap[i] = NULL;
		colorMap[i] = NULL;
	}
	delete numMap;
	delete colorMap;
	numMap = NULL;
	colorMap = NULL;
}

bool CRanKaro::addRandNum(int num1, int num2, int num3, int xPos, int yPos, bool autoMode) {
	if (isFull())
		return false;

	if (!autoMode) {
		numMap[xPos][yPos] = num1;
		numMap[xPos+1][yPos] = num2;
		numMap[xPos+2][yPos] = num3;
		colorMap[xPos][yPos] = colorMap[xPos+1][yPos] = colorMap[xPos+2][yPos] = FILLED_COLOR;
		CGameMove lastMove(num1, num2, num3, xPos, yPos);
		if (!undoStack.empty())
			undoStack.pop();
		undoStack.push(lastMove);
	}
	else {
		int i, j;
		for (i = 0; i < 9; i += 3) {
			for (j = 0; j < 9; j++)
				if (colorMap[i][j] == DEFAULT_VALUE)
					break;
			if (colorMap[i][j] == DEFAULT_VALUE)
				break;
		}
		numMap[i][j] = num1;
		numMap[i+1][j] = num2;
		numMap[i+2][j] = num3;
		colorMap[i][j] = colorMap[i+1][j] = colorMap[i+2][j] = FILLED_COLOR;
		CGameMove lastMove(num1, num2, num3, i, j);
		if (!undoStack.empty())
			undoStack.pop();
		undoStack.push(lastMove);
	}

	return true;
}

int CRanKaro::getGameScore() {
	return (this->score);
}

intPtr CRanKaro::getNumMap() {
	return (this->numMap);
}

bool CRanKaro::isEmpty() {
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
			if (colorMap[i][j] != DEFAULT_VALUE)
				return false;
	return true;
}

bool CRanKaro::isFull() {
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
			if (colorMap[i][j] == DEFAULT_VALUE)
				return false;
	return true;
}

bool CRanKaro::loadGame(char* fileBuffer) {
	char mapBuffer[3];
	mapBuffer[2] = NULL;

	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++) {
			mapBuffer[0] = fileBuffer[(i*9+j)*4];
			mapBuffer[1] = fileBuffer[(i*9+j)*4+1];
			sscanf(mapBuffer, "%d", &numMap[i][j]);
			mapBuffer[0] = fileBuffer[(i*9+j)*4+2];
			mapBuffer[1] = fileBuffer[(i*9+j)*4+3];
			sscanf(mapBuffer, "%d", &colorMap[i][j]);
		}

	if (this->isEmpty())
		return false;

	return true;
}

void CRanKaro::showMap(int mode) {
	std::cout << score << "\n";
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++) {
			if (mode)
				std::cout << numMap[i][j];
			else
				std::cout << colorMap[i][j];
			if (j < 8)
				std::cout << " ";
			else
				std::cout << "\n";
		}
}

bool CRanKaro::undoMove(CGameMove& lastMove) {
	if (this->undoStack.empty() || this->isFull())
		return false;

	lastMove = this->undoStack.top();
	for (int i = 0; i < 3; i++)
		numMap[lastMove.xPos+i][lastMove.yPos] = colorMap[lastMove.xPos+i][lastMove.yPos] = DEFAULT_VALUE;
	this->undoStack.pop();
	updateMapColor();
	return true;
}

// Color the map
intPtr CRanKaro::updateMapColor() {
	int gameScore = 0;

	// Reset color map state
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
			if (colorMap[i][j] != DEFAULT_VALUE)
				colorMap[i][j] = FILLED_COLOR;

	// Update color map & score by rows
	for (int i = 0; i < 9; i++)
		gameScore += updateColorRow(i);

	// Update color map & score by columns
	for (int i = 0; i < 9; i++)
		gameScore += updateColorColumn(i);

	// Update color map & score by diagonal lines
	// Line (i+1, j+1) from (0,0) -> (6,0)
	for (int i = 0; i < 7; i++)
		gameScore += updateColorDiagon(i, 0, true);

	// Line (i+1, j+1) from (0,1) -> (0,6)
	for (int i = 1; i < 7; i++)
		gameScore += updateColorDiagon(0, i, true);

	// Line (i+1, j-1) from (0,2) -> (0,8)
	for (int i = 2; i < 9; i++)
		gameScore += updateColorDiagon(0, i, false);

	// Line (i+1, j-1) from (1,8) -> (6,8)
	for (int i = 1; i < 7; i++)
		gameScore += updateColorDiagon(i, 8, false);

	this->score = gameScore;

	return (this->colorMap);
}
