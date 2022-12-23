#pragma once
#include "framework.h"
#include "Chess.h"
#include "Judge.h"
#include "Myerror.h"
#include "Json.h"

#define MOUSE_SET		51
#define MOUSE_HOVER		52
#define REQUEST_REGRET	121
#define REQUEST_SUSRES	122
#define REQUEST_ANALYS	123
#define REQUEST_REPLAY	124
#define REQUEST_GIVEIN	125

#define REQUEST_REPLAY	126
#define REQUEST_REPEND	127
#define REQUEST_REPNEXT	128
#define REQUEST_REPBACK	129

#define MODEMSG_SET		171
#define MODEMSG_SUSRES	172
#define MODEMSG_REGRET	173
#define MODEMSG_GIVEIN	174

#define MODEREP_START	191
#define MODEREP_END		192
#define MODEREP_NEXT	193
#define MODEREP_BACK	194

#define ID_HUMAN		181
#define ID_AI			182

#define MAXN_RIGHTBLANK 10
#define MAXN_LEFTBLANK  10
#define BLANK_LEFT      1
#define BLANK_RIGHT     2

DWORD WINAPI Judgeproc(LPVOID);

typedef int UNIT_INTERFACE;
typedef float UNIT_GRIDLENGTH;

extern WCHAR** RightBlankString;
extern WCHAR** LeftBlankString;

class Painter {
private:
	Chess* MainChess;
	HANDLE hJudge;
	HWND hWnd;

	UNIT_INTERFACE clientborder, boardlength, leftborder, gridborder, chessradius;//�������
	UNIT_GRIDLENGTH gridlength;
	POINT vertex[4];//�����ĸ�����
	UNIT_SIZE hover[2][2];//�����ͣ
	COLORREF boardcolor;//������ɫ

	void Mode_Gaming(UNIT_STATUS, UNIT_SIZE, UNIT_SIZE);
	int Mode_Replay(UNIT_STATUS);//���ص�ǰ����
	void Mode_Analysis(){}

public:
	Painter(HWND, UNIT_ID, UNIT_ID, UNIT_SIZE, time_t, bool);
	Painter(const WCHAR*, HWND);
	~Painter();

	void PaintBoard(HDC, RECT*);//������
	void DrawChess(HDC, UNIT_STATUS, COLORREF);//������ָ����ɫ����
	void DrawAChess(HDC, UNIT_INTERFACE, UNIT_INTERFACE);//��һ������
	void DrawAChess(UNIT_SIZE, UNIT_SIZE, UNIT_ID);//�Ϻ���
	void PaintHover();//���������ͣ������ʾ
	void RefreshBoard();

	void InitBackground();
	void DrawBlank(HDC, UNIT_STATUS);
	void ChangeBlank(UNIT_STATUS, const WCHAR*, int);
	void ChangeBlankStatus();
	
	void MouseOperation(UNIT_INTERFACE, UNIT_INTERFACE, UNIT_STATUS);
	void SaveToJson(const WCHAR*);

	void RequestMode(UNIT_STATUS);
	void GameOverAlert(UNIT_ID);

	void AddHistory(time_t, UNIT_ID, UNIT_ID, UNIT_ID);
	std::vector<Json> LoadHistory();

	inline HWND GetHwnd() { return hWnd; }//�ڶԻ����д���Painter������Ҫ��
};

class Judgeparam {
public:
	Chess* MainChess;
	Painter* painter;
	Judgeparam(Chess* chess, Painter* painter) :MainChess(chess), painter(painter) {}
};