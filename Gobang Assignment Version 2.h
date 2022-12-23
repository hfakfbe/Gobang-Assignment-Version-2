#pragma once

#include "resource.h"
#include "Painter.h"

INT_PTR CALLBACK Newgame(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK History(HWND, UINT, WPARAM, LPARAM);

extern HINSTANCE hInst;
extern Painter* CurrentPainter;