#include "framework.h"
#include "Gobang Assignment Version 2.h"

extern Painter* CurrentPainter;

INT_PTR CALLBACK Newgame(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hSize, hMode, hTimelimit, hRegret; TCHAR szMessage[20] = L"Hello";
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        hSize = GetDlgItem(hDlg, IDD_NEWGAME_COMBO_SIZE);
        hMode = GetDlgItem(hDlg, IDD_NEWGAME_COMBO_MODE);
        hTimelimit = GetDlgItem(hDlg, IDD_NEWGAME_COMBO_TIMELIMIT);
        hRegret = GetDlgItem(hDlg, IDD_NEWGAME_CHECK_REGRET);

        //SendMessage(hWndComboBox, CB_ADDRSTRING, 0, (LPARAM)szMessage);
        ComboBox_AddString(hSize, (LPARAM)szMessage);
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
