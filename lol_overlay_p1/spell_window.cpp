#include <Windows.h>
#include "spell_window.h"

const wchar_t CLASS_NAME[] = L"Spell Window";
HWND hwndSpellWindow;

void ShowSpellWindow(HWND hWnd)
{
	AnimateWindow(hwndSpellWindow, 10, AW_BLEND | AW_ACTIVATE);
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
		L"Learn to Program Windows",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

	   // Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, 290, 110,

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
		AnimateWindow(hwndSpellWindow, 10, AW_BLEND | AW_HIDE);
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);



		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

		EndPaint(hWnd, &ps);
	}
	return 0;

	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}