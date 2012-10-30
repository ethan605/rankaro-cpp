#define _WIN32_IE 0x0500

#include <windows.h>
#include <commctrl.h>
#include "mainwnd.h"

// Global variables
CMainWnd *mainWin;

void InitComCtl();
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK NewGameProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK JoinGameProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK GameConfigProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK CommonProc(HWND, UINT, WPARAM, LPARAM);

/**
	Entry-point Window
*/
int WINAPI WinMain(HINSTANCE hCurInstance,
					HINSTANCE hPrevInstance,
					LPSTR cmdArgs,
					int nCmdShow) {

	InitComCtl();
	MSG msg;
	mainWin = new CMainWnd(hCurInstance);

	while (GetMessage (&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	delete mainWin;
	return msg.wParam;
}

/**
	Init common controls
	To use newest window's theme
*/
void InitComCtl() {
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icex);
}

/**
	Main window procedure
*/
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_CTLCOLORSTATIC:
			SetBkMode((HDC)wParam, TRANSPARENT);
			for (int i = 0; i < 3; i++)
				if ((HWND)lParam == GetDlgItem(hwnd, ID_RAND_SHOW[i]))
					SetTextColor((HDC)wParam, RAND_SHOW_COLOR);
			if ((HWND)lParam == GetDlgItem(hwnd, ID_SCORE_SHOW))
				SetTextColor((HDC)wParam, SCORE_SHOW_COLOR);
			if (mainWin)
				mainWin->colorMatrix(wParam, lParam);
			return (LRESULT)COLOR_WINDOW;

		// Command receiving event
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case STN_CLICKED:
					if (HIWORD(wParam) == 1)
						mainWin->fill(lParam);
					break;
				case ID_RAND_BUTTON:
					mainWin->random();
					break;

				// Menu process
				case IDM_GAME_NEW:
					mainWin->newGame();
					break;
				case IDM_GAME_JOIN:
					mainWin->joinGame();
					break;
				case IDM_GAME_SAVE:
					mainWin->saveGame();
					break;
				case IDM_GAME_RESUME:
					mainWin->resumeGame();
					break;
				case IDM_GAME_EXIT:
					mainWin->close();
					break;
				case IDM_OPTION_RANDOM:
					mainWin->random();
					break;
				case IDM_OPTION_UNDO:
					mainWin->undoMove();
					break;
				case IDM_OPTION_HIGHSCORE:
					mainWin->highScore();
					break;
				case IDM_OPTION_MULTI_CONFIG:
					mainWin->configMulti();
					break;
				case IDM_OPTION_GAME_CONFIG:
					mainWin->configGame();
					break;
				case IDM_HELP_HELP:
					mainWin->help();
					break;
				case IDM_HELP_ABOUT:
					mainWin->about();
					break;
			}
			break;

		// Hotkeys pressing event
		case WM_HOTKEY:
			switch (wParam) {
				case HOTKEY_CTRL_H:
					mainWin->help();
					break;
				case HOTKEY_CTRL_L:
					mainWin->resumeGame();
					break;
				case HOTKEY_CTRL_N:
					mainWin->newGame();
					break;
				case HOTKEY_CTRL_Q:
					mainWin->close();
					break;
				case HOTKEY_CTRL_S:
					mainWin->saveGame();
					break;
				case HOTKEY_CTRL_X:
					mainWin->random();
					break;
				case HOTKEY_CTRL_Z:
					mainWin->undoMove();
					break;
			}
			break;

		// Close event
		case WM_CLOSE:
			mainWin->close();
			break;

		// Exit event
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}

BOOL CALLBACK NewGameProc(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParm) {
	static int newGameMode = SINGLE_MODE;
	switch (message) {
		case WM_INITDIALOG:
			SetClassLong(hdlg, GCL_HCURSOR, (LONG)LoadCursor(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_CURSOR)));
			SendMessage(GetDlgItem(hdlg, IDC_SINGLE+newGameMode), BM_SETCHECK, 1, 0);
			if (newGameMode) {
				SendMessage(GetDlgItem(hdlg, IDC_MULTI), BM_SETCHECK, 1, 0);
				if (newGameMode == LAN_MODE)
					SetWindowText(GetDlgItem(hdlg, IDC_MODE_HINT), LAN_MODE_HINT);
				if (newGameMode == INTERNET_MODE)
					SetWindowText(GetDlgItem(hdlg, IDC_MODE_HINT), INTERNET_MODE_HINT);
			}
			else {
				SetWindowText(GetDlgItem(hdlg, IDC_MODE_HINT), SINGLE_MODE_HINT);
				EnableWindow(GetDlgItem(hdlg, IDC_MULTI_LAN), false);
				EnableWindow(GetDlgItem(hdlg, IDC_MULTI_INTERNET), false);
			}
			return TRUE;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_SINGLE:
					if (newGameMode) {
						SetWindowText(GetDlgItem(hdlg, IDC_MODE_HINT), SINGLE_MODE_HINT);
						SendMessage(GetDlgItem(hdlg, IDC_SINGLE), BM_SETCHECK, 1, 0);
						SendMessage(GetDlgItem(hdlg, IDC_MULTI), BM_SETCHECK, 0, 0);
						SendMessage(GetDlgItem(hdlg, IDC_MULTI_LAN), BM_SETCHECK, 0, 0);
						SendMessage(GetDlgItem(hdlg, IDC_MULTI_INTERNET), BM_SETCHECK, 0, 0);
						EnableWindow(GetDlgItem(hdlg, IDC_MULTI_LAN), false);
						EnableWindow(GetDlgItem(hdlg, IDC_MULTI_INTERNET), false);
						newGameMode = 0;
					}
					break;
				case IDC_MULTI:
					if (!newGameMode) {
						SetWindowText(GetDlgItem(hdlg, IDC_MODE_HINT), LAN_MODE_HINT);
						EnableWindow(GetDlgItem(hdlg, IDC_MULTI_LAN), true);
						EnableWindow(GetDlgItem(hdlg, IDC_MULTI_INTERNET), true);
						SendMessage(GetDlgItem(hdlg, IDC_SINGLE), BM_SETCHECK, 0, 0);
						SendMessage(GetDlgItem(hdlg, IDC_MULTI), BM_SETCHECK, 1, 0);
						SendMessage(GetDlgItem(hdlg, IDC_MULTI_LAN), BM_SETCHECK, 1, 0);
						newGameMode = 1;
					}
					break;
				case IDC_MULTI_LAN:
					if (newGameMode == INTERNET_MODE) {
						SetWindowText(GetDlgItem(hdlg, IDC_MODE_HINT), LAN_MODE_HINT);
						SendMessage(GetDlgItem(hdlg, IDC_MULTI_LAN), BM_SETCHECK, 1, 0);
						SendMessage(GetDlgItem(hdlg, IDC_MULTI_INTERNET), BM_SETCHECK, 0, 0);
						newGameMode = LAN_MODE;
					}
					break;
				case IDC_MULTI_INTERNET:
					if (newGameMode == LAN_MODE) {
						SetWindowText(GetDlgItem(hdlg, IDC_MODE_HINT), INTERNET_MODE_HINT);
						SendMessage(GetDlgItem(hdlg, IDC_MULTI_LAN), BM_SETCHECK, 0, 0);
						SendMessage(GetDlgItem(hdlg, IDC_MULTI_INTERNET), BM_SETCHECK, 1, 0);
						newGameMode = INTERNET_MODE;
					}
					break;
				case IDC_NEWGAME:
					EndDialog(hdlg, 0);
					switch (newGameMode) {
						case SINGLE_MODE:
							mainWin->newGame_Single();
							break;
						case LAN_MODE:
						case INTERNET_MODE:
							mainWin->newGame_Multi(newGameMode);
							break;
					}
					break;
				case IDCANCEL:
					EndDialog(hdlg, 0);
					break;
			}
			break;
	}
	return FALSE;
}

BOOL CALLBACK GameConfigProc(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam) {
	static int configMode = NORMAL_MODE;
	switch (message) {
		case WM_INITDIALOG:
			SetClassLong(hdlg, GCL_HCURSOR, (LONG)LoadCursor(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_CURSOR)));
			switch (configMode) {
				case NORMAL_MODE:
					SendMessage(GetDlgItem(hdlg, IDC_NORMAL), BM_SETCHECK, 1, 0);
					SendMessage(GetDlgItem(hdlg, IDC_HANDFREE), BM_SETCHECK, 0, 0);
					SetWindowText(GetDlgItem(hdlg, IDC_MODE_HINT), NORMAL_MODE_HINT);
					break;
				case HANDFREE_MODE:
					SendMessage(GetDlgItem(hdlg, IDC_NORMAL), BM_SETCHECK, 0, 0);
					SendMessage(GetDlgItem(hdlg, IDC_HANDFREE), BM_SETCHECK, 1, 0);
					SetWindowText(GetDlgItem(hdlg, IDC_MODE_HINT), HANDFREE_MODE_HINT);
					break;
			}
			return TRUE;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_NORMAL:
					SendMessage(GetDlgItem(hdlg, IDC_NORMAL), BM_SETCHECK, 1, 0);
					SendMessage(GetDlgItem(hdlg, IDC_HANDFREE), BM_SETCHECK, 0, 0);
					SetWindowText(GetDlgItem(hdlg, IDC_MODE_HINT), NORMAL_MODE_HINT);
					configMode = NORMAL_MODE;
					break;
				case IDC_HANDFREE:
					SendMessage(GetDlgItem(hdlg, IDC_NORMAL), BM_SETCHECK, 0, 0);
					SendMessage(GetDlgItem(hdlg, IDC_HANDFREE), BM_SETCHECK, 1, 0);
					SetWindowText(GetDlgItem(hdlg, IDC_MODE_HINT), HANDFREE_MODE_HINT);
					configMode = HANDFREE_MODE;
					break;
				case IDOK:
					mainWin->setGameMode(configMode);
				case IDCANCEL:
					EndDialog(hdlg, 0);
					return TRUE;
			}
			break;
	}
	return FALSE;
}

BOOL CALLBACK CommonProc(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG: {
			SetClassLong(hdlg, GCL_HCURSOR, (LONG)LoadCursor(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_CURSOR)));

			// In case this procedure processes NAME_INPUT dialog
			if (GetDlgItem(hdlg, IDC_NAME_INPUT)) {
				Edit_LimitText(GetDlgItem(hdlg, IDC_NAME_INPUT), 9);
				SetWindowTextA(GetDlgItem(hdlg, IDC_NAME_INPUT), TITLE_DEF_NAME_INPUT);
				return TRUE;
			}

			// In case this procedure processes HIGHSCORE dialog
			if (GetDlgItem(hdlg, IDC_HS_NAME)) {
				mainWin->showHighScore(hdlg);
				return TRUE;
			}
			return TRUE;
		}
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_HS_RESET:
					if (MessageBox(hdlg, NOTICE_RESET_HIGHSCORE, TITLE_HIGHSCORE_RESET,
									MB_YESNO|MB_ICONQUESTION) == IDYES) {
						mainWin->resetHighScore();
						mainWin->showHighScore(hdlg);
					}
					break;
				case IDOK:
					// In case this procedure processes NAME_INPUT dialog
					if (GetDlgItem(hdlg, IDC_NAME_INPUT)) {
						char buffer[10];
						GetWindowTextA(GetDlgItem(hdlg,IDC_NAME_INPUT), buffer, 10);
						mainWin->insertHighScore(buffer);
					}
				case IDCANCEL:
					EndDialog(hdlg, 0);
					return TRUE;
			}
			break;
	}
	return FALSE;
}
