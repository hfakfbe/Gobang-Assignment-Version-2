#include "Judge.h"

std::queue<JudgeMessage> MessageQueue;

DWORD WINAPI Judgeproc(LPVOID param) {
    Chess* MainChess = ((Judgeparam*)param)->MainChess;
    Painter* MainPainter = ((Judgeparam*)param)->painter;
    clock_t start = clock();
    UNIT_ID color = PIECE_BLACK;
    while (1) {
        if (clock() - start > (unsigned long long)MainChess->timelimit) {
            //��ʱ
        }

        if (MessageQueue.empty()) continue;
        JudgeMessage msg = MessageQueue.front(); MessageQueue.pop();

        switch (msg.msg) {
        case JUDGEMSG_SET:
            if (MainChess->status == STATUS_GAMING) {
                MainChess->chessboard[msg.x][msg.y] = color;
                MainChess->step.push_back(std::make_tuple(msg.x, msg.y, color));
                UNIT_ID winner = MainChess->CheckEndLatest();
                //ˢ��
                color = color == PIECE_BLACK ? PIECE_WHITE : PIECE_BLACK;
                MainPainter->RefreshBoard();
                //ʤ������
                if (winner != ID_NULL) {
                    MainChess->winner = winner;
                    MainChess->status = STATUS_END;
                    MainPainter->GameOverAlert(winner);
                }
            }
            break;
        case JUDGEMSG_REGRET:
            if (MainChess->step.size()) {
                MainChess->chessboard[std::get<0>(MainChess->step.back())][std::get<1>(MainChess->step.back())] = 0;
                MainChess->step.pop_back();
                //ˢ��
                MainPainter->RefreshBoard();
            }
            break;
        case JUDGEMSG_SUSPEND_RESTART:
            if (MainChess->status == STATUS_GAMING) MainChess->status = STATUS_SUSPEND;
            else if (MainChess->status == STATUS_SUSPEND) MainChess->status = STATUS_GAMING;
            break;
        }
    }
    return 0;
}