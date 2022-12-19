#include "framework.h"
#include "AI.h"

Trie::Trie() {
	exist = -1;
	for (int i = 0; i < 3; ++i) next[i] = nullptr;
	fail = nullptr;
}

void Trie::insert(const char* str, int pos, int id) {
	if (pos == strlen(str)) {
		exist = id; return;
	}
	if (next[str[pos] - '0'] == nullptr)
		next[str[pos] - '0'] = new Trie;
	next[str[pos] - '0']->insert(str, pos + 1, id);
}

void Trie::getfail() {
	this->fail = nullptr;
	std::queue<Trie*> Q;
	Q.emplace(this);
	while (!Q.empty()) {
		Trie* tmp = Q.front(); Q.pop();
		for (int i = 0; i < 3; ++i) {
			if (tmp->next[i]) {
				Q.emplace(tmp->next[i]);
				Trie* fafail = tmp->fail;
				while (fafail && fafail->next[i] == nullptr) fafail = fafail->fail;
				if (fafail == nullptr) tmp->next[i]->fail = this;
				else tmp->next[i]->fail = fafail->next[i];
			}
		}
	}
}

AI::AI(std::vector<std::tuple<UNIT_SIZE, UNIT_SIZE, UNIT_ID>> step, UNIT_SIZE size) {
	//初始化棋盘
	this->size = size;
	chessboard = new UNIT_BOARD * [size];
	for (int i = 0; i < size; ++i) {
		chessboard[i] = new UNIT_BOARD[size];
		memset(chessboard[i], '0', size);
	}
	//初始化博弈树
	gametree = new Gamenode(nullptr,-1,-1,PIECE_WHITE,0);//空棋盘
	for (int i = 0; i < step.size(); ++i) {
		//空0黑1白2
		UNIT_SIZE x = std::get<0>(step[i]), y = std::get<1>(step[i]);
		UNIT_ID color = std::get<2>(step[i]);
		chessboard[x][y] = color==PIECE_WHITE ? '2' : '1';
		gametree = new Gamenode(gametree, x, y, color, 0);
	}
	//初始化AC自动机
	match[0] = new Trie, match[1] = new Trie;
	for (int i = 0; i < PATTERNMAX; ++i) match[0]->insert(patternsA[i], 0, i), match[1]->insert(patternsB[i], 0, i);
	match[0]->getfail(); match[1]->getfail();
}

inline void AI::Query(const char* str, Trie* head, char len, char* nums) {//匹配AC自动机
	for (int i = 0; i < len; ++i) {
		char c = str[i] - '0';
		while (head->next[c] == NULL && head->fail) head = head->fail;
		if (head->next[c]) head = head->next[c];
		else continue;
		if (head->exist != -1) nums[head->exist]++;
	}
}

int AI::Estimate(char color) {
	int ans = 0;
	char buffer[25], nums[PATTERNMAX];
	memset(nums, 0, sizeof(nums));
	for (int i = 0; i < size; ++i)
		Query(chessboard[i], match[0], size, nums);
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			buffer[j] = chessboard[j][i];
		}
		Query(buffer, match[0], size, nums);
	}

	{//? 
		for (int i = 0; i < size; ++i) {
			for (int j = i; j >= 0; --j) {
				buffer[j] = chessboard[j][i - j];
			}
			Query(buffer, match[0], i + 1, nums);
		}
		for (int i = 0; i < size; ++i) {
			for (int j = i; j >= 0; --j) {
				buffer[j] = chessboard[j][size - i + j - 1];
			}
			Query(buffer, match[0], i + 1, nums);
		}
		for (int i = 1; i < size; ++i) {
			for (int j = size - 1; j >= i; --j) {
				buffer[j - i] = chessboard[j][size - j + i - 1];
			}
			Query(buffer, match[0], size - i, nums);
		}
		for (int i = 1; i < size; ++i) {
			for (int j = size - 1; j >= i; --j) {
				buffer[j - i] = chessboard[j][j - i];
			}
			Query(buffer, match[0], size - i, nums);
		}
	}
	double alpha = 1;
	if (color == 0) alpha = 1.2;
	for (int i = 0; i < PATTERNMAX; ++i) {
		ans += nums[i] * weight[i] * alpha;
	}
	//?? 
	memset(nums, 0, sizeof(nums));
	for (int i = 0; i < size; ++i)
		Query(chessboard[i], match[1], size, nums);
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			buffer[j] = chessboard[j][i];
		}
		Query(buffer, match[1], size, nums);
	}

	{//? 
		for (int i = 0; i < size; ++i) {
			for (int j = i; j >= 0; --j) {
				buffer[j] = chessboard[j][i - j];
			}
			Query(buffer, match[1], i + 1, nums);
		}
		for (int i = 0; i < size; ++i) {
			for (int j = i; j >= 0; --j) {
				buffer[j] = chessboard[j][size - i + j - 1];
			}
			Query(buffer, match[1], i + 1, nums);
		}
		for (int i = 1; i < size; ++i) {
			for (int j = size - 1; j >= i; --j) {
				buffer[j - i] = chessboard[j][size - j + i - 1];
			}
			Query(buffer, match[1], size - i, nums);
		}
		for (int i = 1; i < size; ++i) {
			for (int j = size - 1; j >= i; --j) {
				buffer[j - i] = chessboard[j][j - i];
			}
			Query(buffer, match[1], size - i, nums);
		}
	}
	alpha = 1;//激进
	if (color == 1) alpha = 1.2;
	for (int i = 0; i < PATTERNMAX; ++i) {
		ans -= nums[i] * weight[i] * alpha;
	}
	return ans;
}

void Gamenode_BST::insert(Gamenode* node) {
	int d = node->score > current->score;
	if (son[d] != nullptr) son[d]->insert(node);
	else son[d] = new Gamenode_BST(node, this);
}

Gamenode* AI::Calculate(UNIT_SIZE layer, UNIT_SIZE maxlayer, UNIT_ID color) {//计算本节点的分数，更新父节点分数
	if (layer == maxlayer) {
		gametree->score = Estimate(color); return nullptr;
	}
	char** v = new char* [size];//考虑static，memset替换
	for (int i = 0; i < size; ++i) v[i] = new char[size]();//是否访问
	if (gametree->calclayer == 0) {//如果没算过
		for (int i = 0; i < size; ++i) {
			for (int j = 0; j < size; ++j) {
				if (chessboard[i][j] != '0') {
					for (const auto& dir : Directions) {
						UNIT_SIZE nx = i + dir[0], ny = j + dir[1];
						if (nx >= 0 && nx < 15 && ny >= 0 && ny < 15 && v[nx][ny] == 0 && chessboard[nx][ny] == '0') {
							v[nx][ny] = 1;
							chessboard[nx][ny] = color == PIECE_BLACK ? '1' : '2';//浅尝辄止，为了初次估价
							Gamenode* newnode = new Gamenode(gametree, nx, ny, color, Estimate(color));//生成下一层
							chessboard[nx][ny] = '0';
							gametree->insert_BST(newnode);//插入BST
							if (newnode->score > 75000000 || newnode->score < -75000000) {
								gametree->score = newnode->score; return newnode;//不算了，走newnode就赢了
							}
						}
					}
				}
			}
		}
	}
	for (int i = 0; i < size; ++i) delete[] v[i];
	delete[] v;
	if (gametree->head == nullptr) {//未落子状态，全盘均落子状态应该由Judge处理
		chessboard[size / 2][size / 2] = color == PIECE_BLACK ? '1' : '2';
		Gamenode* newnode = new Gamenode(gametree, size / 2, size / 2, color, Estimate(color));
		chessboard[size / 2][size / 2] = '0';
		return newnode;
	}
	while (true) {
		//color是黑，BST里下的都是黑棋，应该找最大的，也就是最右的，反之最左的
		//找到BST最右/左节点，计算更新calclayer，删除重新插入
		//再算最右/左，直到最右/左的layer是maxlayer
		int d = color == PIECE_BLACK;//黑1白0
		Gamenode_BST* t = gametree->head;
		while (t->son[d] != nullptr) t = t->son[d];
		Gamenode* tnode = t->current;
		if (tnode->calclayer != maxlayer) {
			//先转移到下一个node计算，再返回本node
			UNIT_SIZE x = tnode->x, y = tnode->y;
			chessboard[x][y] = color == PIECE_BLACK ? '1' : '2';
			gametree = t->current;
			Calculate(layer + 1, maxlayer, color == PIECE_BLACK ? PIECE_WHITE : PIECE_BLACK);
			chessboard[x][y] = '0';
			gametree = gametree->father;
			//更新
			tnode->calclayer = maxlayer - layer;
			//删除
			if (t->father == nullptr) gametree->head = t->son[d ^ 1];//t是gametree->head
			else t->father->son[d] = t->son[d ^ 1];
			delete t;
			//插入
			gametree->insert_BST(tnode);
		}
		else {//更新gamenode的父节点
			gametree->father->score = tnode->score; 
			if (layer == 0) return tnode;
			break;
		}
	}
	return nullptr;
}

/*
Gamenode* AI::Calculate(int layer, Gamenode* head) {
	if (head == nullptr) head = gametree;
	std::priority_queue<Gamenode*, std::vector<Gamenode*>, cmp> next;//因为vs2022报错，不能将优先队列放在结构体里
	char iswhite = head->color;
	if (layer == 1) {
		head->score = Estimate(iswhite); return head;
	}
	char** v = new char* [size];
	for (int i = 0; i < size; ++i) v[i] = new char[size]();
	for (int i = 0; i < 15; ++i) {
		for (int j = 0; j < 15; ++j) {
			if (chessboard[i][j] != '0') {
				for (const auto& dir : Directions) {
					int nx = i + dir[0], ny = j + dir[1];
					if (nx >= 0 && nx < 15 && ny >= 0 && ny < 15 && v[nx][ny] == 0 && chessboard[nx][ny] == '0') {
						v[nx][ny] = 1;
						chessboard[nx][ny] = (iswhite ^ 1) + '1';
						Gamenode* newnode = new Gamenode;
						newnode->x = nx, newnode->y = ny, newnode->score = Estimate(iswhite ^ 1);
						newnode->father = head, newnode->color = iswhite ^ 1;
						chessboard[nx][ny] = '0';
						if (newnode->score > 80000000 || newnode->score < -80000000) {
							head->score = newnode->score; return head;
						}
						next.emplace(newnode);
					}
				}
			}
		}
	}
	for (int i = 0; i < size; ++i) delete[] v[i];
	delete[] v;
	if (next.size() == 0) {
		if (head->x == -1) {
			head->x = size / 2, head->y = size / 2;
		}
		return head;
	}
	Gamenode* ans;
	while (1) {
		int last;
		Gamenode* tmp = next.top(); next.pop();
		char nx = tmp->x, ny = tmp->y;
		chessboard[nx][ny] = (iswhite ^ 1) + '1';
		Calculate(layer + 1, tmp);
		chessboard[nx][ny] = '0';
		last = tmp->score;
		next.emplace(tmp);
		if (next.top()->score == last) {
			ans = next.top(); next.pop();
			while (!next.empty()) {
				delete next.top(); next.pop();
			}
			head->score = last; break;
		}
	}
	if (layer == 0) {
		return ans;
	}
	return head;
}*/

void AI::SetChess(Gamenode* next) {
	chessboard[next->x][next->y] = next->color == PIECE_BLACK ? '1' : '2';
	gametree = next;
}

Gamenode* AI::SearchNode(UNIT_SIZE x, UNIT_SIZE y, UNIT_ID color) {//BFS
	std::queue<Gamenode_BST*> Q;
	if(gametree->head != nullptr) Q.emplace(gametree->head);
	while (!Q.empty()) {
		Gamenode_BST* t = Q.front(); Q.pop();
		if (t->current->x == x && t->current->y == y && t->current->color == color) return t->current;
		if (t->son[0]) Q.emplace(t->son[0]);
		if (t->son[1]) Q.emplace(t->son[1]);
	}
	chessboard[x][y] = color == PIECE_BLACK ? '1' : '2';
	Gamenode* newnode = new Gamenode(gametree, x, y, color, Estimate(color));
	chessboard[x][y] = '0';
	gametree->insert_BST(newnode);
	return newnode;
}