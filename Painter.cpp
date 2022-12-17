#include "Painter.h"
#include "Json.h"

const WCHAR* Win_black = L"Black Win!";
const WCHAR* Win_white = L"White Win!";
const WCHAR* Win_title = L"Game Over!";

Painter::Painter(HWND hWnd, UNIT_ID blackid, UNIT_ID whiteid, UNIT_SIZE size, time_t timelimit, bool ifregret) :hWnd(hWnd) {
    MainChess = new Chess(blackid, whiteid, size, timelimit, ifregret);
    Judgeparam* param = new Judgeparam(MainChess, this);
    hJudge = CreateThread(NULL, 0, Judgeproc, (void*)param, 0, NULL);
    boardcolor = RGB(255, 227, 132);
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
    switch (flag) {
    case REQUEST_REGRET:
        Mode_Gaming(MODEMSG_REGRET, -1, -1);
        break;
    case REQUEST_SUSRES:
        Mode_Gaming(MODEMSG_SUSRES, -1, -1);
        break;
    case REQUEST_REPLAY:
        break;
    case REQUEST_ANALYS:
        break;
    case REQUEST_GIVEIN:
        Mode_Gaming(MODEMSG_GIVEIN, -1, -1);
        break;
    }
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

void Painter::RefreshBoard() {
    HDC hdc = GetDC(hWnd);
    RECT rect; GetClientRect(hWnd, &rect);
    PaintBoard(hdc, &rect);
}

void Painter::GameOverAlert(UNIT_ID color) {
    if (color == ID_BLACK) {
        MessageBox(hWnd, Win_black, Win_title, MB_OK);
    }
    else {
        MessageBox(hWnd, Win_white, Win_title, MB_OK);
    }
}