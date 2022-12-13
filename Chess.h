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
typedef char UNIT_ID;

const UNIT_SIZE Directions[8][2] = { {0,1},{0,-1},{1,0},{-1,0},{1,1},{-1,-1},{1,-1},{-1,1} };

class Chess :public Board {
public:
	UNIT_STATUS status;
	UNIT_ID blackid, whiteid, winner;
	time_t begintime, endtime, timelimit;
	std::vector<std::tuple<UNIT_SIZE, UNIT_SIZE, UNIT_ID>> step;

	Chess(UNIT_ID, UNIT_ID, UNIT_SIZE);
	~Chess();
	UNIT_ID CheckEnd();//����Ƿ����������ʤ����ID
	UNIT_ID CheckEndLatest();//������һ�������Ƿ����һ��ʤ��������ʤ����ID
};