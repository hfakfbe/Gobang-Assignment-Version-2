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
	std::vector<std::pair<JSON_BUFFER, JSON_BUFFER>> KeysValues;//��buffer��ʽ�洢����ζ���ַ�����Ҫ��""

	bool Succeed;

	const char space[MAXN_SPACE] = { '\n', '\t', '\r', '\f', '\v', ' ' };
	//��������
	bool SkipSpace(JSON_BUFFER&, int&);//�����Ƿ񵽴�ĩβ
	bool CheckInteger(JSON_BUFFER&, int, int&, JSON_BUFFER&);//�ӵ�һ��buffer��ȡ��int�ǿ�ʼ�±꣬�����������ڶ���buffer
	bool CheckString(JSON_BUFFER&, int, int&, JSON_BUFFER&);
	bool CheckJson(JSON_BUFFER&, int, int&, JSON_BUFFER&);
	bool CheckArray(JSON_BUFFER&, int, int&, JSON_BUFFER&);//�����ٵ���ǰ����
public:
	Json() { Succeed = true; }
	Json(JSON_BUFFER);//buffer��Ҫ����
	JSON_BUFFER Jsontostring();
	inline bool isSucceed() { return Succeed; }

	void Addkey(JSON_KEY_STRING, JSON_VALUE_INT8&);//�����KEY�ظ���
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