#include <Windows.h>
#include <iostream>
#include <string>
#include <gdiplus.h>
#include<vector>
#include "spell_window.h"

namespace SpellWnd
{
	std::pair<int, int> winDim;
	const wchar_t CLASS_NAME[] = L"Spell Window";
	HWND hwndSpellWindow;
	std::vector<char> vectorOfSpells;
	std::string* pathSpellImg;
	int width = 486;

	void ShowSpellWindow(HWND hWnd)
	{
		AnimateWindow(hwndSpellWindow, 10, AW_BLEND | AW_ACTIVATE);
	}
	std::vector<char>* GetVectorOfSpells()
	{
		return &vectorOfSpells;
	}
	//void GetVectorOfSpells(std::vector<char>* pVector)
	//{
	//	pVector = &vectorOfSpells;
	//}

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
			CW_USEDEFAULT, CW_USEDEFAULT, 600, 150,

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
			int czVec = vectorOfSpells.size();
			if (czVec >= 7 || czVec == 0)
			{
				FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
				vectorOfSpells.clear();
				width = 486;
			}
			else
			{
				DrawImg(hdc);
			}
			EndPaint(hWnd, &ps);
		}
		return 0;
		}
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	void DrawImg(HDC hdc) {

		std::wstring pathI;
		Gdiplus::Graphics gf(hdc);

		for (auto& path : vectorOfSpells)
		{
			switch (path)
			{
			case 'Q':
			{
				pathI = std::wstring(pathSpellImg[0].begin(), pathSpellImg[0].end());
				Gdiplus::Image gdImage(pathI.c_str());
				gf.DrawImage(&gdImage, width, 10);
			}
			break;
			case 'W':
			{
				pathI = std::wstring(pathSpellImg[1].begin(), pathSpellImg[1].end());
				Gdiplus::Image gdImage(pathI.c_str());
				gf.DrawImage(&gdImage, width, 10);
			}
			break;
			case 'E':
			{
				pathI = std::wstring(pathSpellImg[2].begin(), pathSpellImg[2].end());
				Gdiplus::Image gdImage(pathI.c_str());
				gf.DrawImage(&gdImage, width, 10);
			}
			break;
			case 'R':
			{
				pathI = std::wstring(pathSpellImg[3].begin(), pathSpellImg[3].end());
				Gdiplus::Image gdImage(pathI.c_str());
				gf.DrawImage(&gdImage, width, 10);
			}
			break;
			case 'D':


				break;
			case 'F':


				break;
			default:
				break;
			}
		}
		width -= 94;
	}

	void SetPathSpellImg(std::string* pathSpellArray)
	{
		pathSpellImg = pathSpellArray;
	}
}