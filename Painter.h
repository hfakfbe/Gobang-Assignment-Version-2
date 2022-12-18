#pragma once
#include "framework.h"
#include "Chess.h"
#include "Judge.h"
#include "Myerror.h"

#define MOUSE_SET		51
#define MOUSE_HOVER		52
#define REQUEST_REGRET	121
#define REQUEST_SUSRES	122
#define REQUEST_ANALYS	123
#define REQUEST_REPLAY	124
#define REQUEST_GIVEIN	125

#define MODEMSG_SET		171
#define MODEMSG_SUSRES	172
#define MODEMSG_REGRET	173
#define MODEMSG_GIVEIN	174

#define ID_HUMAN		181
#define ID_AI			182

DWORD WINAPI Judgeproc(LPVOID);

typedef int UNIT_INTERFACE;
typedef float UNIT_GRIDLENGTH;

class Painter {
private:
	Chess* MainChess;
	HANDLE hJudge;
	HWND hWnd;

	UNIT_INTERFACE clientborder, boardlength, leftborder, gridborder, chessradius;//界面参数
	UNIT_GRIDLENGTH gridlength;
	POINT vertex[4];//棋盘四个顶点
	UNIT_SIZE hover[2][2];//鼠标悬停
	COLORREF boardcolor;//背景颜色

	void Mode_Gaming(UNIT_STATUS, UNIT_SIZE, UNIT_SIZE);
	void Mode_Replay(){}
	void Mode_Analysis(){}

public:
	Painter(HWND, UNIT_ID, UNIT_ID, UNIT_SIZE, time_t, bool);
	Painter(const WCHAR*, HWND);
	~Painter();

	void PaintBoard(HDC, RECT*);//画棋盘
	void DrawChess(HDC, UNIT_STATUS, COLORREF);//画所有指定颜色棋子
	void DrawAChess(HDC, UNIT_INTERFACE, UNIT_INTERFACE);//画一个棋子
	void DrawAChess(UNIT_SIZE, UNIT_SIZE, UNIT_ID);//废函数
	void PaintHover();//绘制鼠标悬停棋子显示
	void RefreshBoard();
	
	void MouseOperation(UNIT_INTERFACE, UNIT_INTERFACE, UNIT_STATUS);
	void SaveToJson(const WCHAR*);

	void RequestMode(UNIT_STATUS);
	void GameOverAlert(UNIT_ID);

	inline HWND GetHwnd() { return hWnd; }//在对话框中创建Painter对象需要用
};

class Judgeparam {
public:
	Chess* MainChess;
	Painter* painter;
	Judgeparam(Chess* chess, Painter* painter) :MainChess(chess), painter(painter) {}
};