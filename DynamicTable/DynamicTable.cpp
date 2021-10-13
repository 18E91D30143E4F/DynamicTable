#include "framework.h"
#include "DynamicTable.h"
#include "FileReader.h"

#define _CRT_SECURE_NO_WARNINGS

#include <objidl.h>
#include <gdiplus.h>
#include <stdio.h>
#include <shellapi.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#define MAX_LOADSTRING 100

// Глобальные переменные:
HWND hwnd;
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
RECT clientRect;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

TCHAR str[4096];

VOID LoadStrDragDrop(WPARAM wParam, TCHAR* str)
{
	char* fileName = NULL;
	HDROP drop = (HDROP)wParam;
	UINT filePathesCount = DragQueryFileW(drop, 0xFFFFFFFF, NULL, 512);
	UINT longestFileNameLength = 0;
	for (UINT i = 0; i < filePathesCount; ++i)
	{
		UINT fileNameLength = DragQueryFileW(drop, i, NULL, 512) + 1;
		if (fileNameLength > longestFileNameLength)
		{
			longestFileNameLength = fileNameLength;
			fileName = (char*)realloc(fileName, longestFileNameLength * sizeof(*fileName));
		}
		DragQueryFileA(drop, i, fileName, fileNameLength);
	}
	DragFinish(drop);

	GetFileContent(fileName, str);
}

int GetColumnWidth(RECT clRect)
{
	return clRect.right / COLUMN_COUNT;
}

VOID DrawHorizontalLines(HDC hdc, int sectionHeight)
{
	Graphics graphics(hdc);
	Pen  pen(Color(255, 0, 0, 255));

	int windowHeight = clientRect.bottom;
	int partCount = windowHeight / sectionHeight;

	for (int i = sectionHeight; i <= windowHeight; i += sectionHeight)
	{
		graphics.DrawLine(&pen, 0, i, clientRect.right, i);
	}
}

int StrLenButFor(TCHAR* s)
{
	int len;
	TCHAR* ptr = s;
	for (len = 0; *ptr; ptr++)
		if (*ptr != '\n' && *ptr != '\r')
			len++;
	return len;
}

int DrawTextOnDC(HDC hdc, int xStart, int yStart, int maxStrLen)
{
	RECT rt;
	SIZE size;
	int margin = 3;

	SetRect(&rt, 0, 0, maxStrLen - margin, clientRect.bottom);

	HDC tempDC = CreateCompatibleDC(hdc);
	// Return text height with transfer on dc
	int fontHeight = DrawText(tempDC, str, StrLenButFor(str), &rt, DT_WORDBREAK);

	SetRect(&rt, xStart + margin, yStart, maxStrLen + xStart - margin, fontHeight + yStart);
	DrawText(hdc, str, StrLenButFor(str), &rt, DT_WORDBREAK);

	DeleteDC(tempDC);

	return fontHeight;
}

VOID DrawVerticalLines(HDC hdc)
{
	Graphics graphics(hdc);
	Pen      pen(Color(255, 0, 0, 255));
	int columnWidth = GetColumnWidth(clientRect);

	for (int i = columnWidth; i <= clientRect.right - 10; i += columnWidth)
	{
		graphics.DrawLine(&pen, i, 0, i, clientRect.bottom);
	}
}

VOID OnPaint(HDC hdc)
{
	int sectionWidth = GetColumnWidth(clientRect);
	DrawVerticalLines(hdc);

	int sectionHeight = DrawTextOnDC(hdc, 0, 0, sectionWidth);

	for (int x = 0; x <= clientRect.right; x += sectionWidth)
	{
		for (int y = 0; y <= clientRect.bottom; y += sectionHeight)
		{
			DrawTextOnDC(hdc, x, y, sectionWidth);
		}
	}

	DrawHorizontalLines(hdc, sectionHeight);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Разместите код здесь.

	// Инициализация глобальных строк
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_DYNAMICTABLE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;

	// Initialize GDI+.
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// Выполнить инициализацию приложения:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DYNAMICTABLE));

	MSG msg;


	// Цикл основного сообщения:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	GdiplusShutdown(gdiplusToken);

	return (int)msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DYNAMICTABLE));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDI_DYNAMICTABLE);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_VSCROLL,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	hwnd = hWnd;

	if (!hWnd)
	{
		return FALSE;
	}

	DragAcceptFiles(hwnd, TRUE);

	GetFileContent("TestTexts\\SomeData.txt", str);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}
HFONT appFont;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_CREATE:
	{
		appFont = CreateFont(20, 0, 0, 0, 700, 1, 0, 0,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T("Arial"));
		break;
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Разобрать выбор в меню:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	case WM_DROPFILES:
	{
		LoadStrDragDrop(wParam, str);
		RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_FRAME);
		UpdateWindow(hwnd);
		break;
	}
	case WM_SIZE:
	{
		GetClientRect(hwnd, &clientRect);
		break;
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		SelectObject(hdc, appFont);
		OnPaint(hdc);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}