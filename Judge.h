#pragma once

#include "framework.h"
#include "Painter.h"

typedef int UNIT_MESSAGE;

class JudgeMessage {
public:
	UNIT_MESSAGE msg;
	UNIT_SIZE x, y;
	JudgeMessage(UNIT_MESSAGE msg):msg(msg){}
	JudgeMessage(UNIT_MESSAGE msg, UNIT_SIZE x, UNIT_SIZE y):msg(msg), x(x), y(y){}
};

#define JUDGEMSG_SET				100
#define JUDGEMSG_REGRET				101
#define JUDGEMSG_SUSPEND_RESTART	102
#define JUDGEMSG_EXIT				103
#define JUDGEMSG_GIVEIN				104

extern std::queue<JudgeMessage> MessageQueue;