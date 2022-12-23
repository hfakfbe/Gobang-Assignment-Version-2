#pragma once

#include "framework.h"
#include "Chess.h"

#define PATTERNMAX		21
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
	const char* patternsA[PATTERNMAX] = { "11111","011110","211110","011112","11011","11101","10111","010110","011010",
		"011100","001110","211100","001112","211000","000112","001100","011000","001000","000100","210000","000012"};
	const char* patternsB[PATTERNMAX] = { "22222","022220","122220","022221","22022","22202","20222","020220","022020",
		"022200","002220","122200","002221","122000","000221","002200","022000","002000","000200","120000","000021"};
	const int weight[PATTERNMAX] = { 100000000,1000000,3000,3000,1000,1000,1000,1500,1500,1500,1500,250,250,20,20,50,40,5,5,1,1 };
public:
	AI(std::vector<std::tuple<UNIT_SIZE, UNIT_SIZE, UNIT_ID>>, UNIT_SIZE);
	int Estimate(char);//对当前局面估值，对行、列、斜用Query
	//计算本节点的分数，更新父节点分数,最后返回计算的最佳手段，否则返回nullptr
	Gamenode* Calculate(UNIT_SIZE, UNIT_SIZE, UNIT_ID);
	void Query(const char*, Trie*, char, char*);//利用AC自动机计算某行棋子的匹配数量
	void SetChess(Gamenode*);
	Gamenode* SearchNode(UNIT_SIZE, UNIT_SIZE, UNIT_ID);
};
