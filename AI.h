#pragma once

#include "framework.h"
#include "Chess.h"

#define PATTERNMAX	13
#define HUMAN		0
#define AI_PLAYER	1
#define AI_BOARD_MAXN 3

class Gamenode {
public:
	Gamenode* father;
	char x, y, color;
	int score;
};

struct cmp {
	bool operator()(Gamenode*& a, Gamenode*& b) {
		if (a->color == 0) return a->score < b->score;
		return a->score > b->score;
	}
};

class Trie {
public:
	Trie* next[AI_BOARD_MAXN], * fail;
	char exist;
	Trie();
	void insert(const char*, int, int);
	void getfail();
};

class AI : public Board {
private:
	Gamenode* gametree;
	Trie* match[2];
	const char* patternsA[PATTERNMAX] = { "11111","011110","211110","011112","01110","21110","01112",
		"2110","0112","0110","010","210","012" };
	const char* patternsB[PATTERNMAX] = { "22222","022220","122220","022221","02220","12220","02221",
		"1220","0221","0220","020","120","021" };
	const int weight[PATTERNMAX] = { 100000000,1000000,3000,3000,2000,250,250,20,20,100,10,1,1 };
public:
	AI(std::vector<POINT>, int);
	int Estimate(char);//对当前局面估值，对行、列、斜用Query
	Gamenode* Calculate(int, Gamenode*);//计算下一手
	void Query(const char*, Trie*, char, char*);//利用AC自动机计算某行棋子的匹配数量
	void SetChess(int, int, int);
	//和Chess类的接口
};
