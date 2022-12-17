#include "framework.h"
#include "Gobang Assignment Version 2.h"
#include "Painter.h"

extern Painter* CurrentPainter;

#define MAXN_STRING_SIZE        4
#define MAXN_STRING_MODE        4
#define MAXN_STRING_TIMELIMIT   4

const TCHAR* COMBOBOX_STRING_SIZE[MAXN_STRING_SIZE] = 
    {L"       9×9", L"     15×15",L"     19×19",L"     25×25"};
const UNIT_SIZE COMBOBOX_PARAM_SIZE[MAXN_STRING_SIZE] = { 9,15,19,25 };
const TCHAR* COMBOBOX_STRING_MODE[MAXN_STRING_MODE] = 
    { L"      P V P", L"      P V E", L"      E V P", L"      E V E" };
const UNIT_ID COMBOBOX_PARAM_MODE[MAXN_STRING_MODE][2] = 
    { {ID_HUMAN,ID_HUMAN},{ID_HUMAN,ID_AI},{ID_AI,ID_HUMAN},{ID_AI,ID_AI} };
const TCHAR* COMBOBOX_STRING_TIMELIMIT[MAXN_STRING_TIMELIMIT] = 
    { L"    每步15s", L"    每步60s", L"    每步120s", L"    不限时" };
const time_t COMBOBOX_PARAM_TIMELIMIT[MAXN_STRING_TIMELIMIT] = { 15,60,120,-1 };

INT_PTR CALLBACK Newgame(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hSize, hMode, hTimelimit, hRegret;
    //默认占位，没什么用
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG: 
    {
        hSize = CreateWindowEx(0, L"combobox", NULL, 
            WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_HASSTRINGS | CBS_DROPDOWNLIST,
            140, 59, 110, 200, hDlg, (HMENU)1001, hInst, 0);
        hMode = CreateWindowEx(0, L"combobox", NULL,
            WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_HASSTRINGS | CBS_DROPDOWNLIST,
            140, 116, 110, 200, hDlg, (HMENU)1001, hInst, 0);
        hTimelimit = CreateWindowEx(0, L"combobox", NULL,
            WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_HASSTRINGS | CBS_DROPDOWNLIST,
            140, 173, 110, 200, hDlg, (HMENU)1001, hInst, 0);
        hRegret = GetDlgItem(hDlg, IDD_NEWGAME_CHECK_REGRET);
        for (int i = 0; i < MAXN_STRING_SIZE; ++i) ComboBox_AddString(hSize, (LPARAM)COMBOBOX_STRING_SIZE[i]);
        for (int i = 0; i < MAXN_STRING_MODE; ++i) ComboBox_AddString(hMode, (LPARAM)COMBOBOX_STRING_MODE[i]);
        for (int i = 0; i < MAXN_STRING_TIMELIMIT; ++i) ComboBox_AddString(hTimelimit, (LPARAM)COMBOBOX_STRING_TIMELIMIT[i]);
        //默认选项
        ComboBox_SetCurSel(hSize, 1);//15*15
        ComboBox_SetCurSel(hMode, 0);//pvp
        ComboBox_SetCurSel(hTimelimit, 3);//-1
        Button_SetCheck(hRegret, BST_CHECKED);//勾选
        return (INT_PTR)TRUE;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            //Declare
            UNIT_SIZE size;
            UNIT_ID blackid, whiteid;
            time_t timelimit;
            bool ifregret;
            //getValues
            size = COMBOBOX_PARAM_SIZE[ComboBox_GetCurSel(hSize)];
            int idx = ComboBox_GetCurSel(hMode);
            blackid = COMBOBOX_PARAM_MODE[idx][0], whiteid = COMBOBOX_PARAM_MODE[idx][1];
            timelimit = COMBOBOX_PARAM_TIMELIMIT[ComboBox_GetCurSel(hTimelimit)];
            if (Button_GetCheck(hRegret) == BST_CHECKED) ifregret = true;
            else ifregret = false;
            //创建新Painter
            HWND hWnd = CurrentPainter->GetHwnd();
            delete CurrentPainter;
            CurrentPainter = new Painter(hWnd, blackid, whiteid, size, timelimit, ifregret);
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }else if(LOWORD(wParam) == CBN_SELCHANGE)
        break;
    }
    return (INT_PTR)FALSE;
}
