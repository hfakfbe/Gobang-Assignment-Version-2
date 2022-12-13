#pragma once
#include "Painter.h"

#define ERR_FILEOPENFAIL	151
#define ERR_FILESAVEFAIL	152
#define ERR_THREADCREATE	153

class Myerror {
public:
	Myerror(UNIT_STATUS e, HWND hWnd) {
		switch (e) {
		case ERR_FILEOPENFAIL:
		case ERR_FILESAVEFAIL:
		case ERR_THREADCREATE:
			break;
		}
		//Ìí¼Ólog
		abort();
	}
};