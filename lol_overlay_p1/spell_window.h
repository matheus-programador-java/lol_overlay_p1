#pragma once
namespace SpellWnd
{
	HWND CreateSpellWindow(HWND hWnd, HINSTANCE hInstance);
	LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void ShowSpellWindow(HWND hWnd);
	void GetVectorOfSpells(std::vector<char>* pVector);
	void DrawImg(HDC hdc);
	void SetPathSpellImg(std::string* pathSpellArray);
	std::vector<char>* GetVectorOfSpells();
	void ClearScreen();
	//extern std::vector<char> vectorOfSpells;
}