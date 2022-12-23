#include "Painter.h"

const WCHAR* Win_black = L"Black Win!";
const WCHAR* Win_white = L"White Win!";
const WCHAR* Win_title = L"Game Over!";

WCHAR** RightBlankString;
WCHAR** LeftBlankString;

Painter::Painter(HWND hWnd, UNIT_ID blackid, UNIT_ID whiteid, UNIT_SIZE size, time_t timelimit, bool ifregret) :hWnd(hWnd) {
    MainChess = new Chess(blackid, whiteid, size, timelimit, ifregret);
    Judgeparam* param = new Judgeparam(MainChess, this);
    hJudge = CreateThread(NULL, 0, Judgeproc, (void*)param, 0, NULL);
    boardcolor = RGB(255, 227, 132);
    //背景
    InitBackground();
    if (blackid == ID_AI) {
        ChangeBlank(BLANK_LEFT, L"黑方：AI", 7);
    }
    else ChangeBlank(BLANK_LEFT, L"黑方：人类", 7);
    if (whiteid == ID_AI) {
        ChangeBlank(BLANK_LEFT, L"白方：AI", 8);
    }
    else ChangeBlank(BLANK_LEFT, L"白方：人类", 8);
    //刷新
    RefreshBoard();
}

Painter::Painter(const WCHAR* filename, HWND hWnd) :hWnd(hWnd) {
    boardcolor = RGB(255, 227, 132);
    std::ifstream infile;
    infile.open(filename);
    infile.seekg(0, infile.end);
    auto fos = infile.tellg();
    unsigned long long filesize = fos;
    bool flag = true;//是否成功加载
    if (filesize < 100000) {
        char* buffertmp = new char[filesize]();
        infile.clear();
        infile.seekg(0L, std::ios::beg);
        infile.read(buffertmp,filesize);
        std::string buffer(buffertmp);
        for (int i = buffer.size()-1; i >= 0; --i) //不知道为啥，有时候末尾会出现4个0xFC（-3）
            if (buffer[i] == '}') break;
            else buffer[i] = 0;
        delete[] buffertmp;
        //载入+合法性判断
        Json openjson(buffer);
        std::string tmp;
        UNIT_SIZE size; openjson.Askkey("size", tmp); size = atoi(tmp.c_str()); if (size < 9 || size>25) flag = false;
        UNIT_STATUS status; openjson.Askkey("status", tmp); status = atoi(tmp.c_str());
        if (status != STATUS_END) status = STATUS_GAMING;//统一一下状态（康师傅不服）
        UNIT_ID blackid, whiteid, winner;
        openjson.Askkey("blackid", tmp); blackid = atoi(tmp.c_str()); if (blackid != ID_HUMAN && blackid != ID_AI) flag = false;
        openjson.Askkey("whiteid", tmp); whiteid = atoi(tmp.c_str()); if (whiteid != ID_HUMAN && whiteid != ID_AI) flag = false;
        openjson.Askkey("winner", tmp); winner = atoi(tmp.c_str()); 
        if (winner != ID_BLACK && winner != ID_WHITE && winner != ID_NULL) flag = false;
        time_t begintime, endtime, timelimit;
        openjson.Askkey("begintime", tmp); begintime = atoll(tmp.c_str());
        openjson.Askkey("endtime", tmp); endtime = atoll(tmp.c_str());
        openjson.Askkey("timelimit", tmp); timelimit = atoll(tmp.c_str());
        bool ifregret; openjson.Askkey("ifregret", tmp);
        if (tmp == "true") ifregret = true;
        else if (tmp == "false") ifregret = false;
        else flag = false;
        openjson.Askkey("step", tmp);
        std::vector<std::string> steptmp;
        openjson.Translatevector(tmp, steptmp);
        if (flag) {
            MainChess = new Chess(blackid, whiteid, size, timelimit, ifregret); 
            MainChess->status = status;
            MainChess->begintime = begintime;
            MainChess->endtime = endtime;
            MainChess->winner = winner;
            for (int i = 0; i < steptmp.size(); ++i) {//初始化step
                Json subjson(steptmp[i]);
                UNIT_SIZE x, y;
                UNIT_ID color;
                subjson.Askkey("x", tmp); x = atoi(tmp.c_str());
                subjson.Askkey("y", tmp); y = atoi(tmp.c_str());
                subjson.Askkey("color", tmp); color = atoi(tmp.c_str());
                if (x >= size || x < 0 || y >= size || y < 0 || (color != PIECE_BLACK && color != PIECE_WHITE)) break;
                MainChess->step.push_back(std::make_tuple(x, y, color));
                MainChess->chessboard[x][y] = color;
            }
            Judgeparam* param = new Judgeparam(MainChess, this);
            hJudge = CreateThread(NULL, 0, Judgeproc, (void*)param, 0, NULL);
            RefreshBoard();
            return;
        }
    }
    //默认初始化，因为C++不允许在构造函数中调用构造函数，也不会委托构造函数，就CV一遍了
    MainChess = new Chess(ID_HUMAN, ID_HUMAN, 15, -1, true);
    Judgeparam* param = new Judgeparam(MainChess, this);
    hJudge = CreateThread(NULL, 0, Judgeproc, (void*)param, 0, NULL);
    RefreshBoard();
}

Painter::~Painter() {
    MessageQueue.emplace(JudgeMessage(JUDGEMSG_EXIT));
    delete MainChess;
    //MyError
}

inline void Painter::DrawAChess(HDC hdc, UNIT_INTERFACE x, UNIT_INTERFACE y) {
    Ellipse(hdc, x - chessradius, y - chessradius, x + chessradius, y + chessradius);
}

void Painter::DrawAChess(UNIT_SIZE x, UNIT_SIZE y, UNIT_ID color) {
    HDC hdc = GetDC(hWnd);
    HBRUSH brh = CreateSolidBrush(color == PIECE_BLACK ? RGB(0, 0, 0) : RGB(255, 255, 255));
    SelectObject(hdc, brh);
    UNIT_INTERFACE rx = vertex[0].x + gridborder + x * gridlength;
    UNIT_INTERFACE ry = vertex[0].y + gridborder + y * gridlength;
    Ellipse(hdc, rx - chessradius, ry - chessradius, rx + chessradius, ry + chessradius);
    DeleteObject(brh);
    ReleaseDC(hWnd, hdc);
}

void Painter::DrawChess(HDC hdc, UNIT_STATUS flag, COLORREF color) {
    HBRUSH brh = CreateSolidBrush(color);
    SelectObject(hdc, brh);
    chessradius = boardlength / MainChess->size / 2 - 1;
    for (int i = 0; i < MainChess->size; ++i) {
        UNIT_INTERFACE rx = vertex[0].x + gridborder + i * gridlength;
        for (int j = 0; j < MainChess->size; ++j) {
            if (MainChess->chessboard[i][j] == flag) {
                DrawAChess(hdc, rx, vertex[0].y + gridborder + j * gridlength);
            }
        }
    }
    DeleteObject(brh);
}

void Painter::PaintBoard(HDC hdc, RECT* wndr) {
    HBRUSH brh = CreateSolidBrush(boardcolor);
    SelectObject(hdc, brh);
    clientborder = (wndr->bottom - wndr->top) / 100;
    boardlength = wndr->bottom - wndr->top - clientborder * 2;
    leftborder = (wndr->right - wndr->left - boardlength) >> 1;
    vertex[0].y = clientborder, vertex[0].x = leftborder;
    vertex[1].y = clientborder, vertex[1].x = leftborder + boardlength;
    vertex[2].y = boardlength + clientborder, vertex[2].x = leftborder + boardlength;
    vertex[3].y = boardlength + clientborder, vertex[3].x = leftborder;
    Polygon(hdc, vertex, 4);
    gridborder = boardlength / MainChess->size / 2;
    gridlength = 1.0 * (boardlength - gridborder * 2) / (MainChess->size - 1);
    for (int i = 0; i < MainChess->size; ++i) {
        MoveToEx(hdc, vertex[0].x + gridborder + i * gridlength, vertex[0].y + gridborder, NULL);
        LineTo(hdc, vertex[3].x + gridborder + i * gridlength, vertex[3].y - gridborder);
        MoveToEx(hdc, vertex[0].x + gridborder, vertex[0].y + gridborder + i * gridlength, NULL);
        LineTo(hdc, vertex[1].x - gridborder, vertex[1].y + gridborder + i * gridlength);
    }
    DeleteObject(brh);
    DrawChess(hdc, PIECE_BLACK, RGB(0, 0, 0));
    DrawChess(hdc, PIECE_WHITE, RGB(255, 255, 255));
}

void Painter::PaintHover() {
    HDC hdc = GetDC(hWnd);
    if (hover[1][0] >= 0 && hover[1][0] < MainChess->size && hover[1][1] >= 0 && hover[1][1] < MainChess->size) {
        HBRUSH brh;
        if (MainChess->step.size() == 0 || std::get<2>(MainChess->step.back()) == PIECE_WHITE)
            brh = CreateSolidBrush(RGB(115, 115, 115));
        else brh = CreateSolidBrush(RGB(225, 225, 225));
        SelectObject(hdc, brh);
        DrawAChess(hdc, vertex[0].x + gridborder + hover[1][0] * gridlength, vertex[0].y + gridborder + hover[1][1] * gridlength);
        DeleteObject(brh);
    }
    if (hover[0][0] >= 0 && hover[0][0] < MainChess->size && hover[0][1] >= 0 && hover[0][1] < MainChess->size
            && MainChess->chessboard[hover[0][0]][hover[0][1]] == 0) {
        HBRUSH brh = CreateSolidBrush(boardcolor);
        UNIT_INTERFACE cx = vertex[0].x + gridborder + hover[0][0] * gridlength;
        UNIT_INTERFACE cy = vertex[0].y + gridborder + hover[0][1] * gridlength;
        RECT outline;
        outline.left = cx - chessradius;
        outline.right = cx + chessradius;
        outline.top = cy - chessradius;
        outline.bottom = cy + chessradius;
        FillRect(hdc, &outline, brh);
        DeleteObject(brh);
        MoveToEx(hdc, max(outline.left, vertex[0].x + gridborder), cy, nullptr);
        LineTo(hdc, min(outline.right, vertex[1].x - gridborder), cy);
        MoveToEx(hdc, cx, max(outline.top, vertex[0].y + gridborder), nullptr);
        LineTo(hdc, cx, min(outline.bottom, vertex[2].y - gridborder));
    }
    ReleaseDC(hWnd, hdc);
}

void Painter::MouseOperation(UNIT_INTERFACE x, UNIT_INTERFACE y, UNIT_STATUS flag) {
    x -= leftborder;
    y -= clientborder;
    UNIT_SIZE posx = -1, posy = -1;
    if (x > 0 && x < boardlength && y > 0 && y < boardlength) {
        posx = x / gridlength;
        posy = y / gridlength;
        if (flag == MOUSE_SET) {
            Mode_Gaming(MODEMSG_SET, posx, posy);
        }
        else if (flag == MOUSE_HOVER) {
            if (MainChess->status == STATUS_GAMING && (posx != hover[1][0] || posy != hover[1][1]) && MainChess->chessboard[posx][posy] == 0) {
                hover[0][0] = hover[1][0], hover[0][1] = hover[1][1];
                hover[1][0] = posx, hover[1][1] = posy;
                PaintHover();
            }
        }
    }
}

void Painter::SaveToJson(const WCHAR* filename) {
    std::ofstream outfile;
    outfile.open(filename, std::ios::out | std::ios::trunc);
    Json savejson;
    savejson.Addkey("size", MainChess->size);
    savejson.Addkey("status", MainChess->status);
    savejson.Addkey("blackid", MainChess->blackid);
    savejson.Addkey("whiteid", MainChess->whiteid);
    savejson.Addkey("winner", MainChess->winner);
    savejson.Addkey("begintime", MainChess->begintime);
    savejson.Addkey("endtime", MainChess->endtime);
    savejson.Addkey("timelimit", MainChess->timelimit);
    savejson.Addkey("ifregret", MainChess->ifregret);
    std::vector<Json> subjson;
    for (int i = 1; i <= MainChess->step.size(); ++i) {
        Json tmp;
        tmp.Addkey("index", i);
        tmp.Addkey("x", std::get<0>(MainChess->step[i-1]));
        tmp.Addkey("y", std::get<1>(MainChess->step[i-1]));
        tmp.Addkey("color", std::get<2>(MainChess->step[i-1]));
        subjson.push_back(tmp);
    }
    savejson.Addkey("step", subjson);
    outfile << savejson.Jsontostring();
    outfile.close();
}

void Painter::RequestMode(UNIT_STATUS flag) {
    int index = -1;
    switch (flag) {
    case REQUEST_REGRET:
        Mode_Gaming(MODEMSG_REGRET, -1, -1);
        break;
    case REQUEST_SUSRES:
        Mode_Gaming(MODEMSG_SUSRES, -1, -1);
        break;
    case REQUEST_REPLAY:
        index = Mode_Replay(MODEREP_START);
        break;
    case REQUEST_REPEND:
        index = Mode_Replay(MODEREP_END);
        break;
    case REQUEST_REPNEXT:
        index = Mode_Replay(MODEREP_NEXT);
        break;
    case REQUEST_REPBACK:
        index = Mode_Replay(MODEREP_BACK);
        break;
    case REQUEST_ANALYS:
        break;
    case REQUEST_GIVEIN:
        Mode_Gaming(MODEMSG_GIVEIN, -1, -1);
        break;
    }
    WCHAR buffer[20] = {};
    if (index != -1) {
        wsprintfW(buffer, L"回放：第%d手", index);
    }
    ChangeBlank(BLANK_RIGHT, buffer, 5);
    ChangeBlankStatus();
    RefreshBoard();
}

void Painter::Mode_Gaming(UNIT_STATUS flag, UNIT_SIZE x, UNIT_SIZE y) {
    switch (flag) {
    case MODEMSG_SET:
        MessageQueue.emplace(JudgeMessage(JUDGEMSG_SET, x, y));
        break;
    case MODEMSG_SUSRES:
        MessageQueue.emplace(JudgeMessage(JUDGEMSG_SUSPEND_RESTART));
        break;
    case MODEMSG_REGRET:
        MessageQueue.emplace(JudgeMessage(JUDGEMSG_REGRET));
        break;
    case MODEMSG_GIVEIN:
        MessageQueue.emplace(JudgeMessage(JUDGEMSG_GIVEIN));
        break;
    }
}

int Painter::Mode_Replay(UNIT_STATUS flag) {
    //RightBlank(L"11111", 1);
    static UNIT_STATUS previous;
    static int index = 0;
    switch (flag) {
    case MODEREP_START:
        if (MainChess->step.size() == 0) return -1;
        previous = MainChess->status;
        MainChess->status = STATUS_REPLAY;
        for (int i = 0; i < MainChess->size; ++i) {
            for (int j = 0; j < MainChess->size; ++j)
                MainChess->chessboard[i][j] = 0;
        }
        break;
    case MODEREP_END:
        if (MainChess->status != STATUS_REPLAY) return -1;
        for (; index < MainChess->step.size(); ++index) {
            MainChess->chessboard[std::get<0>(MainChess->step[index])][std::get<1>(MainChess->step[index])] = std::get<2>(MainChess->step[index]);
        }
        MainChess->status = previous;
        index = -1;
        break;
    case MODEREP_BACK:
        if (MainChess->status != STATUS_REPLAY) return -1;
        if (index == 0) return 0;
        index--;
        MainChess->chessboard[std::get<0>(MainChess->step[index])][std::get<1>(MainChess->step[index])] = 0;
        break;
    case MODEREP_NEXT:
        if (MainChess->status != STATUS_REPLAY) return -1;
        if (index >= MainChess->step.size()) return MainChess->step.size() - 1;
        MainChess->chessboard[std::get<0>(MainChess->step[index])][std::get<1>(MainChess->step[index])] = std::get<2>(MainChess->step[index]);
        index++;
        break;
    }
    return index;
}

void Painter::RefreshBoard() {
    HDC hdc = GetDC(hWnd);
    RECT rect; GetClientRect(hWnd, &rect);
    PaintBoard(hdc, &rect);
    //
    ChangeBlankStatus();
    DrawBlank(hdc, BLANK_LEFT);
    DrawBlank(hdc, BLANK_RIGHT);
}

void Painter::GameOverAlert(UNIT_ID color) {
    if (color == ID_BLACK) {
        MessageBox(hWnd, Win_black, Win_title, MB_OK);
    }
    else {
        MessageBox(hWnd, Win_white, Win_title, MB_OK);
    }
}

void Painter::DrawBlank(HDC hdc, UNIT_STATUS flag) {
    WCHAR** cur;
    if (flag == BLANK_LEFT) cur = LeftBlankString;
    else cur = RightBlankString;
    HFONT hFont = CreateFont(30, 0, 0, 0, 900, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, L"黑体");
    SelectObject(hdc, hFont);
    //SetTextColor(hdc, RGB(255, 0, 0));
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    SelectObject(hdc, hPen);
    HBRUSH hBrh = CreateSolidBrush(RGB(255, 255, 255));
    SelectObject(hdc, hBrh);
    RECT rect; GetClientRect(hWnd, &rect);
    rect.top = 0;
    rect.left = flag == BLANK_LEFT ? 0 : vertex[1].x;
    rect.right = flag == BLANK_LEFT ? vertex[0].x : rect.right;
    Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
    for (int i = 0; i < (flag == BLANK_LEFT ? MAXN_LEFTBLANK : MAXN_RIGHTBLANK); ++i) {
        rect.top += rect.bottom / (MAXN_RIGHTBLANK + 1);
        int len = lstrlen(cur[i]);
        DrawText(hdc, cur[i], len, &rect, DT_CENTER);
    }
    DeleteObject(hFont);
    DeleteObject(hPen);
    DeleteObject(hBrh);
}

void Painter::InitBackground() {//背景图，左右栏
    //左栏2行显示对局者
    //右栏2行显示时间，状态
    RightBlankString = new WCHAR * [MAXN_RIGHTBLANK];
    LeftBlankString = new WCHAR * [MAXN_LEFTBLANK];
    for (int i = 0; i < MAXN_RIGHTBLANK; ++i) {
        RightBlankString[i] = new WCHAR();
    }
    for (int i = 0; i < MAXN_LEFTBLANK; ++i) {
        LeftBlankString[i] = new WCHAR();
    }
}

void Painter::ChangeBlank(UNIT_STATUS flag, const WCHAR* str, int index) {
    WCHAR** cur;
    if (flag == BLANK_LEFT) cur = LeftBlankString;
    else cur = RightBlankString;
    int len = lstrlen(str);
    delete[] cur[index];
    cur[index] = new WCHAR[len + 1];
    for (int i = 0; i < len; ++i) {
        cur[index][i] = str[i];
    }
    cur[index][len] = L'\0';
}

void Painter::ChangeBlankStatus() {
    if (MainChess->status == STATUS_GAMING)
        ChangeBlank(BLANK_RIGHT, L"正在对局", 8);
    else if (MainChess->status == STATUS_SUSPEND)
        ChangeBlank(BLANK_RIGHT, L"暂停", 8);
    else if (MainChess->status == STATUS_REPLAY) {
        ChangeBlank(BLANK_RIGHT, L"回放模式\n左右键，ESC", 8);
    }
    else if (MainChess->status == STATUS_END) {
        if (MainChess->winner == ID_BLACK)
            ChangeBlank(BLANK_RIGHT, L"黑胜", 8);
        else if (MainChess->winner == ID_WHITE)
            ChangeBlank(BLANK_RIGHT, L"白胜", 8);
    }
}

std::vector<Json> Painter::LoadHistory() {
    std::vector<std::string> ans;
    std::ifstream infile;
    infile.open("history.json");
    if (infile.is_open()){
        infile.seekg(0, infile.end);
        auto fos = infile.tellg();
        unsigned long long filesize = fos;
        char* tmp = new char[filesize]();
        infile.clear();
        infile.seekg(0L, std::ios::beg);
        infile.read(tmp, filesize);
        infile.close();
        std::string buffer(tmp);
        delete[] tmp;
        for (int i = buffer.size() - 1; i >= 0; --i)
            if (buffer[i] == '}') break; else buffer[i] = '\0';
        Json json(buffer);
        std::string vectorbuffer;
        json.Askkey("history", vectorbuffer);
        json.Translatevector(vectorbuffer, ans);
    }
    std::vector<Json> history;
    for (int i = 0; i < ans.size(); ++i) {
        history.push_back(Json(ans[i]));
    }
    return history;
}

void Painter::AddHistory(time_t endtime, UNIT_ID blackid, UNIT_ID whiteid, UNIT_ID winner) {
    std::vector<Json> history = LoadHistory();
    Json subjson;
    subjson.Addkey("endtime", endtime);
    subjson.Addkey("blackid", blackid);
    subjson.Addkey("whiteid", whiteid);
    subjson.Addkey("winner", winner);
    history.push_back(subjson);
    Json newjson;
    newjson.Addkey("history", history);
    std::ofstream outfile;
    outfile.open("history.json", std::ios::out | std::ios::trunc);
    outfile << newjson.Jsontostring();
    outfile.close();
}