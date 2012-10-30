#ifndef _RESOURCE_H_
#define _RESOURCE_H_
#include <string.h>

/**************************************/
#define ID_MENU						101

#define IDM_GAME_NEW				1102
#define IDM_GAME_JOIN				1103
#define IDM_GAME_SAVE				1104
#define IDM_GAME_RESUME				1105
#define IDM_GAME_EXIT				1106

#define IDM_OPTION_RANDOM			1201
#define IDM_OPTION_UNDO				1202
#define IDM_OPTION_HIGHSCORE		1203
#define IDM_OPTION_MULTI_CONFIG		1204
#define IDM_OPTION_GAME_CONFIG		1205

#define IDM_HELP_HELP				1301
#define IDM_HELP_ABOUT				1302

/**************************************/
#define IDD_GAME_NEW				TEXT("GAME_NEW")
#define IDD_GAME_JOIN				TEXT("GAME_JOIN")

#define IDD_OPTION_HIGHSCORE		TEXT("HIGHSCORE")
#define IDD_OPTION_GAME_CONFIG		TEXT("GAME_CONFIG")
#define IDD_OPTION_MULTI_CONFIG		TEXT("MULTI_CONFIG")

#define IDD_HELP_HELP				TEXT("INSTRUCTIONS")
#define IDD_HELP_ABOUT				TEXT("ABOUTBOX")

#define IDD_NAME_INPUT				TEXT("NAME_INPUT")

/**************************************/
#define ID_ICON						102
#define ID_CURSOR					103
#define ID_RAND_BUTTON				104
#define ID_SCORE_SHOW				105
#define HOTKEY_CTRL_H				106
#define HOTKEY_CTRL_L				107
#define HOTKEY_CTRL_N				108
#define HOTKEY_CTRL_Q				109
#define HOTKEY_CTRL_S				110
#define HOTKEY_CTRL_X				111
#define HOTKEY_CTRL_Z				112

#define IDC_STATIC					-1

// New game dialog control id
#define IDC_NEWGAME					201
#define IDC_SINGLE					202
#define IDC_MULTI_LAN				203
#define IDC_MULTI_INTERNET			204
#define IDC_MULTI					205
#define IDC_MULTI_GROUP				206
#define IDC_MODE_HINT				207

// Game config dialog control id
#define IDC_NORMAL					208
#define IDC_HANDFREE				209

#define IDC_NAME_INPUT				300
#define IDC_HS_NAME					301
#define IDC_HS_SCORE				311
#define IDC_HS_RESET				321
const int IDC_HIGHSCORE[20] = {301,302,303,304,305,306,307,308,309,310,
								311,312,313,314,315,316,317,318,319,320};

/**************************************/
#define CLASSNAME_BUTTON		(CHARTYPE*)"BUTTON"
#define CLASSNAME_STATIC		(CHARTYPE*)"STATIC"

#define MAIN_WND_TITLE			(CHARTYPE*)"RanKaro v1.5.2"
#define MAIN_WND_CLASS_NAME		(CHARTYPE*)"MainWindow"
#define MAIN_MENU_NAME			(CHARTYPE*)"MainMenu"
#define MUTEX_NAME				(CHARTYPE*)"MainMutex"

#define FILENAME_SAVE			(CHARTYPE*)".\\rankaro.dat"

#define FONT_PRIMARY			(CHARTYPE*)"Tahoma"
#define FONT_SECONDARY			(CHARTYPE*)"Courier New"

#define SINGLE_MODE_HINT		(CHARTYPE*)"This mode give you\na new single game"
#define LAN_MODE_HINT			(CHARTYPE*)"This mode give you\na new LAN game"
#define INTERNET_MODE_HINT		(CHARTYPE*)"This mode give you\na new Internet game"
#define NORMAL_MODE_HINT		(CHARTYPE*)"You have to manually generate\nrandom numbers and are able\nto undo moves"
#define HANDFREE_MODE_HINT		(CHARTYPE*)"Random numbers are automatically\ngenerated but you're\nnot able to undo moves"

#define NOTICE_CUR_GAME_UNFINISHED	(CHARTYPE*)"This current game hasn't been finished,\ndo you want to save it?"
#define NOTICE_DATAFILE_CORRUPT		(CHARTYPE*)"Data file is corrupted! It will be reset!"
#define NOTICE_EXISTED_GAME			(CHARTYPE*)"An existed window has been opened!"
#define NOTICE_FINISH_GAME			(CHARTYPE*)"Congratulation! you've finished the game\nYour score is: %d"
#define NOTICE_GAME_SAVED			(CHARTYPE*)"Game saved!"
#define NOTICE_LOAD_GAME			(CHARTYPE*)"Game hasn't finished, do you really want to continue saved game?"
#define NOTICE_NEW_HIGHSCORE		(CHARTYPE*)"New game high score!\nDo you want to submit your high score?"
#define NOTICE_PREV_GAME_EXIST		(CHARTYPE*)"A previous game hasn't been finished,\ndo you want to continue?"
#define NOTICE_QUIT_GAME			(CHARTYPE*)"Do you really want to quit game?"
#define NOTICE_RESET_HIGHSCORE		(CHARTYPE*)"Do you really want to reset high score?"
#define NOTICE_UNFINISHED_GAME		(CHARTYPE*)"Game hasn't finished, do you really want to start new game?"
#define NOTICE_WNDCL_REG_ERROR		(CHARTYPE*)"Error in registering window class!"

#define TITLE_DATAFILE_CORRUPT		(CHARTYPE*)"Data file corrupted!"
#define TITLE_DEFAULT_SHOW			(CHARTYPE*)" "
#define TITLE_DEF_NAME_INPUT		(CHARTYPE*)"Anonymous"
#define TITLE_DEF_SAVED_NAME		(CHARTYPE*)"Noname"
#define TITLE_DEF_SCORE				(CHARTYPE*)"0"
#define TITLE_ERROR					(CHARTYPE*)"Error!"
#define TITLE_FINISH_GAME			(CHARTYPE*)"Game finish"
#define TITLE_GAME_SAVE				(CHARTYPE*)"Save game"
#define TITLE_HIGHSCORE				(CHARTYPE*)"High score"
#define TITLE_HIGHSCORE_RESET		(CHARTYPE*)"High score reset"
#define TITLE_NEW_HIGHSCORE			(CHARTYPE*)"New highscore"
#define TITLE_PREV_GAME_EXIST		(CHARTYPE*)"Previous game existed"
#define TITLE_RAND_BUTTON			(CHARTYPE*)"Random"
#define TITLE_SAVE_GAME				(CHARTYPE*)"Save game"
#define TITLE_START_BUTTON			(CHARTYPE*)"Start"
#define TITLE_SCORE_TEXT			(CHARTYPE*)"SCORE"
#define TITLE_WARNING				(CHARTYPE*)"Warning!"

#define SINGLE_MODE				0
#define LAN_MODE				1
#define INTERNET_MODE			2
#define NORMAL_MODE				3
#define HANDFREE_MODE			4

#define DEFAULT_WND_WIDTH		310
#define DEFAULT_WND_HEIGHT		460

#define RAND_SHOW_COLOR			RGB(128,0,0)
#define SCORE_SHOW_COLOR		RGB(0,0,255)

const COLORREF MATRIX_SHOW_COLOR[12] = {RGB(0,0,0), RGB(0,0,0), RGB(0,0,0), RGB(0,120,0),RGB(0,120,0),RGB(0,120,0),RGB(255,128,0),
																RGB(255,128,0), RGB(255,128,0), RGB(255,0,0), RGB(255,0,0), RGB(255,0,0)};
const int ID_RAND_SHOW[3] = {2000, 2001, 2002};
/*
const int ID_MATRIX_SHOW[9][9] =   {{1000, 1001, 1002, 1003, 1004, 1005, 1006, 1007, 1008},
									{1010, 1011, 1012, 1013, 1014, 1015, 1016, 1017, 1018},
									{1020, 1021, 1022, 1023, 1024, 1025, 1026, 1027, 1028},
									{1030, 1031, 1032, 1033, 1034, 1035, 1036, 1037, 1038},
									{1040, 1041, 1042, 1043, 1044, 1045, 1046, 1047, 1048},
									{1050, 1051, 1052, 1053, 1054, 1055, 1056, 1057, 1058},
									{1060, 1061, 1062, 1063, 1064, 1065, 1066, 1067, 1068},
									{1070, 1071, 1072, 1073, 1074, 1075, 1076, 1077, 1078},
									{1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1088}};
*/

#endif
