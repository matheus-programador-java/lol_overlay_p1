#pragma once
void CreateSpellWindow(HWND hWnd, HINSTANCE hInstance);
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void ShowSpellWindow(int nCmdShow);