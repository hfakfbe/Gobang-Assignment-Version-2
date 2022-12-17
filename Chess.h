#pragma once
#include "Board.h"
#include "framework.h"

#define PIECE_BLACK		'1'
#define PIECE_WHITE		'2'

#define STATUS_GAMING	1
#define STATUS_SUSPEND	2
#define STATUS_END		3
#define STATUS_REPLAY	4

#define ID_BLACK	1
#define ID_WHITE	2
#define ID_NULL		0

typedef int UNIT_STATUS;
typedef short UNIT_ID;

const UNIT_SIZE Directions[8][2] = { {0,1},{0,-1},{1,0},{-1,0},{1,1},{-1,-1},{1,-1},{-1,1} };

class Chess :public Board {
public:
	UNIT_STATUS status;
	UNIT_ID blackid, whiteid, winner;
	time_t begintime, endtime, timelimit;
	std::vector<std::tuple<UNIT_SIZE, UNIT_SIZE, UNIT_ID>> step;
	bool ifregret;

	Chess(UNIT_ID, UNIT_ID, UNIT_SIZE, time_t, bool);
	~Chess();
	UNIT_ID CheckEnd();//检查是否结束，返回胜利者ID
	UNIT_ID CheckEndLatest();//检查最后一颗棋子是否造成一方胜利，返回胜利者ID
};