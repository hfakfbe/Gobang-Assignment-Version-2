// Gobang Assignment Version 2.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "Gobang Assignment Version 2.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

Painter* CurrentPainter;

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GOBANGASSIGNMENTVERSION2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GOBANGASSIGNMENTVERSION2));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GOBANGASSIGNMENTVERSION2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GOBANGASSIGNMENTVERSION2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        CurrentPainter = new Painter(hWnd, ID_HUMAN, ID_HUMAN, 15, -1, true);//默认棋盘
        break;
    case WM_MOUSEMOVE:
        CurrentPainter->MouseOperation(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MOUSE_HOVER);
        break;
    case WM_LBUTTONDOWN:
        CurrentPainter->MouseOperation(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), MOUSE_SET);
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
            case IDM_NEWGAME:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_NEWGAME), hWnd, Newgame);
                break;
            case IDM_REGRET:
                CurrentPainter->RequestMode(REQUEST_REGRET);
                break;
            case IDM_GIVEIN:
                CurrentPainter->RequestMode(REQUEST_GIVEIN);
                break;
            case IDM_SUSRES:
                CurrentPainter->RequestMode(REQUEST_SUSRES);
                break;
            case IDM_SAVE:
            {
                OPENFILENAME ofn;
                WCHAR* szFile = new WCHAR[512];
                WCHAR* szFileTitle = new WCHAR[512];
                memset(&ofn, 0, sizeof(ofn));
                memset(szFile, 0, sizeof(WCHAR) * 512);
                memset(szFileTitle, 0, sizeof(WCHAR) * 512);
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hWnd;
                ofn.hInstance = hInst;
                ofn.lpstrFilter = L"Json File(*.json)\0*.json\0";
                ofn.nFilterIndex = 1;
                ofn.lpstrFile = szFile;
                ofn.nMaxFile = sizeof(WCHAR) * 512;
                ofn.lpstrFileTitle = szFileTitle;
                ofn.nMaxFileTitle = sizeof(WCHAR) * 512;
                ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
                ofn.lpstrTitle = L"另存为...";

                // 按下确定按钮
                BOOL ok = GetSaveFileName(&ofn);
                if (ok) {
                    CurrentPainter->SaveToJson(szFileTitle);
                    SendMessage(hWnd, WM_SETFOCUS, 0, 0);
                }
                break;
            }
            case IDM_OPEN: 
            {
                OPENFILENAME ofn;
                WCHAR* szFile = new WCHAR[512];
                WCHAR* szFileTitle = new WCHAR[512];
                memset(&ofn, 0, sizeof(ofn));
                memset(szFile, 0, sizeof(WCHAR) * 512);
                memset(szFileTitle, 0, sizeof(WCHAR) * 512);
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hWnd;
                ofn.hInstance = hInst;
                ofn.lpstrFilter = L"Json File(.json)\0*.json\0";
                ofn.nFilterIndex = 1;
                ofn.lpstrFile = szFile;
                ofn.nMaxFile = sizeof(WCHAR) * 512;
                ofn.lpstrFileTitle = szFileTitle;
                ofn.nMaxFileTitle = sizeof(WCHAR) * 512;
                ofn.Flags = OFN_FILEMUSTEXIST | OFN_EXPLORER;

                // 按下确定按钮
                BOOL ok = GetOpenFileName(&ofn);
                if (ok) {
                    delete CurrentPainter;
                    CurrentPainter = new Painter(szFileTitle, hWnd);
                }
                break;
            }
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
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            RECT rect; GetClientRect(hWnd, &rect);
            CurrentPainter->PaintBoard(hdc, &rect);
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

// “关于”框的消息处理程序。
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
