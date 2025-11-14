#pragma once
#ifndef _FREE_HANDS_H_
#define _FRE_FREE_HANDS_H_

#include "resource.h"
#include "framework.h"
#include <vector>
#include <string>
#include <thread>
#include <shlwapi.h>

extern "C" {
#include "vmulticlient.h"
#pragma comment(lib, "vmulticlient.lib")
}

// Global Variables:

//UI Define >>>
#define MAX_LOADSTRING 100

#define IDC_ADD_BUTTON			1001
#define IDC_START_BUTTON	    1002
#define IDC_SCROLL_CONTAINER	1003
#define IDC_STATUS_TEXT         1004
#define IDC_DEALY_BUTTON        1005
#define IDC_FIRST_COMBOBOX		1100
#define IDC_END_COMBOBOX		1999
//1100~1999 we reseved for combox list.
#define IDC_FIRST_SCROLL_INFO   1200

const int COMBO_WIDTH = 100;
const int COMBO_HEIGHT = 150;
const int START_X = 20;
const int START_Y = 20;
const int INFO_WIDTH = 80;
const int INFO_HEIGHT = 20;
const int CONTROL_START_X = 70;
const int CONTROL_START_Y = 20;
const int COMBO_SPACING = 35;

const int SCROLL_WIDTH = 250;
const int SCROLL_HEIGHT = 300;
const int SCROLL_RANGE = 1000; // Scroll Range

constexpr auto KEYCODE_DELAY_SIG = 1000;
constexpr auto CONTROL_KEYCODE_STRING = L"KeyCode:";
constexpr auto CONTROL_DELAY_STRING = L"Delay(ms):";
constexpr auto ITEMSTRING_RELEASE_MOUSE = L"Release Button";
constexpr auto ITEMSTRING_LEFT_MOUSE = L"Left Button";
constexpr auto ITEMSTRING_RIGHT_MOUSE = L"Right Button";
constexpr auto ITEMSTRING_MID_MOUSE = L"Mid Button";
//UI Define <<<

extern HINSTANCE hInst;                                // current instance
extern WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
extern WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

extern HWND gScrollContainer, gAddButton, gGetButton, gStaticInfo;

extern std::vector<HWND>    gScrollInfoHWNDs;
extern std::vector<HWND>    gControlListHWNDs;  //Store combobox list.
extern DWORD				gKeycodeArray[100];
extern pvmulti_client       vmulti;
extern const wchar_t		*KeyboardItems[];
extern const int			KeyboardItemSize;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    ScrollContainerProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

INT_PTR CALLBACK CreateControlList(HWND hWnd, WPARAM wParam);
INT_PTR UpdateKeycodeInfo(HWND hWnd, WPARAM wParam);
VOID SendHidRequests();
#endif