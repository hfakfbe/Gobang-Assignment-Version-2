#pragma once

#include "framework.h"
#include "Chess.h"

#define PATTERNMAX		13
#define HUMAN			0
#define AI_PLAYER		1
#define AI_BOARD_MAXN	3

class Gamenode;

class Gamenode_BST {
public:
	Gamenode_BST* son[2], *father;
	Gamenode* current;
	Gamenode_BST(Gamenode* head, Gamenode_BST* father):father(father) {
		current = head;
		son[0] = son[1] = nullptr;
	}
	void insert(Gamenode*);
};

class Gamenode {
public:
	Gamenode* father;
	Gamenode_BST* head;//sons

	UNIT_SIZE x, y, calclayer;
	UNIT_ID color;
	int score;
	Gamenode(Gamenode* father, UNIT_SIZE x, UNIT_SIZE y, UNIT_ID color, int score):
		father(father), x(x),y(y),color(color),score(score){
		calclayer = 0;
		head = nullptr;
	}
	void insert_BST(Gamenode* node) {
		if (head == nullptr) head = new Gamenode_BST(node, nullptr);
		else head->insert(node);
	}
	void rotate_BST(Gamenode_BST* &node, int d) {
		Gamenode_BST* t = node->son[d ^ 1];
		node->son[d ^ 1] = t->son[d];
		t->son[d] = node;
		node = t;
	}
};

/*
struct cmp {
	bool operator()(Gamenode*& a, Gamenode*& b) {
		if (a->color == 0) return a->score < b->score;
		return a->score > b->score;
	}
};*/

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
	//const
	const char* patternsA[PATTERNMAX] = { "11111","011110","211110","011112","01110","21110","01112",
		"2110","0112","0110","010","210","012" };
	const char* patternsB[PATTERNMAX] = { "22222","022220","122220","022221","02220","12220","02221",
		"1220","0221","0220","020","120","021" };
	const int weight[PATTERNMAX] = { 100000000,1000000,3000,3000,2000,250,250,20,20,100,10,1,1 };
public:
	AI(std::vector<std::tuple<UNIT_SIZE, UNIT_SIZE, UNIT_ID>>, UNIT_SIZE);
	int Estimate(char);//对当前局面估值，对行、列、斜用Query
	//计算本节点的分数，更新父节点分数,最后返回计算的最佳手段，否则返回nullptr
	Gamenode* Calculate(UNIT_SIZE, UNIT_SIZE, UNIT_ID);
	void Query(const char*, Trie*, char, char*);//利用AC自动机计算某行棋子的匹配数量
	void SetChess(Gamenode*);
	Gamenode* SearchNode(UNIT_SIZE, UNIT_SIZE, UNIT_ID);
};
