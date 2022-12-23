#include "Gobang Assignment Version 2.h"

INT_PTR CALLBACK History(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG: 
    {
        HWND  listbox = CreateWindowW(L"LISTBOX", L"",
            WS_CHILD | WS_BORDER | WS_VISIBLE | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | WS_VSCROLL | LBS_HASSTRINGS,
            20, 30, 675, 350, hDlg, (HMENU)IDM_HISTORY, NULL, NULL);
        std::vector<Json> history = CurrentPainter->LoadHistory();
        std::string endtime, blackid, whiteid, winner;
        for (int i = 0; i < history.size(); ++i) {
            history[i].Askkey("endtime", endtime);
            time_t et = atoll(endtime.c_str());
            tm p; localtime_s(&p, &et);
            endtime = std::to_string(p.tm_year + 1900) + "年" + std::to_string(p.tm_mon + 1) + "月" + std::to_string(p.tm_mday) + 
                "日" + std::to_string(p.tm_hour) + "时" + std::to_string(p.tm_min) + "/分" + std::to_string(p.tm_sec) + "秒";
            history[i].Askkey("blackid", blackid);
            if (atoi(blackid.c_str()) == ID_HUMAN) blackid = "Human"; else blackid = "Ai";
            history[i].Askkey("whiteid", whiteid);
            if (atoi(whiteid.c_str()) == ID_HUMAN) whiteid = "Human"; else whiteid = "Ai";
            history[i].Askkey("winner", winner);
            if (atoi(winner.c_str()) == ID_BLACK) winner = "Black";
            else if (atoi(winner.c_str()) == ID_WHITE) winner = "White";
            else winner = "NULL";
            WCHAR* wc = new WCHAR[256]();
            wsprintf(wc, L"【%d】时间：%S 黑棋：%S 白棋：%S 胜者：%S",
                i + 1, endtime.c_str(), blackid.c_str(), whiteid.c_str(), winner.c_str());
            ListBox_AddString(listbox, wc);
        }
        return (INT_PTR)TRUE;
    }
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
