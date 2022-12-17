#pragma once

#include "framework.h"

#define MAXN_SPACE 6

typedef char		JSON_VALUE_INT8;
typedef short		JSON_VALUE_INT16;
typedef int			JSON_VALUE_INT32;
typedef long long	JSON_VALUE_INT64;
typedef std::string	JSON_VALUE_STRING;
typedef std::string	JSON_KEY_STRING;
typedef std::string	JSON_BUFFER;


class Json {
private:
	std::vector<std::pair<JSON_BUFFER, JSON_BUFFER>> KeysValues;//以buffer形式存储，意味着字符串需要加""

	bool Succeed;

	const char space[MAXN_SPACE] = { '\n', '\t', '\r', '\f', '\v', ' ' };
	//解析调用
	bool SkipSpace(JSON_BUFFER&, int&);//返回是否到达末尾
	bool CheckInteger(JSON_BUFFER&, int, int&, JSON_BUFFER&);//从第一个buffer读取，int是开始下标，解析结果存入第二个buffer
	bool CheckString(JSON_BUFFER&, int, int&, JSON_BUFFER&);
	bool CheckJson(JSON_BUFFER&, int, int&, JSON_BUFFER&);
	bool CheckArray(JSON_BUFFER&, int, int&, JSON_BUFFER&);//里面再调用前三者
public:
	Json() { Succeed = true; }
	Json(JSON_BUFFER);//buffer需要解析
	JSON_BUFFER Jsontostring();
	inline bool isSucceed() { return Succeed; }

	void Addkey(JSON_KEY_STRING, JSON_VALUE_INT8&);//不检查KEY重复性
	void Addkey(JSON_KEY_STRING, JSON_VALUE_INT16&);
	void Addkey(JSON_KEY_STRING, JSON_VALUE_INT32&);
	void Addkey(JSON_KEY_STRING, JSON_VALUE_INT64&);
	void Addkey(JSON_KEY_STRING, JSON_VALUE_STRING&);
	void Addkey(JSON_KEY_STRING, Json&);
	void Addkey(JSON_KEY_STRING, std::vector<JSON_VALUE_INT8>&);
	void Addkey(JSON_KEY_STRING, std::vector<JSON_VALUE_INT16>&);
	void Addkey(JSON_KEY_STRING, std::vector<JSON_VALUE_INT32>&);
	void Addkey(JSON_KEY_STRING, std::vector<JSON_VALUE_INT64>&);
	void Addkey(JSON_KEY_STRING, std::vector<JSON_VALUE_STRING>&);
	void Addkey(JSON_KEY_STRING, std::vector<Json>&);

	void Askkey(JSON_KEY_STRING, JSON_BUFFER&);

	void Translatevector(JSON_KEY_STRING, std::vector<JSON_BUFFER>&);
};