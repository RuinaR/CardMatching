#include <windows.h>		// ������ �������
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <map>
#include <string>
#include "resource.h"
using namespace std;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;										//�ν��Ͻ� �ڵ�
LPCTSTR lpszClass = TEXT("�ֿ���_22311019");				//���� ǥ���ٿ� ǥ�õǴ� ����

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam,
	int nCmdShow)
{
	HWND hWnd;											//������ �ڵ� ����
	MSG Message;										//�޼��� ����ü ���� ����
	WNDCLASS WndClass;									//Windows Class ����ü ���� ����
	g_hInst = hInstance;								//hInstance���� �ܺο����� ����� �� �ֵ��� ���������� ���� ����

	WndClass.cbClsExtra = 0;							//���� ����. ������ ���X
	WndClass.cbWndExtra = 0;							//���� ����
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// �������� ��� ������ ����
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);		//�������� ���콺������ ����� ����
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);	//�������� Ŀ�� ������ ��� ����
	WndClass.hInstance = hInstance;						//������ Ŭ������ ����ϴ� ���α׷� ��ȣ
	WndClass.lpfnWndProc = WndProc;						//������ �޼��� ó�� �Լ� ����
	WndClass.lpszClassName = lpszClass;					//������ Ŭ������ �̸� ����
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;			//�������� ��Ÿ���� ����

	RegisterClass(&WndClass);							//WNDCLASS ����ü�� ������ ����


	hWnd = CreateWindow(lpszClass, lpszClass,			//�����츦 ����
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT
		/* 100,100,500,500*/, NULL, (HMENU)NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);

	//�޼��� ������ ����
	while (GetMessage(&Message, NULL, 0, 0)) {			//Queue�� �ִ� �޼����� �о���δ�
		TranslateMessage(&Message);						//Ű���� �Է� �޼����� �����Ͽ� ���α׷����� ���� 
														//  ����� �� �ֵ���
		DispatchMessage(&Message);						//�޼����� �������� �޼��� ó�� �Լ� WndProc�� ����
	}
	return (int)Message.wParam;							//Ż�� �ڵ�. ���α׷� ����
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

//����
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
const TCHAR* strLv[(int)LV::LV_MAX] = { "�ʱ�", "�߱�", "���" };

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
	id_text = CreateWindow(TEXT("static"), TEXT("���̵�"), WS_CHILD | WS_VISIBLE,
		100, 100, 300, 50, hWnd, (HMENU)CHILD::ID_TEXT, g_hInst, NULL);
	id_input = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
		100, 150, 500, 50, hWnd, (HMENU)CHILD::ID_INPUT, g_hInst, NULL);
	pw_text = CreateWindow(TEXT("static"), TEXT("��й�ȣ"), WS_CHILD | WS_VISIBLE,
		100, 200, 300, 50, hWnd, (HMENU)CHILD::PW_TEXT, g_hInst, NULL);
	pw_input = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
		100, 250, 500, 50, hWnd, (HMENU)CHILD::PW_INPUT, g_hInst, NULL);
	loginBtn = CreateWindow(TEXT("button"), TEXT("�α���"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
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
	sel_lvText = CreateWindow(TEXT("button"), TEXT("����"), WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		300, 200, 500, 100, hWnd, (HMENU)CHILD::SELECT_LV_TEXT, g_hInst, NULL);
	sel_lv1 = CreateWindow(TEXT("button"), TEXT("�ʱ�"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
		310, 250, 100, 30, hWnd, (HMENU)CHILD::SELECT_LV1, g_hInst, NULL);
	sel_lv2 = CreateWindow(TEXT("button"), TEXT("�߱�"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		500, 250, 100, 30, hWnd, (HMENU)CHILD::SELECT_LV2, g_hInst, NULL);
	sel_lv3 = CreateWindow(TEXT("button"), TEXT("���"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		690, 250, 100, 30, hWnd, (HMENU)CHILD::SELECT_LV3, g_hInst, NULL);
	sel_btn = CreateWindow(TEXT("button"), TEXT("Ȯ��"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
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
		//���̵� or ��� �Է� ����
		MessageBoxA(hWnd, "���̵� Ȥ�� ��й�ȣ �Է��� �ؾ��մϴ�.", "����", MB_OK);
	}
	else if (accounts.find(id) == accounts.end())
	{
		//�ű԰���
		USERINFO userinfo;
		userinfo.lv = LV::LV_NULL;
		userinfo.password = pw;
		accounts[id] = userinfo;
		curLoginID = id;
		isLogin = true;
		SetLoginMenu();
		MessageBoxA(hWnd, "�ű� �����߽��ϴ�. ���̵��� �����ϼ���.", "�˸�", MB_OK);
	}
	else if (accounts.find(id)->second.password != pw)
	{
		//���Ʋ��
		MessageBoxA(hWnd, "�߸��� ��й�ȣ �Դϴ�.", "���", MB_OK);
	}
	else
	{
		//�α���
		curLoginID = id;
		isLogin = true;
		SetLoginMenu();
		if (accounts[curLoginID].lv != LV::LV_NULL)
		{
			TCHAR str[100];
			sprintf_s(str, "����� %s ���̵��� �ϴ� ���̾����ϴ�.", strLv[(int)accounts[curLoginID].lv]);
			MessageBoxA(hWnd, str, "�˸�", MB_OK);
			StartGame(hWnd, accounts[curLoginID].lv);
		}
		else
			MessageBoxA(hWnd, "�α��� ����. ���̵��� �����ϼ���.", "�˸�", MB_OK);
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
			MessageBoxA(hWnd, "���� �α����ϼ���!!", "���", MB_OK);
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
		//���ӽ���
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
				//����
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
				//������
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
		sprintf_s(current, "%d�� ���� / %d�� ����", doneCnt, (max / 2) - doneCnt);
		TextOut(hdcBuff, 700, 200, current, strlen(current));
	}

	BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, hdcBuff, 0, 0, SRCCOPY);
	if (doneCnt >= max / 2)	//�¸�
	{
		MessageBox(hWnd, "�������� Ŭ����!", "�˸�", MB_OK);
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