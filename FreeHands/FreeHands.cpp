//
// FreeHands.cpp : Defines the entry point for the application.
//
#include "FreeHands.h"
#include "HidMap.h"



HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

HWND gAddButton, gSartStopButton, gStaticInfo, gStatusInfo, gDelayButton;
pvmulti_client      vmulti;
bool g_isBackgroundTaskRunning = false;

#define WM_UPDATE_STATUS   (WM_USER + 1)
#define WM_TASK_COMPLETED  (WM_USER + 2)


wchar_t gStatusString[100];
const wchar_t gStatusTitle[] = L"Task Status:";


DWORD gKeycodeArray[100] = { 0 };

std::vector<HWND> gControlListHWNDs;  //Store combobox list.
std::vector<HWND> gScrollInfoHWNDs;
std::thread g_workerThread;

// 后台工作线程函数
void WorkerThread(HWND hWnd)
{
    //We start after 3 s.
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    PostMessage(hWnd, WM_UPDATE_STATUS, 0, 0);
    while (g_isBackgroundTaskRunning)
    {
        SendHidRequests();
        if (!g_isBackgroundTaskRunning) break;
    }
    PostMessage(hWnd, WM_TASK_COMPLETED, 0, 0);
    return;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    //
    // File device
    //
    vmulti = vmulti_alloc();

    if (vmulti == NULL)
    {
        MessageBox(NULL, L"Out of Memory!", L"Error", MB_OK | MB_ICONINFORMATION);
        return ERROR_OUTOFMEMORY;
    }

    if (!vmulti_connect(vmulti))
    {
        vmulti_free(vmulti);
        MessageBox(NULL, L"Cound't find vmutli device!", L"Error", MB_OK | MB_ICONINFORMATION);
        return ERROR_DEVICE_ENUMERATION_ERROR;
    }

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FREEHANDS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FREEHANDS));

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

    vmulti_disconnect(vmulti);

    vmulti_free(vmulti);

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
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FREEHANDS));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_FREEHANDS);
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

    HWND hWnd = CreateWindow(szWindowClass, szTitle,
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, // 去掉最大化按钮,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, nullptr, nullptr, hInstance, nullptr);

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
        case IDC_ADD_BUTTON:
        case IDC_DEALY_BUTTON:
            CreateControlList(hWnd, wParam);
            break;
        case IDC_START_BUTTON:
            if (!g_isBackgroundTaskRunning)
            {
                // 启动任务
                g_isBackgroundTaskRunning = true;
                SetWindowText(gSartStopButton, L"Stop");
                swprintf(gStatusString, 100, L"%s %s", gStatusTitle, L"Start...");
                SetWindowText(gStatusInfo, gStatusString);
                g_workerThread = std::thread(WorkerThread, hWnd);

            }
            else
            {
                // 停止任务
                g_isBackgroundTaskRunning = false;
                SetWindowText(gSartStopButton, L"Run");
                swprintf(gStatusString, 100, L"%s %s", gStatusTitle, L"Stoping...");
                SetWindowText(gStatusInfo, gStatusString);

                //Wait the thread stop.
                if (g_workerThread.joinable())
                {
                    if (g_workerThread.get_id() != std::this_thread::get_id())
                    {

                        g_workerThread.join();
                    }
                }

                swprintf(gStatusString, 100, L"%s %s", gStatusTitle, L"Stop");
                SetWindowText(gStatusInfo, gStatusString);
            }
            
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_CREATE:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...

        // 注册滚动容器窗口类
        WNDCLASS wcScroll = {};
        wcScroll.lpfnWndProc = ScrollContainerProc;
        wcScroll.hInstance = hInst;
        wcScroll.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcScroll.lpszClassName = L"ScrollContainer";
        wcScroll.hCursor = LoadCursor(NULL, IDC_ARROW);
        RegisterClass(&wcScroll);

        gAddButton = CreateWindow(
            L"BUTTON",
            L"Add Keycode",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
            20, 20, 120, 30,
            hWnd,
            (HMENU)IDC_ADD_BUTTON,
            hInst,
            NULL
        );
        gDelayButton = CreateWindow(
            L"BUTTON",
            L"Add Delay",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
            20, 60, 120, 30,
            hWnd,
            (HMENU)IDC_DEALY_BUTTON,
            hInst,
            NULL
        );

        gSartStopButton = CreateWindow(
            L"BUTTON",
            L"RUN",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
            20, 100, 120, 30,
            hWnd,
            (HMENU)IDC_START_BUTTON,
            hInst,
            NULL
        );

        gStaticInfo = CreateWindow(
            L"STATIC",
            L"KeyCode\n",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            20, 140, 120, 60,
            hWnd,
            NULL,
            hInst,
            NULL);

        swprintf(gStatusString, 100, L"%s %s", gStatusTitle, L"Stop");
        gStatusInfo = CreateWindow(
            L"STATIC",
            gStatusString,
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            20, 200, 120, 60,
            hWnd,
            (HMENU)IDC_STATUS_TEXT,
            hInst,
            NULL);

        // 创建带滚动条的容器
        gScrollContainer = CreateWindow(
            L"ScrollContainer",
            L"",
            WS_CHILD | WS_VISIBLE | WS_VSCROLL,
            200, 20,
            SCROLL_WIDTH, SCROLL_HEIGHT,
            hWnd,
            (HMENU)IDC_SCROLL_CONTAINER,
            hInst,
            NULL);

        // 设置滚动条范围
        SetScrollRange(gScrollContainer, SB_VERT, 0, SCROLL_RANGE, FALSE);
        SetScrollPos(gScrollContainer, SB_VERT, 0, TRUE);

        HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        SendMessage(gAddButton, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(gDelayButton, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(gSartStopButton, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(gStaticInfo, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(gStatusInfo, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(gScrollContainer, WM_SETFONT, (WPARAM)hFont, TRUE);


        EndPaint(hWnd, &ps);
    }
    break;
    case WM_UPDATE_STATUS:
    {
        //Update Status
        swprintf(gStatusString, 100, L"%s %s", gStatusTitle, L"Running...");
        SetWindowText(gStatusInfo, gStatusString);
    }
        break;
    case WM_TASK_COMPLETED:
    {
        // 线程自然结束（不是通过按钮停止）
        if (g_isBackgroundTaskRunning)
        {
            g_isBackgroundTaskRunning = false;
            SetWindowText(gAddButton, L"Run");
            SetWindowText(gStatusInfo, L"Task Status: completed");
        }
    }
    break;
    case WM_DESTROY:
        if (g_isBackgroundTaskRunning)
        {
            g_isBackgroundTaskRunning = false;
            if (g_workerThread.joinable())
            {
                g_workerThread.join();
            }
        }
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

INT_PTR CreateControlList(HWND hWnd, WPARAM wParam) {

    int wmId = LOWORD(wParam);
    // Calcault control option place
    int comboCount = gControlListHWNDs.size();
    int yPos = START_Y + (comboCount * COMBO_SPACING);

    if (comboCount + IDC_FIRST_COMBOBOX > IDC_END_COMBOBOX) {
        MessageBox(hWnd, L"Out of button range!", L"Error", MB_OK | MB_ICONINFORMATION);
        return (INT_PTR)FALSE;
    }

    HWND hNewControlHWND;
    HWND hNewScrollInfoHWND;
    if (wmId == IDC_ADD_BUTTON) {
        //
        hNewScrollInfoHWND = CreateWindow(
            L"STATIC",
            CONTROL_KEYCODE_STRING,
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            START_X, yPos, INFO_WIDTH, INFO_HEIGHT,
            gScrollContainer, 
            (HMENU)(IDC_FIRST_SCROLL_INFO + comboCount),
            GetModuleHandle(NULL),
            NULL);
        //Creat Keycode Combobox
        hNewControlHWND = CreateWindow(
            L"COMBOBOX",
            TEXT(""),
            WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
            CONTROL_START_X, yPos,
            COMBO_WIDTH, COMBO_HEIGHT,
            gScrollContainer,
            (HMENU)(IDC_FIRST_COMBOBOX + comboCount),  // Dymaic ID
            GetModuleHandle(NULL),
            NULL
        );
    }
    else {
        hNewScrollInfoHWND = CreateWindow(
            L"STATIC",
            CONTROL_DELAY_STRING,
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            START_X, yPos, INFO_WIDTH, INFO_HEIGHT,
            gScrollContainer,
            (HMENU)(IDC_FIRST_SCROLL_INFO + comboCount),
            GetModuleHandle(NULL),
            NULL);

        //Creat Delay
        hNewControlHWND = CreateWindow(
            L"EDIT",
            TEXT("500"),
            WS_CHILD | WS_VISIBLE | WS_BORDER |ES_NUMBER ,
            CONTROL_START_X, yPos,
            COMBO_WIDTH, 20,
            gScrollContainer,
            (HMENU)(IDC_FIRST_COMBOBOX + comboCount),  // Dymaic ID
            GetModuleHandle(NULL),
            NULL
        );
    }


    if (hNewControlHWND)
    {
        // 设置字体
        HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        SendMessage(hNewControlHWND, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hNewScrollInfoHWND, WM_SETFONT, (WPARAM)hFont, TRUE);

        if (wmId == IDC_ADD_BUTTON) {
            for (int i = 0; i < KeyboardItemSize; i++) {
                SendMessage(hNewControlHWND, CB_ADDSTRING, 0, (LPARAM)KeyboardItems[i]);
            }
            // 设置默认选中第一项
            SendMessage(hNewControlHWND, CB_SETCURSEL, 0, 0);
        }
        // 添加到组合框列表
        gScrollInfoHWNDs.push_back(hNewScrollInfoHWND);
        gControlListHWNDs.push_back(hNewControlHWND);

        UpdateKeycodeInfo(hWnd, wParam);

        // 如果需要，更新滚动范围（基于组合框数量）
        int neededRange = comboCount * COMBO_SPACING + 20 - SCROLL_HEIGHT;
        if (neededRange > 0) {
            SetScrollRange(gScrollContainer, SB_VERT, 0, neededRange, TRUE);
        }
    }
    return (INT_PTR)TRUE;
}

INT_PTR UpdateKeycodeInfo(HWND hWnd, WPARAM wParam) {
    if (g_isBackgroundTaskRunning) {
        MessageBox(NULL, L"Task is running, please stop to refresh key code!", L"Warning", MB_OK | MB_ICONINFORMATION);
        return (INT_PTR)FALSE;
    }

    wchar_t result[1024] = L"KeyCode:\n";
    for (size_t i = 0; i < gControlListHWNDs.size(); i++)
    {
        //
        //Update Text Information
        //
        wchar_t buffer[256];
        int     BufferLen;

        BufferLen = GetWindowText(gControlListHWNDs[i], (LPWSTR)buffer, 256);
        
        wchar_t item[300];
        if (i != gControlListHWNDs.size() - 1) {
            swprintf(item, 300, L"%s + ", buffer);
        }
        else {
            swprintf(item, 300, L"%s", buffer);
        }
        wcscat_s(result, item);

        //
        //Update key code array
        //
        if (wcscmp(buffer, ITEMSTRING_LEFT_MOUSE) == 0) {//mouse
            gKeycodeArray[i] = 0x01;//Left button
        }
        else if (wcscmp(buffer, ITEMSTRING_RIGHT_MOUSE) == 0) {
            gKeycodeArray[i] = 0x02;//Right button
        }
        else if (wcscmp(buffer, ITEMSTRING_MID_MOUSE) == 0) {
            gKeycodeArray[i] = 0x03;//Mid button
        }
        else if (wcscmp(buffer, ITEMSTRING_RELEASE_MOUSE) == 0) {
            gKeycodeArray[i] = 0x00;//Release button
        }
        else if (HIDWide::isAlphaWide(buffer[0])) { 
            gKeycodeArray[i] = HIDWide::getKeycode(buffer[0]);
        }
        else if (HIDWide::isPositiveInteger(buffer, BufferLen)) {
            gKeycodeArray[i] = KEYCODE_DELAY_SIG + wcstoul(buffer, NULL, 10);
        }
        else {
            //do nothing
        }
    }
    // Display result
    SetWindowText(gStaticInfo, result);
    return (INT_PTR)TRUE;
}