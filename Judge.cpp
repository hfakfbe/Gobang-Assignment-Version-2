#include "Judge.h"
#include "AI.h"

std::queue<JudgeMessage> MessageQueue;

DWORD WINAPI Judgeproc(LPVOID param) {
    Chess* MainChess = ((Judgeparam*)param)->MainChess;
    Painter* MainPainter = ((Judgeparam*)param)->painter;
    clock_t start = clock(); 
    UNIT_ID color = PIECE_BLACK;
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
                    Gamenode* next = ai->Calculate(0, 6, color);
                    MainChess->chessboard[next->x][next->y] = color;
                    MainChess->step.push_back(std::make_tuple(next->x, next->y, color));
                    ai->SetChess(next);
                }
                UNIT_ID winner = MainChess->CheckEndLatest();
                //刷新
                color = color == PIECE_BLACK ? PIECE_WHITE : PIECE_BLACK;
                start = clock();
                MainPainter->RefreshBoard();
                //胜利宣言
                if (winner != ID_NULL) {
                    MainChess->winner = winner;
                    MainChess->status = STATUS_END;
                    MainPainter->GameOverAlert(winner);
                }
            }
            break;
        case JUDGEMSG_REGRET:
            if (MainChess->step.size() && MainChess->ifregret == true) {
                MainChess->chessboard[std::get<0>(MainChess->step.back())][std::get<1>(MainChess->step.back())] = 0;
                MainChess->step.pop_back();
                color = color == PIECE_BLACK ? PIECE_WHITE : PIECE_BLACK;
                //刷新
                MainPainter->RefreshBoard();
            }
            break;
        case JUDGEMSG_SUSPEND_RESTART:
            if (MainChess->status == STATUS_GAMING) MainChess->status = STATUS_SUSPEND;
            else if (MainChess->status == STATUS_SUSPEND) MainChess->status = STATUS_GAMING;
            break;
        case JUDGEMSG_GIVEIN:
            MainChess->winner = color == PIECE_BLACK ? ID_WHITE : ID_BLACK;
            MainChess->status = STATUS_END;
            MainPainter->GameOverAlert(MainChess->winner);
            break;
        case JUDGEMSG_EXIT:
            return 0;
        }
    }
    return 0;
}