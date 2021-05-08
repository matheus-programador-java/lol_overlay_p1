#include <Windows.h>
#include <iostream>
#include <string>
#include <gdiplus.h>
#include<vector>
#include "spell_window.h"

std::pair<int, int> winDim;
const wchar_t CLASS_NAME[] = L"Spell Window";
HWND hwndSpellWindow;
std::vector<char> vectorOfSpells;

void ShowSpellWindow(HWND hWnd)
{
	AnimateWindow(hwndSpellWindow, 10, AW_BLEND | AW_ACTIVATE);
}

void GetVectorOfSpells(std::vector<char>* pVector)
{
	pVector = &vectorOfSpells;
}

HWND CreateSpellWindow(HWND hWndParent, HINSTANCE hInstance)
{

	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProcedure;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	RegisterClass(&wc);

	//CREATE SPELLWINDOW
	HWND hWnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		L"Capture spell window.",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

	   // Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, 600, 80,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	hwndSpellWindow = hWnd;

	return hWnd;
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		AnimateWindow(hwndSpellWindow, 1000, AW_BLEND | AW_HIDE);
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		Gdiplus::Graphics gf(hdc);
		Gdiplus::Image gdImage(L"C:\\Users\\Matheus2\\AppData\\Local\\Temp\\lol_overlay\\E.png");
		gf.DrawImage(&gdImage, 2, 2);


		//FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

		EndPaint(hWnd, &ps);
	}

	return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}