/**
	mainwnd.cpp
	CMainWnd class implementation file
*/

#include "mainwnd.h"

/**************************************************/
/** CMainWnd class private section                */
/** This section includes private methods         */
/** which used to support public methods          */
/**************************************************/
/**
	Method: createInterface()
	Return: void
	Description: Create interface for main window, includes:
		- A matrix of 9*9 static controls used to display random numbers which were added by users
		- A group of 3 static controls used to display random numbers generated in each turn
		- A static text with title "SCORE"
		- A static controls used to display current score of the game
		- A push button used to generate random numbers (in normal mode) or to start a game (in hand-free mode)
		All these components below are set a logical font face and bitmap cursor over them
*/
inline void CMainWnd::createInterface() {
	HFONT hFont = CreateFont(-17,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
							CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,FONT_PRIMARY);

	// Create random numbers matrix
	// these controls are child, bordered windows, middle-center aligned text
	// they could receive mouse events from users
	// and default label of TITLE_DEFAULT_SHOW
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++) {
			hMatrixWnd[i][j] = CreateWindow(CLASSNAME_STATIC,TITLE_DEFAULT_SHOW,WS_BORDER|WS_CHILD|WS_VISIBLE|SS_CENTERIMAGE|SS_CENTER|SS_NOTIFY,
											10+j*31+(j/3)*3,120+i*31+(i/3)*3,30,30,this->hMainWnd,NULL,this->hMainInst,NULL);
			SendMessage(hMatrixWnd[i][j], WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));

			// Set bitmap cursor over the matrix's controls
			SetClassLong(hMatrixWnd[i][j], GCL_HCURSOR, (LONG)LoadCursor(hMainInst, MAKEINTRESOURCE(ID_CURSOR)));
		}

	// Create random numbers show area
	// these controls have same styles with random numbers matrix
	// except they don't receive any mouse events from users
	// and have pre-defined IDs: ID_RAND_SHOW[i]
	for (int i = 0; i < 3; i++) {
		hRandShowWnd[i] = CreateWindow(CLASSNAME_STATIC,TITLE_DEFAULT_SHOW,WS_BORDER|WS_CHILD|WS_VISIBLE|SS_CENTERIMAGE|SS_CENTER,
										40,10+i*31,30,30,this->hMainWnd,(HMENU)ID_RAND_SHOW[i],this->hMainInst,NULL);
		SendMessage(hRandShowWnd[i], WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
	}

	// Create score show area
	// "Score" text
	// This static text has label of TITLE_SCORE_TEXT
	HWND hScoreText = CreateWindow(CLASSNAME_STATIC,TITLE_SCORE_TEXT,WS_CHILD|WS_VISIBLE|SS_CENTERIMAGE|SS_CENTER,
									120, 15, 70, 25,this->hMainWnd,NULL,this->hMainInst,NULL);
	hFont = CreateFont(-17,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
						CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,FONT_PRIMARY);
	SendMessage(hScoreText, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));

	// Score number
	// This static control has default label of TITLE_DEF_SCORE
	hScoreShow = CreateWindow(CLASSNAME_STATIC,TITLE_DEF_SCORE,WS_CHILD|WS_VISIBLE|SS_CENTERIMAGE|SS_CENTER,
								105, 50, 100, 30,this->hMainWnd,(HMENU)ID_SCORE_SHOW,this->hMainInst,NULL);
	hFont = CreateFont(-35,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
						CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,FONT_SECONDARY);
	SendMessage(hScoreShow, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));

	// Create "Random" button
	// This button has default label of TITLE_RAND_BUTTON
	hRandButton = CreateWindow(CLASSNAME_BUTTON,TITLE_RAND_BUTTON,WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_DEFPUSHBUTTON,
								215, 80, 65, 30,this->hMainWnd,(HMENU)ID_RAND_BUTTON,this->hMainInst,NULL);
	hFont = CreateFont(-14,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
						CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,FONT_PRIMARY);
	SendMessage(hRandButton, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));

	// Set bitmap cursor over the button
	SetClassLong(hRandButton, GCL_HCURSOR, (LONG)LoadCursor(hMainInst, MAKEINTRESOURCE(ID_CURSOR)));
}

/**
	Method: displayRandNum()
	Return type: void
	Description: the method use to generate and display random numbers
		when user manually click on "Random" button (in normal mode)
		or automatically after each turn of filling numbers (in hand-free mode)
*/
inline void CMainWnd::displayRandNum() {
	srand((unsigned)(time(NULL)+1000));
	CHARTYPE buffer[2];

	for (int i = 0; i < 3; i++) {
		this->randNum[i] = rand() % 10;
		sprintf(buffer, (CHARTYPE*)"%d", this->randNum[i]);
		SetWindowText(this->hRandShowWnd[i], buffer);
	}
}

/**
	Method: fillRandNum()
	Return type: bool
	Param: a LPARAM-type value to determine which static controls
		receive mouse double-click event from user
	Description: this method analyse the location user double-click on
		and decide if new sequence of random numbers could place at that location
		- It return true whenever user click in a valid location (blank controls of the controls matrix)
		- It return false if:
			+ The location is invalid (filled controls)
			+ The game is finished
		When the game is finished, a message box will appear to announce the game's score
		and new high score if it is
*/
inline bool CMainWnd::fillRandNum(LPARAM lParam) {
	CHARTYPE matrixText[2];
	int xPos, yPos;
	this->getMatrixPos((HWND)lParam, xPos, yPos);
	xPos = (xPos/3)*3;

	// Get the co-ordinates of static control which was double clicked
	GetWindowText(this->hMatrixWnd[xPos][yPos], matrixText, 2);

	// Return false if this is an invalid location
	if (strcmp(matrixText, TITLE_DEFAULT_SHOW))
		return false;

	// In case of valid location
	CHARTYPE buffer[60];
	// In case of the game is saved before this turn of filling numbers
	// Enable Game->Save menu to save the game
	// Reset data of the game in file stream buffer
	// Set save state of the game
	if (this->gameSaved) {
		EnableMenuItem(GetSubMenu(this->hMenu, 0), IDM_GAME_SAVE, MF_BYCOMMAND|MF_ENABLED);
		this->fileBuffer->resetMap();
		this->gameSaved = false;
	}

	// Fill numbers into CRanKaro object
	// and get a new map of colors
	ranKaro->addRandNum(randNum[0], randNum[1], randNum[2], xPos, yPos, false);
	ranKaro->updateMapColor();

	// Show numbers in selected location
	for (int i = 0; i < 3; i++) {
		sprintf(buffer, (CHARTYPE*)"%d", randNum[i]);
		SetWindowText(this->hMatrixWnd[xPos+i][yPos], buffer);
	}

	// Show the score in score static control
	sprintf(buffer, (CHARTYPE*)"%d", ranKaro->getGameScore());
	SetWindowText(this->hScoreShow, buffer);

	// Send an event to all the static controls in the matrix
	// to update their color
	// through the WM_CTLCOLORSTATIC message
	this->updateMatrix();

	// In case of the game is finish
	if (ranKaro->isFull()) {
		this->setRandButton(false);
		EnableMenuItem(GetSubMenu(this->hMenu, 0), IDM_GAME_SAVE, MF_BYCOMMAND|MF_GRAYED);
		sprintf(buffer, NOTICE_FINISH_GAME, ranKaro->getGameScore());
		MessageBox(this->hMainWnd, buffer, TITLE_FINISH_GAME, MB_OK|MB_ICONINFORMATION);
		updateHighScore(this->ranKaro->getGameScore());
		return false;
	}
	return true;
}

/**
	Method: getMatrixPos()
	Return type: void
	Params:
		- A handle to the window which was double clicked
		- x & y co-ordinates of the location
	Description: the method to get the position of clicked control
*/
inline void CMainWnd::getMatrixPos(HWND hwnd, int& xPos, int& yPos) {
	bool found = false;
	for (xPos = 0; xPos < 9; xPos++) {
		for (yPos = 0; yPos < 9; yPos++)
			if (hwnd == this->hMatrixWnd[xPos][yPos]) {
				found = true;
				break;
			}
		if (found)
			break;
	}
}

inline void CMainWnd::init() {
	this->fileBuffer->loadFile(FILENAME_SAVE);

	this->hScore->loadHighScore(this->fileBuffer->getFileStream());

	if (!this->ranKaro->loadGame(this->fileBuffer->getFileStream()))
		return;

	if (MessageBox(this->hMainWnd, NOTICE_PREV_GAME_EXIST, TITLE_PREV_GAME_EXIST, MB_YESNO|MB_ICONQUESTION) == IDNO) {
		this->newGame_Single();
		EnableMenuItem(GetSubMenu(this->hMenu, 0), IDM_GAME_RESUME, MF_BYCOMMAND|MF_ENABLED);
		return;
	}

	this->loadRanKaro(this->ranKaro);
	EnableMenuItem(GetSubMenu(this->hMenu, 0), IDM_GAME_SAVE, MF_BYCOMMAND|MF_GRAYED);
	EnableMenuItem(GetSubMenu(this->hMenu, 0), IDM_GAME_RESUME, MF_BYCOMMAND|MF_GRAYED);
}

inline void CMainWnd::loadRanKaro(CRanKaro *game) {
	intPtr numMap = game->getNumMap();
	intPtr colorMap = game->updateMapColor();

	CHARTYPE buffer[6];

	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
			if (colorMap[i][j]) {
				sprintf(buffer, (CHARTYPE*)"%d", numMap[i][j]);
				SetWindowText(hMatrixWnd[i][j], buffer);
			}
			else
				SetWindowText(hMatrixWnd[i][j], TITLE_DEFAULT_SHOW);

	for (int i = 0; i < 3; i++)
		SetWindowText(hRandShowWnd[i], TITLE_DEFAULT_SHOW);

	sprintf(buffer, (CHARTYPE*)"%d", game->getGameScore());
	SetWindowText(hScoreShow, buffer);
	this->updateMatrix();
	this->setRandButton(true);
	randButtonClicked = false;
	gameSaved = true;
	randNum[0] = randNum[1] = randNum[2] = 0;
	EnableMenuItem(GetSubMenu(this->hMenu, 0), IDM_GAME_SAVE, MF_BYCOMMAND|MF_GRAYED);
	EnableMenuItem(GetSubMenu(this->hMenu, 1), IDM_OPTION_UNDO, MF_BYCOMMAND|MF_GRAYED);
}

/**
	Process existance check
*/
inline void CMainWnd::mutexCheck() {
	CreateMutex(NULL, true, MUTEX_NAME);

	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		MessageBox(NULL, NOTICE_EXISTED_GAME, TITLE_ERROR, MB_OK|MB_ICONWARNING);
		BringWindowToTop(this->hMainWnd);
		SetActiveWindow(this->hMainWnd);
		SetForegroundWindow(this->hMainWnd);
		exit(0);
	}
}

inline void CMainWnd::setHotKey() {
	// Set Ctrl + H to help
	if (!RegisterHotKey(this->hMainWnd, HOTKEY_CTRL_H, MOD_CONTROL, 0x48))
		MessageBox(this->hMainWnd, (CHARTYPE*)"Hotkey registering error\nCtrl + H disabled", TITLE_ERROR, MB_OK|MB_ICONERROR);

	// Set Ctrl + L to load
	if (!RegisterHotKey(this->hMainWnd, HOTKEY_CTRL_L, MOD_CONTROL, 0x4C))
		MessageBox(this->hMainWnd, (CHARTYPE*)"Hotkey registering error\nCtrl + L disabled", TITLE_ERROR, MB_OK|MB_ICONERROR);

	// Set Ctrl + N to new game
	if (!RegisterHotKey(this->hMainWnd, HOTKEY_CTRL_N, MOD_CONTROL, 0x4E))
		MessageBox(this->hMainWnd, (CHARTYPE*)"Hotkey registering error\nCtrl + N disabled", TITLE_ERROR, MB_OK|MB_ICONERROR);

	// Set Ctrl + Q to quit
	if (!RegisterHotKey(this->hMainWnd, HOTKEY_CTRL_Q, MOD_CONTROL, 0x51))
		MessageBox(this->hMainWnd, (CHARTYPE*)"Hotkey registering error\nCtrl + Q disabled", TITLE_ERROR, MB_OK|MB_ICONERROR);

	// Set Ctrl + S to save
	if (!RegisterHotKey(this->hMainWnd, HOTKEY_CTRL_S, MOD_CONTROL, 0x53))
		MessageBox(this->hMainWnd, (CHARTYPE*)"Hotkey registering error\nCtrl + S disabled", TITLE_ERROR, MB_OK|MB_ICONERROR);

	// Set Ctrl + X to random
	if (!RegisterHotKey(this->hMainWnd, HOTKEY_CTRL_X, MOD_CONTROL, 0x58))
		MessageBox(this->hMainWnd, (CHARTYPE*)"Hotkey registering error\nCtrl + X disabled", TITLE_ERROR, MB_OK|MB_ICONERROR);

	// Set Ctrl + Z to undo
	if (!RegisterHotKey(this->hMainWnd, HOTKEY_CTRL_Z, MOD_CONTROL, 0x5A))
		MessageBox(this->hMainWnd, (CHARTYPE*)"Hotkey registering error\nCtrl + Z disabled", TITLE_ERROR, MB_OK|MB_ICONERROR);
}

inline void CMainWnd::setRandButton(bool mode) {
	Button_Enable(this->hRandButton, mode);
	if (mode) {
		EnableMenuItem(GetSubMenu(this->hMenu, 1), IDM_OPTION_RANDOM, MF_BYCOMMAND|MF_ENABLED);
		EnableMenuItem(GetSubMenu(this->hMenu, 1), IDM_OPTION_UNDO, MF_BYCOMMAND|MF_ENABLED);
	}
	else {
		EnableMenuItem(GetSubMenu(this->hMenu, 1), IDM_OPTION_RANDOM, MF_BYCOMMAND|MF_GRAYED);
		EnableMenuItem(GetSubMenu(this->hMenu, 1), IDM_OPTION_UNDO, MF_BYCOMMAND|MF_GRAYED);
	}
}

inline void CMainWnd::updateHighScore(int currentScore) {
	int insertPos = hScore->newHighScore(currentScore);

	if (insertPos == -1)
		return;

	if (MessageBox(this->hMainWnd, NOTICE_NEW_HIGHSCORE, TITLE_NEW_HIGHSCORE, MB_YESNO|MB_ICONINFORMATION) != IDYES)
		return;

	DialogBox(this->hMainInst, IDD_NAME_INPUT, this->hMainWnd, CommonProc);
	DialogBox(this->hMainInst, IDD_OPTION_HIGHSCORE, this->hMainWnd, CommonProc);
}

inline void CMainWnd::updateMatrix() {
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++) {
			CHARTYPE buffer[2];
			GetWindowText(this->hMatrixWnd[i][j], buffer, 2);
			SetWindowText(this->hMatrixWnd[i][j], buffer);
		}
}

/*******************************************************/
/**
	Default constructor
	Create default windows structures
*/
CMainWnd::CMainWnd(HINSTANCE hInstance) {
	hMainInst = hInstance;
	ranKaro = new CRanKaro;
	hScore = new CHighScore;
	fileBuffer = new CFileBuffer;
	randButtonClicked = false;
	gameSaved = true;
	gameMode = NORMAL_MODE;
	randNum[0] = randNum[1] = randNum[2] = 0;

	mainWndCl.hInstance = this->hMainInst;
    mainWndCl.lpszClassName = MAIN_WND_CLASS_NAME;
    mainWndCl.lpfnWndProc = WndProc;
    mainWndCl.style = CS_DBLCLKS;
    mainWndCl.cbSize = sizeof(WNDCLASSEX);
    mainWndCl.hIcon = LoadIcon(this->hMainInst, MAKEINTRESOURCE(ID_ICON));
    mainWndCl.hIconSm = (HICON)LoadImage(this->hMainInst, MAKEINTRESOURCE(ID_ICON), IMAGE_ICON, 16, 16, 0);
    mainWndCl.hCursor = LoadCursor(this->hMainInst, MAKEINTRESOURCE(ID_CURSOR));
    mainWndCl.lpszMenuName = MAIN_MENU_NAME;
    mainWndCl.cbClsExtra = 0;
    mainWndCl.cbWndExtra = 0;
    mainWndCl.hbrBackground = (HBRUSH)COLOR_WINDOW;

    if (!RegisterClassEx(&mainWndCl)) {
		MessageBox(this->hMainWnd, NOTICE_WNDCL_REG_ERROR, TITLE_ERROR, MB_OK|MB_ICONERROR);
		exit(1);
	}

    this->hMenu = LoadMenu(hInstance, MAKEINTRESOURCE (ID_MENU));

    hMainWnd = CreateWindow(MAIN_WND_CLASS_NAME,						// Class name
							MAIN_WND_TITLE,								// Window title
							WS_CAPTION|WS_SYSMENU|
							WS_MINIMIZEBOX|WS_CLIPCHILDREN,				// Window style
							CW_USEDEFAULT,								// X position
							CW_USEDEFAULT,								// Y position
							DEFAULT_WND_WIDTH,							// Window width
							DEFAULT_WND_HEIGHT,							// Window height
							HWND_DESKTOP,								// The window is a child-window to desktop
							hMenu,										// Window menu
							this->hMainInst,							// Main window instance
							NULL);										// Child window instance

	this->mutexCheck();
	this->setHotKey();
	this->createInterface();
	this->init();
	ShowWindow(this->hMainWnd, SW_NORMAL);
}

/**
	Destructor
*/
CMainWnd::~CMainWnd() {
	delete ranKaro;
	delete hScore;
	delete fileBuffer;
}

void CMainWnd::colorMatrix(WPARAM wParam, LPARAM lParam) {
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
			if ((HWND)lParam == this->hMatrixWnd[i][j]) {
				intPtr colorMap = this->ranKaro->updateMapColor();
				SetTextColor((HDC)wParam, MATRIX_SHOW_COLOR[colorMap[i][j]]);
			}
}

void CMainWnd::fill(LPARAM lParam) {
	if (!(this->randButtonClicked && this->fillRandNum(lParam)))
		return;

	switch (this->gameMode) {
		case NORMAL_MODE:
			if (this->randButtonClicked) {
				this->randButtonClicked = false;
				this->setRandButton(true);
			}
			break;
		case HANDFREE_MODE:
			this->displayRandNum();
			break;
	}
}

void CMainWnd::insertHighScore(char* name) {
	this->hScore->insertHighScore(name, this->ranKaro->getGameScore());
}

void CMainWnd::resetHighScore() {
	this->hScore->resetHighScore();
	this->fileBuffer->resetHighScore();
}

void CMainWnd::setGameMode(int setMode) {
	this->gameMode = setMode;
	switch (this->gameMode) {
		case NORMAL_MODE:
			SetWindowText(this->hRandButton, TITLE_RAND_BUTTON);
			EnableMenuItem(GetSubMenu(this->hMenu, 1), IDM_OPTION_RANDOM, MF_BYCOMMAND|MF_ENABLED);
			break;
		case HANDFREE_MODE:
			SetWindowText(this->hRandButton, TITLE_START_BUTTON);
			EnableMenuItem(GetSubMenu(this->hMenu, 1), IDM_OPTION_RANDOM, MF_BYCOMMAND|MF_GRAYED);
			break;
	}
}

void CMainWnd::showHighScore(HWND hdlg) {
	this->hScore->showHighScore(hdlg);
}

void CMainWnd::newGame() {
	if (!this->ranKaro->isFull() && !this->ranKaro->isEmpty()) {
		if (MessageBox(this->hMainWnd, NOTICE_UNFINISHED_GAME, TITLE_WARNING, MB_YESNO|MB_ICONWARNING) == IDNO)
			return;
	}
	DialogBox(this->hMainInst, IDD_GAME_NEW, this->hMainWnd, NewGameProc);
}

void CMainWnd::newGame_Single() {
	delete ranKaro;
	ranKaro = new CRanKaro;
	this->loadRanKaro(this->ranKaro);
}

void CMainWnd::newGame_Multi(int mode) {
	MessageBox(this->hMainWnd, (CHARTYPE*)"Multiplayer mode is under construction", (CHARTYPE*)"New multiplayer game", MB_OK|MB_ICONINFORMATION);
	this->newGame_Single();
}

void CMainWnd::joinGame() {
	return;
}

void CMainWnd::saveGame() {
	if (this->ranKaro->isEmpty() || this->ranKaro->isFull()) {
		return;
	}
	this->fileBuffer->saveMap(this->ranKaro);
	this->gameSaved = true;
	EnableMenuItem(GetSubMenu(this->hMenu, 0), IDM_GAME_SAVE, MF_BYCOMMAND|MF_GRAYED);
	EnableMenuItem(GetSubMenu(this->hMenu, 0), IDM_GAME_RESUME, MF_BYCOMMAND|MF_ENABLED);
	MessageBox(this->hMainWnd, NOTICE_GAME_SAVED, TITLE_GAME_SAVE, MB_OK|MB_ICONINFORMATION);
}

void CMainWnd::resumeGame() {
	bool gameEmpty = this->ranKaro->isEmpty();
	delete this->ranKaro;
	this->ranKaro = new CRanKaro;
	if (!this->ranKaro->loadGame(this->fileBuffer->getFileStream()))
		return;
	if (!gameEmpty)
		if (MessageBox(this->hMainWnd, NOTICE_LOAD_GAME, TITLE_WARNING, MB_YESNO|MB_ICONWARNING) == IDNO)
			return;
	this->loadRanKaro(this->ranKaro);
	EnableMenuItem(GetSubMenu(this->hMenu, 0), IDM_GAME_SAVE, MF_BYCOMMAND|MF_ENABLED);
	EnableMenuItem(GetSubMenu(this->hMenu, 0), IDM_GAME_RESUME, MF_BYCOMMAND|MF_GRAYED);
}

void CMainWnd::close() {
	if (!this->ranKaro->isEmpty() && !this->ranKaro->isFull() && !this->gameSaved)
		if (MessageBox(this->hMainWnd, NOTICE_CUR_GAME_UNFINISHED, TITLE_SAVE_GAME, MB_YESNO|MB_ICONQUESTION) == IDYES) {
			this->fileBuffer->saveBuffer(this->ranKaro, this->hScore);
			this->fileBuffer->saveFile(FILENAME_SAVE);
			DestroyWindow(this->hMainWnd);
			return;
		}
	if (MessageBox(this->hMainWnd, NOTICE_QUIT_GAME, TITLE_WARNING, MB_YESNO|MB_ICONQUESTION) == IDYES) {
		this->fileBuffer->saveHighScore(this->hScore);
		this->fileBuffer->saveFile(FILENAME_SAVE);
		DestroyWindow(this->hMainWnd);
	}
}

void CMainWnd::random() {
	if (!this->randButtonClicked) {
		this->displayRandNum();
		this->randButtonClicked = true;
		this->setRandButton(false);
	}
}

void CMainWnd::undoMove() {
	CGameMove lastMove;
	CHARTYPE buffer[2];
	if ((ranKaro->undoMove(lastMove)) && !this->randButtonClicked) {
		this->randButtonClicked = true;
		this->setRandButton(false);
		sprintf(buffer, (CHARTYPE*)"%d", this->ranKaro->getGameScore());
		SetWindowText(this->hScoreShow, buffer);
		for (int i = 0; i < 3; i++) {
			randNum[i] = lastMove.number[i];
			sprintf(buffer, (CHARTYPE*)"%d", this->randNum[i]);
			SetWindowText(this->hRandShowWnd[i], buffer);
			SetWindowText(this->hMatrixWnd[lastMove.xPos+i][lastMove.yPos], TITLE_DEFAULT_SHOW);
		}
		EnableMenuItem(GetSubMenu(this->hMenu, 1), IDM_OPTION_UNDO, MF_BYCOMMAND|MF_GRAYED);
		this->updateMatrix();
	}
}

void CMainWnd::highScore() {
	DialogBox(this->hMainInst, IDD_OPTION_HIGHSCORE, this->hMainWnd, CommonProc);
}

void CMainWnd::configGame() {
	DialogBox(this->hMainInst, IDD_OPTION_GAME_CONFIG, this->hMainWnd, GameConfigProc);
}

void CMainWnd::configMulti() {
	return;
}

void CMainWnd::help() {
	DialogBox(this->hMainInst, IDD_HELP_HELP, this->hMainWnd, CommonProc);
}

void CMainWnd::about() {
	DialogBox(this->hMainInst, IDD_HELP_ABOUT, this->hMainWnd, CommonProc);
}
