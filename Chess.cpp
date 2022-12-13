#include "Chess.h"

Chess::Chess(UNIT_ID blackid, UNIT_ID whiteid, UNIT_SIZE size):blackid(blackid), whiteid(whiteid) {
	this->size = size;
	chessboard = new UNIT_BOARD * [size];
	for (int i = 0; i < size; ++i) {
		chessboard[i] = new UNIT_BOARD[size]();
	}
	begintime = time(nullptr);
	endtime = -1;
	timelimit = -1;
	winner = -1;
	status = STATUS_GAMING;
}

Chess::~Chess(){
	for (int i = 0; i < size; ++i) {
		delete[] chessboard[i];
	}
	delete[] chessboard;
}

UNIT_ID Chess::CheckEnd() {
    int wwin = 0, bwin = 0;
    auto win = [](auto& a, auto& b, auto& c) {a = 0; b++; if (b == 5) b = 0, c++; };
    auto func = [&](const auto def1, const auto def2) {
        int w1, w2, b1, b2;
        for (int i = 0; i < size; ++i) {
            w1 = 0, w2 = 0, b1 = 0, b2 = 0;
            for (int j = 0; j < size; ++j) {
                if (chessboard[i][j] == PIECE_BLACK || chessboard[i][j] == def1) win(w1, b1, bwin);
                else if (chessboard[i][j] == PIECE_WHITE || chessboard[i][j] == def2) win(b1, w1, wwin);
                else w1 = b1 = 0;
                if (chessboard[j][i] == PIECE_BLACK || chessboard[j][i] == def1) win(w2, b2, bwin);
                else if (chessboard[j][i] == PIECE_WHITE || chessboard[j][i] == def2) win(b2, w2, wwin);
                else w2 = b2 = 0;
            }
        }
        for (int i = 4; i < size; ++i) {
            w1 = 0, w2 = 0, b1 = 0, b2 = 0;
            for (int j = i; j >= 0; --j) {
                if (chessboard[j][i - j] == PIECE_BLACK || chessboard[j][i - j] == def1) win(w1, b1, bwin);
                else if (chessboard[j][i - j] == PIECE_WHITE || chessboard[j][i - j] == def2) win(b1, w1, wwin);
                else w1 = b1 = 0;
                if (chessboard[i - j][size - j - 1] == PIECE_BLACK || chessboard[i - j][size - j - 1] == def1) win(w2, b2, bwin);
                else if (chessboard[i - j][size - j - 1] == PIECE_WHITE || chessboard[i - j][size - j - 1] == def2) win(b2, w2, wwin);
                else w2 = b2 = 0;
            }
        }
        for (int i = 1; i < size - 4; ++i) {
            w1 = 0, w2 = 0, b1 = 0, b2 = 0;
            for (int j = size - 1; j >= i; --j) {
                if (chessboard[j][size - j + i - 1] == PIECE_BLACK || chessboard[j][size - j + i - 1] == def1) win(w1, b1, bwin);
                else if (chessboard[j][size - j + i - 1] == PIECE_WHITE || chessboard[j][size - j + i - 1] == def2) win(b1, w1, wwin);
                else w1 = b1 = 0;
                if (chessboard[j][j - i] == PIECE_BLACK || chessboard[j][j - i] == def1) win(w2, b2, bwin);
                else if (chessboard[j][j - i] == PIECE_WHITE || chessboard[j][j - i] == def2) win(b2, w2, wwin);
                else w2 = b2 = 0;
            }
        }
    };
    func(-1, -1);
    if (wwin != 0 && bwin != 0) return ID_NULL;
    else if (wwin == 0 && bwin != 0) return ID_BLACK;
    else if (wwin != 0 && bwin == 0) return ID_WHITE;
    else {
        func(0, -1);
        func(-1, 0);
        if (wwin == 0 && bwin == 0) return ID_NULL;
    }
}

UNIT_ID Chess::CheckEndLatest() {
    if (step.size() == 0) return ID_NULL;
    UNIT_SIZE x = std::get<0>(step.back()), y = std::get<1>(step.back());
    UNIT_ID color = std::get<2>(step.back());
    UNIT_SIZE cnt;
    for (int i = 0; i < 8; ++i) {
        if ((i & 1) == 0) cnt = 1;
        UNIT_SIZE nx = x, ny = y;
        for (int j = 1; j < 5; ++j) {
            nx += Directions[i][0], ny += Directions[i][1];
            if (nx >= 0 && nx < size && ny >= 0 && ny < size && chessboard[nx][ny] == color) cnt++;
            else break;
        }
        if (cnt >= 5) {
            if (color == PIECE_BLACK) return ID_BLACK;
            else return ID_WHITE;
        }
    }
    return ID_NULL;
}