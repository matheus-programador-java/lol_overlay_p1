// lol_overlay_p1.cpp : Defines the entry point for the application.
//
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include "framework.h"
#include "lol_overlay_p1.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <atlstr.h>
#include <sys/stat.h>

#pragma comment(lib, "urlmon.lib")

#define MAX_LOADSTRING 100

using json = nlohmann::json;
using namespace std;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
TCHAR selectedChamp[256] = {};
nlohmann::basic_json<> championList;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

string GetChampionJsonFile();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_LOLOVERLAYP1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LOLOVERLAYP1));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LOLOVERLAYP1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 0);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LOLOVERLAYP1);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);


	//  BLOCO NOVO
	//CREATE COM HWND's
	CreateWindowW(L"STATIC", L"Choose your champion:", WS_VISIBLE | WS_CHILD, 10, 10, 200, 100, hWnd, NULL, hInst, NULL);
	HWND hWndComboChamp = CreateWindowW(L"COMBOBOX", NULL, WS_BORDER | WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VISIBLE | WS_VSCROLL, 200, 10, 200, 300, hWnd, NULL, hInst, NULL);
	HWND hWndEdit = CreateWindowW(L"EDIT", NULL, WS_BORDER | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOHSCROLL, 10, 50, 500, 300, hWnd, NULL, hInst, NULL);

	//DOWNLOAD CHAMPION.JSON
	IStream* stream;
	//const char* URL = "http://ddragon.leagueoflegends.com/cdn/11.9.1/data/pt_BR/champion";//test error string.
	const char* URL = "http://ddragon.leagueoflegends.com/cdn/11.9.1/data/pt_BR/champion.json";
	boolean championError = false;

	string webJsonF;
	if (S_OK == URLOpenBlockingStreamA(0, URL, &stream, 0, 0))
	{
		char buff[100];
		unsigned long bytesRead;
		while (true)
		{
			memset(&buff, 0, 100);
			stream->Read(buff, 100, &bytesRead);

			if (0U == bytesRead)
			{
				break;
			}

			webJsonF.append(buff, bytesRead);
		}
	}
	else
	{
		MessageBox(NULL, (LPCWSTR)L"Erro ao baixar arquivo \"champion.json\"!", (LPCWSTR)L"Error!", MB_OK);
		championError = true;
	}
	stream->Release();

	//CREATE|OVERWRITE TEMP FILE CAMPEAO.JSON
	CHAR czTempPath[MAX_PATH] = { 0 };
	GetTempPathA(MAX_PATH, czTempPath);
	string pathOutFile = czTempPath;

	pathOutFile += "campeao.json";

	ofstream outFile;
	//CHECK IF CAMPEAO.JSON EXIST IN ERROR CASE.
	struct stat statBuf;

	if (championError) {

		if (stat(pathOutFile.c_str(), &statBuf) == -1) {
			MessageBox(NULL, (LPCWSTR)L"File champion.json doesn't exist.", (LPCWSTR)L"Error! champion.json", MB_OK);
			PostQuitMessage(1);
			return 1;
		}
	}
	else
	{
		outFile.open(pathOutFile.c_str());
		outFile << webJsonF.c_str();
		outFile.close();
	}

	// LEITURA DO ARQUIVO 
	ifstream file(pathOutFile.c_str());
	wstring jsonText((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();

	nlohmann::basic_json<> j = json::parse(jsonText);
	championList = j["data"];

	for (auto it = championList.begin(); it != championList.end(); it++) {

		string champ = (*it)["name"].get<string>();
		wstring wS(champ.begin(), champ.end());

		SendMessage(hWndComboChamp, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)wS.c_str());

		if (!wcscmp(selectedChamp, L"")) {
			wcscpy_s(selectedChamp, wS.c_str());
		}
	}

	SendMessage(hWndComboChamp, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
	GetChampionJsonFile();

	LPCWSTR result = jsonText.c_str();


	SetWindowTextW(hWndEdit, result);




	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		//GET SELECTED CHAMPION
		int itemIndex = 0;
		if (HIWORD(wParam) == CBN_SELCHANGE)
		{
			itemIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
			SendMessage((HWND)lParam, (UINT)CB_GETLBTEXT, (WPARAM)itemIndex, (LPARAM)selectedChamp);

			string pathOutFile;
			pathOutFile = GetChampionJsonFile();

			// LEITURA DO ARQUIVO - GET SPELLS
			ifstream file(pathOutFile.c_str());
			wstring jsonText((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			file.close();

			string spells[4];
			nlohmann::basic_json<> j = json::parse(jsonText);
			auto data = j["data"];

			for (auto it = data.begin(); it != data.end(); it++)
			{
				auto champ = (*it)["spells"];
				int spellCount = 0;
				for (auto is = champ.begin(); is != champ.end(); is++)
				{
					auto spell = (*is)["image"]["full"].get<string>();
					spells[spellCount] = spell;
					spellCount++;
				}
			}

			//GET SPELL PNG
			for (string spell : spells) {

				IStream* stream;
				string URL = "http://ddragon.leagueoflegends.com/cdn/11.9.1/img/spell/";
				URL += spell;

				CHAR czTempPath[MAX_PATH] = { 0 };
				GetTempPathA(MAX_PATH, czTempPath);
				string pathOutFile = czTempPath;

				pathOutFile += spell;
				ofstream outFile(pathOutFile);

				//boolean downloadSpellError = false;
				//string webJsonF;
				if (S_OK == URLOpenBlockingStreamA(0, URL.c_str(), &stream, 0, 0))
				{
					char buff[100];
					unsigned long bytesRead;
					while (true)
					{
						memset(&buff, 0, 100);
						stream->Read(buff, 100, &bytesRead);

						if (0U == bytesRead)
						{
							break;
						}
						outFile << buff;
						outFile.write(buff, bytesRead);
						outFile.flush();
						//webJsonF.append(buff, bytesRead);
					}
					outFile.close();
				}
				else
				{
					MessageBox(NULL, (LPCWSTR)L"Erro ao baixar arquivo de habilidade do campe�o!", (LPCWSTR)L"Error!", MB_OK);
					//downloadSpellError = true;
				}

				stream->Release();

				////CREATE|OVERWRITE TEMP FILE CAMPEAO.JSON
				//CHAR czTempPath[MAX_PATH] = { 0 };
				//GetTempPathA(MAX_PATH, czTempPath);
				//string pathOutFile = czTempPath;

				//pathOutFile += spell;

				//ofstream outFile;
				////CHECK IF CAMPEAO.JSON EXIST IN ERROR CASE.
				//struct stat statBuf;

				//if (downloadSpellError) {

				//	if (stat(pathOutFile.c_str(), &statBuf) == -1) {
				//		MessageBox(NULL, (LPCWSTR)L"Arquivo de habilidade n�o existe.", (LPCWSTR)L"Error!", MB_OK);
				//		PostQuitMessage(1);
				//		return 1;
				//	}
				//}
				//else
				//{
				//	outFile.open(pathOutFile.c_str());
				//	outFile << webJsonF.c_str();
				//	outFile.close();
				//}
			}
		}


		int wmId = LOWORD(wParam);
		// Parse the menu selections:
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
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
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

// Message handler for about box.
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

string GetChampionJsonFile() {
	//DOWNLOAD CHAMPION DATA
	IStream* stream;
	wstring s(selectedChamp);
	string sChampionF(s.begin(), s.end());
	sChampionF += ".json";

	string URL = "http://ddragon.leagueoflegends.com/cdn/11.9.1/data/pt_BR/champion/";
	URL += sChampionF;

	boolean selectChampError = false;
	string webJsonF;
	if (S_OK == URLOpenBlockingStreamA(0, URL.c_str(), &stream, 0, 0))
	{
		char buff[100];
		unsigned long bytesRead;
		while (true)
		{
			memset(&buff, 0, 100);
			stream->Read(buff, 100, &bytesRead);

			if (0U == bytesRead)
			{
				break;
			}

			webJsonF.append(buff, bytesRead);
		}
	}
	else
	{
		MessageBox(NULL, (LPCWSTR)L"Erro ao baixar arquivo do campe�o selecionado!", (LPCWSTR)L"Error!", MB_OK);
		selectChampError = true;
	}

	stream->Release();

	//CREATE|OVERWRITE TEMP FILE CAMPEAO.JSON
	CHAR czTempPath[MAX_PATH] = { 0 };
	GetTempPathA(MAX_PATH, czTempPath);
	string pathOutFile = czTempPath;

	pathOutFile += sChampionF;

	ofstream outFile;
	//CHECK IF CAMPEAO.JSON EXIST IN ERROR CASE.
	struct stat statBuf;

	if (selectChampError) {

		if (stat(pathOutFile.c_str(), &statBuf) == -1) {
			MessageBox(NULL, (LPCWSTR)L"File to selected champ doesn't exist.", (LPCWSTR)L"Error!", MB_OK);
			PostQuitMessage(1);
		}
	}
	else
	{
		outFile.open(pathOutFile.c_str());
		outFile << webJsonF.c_str();
		outFile.close();
	}

	return pathOutFile;
}