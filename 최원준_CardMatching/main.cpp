#include <windows.h>		// 윈도우 헤더파일
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <map>
#include <string>
#include "resource.h"
using namespace std;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;										//인스턴스 핸들
LPCTSTR lpszClass = TEXT("최원준_22311019");				//제목 표시줄에 표시되는 내용

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam,
	int nCmdShow)
{
	HWND hWnd;											//윈도우 핸들 선언
	MSG Message;										//메세지 구조체 변수 선언
	WNDCLASS WndClass;									//Windows Class 구조체 변수 선언
	g_hInst = hInstance;								//hInstance값을 외부에서도 사용할 수 있도록 전역변수에 값을 저장

	WndClass.cbClsExtra = 0;							//예약 영역. 지금은 사용X
	WndClass.cbWndExtra = 0;							//예약 영역
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// 윈도우의 배경 색상을 지정
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);		//윈도우의 마우스포인터 모양을 지정
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);	//윈도우의 커서 아이콘 모양 지정
	WndClass.hInstance = hInstance;						//윈도우 클래스를 등록하는 프로그램 번호
	WndClass.lpfnWndProc = WndProc;						//윈도우 메세지 처리 함수 지정
	WndClass.lpszClassName = lpszClass;					//윈도우 클래스의 이름 지정
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;			//윈도우의 스타일을 정의

	RegisterClass(&WndClass);							//WNDCLASS 구조체의 번지를 전달


	hWnd = CreateWindow(lpszClass, lpszClass,			//윈도우를 생성
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT
		/* 100,100,500,500*/, NULL, (HMENU)NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);

	//메세지 루프를 실행
	while (GetMessage(&Message, NULL, 0, 0)) {			//Queue에 있는 메세지를 읽어들인다
		TranslateMessage(&Message);						//키보드 입력 메세지를 가공하여 프로그램에서 쉽게 
														//  사용할 수 있도록
		DispatchMessage(&Message);						//메세지를 윈도우의 메세지 처리 함수 WndProc로 전달
	}
	return (int)Message.wParam;							//탈출 코드. 프로그램 종료
}

BOOL InRect(const RECT* rect, int mx, int my)
{
	if (rect->left < mx &&
		rect->right > mx &&
		rect->top < my &&
		rect->bottom > my)
	{
		return TRUE;
	}
	return FALSE;
}

void DrawBitmap(HDC hdc, int x, int y, int w, int h, HBITMAP hbit)
{
	HDC MemoryDC;
	int BitmapX, BitmapY;
	BITMAP Bitmap_;
	HBITMAP OldBitmap;
	MemoryDC = CreateCompatibleDC(hdc);
	OldBitmap = (HBITMAP)SelectObject(MemoryDC, hbit);
	GetObject(hbit, sizeof(BITMAP), &Bitmap_);
	BitmapX = Bitmap_.bmWidth;
	BitmapY = Bitmap_.bmHeight;
	StretchBlt(hdc, x, y, w, h, MemoryDC, 0, 0, BitmapX, BitmapY, SRCCOPY);
	SelectObject(MemoryDC, OldBitmap);
	DeleteDC(MemoryDC);
}

#define OFFSET_X 100
#define OFFSET_Y 100
#define CELL_SIZE 50
#define BITMAP_MAX 18
#define CELL_MAX 36
enum class LV
{
	LV_NULL = -1,
	LV_1,
	LV_2,
	LV_3,
	LV_MAX
};
enum class CHILD
{
	ID_TEXT = 500,
	ID_INPUT,
	PW_TEXT,
	PW_INPUT,
	LOGIN_BTN,
	SELECT_LV_TEXT,
	SELECT_LV1,
	SELECT_LV2,
	SELECT_LV3,
	SELECT_BTN
};

typedef struct
{
	int x;
	int y;
	int num;
	bool isCheck;
	bool isDone;
}CELL;

typedef struct
{
	int readyTime;
	int cellMax;
	int mapSize;
}LVDATA;

typedef struct
{
	string password;
	LV lv;
}USERINFO;

//변수
HDC hdc;
PAINTSTRUCT ps;
RECT clientRect;
HDC hdcBuff;
HBITMAP hBmpBuff;
HBITMAP hBmpBuffOld;

HMENU hMenu;
HMENU hSubMenu;

HWND id_text;
HWND pw_text;
HWND id_input;
HWND pw_input;
HWND loginBtn;

HWND sel_lvText;
HWND sel_lv1;
HWND sel_lv2;
HWND sel_lv3;
HWND sel_btn;

HBITMAP hBit[BITMAP_MAX];

map<string, USERINFO> accounts;
string curLoginID;
LVDATA arrLvData[(int)LV::LV_MAX];
CELL cells[CELL_MAX];
int doneCnt = 0;
int curCellIndex = -1;
bool isLogin = false;
bool isReady = false;
bool isGameStart = false;
bool isCheck = false;

const int readyTimer = 1;
const int checkTimer = 2;
const int checkTime = 500;
const TCHAR* strLv[(int)LV::LV_MAX] = { "초급", "중급", "고급" };

bool CheakOverlap(int value, LVDATA& lvData, int size)
{
	for (int i = 0; i < size; i++)
	{
		if (cells[i].num == value)
		{
				return true;
		}
	}
	return false;
}

void SetCells(LVDATA& lvData)
{
	for (int i = 0; i < lvData.cellMax; i++)
	{
		cells[i].num = -1;
	}

	for (int i = 0; i < lvData.cellMax; i++)
	{
		cells[i].isCheck = false;
		cells[i].isDone = false;
		cells[i].x = OFFSET_X + (i % lvData.mapSize) * CELL_SIZE * 2;
		cells[i].y = OFFSET_Y + (i / lvData.mapSize) * CELL_SIZE * 2;

		if (cells[i].num != -1)
			continue;

		do {
			cells[i].num = rand() % BITMAP_MAX;
		} while (CheakOverlap(cells[i].num, lvData, i));

		int j = 0;
		do {
			j = i + (rand() % (lvData.cellMax - i + 1));
		} while (cells[j].num != -1);
		cells[j].num = cells[i].num;
	}
}

void CreateLoginWin(HWND hWnd)
{
	id_text = CreateWindow(TEXT("static"), TEXT("아이디"), WS_CHILD | WS_VISIBLE,
		100, 100, 300, 50, hWnd, (HMENU)CHILD::ID_TEXT, g_hInst, NULL);
	id_input = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
		100, 150, 500, 50, hWnd, (HMENU)CHILD::ID_INPUT, g_hInst, NULL);
	pw_text = CreateWindow(TEXT("static"), TEXT("비밀번호"), WS_CHILD | WS_VISIBLE,
		100, 200, 300, 50, hWnd, (HMENU)CHILD::PW_TEXT, g_hInst, NULL);
	pw_input = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
		100, 250, 500, 50, hWnd, (HMENU)CHILD::PW_INPUT, g_hInst, NULL);
	loginBtn = CreateWindow(TEXT("button"), TEXT("로그인"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		100, 300, 200, 100, hWnd, (HMENU)CHILD::LOGIN_BTN, g_hInst, NULL);
}

void SetLoginWinShow(int nCmdShow)
{
	ShowWindow(id_text, nCmdShow);
	ShowWindow(id_input, nCmdShow);
	ShowWindow(pw_text, nCmdShow);
	ShowWindow(pw_input, nCmdShow);
	ShowWindow(loginBtn, nCmdShow);
}

void CreateSelectLvMenu(HWND hWnd)
{
	sel_lvText = CreateWindow(TEXT("button"), TEXT("선택"), WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		300, 200, 500, 100, hWnd, (HMENU)CHILD::SELECT_LV_TEXT, g_hInst, NULL);
	sel_lv1 = CreateWindow(TEXT("button"), TEXT("초급"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
		310, 250, 100, 30, hWnd, (HMENU)CHILD::SELECT_LV1, g_hInst, NULL);
	sel_lv2 = CreateWindow(TEXT("button"), TEXT("중급"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		500, 250, 100, 30, hWnd, (HMENU)CHILD::SELECT_LV2, g_hInst, NULL);
	sel_lv3 = CreateWindow(TEXT("button"), TEXT("고급"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		690, 250, 100, 30, hWnd, (HMENU)CHILD::SELECT_LV3, g_hInst, NULL);
	sel_btn = CreateWindow(TEXT("button"), TEXT("확인"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		500, 400, 100, 25, hWnd, (HMENU)CHILD::SELECT_BTN, g_hInst, NULL);
}

void SetSelectLvWinShow(int nCmdShow)
{
	ShowWindow(sel_lvText, nCmdShow);
	ShowWindow(sel_lv1, nCmdShow);
	ShowWindow(sel_lv2, nCmdShow);
	ShowWindow(sel_lv3, nCmdShow);
	ShowWindow(sel_btn, nCmdShow);
}

void SetLoginMenu(void)
{
	if (isGameStart)
	{
		EnableMenuItem(hSubMenu, ID_LOGIN, MF_GRAYED);
		EnableMenuItem(hSubMenu, ID_LOGOUT, MF_ENABLED);
		EnableMenuItem(hSubMenu, ID_SELECTLV, MF_GRAYED);
		return;
	}
	if (isLogin)
	{
		EnableMenuItem(hSubMenu, ID_LOGIN, MF_GRAYED);
		EnableMenuItem(hSubMenu, ID_LOGOUT, MF_ENABLED);
		EnableMenuItem(hSubMenu, ID_SELECTLV, MF_ENABLED);
	}
	else
	{
		EnableMenuItem(hSubMenu, ID_LOGIN, MF_ENABLED);
		EnableMenuItem(hSubMenu, ID_LOGOUT, MF_GRAYED);
		EnableMenuItem(hSubMenu, ID_SELECTLV, MF_ENABLED);
	}
}

void InitCells(void)
{
	for (int i = 0; i < CELL_MAX; i++)
	{
		cells[i].isCheck = false;
		cells[i].isDone = false;
	}
}

void StartGame(HWND hWnd, LV lv)
{
	isReady = true;
	isGameStart = true;
	SetCells(arrLvData[(int)lv]);
	SetLoginMenu();
	InvalidateRect(hWnd, NULL, false);
	SetTimer(hWnd, readyTimer, arrLvData[(int)lv].readyTime, NULL);
}

void LoginBtn(HWND hWnd)
{
	TCHAR ID[30];
	TCHAR PW[30];
	GetWindowText(id_input, ID, 30);
	GetWindowText(pw_input, PW, 30);
	string id = ID;
	string pw = PW;

	SetLoginWinShow(SW_HIDE);
	if (id.empty() || pw.empty())
	{
		//아이디 or 비번 입력 안함
		MessageBoxA(hWnd, "아이디 혹은 비밀번호 입력을 해야합니다.", "오류", MB_OK);
	}
	else if (accounts.find(id) == accounts.end())
	{
		//신규가입
		USERINFO userinfo;
		userinfo.lv = LV::LV_NULL;
		userinfo.password = pw;
		accounts[id] = userinfo;
		curLoginID = id;
		isLogin = true;
		SetLoginMenu();
		MessageBoxA(hWnd, "신규 가입했습니다. 난이도를 선택하세요.", "알림", MB_OK);
	}
	else if (accounts.find(id)->second.password != pw)
	{
		//비번틀림
		MessageBoxA(hWnd, "잘못된 비밀번호 입니다.", "경고", MB_OK);
	}
	else
	{
		//로그인
		curLoginID = id;
		isLogin = true;
		SetLoginMenu();
		if (accounts[curLoginID].lv != LV::LV_NULL)
		{
			TCHAR str[100];
			sprintf_s(str, "당신은 %s 난이도를 하던 중이었습니다.", strLv[(int)accounts[curLoginID].lv]);
			MessageBoxA(hWnd, str, "알림", MB_OK);
			StartGame(hWnd, accounts[curLoginID].lv);
		}
		else
			MessageBoxA(hWnd, "로그인 성공. 난이도를 선택하세요.", "알림", MB_OK);
	}
}

void Init(HWND hWnd, LPARAM lParam)
{
	srand(time(NULL));
	GetClientRect(hWnd, &clientRect);
	hdc = GetDC(hWnd);
	hBmpBuff = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
	ReleaseDC(hWnd, hdc);
	for (int i = 0; i < BITMAP_MAX; i++)
	{
		hBit[i] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP1 + i));
	}
	arrLvData[(int)LV::LV_1].cellMax = 4;
	arrLvData[(int)LV::LV_1].mapSize = 2;
	arrLvData[(int)LV::LV_1].readyTime = 2000;

	arrLvData[(int)LV::LV_2].cellMax = 16;
	arrLvData[(int)LV::LV_2].mapSize = 4;
	arrLvData[(int)LV::LV_2].readyTime = 3000;

	arrLvData[(int)LV::LV_3].cellMax = 36;
	arrLvData[(int)LV::LV_3].mapSize = 6;
	arrLvData[(int)LV::LV_3].readyTime = 4000;

	isLogin = false;
	hMenu = GetMenu(hWnd);
	hSubMenu = GetSubMenu(hMenu, 0);
	SetLoginMenu();
	CreateLoginWin(hWnd);
	CreateSelectLvMenu(hWnd);
	SetLoginWinShow(SW_HIDE);
	SetSelectLvWinShow(SW_HIDE);
}

void InitGamePlay(HWND hWnd)
{
	isReady = false;
	isGameStart = false;
	isCheck = false;
	curCellIndex = -1;
	doneCnt = 0;
	InitCells();
	SetLoginMenu();
	KillTimer(hWnd, readyTimer);
	KillTimer(hWnd, checkTimer);
}

void Command(HWND hWnd, WPARAM wParam)
{
	switch (LOWORD(wParam))
	{
	case ID_LOGIN:
		SetWindowText(id_input, "");
		SetWindowText(pw_input, "");
		SetSelectLvWinShow(SW_HIDE);
		SetLoginWinShow(SW_SHOW);
		break;
	case (int)CHILD::LOGIN_BTN:
		LoginBtn(hWnd);
		break;
	case ID_SELECTLV:
		SetLoginWinShow(SW_HIDE);
		SetSelectLvWinShow(SW_SHOW);
		CheckRadioButton(hWnd, (int)CHILD::SELECT_LV1, (int)CHILD::SELECT_LV3, (int)CHILD::SELECT_LV1);
		break;
	case (int)CHILD::SELECT_BTN:
	{
		if (!isLogin)
		{
			MessageBoxA(hWnd, "먼저 로그인하세요!!", "경고", MB_OK);
			break;
		}
		LV lv = LV::LV_NULL;
		if (SendMessage(sel_lv1, BM_GETCHECK, 0, 0) == BST_CHECKED)
			lv = LV::LV_1;
		else if (SendMessage(sel_lv2, BM_GETCHECK, 0, 0) == BST_CHECKED)
			lv = LV::LV_2;
		else if (SendMessage(sel_lv3, BM_GETCHECK, 0, 0) == BST_CHECKED)
			lv = LV::LV_3;

		if (lv == LV::LV_NULL)
			break;
		accounts[curLoginID].lv = lv;
		SetSelectLvWinShow(SW_HIDE);
		//게임시작
		StartGame(hWnd, lv);
	}
	break;
	case ID_LOGOUT:
		SetSelectLvWinShow(SW_HIDE);
		curLoginID.clear();
		isLogin = false;
		InitGamePlay(hWnd);
		InvalidateRect(hWnd, NULL, false);
		break;
	case ID_EXIT:
		DestroyWindow(hWnd);
		break;
	}
}

void TimerReady(HWND hWnd)
{
	isReady = false;
	InvalidateRect(hWnd, NULL, false);
	KillTimer(hWnd, readyTimer);
}

void TimerCheck(HWND hWnd)
{
	isCheck = false;
	int max = arrLvData[(int)accounts[curLoginID].lv].cellMax;
	for (int i = 0; i < max; i++)
	{
		if (cells[i].isCheck)
			cells[i].isCheck = false;
	}
	InvalidateRect(hWnd, NULL, false);
	KillTimer(hWnd, checkTimer);
}

void LBtnDown(HWND hWnd, LPARAM lParam)
{
	RECT tmpCellRect;
	int max = arrLvData[(int)accounts[curLoginID].lv].cellMax;
	for (int i = 0; i < max; i++)
	{
		tmpCellRect = { cells[i].x - CELL_SIZE,  cells[i].y - CELL_SIZE, cells[i].x + CELL_SIZE, cells[i].y + CELL_SIZE };
		if (InRect(&tmpCellRect, LOWORD(lParam), HIWORD(lParam)))
		{
			if (curCellIndex == -1)
			{
				if (cells[i].isDone)
					return;
				curCellIndex = i;
				cells[i].isCheck = true;
			}
			else if (cells[i].num == cells[curCellIndex].num)
			{
				if (i == curCellIndex)
					return;
				//맞춤
				cells[i].isCheck = false;
				cells[curCellIndex].isCheck = false;
				cells[i].isDone = true;
				cells[curCellIndex].isDone = true;
				curCellIndex = -1;
				doneCnt++;
			}
			else
			{
				if (cells[i].isDone)
					return;
				//못맞춤
				isCheck = true;
				cells[i].isCheck = true;
				cells[curCellIndex].isCheck = true;
				curCellIndex = -1;
				SetTimer(hWnd, checkTimer, checkTime, NULL);
			}
			InvalidateRect(hWnd, NULL, false);
			break;
		}
	}
}

void Paint(HWND hWnd)
{
	hdc = BeginPaint(hWnd, &ps);
	hdcBuff = CreateCompatibleDC(hdc);
	hBmpBuffOld = (HBITMAP)SelectObject(hdcBuff, hBmpBuff);
	FillRect(hdcBuff, &clientRect, (HBRUSH)COLOR_WINDOWFRAME);
	int max = arrLvData[(int)accounts[curLoginID].lv].cellMax;
	if (isGameStart)
	{
		for (int i = 0; i < max; i++)
		{
			Rectangle(hdcBuff, cells[i].x - CELL_SIZE, cells[i].y - CELL_SIZE, cells[i].x + CELL_SIZE, cells[i].y + CELL_SIZE);
			if (isReady || cells[i].isCheck || cells[i].isDone)
			{
				DrawBitmap(hdcBuff, cells[i].x - CELL_SIZE + 5, cells[i].y - CELL_SIZE + 5,
					CELL_SIZE * 2 - 10, CELL_SIZE * 2 - 10, hBit[cells[i].num]);
			}
		}
		TCHAR current[100];
		sprintf_s(current, "%d장 맞춤 / %d장 남음", doneCnt, (max / 2) - doneCnt);
		TextOut(hdcBuff, 700, 200, current, strlen(current));
	}

	BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, hdcBuff, 0, 0, SRCCOPY);
	if (doneCnt >= max / 2)	//승리
	{
		MessageBox(hWnd, "스테이지 클리어!", "알림", MB_OK);
		FillRect(hdcBuff, &clientRect, (HBRUSH)COLOR_WINDOWFRAME);
		BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, hdcBuff, 0, 0, SRCCOPY);
		InitGamePlay(hWnd);
	}
	SelectObject(hdcBuff, hBmpBuffOld);
	DeleteDC(hdcBuff);
	EndPaint(hWnd, &ps);
}

void End(HWND hWnd)
{
	for (int i = 0; i < BITMAP_MAX; i++)
		DeleteObject(hBit[i]);
	DeleteObject(hBmpBuff);
	KillTimer(hWnd, readyTimer);
	KillTimer(hWnd, checkTimer);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage) {
	case WM_CREATE:
		Init(hWnd, lParam);
		break;
	case WM_COMMAND:
		Command(hWnd, wParam);
		break;
	case WM_TIMER:
		if (wParam == readyTimer)
			TimerReady(hWnd);
		else if (wParam == checkTimer)
			TimerCheck(hWnd);
		break;
	case WM_LBUTTONDOWN:
		if (!isGameStart || isReady || isCheck)
			break;
		LBtnDown(hWnd, lParam);
		break;
	case WM_PAINT:
		Paint(hWnd);
		break;
	case WM_DESTROY:
		End(hWnd);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}