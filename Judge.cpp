#include "Judge.h"
#include "AI.h"

std::queue<JudgeMessage> MessageQueue;

DWORD WINAPI Judgeproc(LPVOID param) {
    Chess* MainChess = ((Judgeparam*)param)->MainChess;
    Painter* MainPainter = ((Judgeparam*)param)->painter;
    clock_t start = clock(); 
    UNIT_ID color = PIECE_BLACK;
    while (!MessageQueue.empty()) MessageQueue.pop();//清空消息队列
    //读取文件
    if (MainChess->step.size()) color = std::get<2>(MainChess->step.back()) == PIECE_BLACK ? PIECE_WHITE : PIECE_BLACK;
    AI* ai = new AI(MainChess->step, MainChess->size);
    while (true) {
        if (clock() - start > (unsigned long long)MainChess->timelimit) {
            //超时
        }

        if (MainChess->status == STATUS_GAMING && //该AI下，发个消息，不知道会不会冲突
        (color == PIECE_BLACK ? MainChess->blackid : MainChess->whiteid) == ID_AI)
            MessageQueue.emplace(JudgeMessage(JUDGEMSG_SET));

        if (MessageQueue.empty()) continue;
        JudgeMessage msg = MessageQueue.front(); MessageQueue.pop();
        switch (msg.msg) {
        case JUDGEMSG_SET:
            if (MainChess->status == STATUS_GAMING) {
                //判断当前该人下还是AI下
                if ((color == PIECE_BLACK ? MainChess->blackid : MainChess->whiteid) == ID_HUMAN &&
                MainChess->chessboard[msg.x][msg.y] == 0) {
                    MainChess->chessboard[msg.x][msg.y] = color;
                    MainChess->step.push_back(std::make_tuple(msg.x, msg.y, color));
                    ai->SetChess(ai->SearchNode(msg.x, msg.y, color));
                }
                else if((color == PIECE_BLACK ? MainChess->blackid : MainChess->whiteid) == ID_AI){
                    Gamenode* next = ai->Calculate(0, 15, color);
                    MainChess->chessboard[next->x][next->y] = color;
                    MainChess->step.push_back(std::make_tuple(next->x, next->y, color));
                    ai->SetChess(next);
                    //MessageBox(0, 0, 0, 0);
                }
                UNIT_ID winner = MainChess->CheckEndLatest();
                //刷新
                color = color == PIECE_BLACK ? PIECE_WHITE : PIECE_BLACK;
                start = clock();
                //胜利宣言
                if (winner != ID_NULL) {
                    MainChess->endtime = time(nullptr);
                    MainChess->winner = winner;
                    MainChess->status = STATUS_END;
                    MainPainter->AddHistory(MainChess->endtime, MainChess->blackid, MainChess->whiteid, MainChess->winner);
                    MainPainter->GameOverAlert(winner);
                }
                WCHAR buffer[20];
                wsprintfW(buffer, L"第%d手\nnext: %S棋", MainChess->step.size(), color == PIECE_BLACK ? "黑" : "白");
                MainPainter->ChangeBlank(BLANK_RIGHT, buffer, 5);
                MainPainter->RefreshBoard();
            }
            break;
        case JUDGEMSG_REGRET:
            if (MainChess->step.size() > 1 && MainChess->ifregret == true) {
                if ((color == PIECE_BLACK ? MainChess->blackid : MainChess->whiteid) == ID_HUMAN) {
                    for (int i = 0; i < 2; ++i) {
                        MainChess->chessboard[std::get<0>(MainChess->step.back())][std::get<1>(MainChess->step.back())] = 0;
                        MainChess->step.pop_back();
                    }
                }
                //刷新
                MainPainter->RefreshBoard();
            }
            break;
        case JUDGEMSG_SUSPEND_RESTART:
            if (MainChess->status == STATUS_GAMING) MainChess->status = STATUS_SUSPEND;
            else if (MainChess->status == STATUS_SUSPEND) MainChess->status = STATUS_GAMING;
            MainPainter->RefreshBoard();
            break;
        case JUDGEMSG_GIVEIN:
            MainChess->endtime = time(nullptr);
            MainChess->winner = color == PIECE_BLACK ? ID_WHITE : ID_BLACK;
            MainChess->status = STATUS_END;
            MainPainter->AddHistory(MainChess->endtime, MainChess->blackid, MainChess->whiteid, MainChess->winner);
            MainPainter->GameOverAlert(MainChess->winner);
            MainPainter->RefreshBoard();
            break;
        case JUDGEMSG_EXIT:
            return 0;
        }
    }
    return 0;
}