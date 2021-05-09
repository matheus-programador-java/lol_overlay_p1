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
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <atlstr.h>
#include <sys/stat.h>
#include <gdiplus.h>
#include <cmath>
#include "spell_window.h"
#include <thread>
#include <chrono>
#include <vector>

#pragma comment(lib, "urlmon.lib")
//#pragma comment(lib, "curl/curl.lib")

#define MAX_LOADSTRING 100

using json = nlohmann::json;
using namespace std;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
TCHAR selectedChamp[256] = {};
nlohmann::basic_json<> championList;
string pathSpellImg[4];
bool exitThread = false;
HWND hWndEdit;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

string GetChampionJsonFile();
size_t callback_funtion(void* ptr, size_t size, size_t nmemb, void* user_data);
void GetSpellImg(string pathOutFile, HWND wHnd);
void DrawImg(HDC hdc);
HANDLE threadHandle;
HWND spellHwnd;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI KeyboardListner(_In_ LPVOID lpParameter);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	//Initialize GDI+
	Gdiplus::GdiplusStartupInput gdiInput;
	ULONG_PTR gdiToken;
	Gdiplus::GdiplusStartup(&gdiToken, &gdiInput, nullptr);



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


	//Thread ketyboard listner
	int* threadHeap;
	threadHeap = (int*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(threadHeap));

	if (threadHeap == NULL)
	{
		ExitProcess(2);
	}

	DWORD threadID;
	threadHandle = CreateThread(NULL, 0, KeyboardListner,
		0, //Param to thread function 
		0, &threadID);


	// Main message loop:
	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}


	//Need revision.
	//CloseHandle(threadHandle);
	//if (threadHeap != NULL)
		//HeapFree(GetProcessHeap, 0, threadHeap);



	Gdiplus::GdiplusShutdown(gdiToken);

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
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
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
	//CREATE MAIN WINDOW 
	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	//CREATE SPELLWINDOW
	spellHwnd = SpellWnd::CreateSpellWindow(hWnd, hInstance);

	//CREATE COM HWND's
	CreateWindowW(L"STATIC", L"Choose your champion:", WS_VISIBLE | WS_CHILD, 10, 10, 200, 100, hWnd, NULL, hInst, NULL);
	HWND hWndComboChamp = CreateWindowW(L"COMBOBOX", NULL, WS_BORDER | WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VISIBLE | WS_VSCROLL, 200, 10, 200, 300, hWnd, NULL, hInst, NULL);
	hWndEdit = CreateWindowW(L"EDIT", NULL, WS_BORDER | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOHSCROLL, 10, 50, 500, 300, hWnd, NULL, hInst, NULL);

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
	string initChamp = GetChampionJsonFile();
	GetSpellImg(initChamp, hWnd);

	LPCWSTR result = jsonText.c_str();


	SetWindowTextW(hWndEdit, result);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	ShowWindow(spellHwnd, nCmdShow);
	UpdateWindow(spellHwnd);

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
			GetSpellImg(pathOutFile, hWnd);

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

		case ID_FILE_SPELLWINDOW:
			SpellWnd::ShowSpellWindow(hWnd);
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
		DrawImg(hdc);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		exitThread = true;
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

void  GetSpellImg(string pathOutFile, HWND hWnd) {
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
	int spellCount = 0;
	for (string spell : spells) {

		FILE* fp;
		CURL* curl = curl_easy_init();
		CURLcode res_code;

		string URL = "http://ddragon.leagueoflegends.com/cdn/11.9.1/img/spell/";
		URL += spell;

		CHAR czTempPath[MAX_PATH] = { 0 };
		GetTempPathA(MAX_PATH, czTempPath);

		string pathSpellFile = czTempPath;
		string mkDirCommand = "mkdir ";

		mkDirCommand += czTempPath;
		mkDirCommand += "\\lol_overlay";
		system(mkDirCommand.c_str());

		// WRITE SPELL PNG
		string spellBtn;

		switch (spellCount)
		{
		case 0:
			spellBtn = "Q";
			break;
		case 1:
			spellBtn = "W";
			break;
		case 2:
			spellBtn = "E";
			break;
		case 3:
			spellBtn = "R";
			break;
		}

		pathSpellFile += "\\lol_overlay\\";
		pathSpellFile += spellBtn;
		pathSpellFile += ".png";

		pathSpellImg[spellCount] = pathSpellFile;
		fopen_s(&fp, pathSpellFile.c_str(), "wb");
		spellCount++;

		curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_funtion);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION);

		res_code = curl_easy_perform(curl);

		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res_code);
		if (!((res_code == 200 || res_code == 201) && res_code != CURLE_ABORTED_BY_CALLBACK))
		{
			printf("!!! Response code: %d\n", res_code);
		}

		fclose(fp);
		file.close();
		curl_easy_cleanup(curl);
	}

	SpellWnd::SetPathSpellImg(pathSpellImg);

	InvalidateRect(hWnd, NULL, FALSE);
}

string GetChampionJsonFile() {
	//DOWNLOAD CHAMPION DATA
	IStream* stream;
	string champId;
	wstring s(selectedChamp);
	string sChampionF(s.begin(), s.end());

	string URL = "http://ddragon.leagueoflegends.com/cdn/11.9.1/data/pt_BR/champion/";
	for (auto it = championList.begin(); it != championList.end(); it++) {

		string champ = (*it)["name"].get<string>();
		if (champ.compare(sChampionF) == 0) {
			champId = (*it)["id"].get<string>();
		}
	}

	champId += ".json";
	URL += champId;

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
		MessageBox(NULL, (LPCWSTR)L"Erro ao baixar arquivo do campeão selecionado!", (LPCWSTR)L"Error!", MB_OK);
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

size_t callback_funtion(void* ptr, size_t size, size_t nmemb, void* userdata)
{
	FILE* stream = (FILE*)userdata;
	if (!stream)
	{
		printf("!!! No stream\n");
		return 0;
	}

	size_t written = fwrite((FILE*)ptr, size, nmemb, stream);
	return written;
}

void DrawImg(HDC hdc) {
	wstring pathI;
	int heigth = 10;

	for (int i = 0; i < 4; i++) {
		pathI = wstring(pathSpellImg[i].begin(), pathSpellImg[i].end());
		Gdiplus::Graphics gf(hdc);
		Gdiplus::Image gdImage(pathI.c_str());
		gf.DrawImage(&gdImage, 600, heigth);

		heigth += 90;
	}
}

DWORD WINAPI KeyboardListner(_In_ LPVOID lpParam)
{
	SetWindowText(hWndEdit, L"");
	std::vector<char>* vector;
	vector = SpellWnd::GetVectorOfSpells();
	while (!exitThread)
	{
		SHORT kCodeReturn = 0;
		kCodeReturn = GetAsyncKeyState(0x51);// Q key
		if (kCodeReturn != 0)
		{
			SetWindowText(hWndEdit, L"Q");
			
			(*vector).push_back('Q');
			InvalidateRect(spellHwnd, NULL, FALSE);
		}
		kCodeReturn = GetAsyncKeyState(0x57);// W key
		if (kCodeReturn != 0)
		{
			SetWindowText(hWndEdit, L"W");
		
			(*vector).push_back('W');
			InvalidateRect(spellHwnd, NULL, FALSE);
		}
		kCodeReturn = GetAsyncKeyState(0x45);// E key
		if (kCodeReturn != 0)
		{
			SetWindowText(hWndEdit, L"E");
		
			(*vector).push_back('E');
			InvalidateRect(spellHwnd, NULL, FALSE);
		}
		kCodeReturn = GetAsyncKeyState(0x52);// R key
		if (kCodeReturn != 0)
		{
			SetWindowText(hWndEdit, L"R");
		
			(*vector).push_back('R');
			InvalidateRect(spellHwnd, NULL, FALSE);
		}
		kCodeReturn = GetAsyncKeyState(0x44);// D key
		if (kCodeReturn != 0)
		{
			SetWindowText(hWndEdit, L"D");
			(*vector).push_back('D');
		}
		kCodeReturn = GetAsyncKeyState(0x46);// F key
		if (kCodeReturn != 0)
		{
			SetWindowText(hWndEdit, L"F");
			(*vector).push_back('F');
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(160));
	}

	return EXIT_SUCCESS;
}