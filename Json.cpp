#include "Json.h"

void Json::Addkey(const JSON_KEY_STRING key, JSON_VALUE_BOOL& value) {
	KeysValues.push_back({ "\"" + key + "\"" ,value?"true":"false"});
}

void Json::Addkey(const JSON_KEY_STRING key, JSON_VALUE_INT8& value) {
	KeysValues.push_back({ "\"" + key + "\"" ,std::to_string(value) });
}

void Json::Addkey(const JSON_KEY_STRING key, JSON_VALUE_INT16& value) {
	KeysValues.push_back({ "\"" + key + "\"" ,std::to_string(value) });
}

void Json::Addkey(const JSON_KEY_STRING key, JSON_VALUE_INT32& value) {
	KeysValues.push_back({ "\"" + key + "\"" ,std::to_string(value) });
}

void Json::Addkey(const JSON_KEY_STRING key, JSON_VALUE_INT64& value) {
	KeysValues.push_back({ "\"" + key + "\"" ,std::to_string(value) });
}

void Json::Addkey(const JSON_KEY_STRING key, JSON_VALUE_STRING& value) {
	KeysValues.push_back({ "\"" + key + "\"" ,"\"" + value + "\"" });
}

void Json::Addkey(const JSON_KEY_STRING key, Json& value) {
	KeysValues.push_back({ "\"" + key + "\"" ,value.Jsontostring() });
}

void Json::Addkey(const JSON_KEY_STRING key, std::vector<JSON_VALUE_BOOL>& values) {
	JSON_BUFFER tmp = "[";
	for (int i = 0; i < values.size(); ++i) {
		if (i != 0) tmp += ",";
		tmp += values[i]?"true":"false";
	}
	tmp += "]";
	KeysValues.push_back({ "\"" + key + "\"" ,tmp });
}

void Json::Addkey(const JSON_KEY_STRING key, std::vector<JSON_VALUE_INT8>& values) {
	JSON_BUFFER tmp = "[";
	for (int i = 0; i < values.size(); ++i) {
		if (i != 0) tmp += ",";
		tmp += std::to_string(values[i]);
	}
	tmp += "]";
	KeysValues.push_back({ "\"" + key + "\"" ,tmp });
}

void Json::Addkey(const JSON_KEY_STRING key, std::vector<JSON_VALUE_INT16>& values) {
	JSON_BUFFER tmp = "[";
	for (int i = 0; i < values.size(); ++i) {
		if (i != 0) tmp += ",";
		tmp += std::to_string(values[i]);
	}
	tmp += "]";
	KeysValues.push_back({ "\"" + key + "\"" ,tmp });
}

void Json::Addkey(const JSON_KEY_STRING key, std::vector<JSON_VALUE_INT32>& values) {
	JSON_BUFFER tmp = "[";
	for (int i = 0; i < values.size(); ++i) {
		if (i != 0) tmp += ",";
		tmp += std::to_string(values[i]);
	}
	tmp += "]";
	KeysValues.push_back({ "\"" + key + "\"" ,tmp });
}

void Json::Addkey(const JSON_KEY_STRING key, std::vector<JSON_VALUE_INT64>& values) {
	JSON_BUFFER tmp = "[";
	for (int i = 0; i < values.size(); ++i) {
		if (i != 0) tmp += ",";
		tmp += std::to_string(values[i]);
	}
	tmp += "]";
	KeysValues.push_back({ "\"" + key + "\"" ,tmp });
}

void Json::Addkey(const JSON_KEY_STRING key, std::vector<JSON_VALUE_STRING>& values) {
	JSON_BUFFER tmp = "[";
	for (int i = 0; i < values.size(); ++i) {
		if (i != 0) tmp += ",";
		tmp += "\"" + values[i] + "\"";
	}
	tmp += "]";
	KeysValues.push_back({ "\"" + key + "\"" ,tmp });
}

void Json::Addkey(const JSON_KEY_STRING key, std::vector<Json>& values) {
	JSON_BUFFER tmp = "[";
	for (int i = 0; i < values.size(); ++i) {
		if (i != 0) tmp += ",";
		tmp += values[i].Jsontostring();
	}
	tmp += "]";
	KeysValues.push_back({ "\"" + key + "\"" ,tmp });
}

inline bool Json::SkipSpace(JSON_BUFFER& buffer, int& pos) {
	while (pos < buffer.size()) {
		bool flag = true;
		for (int i = 0; i < MAXN_SPACE; ++i) {
			if (buffer[pos] == space[i]) {
				flag = false;  pos++;  break;
			}
		}
		if (flag) return false;
	}
	return true;//到这里就失败了
}

bool Json::CheckBoolean(JSON_BUFFER& buffer, int pos, int& end, JSON_BUFFER& value) {
	const std::string boolstring[2] = { "true", "false" };
	for (int j = 0; j < 2; ++j) {
		if (pos + boolstring[j].size() - 1 < buffer.size()) {
			bool success = true;
			for (int i = 0; i < boolstring[j].size(); ++i)
				if (buffer[pos + i] != boolstring[j][i]) {
					success = false; break;
				}
			if (success) { end = pos + boolstring[j].size(); value = boolstring[j]; return true; }
		}
	}
	return false;
}

bool Json::CheckInteger(JSON_BUFFER& buffer, int pos, int& end, JSON_BUFFER& value) {//从当前开始读取，end是最后成功读取之后的
	value = "";
	bool flag = false;
	while (pos < buffer.size()) {
		if (buffer[pos] >= '0' && buffer[pos] <= '9') flag = true, value += buffer[pos++];
		else if (flag == false && pos + 1 < buffer.size() && buffer[pos] == '-' && buffer[pos + 1] >= '0' && buffer[pos + 1] <= '9')
			flag = true,  value += buffer[pos++];//负号支持，只允许第一个为负号，防止999-999的情况
		else break;
	}
	end = pos;
	return flag;
}

bool Json::CheckString(JSON_BUFFER& buffer, int pos, int& end, JSON_BUFFER& value) {
	if (buffer[pos++] != '"') return false;
	value = '"';
	bool flag = false, last = true;//flag是否成功匹配，last是否上个不是\符号
	for (; pos < buffer.size(); pos++) {
		value += buffer[pos];
		if (last == false) {
			last = true; continue;
		}
		if (buffer[pos] == '"') {
			flag = true; break;
		}
		else if (buffer[pos] == '\\') {
			last = false;
		}
	}
	end = ++pos;
	return flag;
}

bool Json::CheckJson(JSON_BUFFER& buffer, int pos, int& end, JSON_BUFFER& value) {
	value = "";
	if (buffer[pos] != '{') return false;
	std::stack<char> findbracket;
	JSON_BUFFER subbuffer;
	for (; pos < buffer.size(); ++pos) {
		subbuffer += buffer[pos];
		if (buffer[pos] == '{') findbracket.emplace('{');
		else if (buffer[pos] == '}') findbracket.pop();
		if (!findbracket.size()) break;
	}
	if (findbracket.size()) return false;
	Json subjson(subbuffer);
	if (subjson.isSucceed() == false) return false;
	value = subjson.Jsontostring();
	end = ++pos;
	return true;
}

bool Json::CheckArray(JSON_BUFFER& buffer, int pos, int& end, JSON_BUFFER& value) {
	if (buffer[pos++] != '[') return false;
	if (SkipSpace(buffer, pos)) return false;
	int endtmp;
	value = "[";
	JSON_BUFFER tmp;
	int type = -1;
	if (CheckBoolean(buffer, pos, endtmp, tmp)) type = 0;
	else if (CheckInteger(buffer, pos, endtmp, tmp)) type = 1;
	else if (CheckString(buffer, pos, endtmp, tmp)) type = 2;
	else if (CheckJson(buffer, pos, endtmp, tmp)) type = 3;
	else if (CheckArray(buffer, pos, endtmp, tmp)) type = 4;
	else if (buffer[pos] == ']') {
		value += ']'; end = ++pos; return true;//[ ]
	}
	else return false;
	pos = endtmp;
	value += tmp;
	if (SkipSpace(buffer, pos)) return false;
	while (buffer[pos] == ',') {
		pos++;
		if (SkipSpace(buffer, pos)) return false;
		if(type == 0) { if (CheckBoolean(buffer, pos, end, tmp) == false) return false; }
		else if (type == 1) { if (CheckInteger(buffer, pos, end, tmp) == false) return false; }
		else if (type == 2) { if (CheckString(buffer, pos, end, tmp) == false) return false; }
		else if (type == 3) { if (CheckJson(buffer, pos, end, tmp) == false) return false; }
		else if (type == 4) { if (CheckArray(buffer, pos, end, tmp) == false) return false; }
		pos = end;
		value += ',' + tmp;
		if (SkipSpace(buffer, pos)) return false;
	}
	if (buffer[pos] == ']') {
		value += ']';
	}
	end = ++pos;
	return true;
}

#define TRANSLATE_FAIL do{ Succeed = false; KeysValues.clear(); return; }while(0)

Json::Json(JSON_BUFFER& buffer) {//排除其他不合法字符。因为文件有时达到几万字节，应改为引用
	Succeed = true;
	int pos = 0;
	if (SkipSpace(buffer, pos)) return;
	if (buffer[pos] != '{') TRANSLATE_FAIL;
	do {
		pos++;
		if (SkipSpace(buffer, pos)) TRANSLATE_FAIL;
		JSON_KEY_STRING key;
		bool flag = false;
		if (CheckString(buffer, pos, pos, key) == false) {//KEY
			if (KeysValues.size()) TRANSLATE_FAIL;
			else flag = true;//回退一格，对齐
		}
		if (flag) break;
		if (SkipSpace(buffer, pos)) TRANSLATE_FAIL;
		if (buffer[pos++] != ':') TRANSLATE_FAIL;//:
		if (SkipSpace(buffer, pos)) TRANSLATE_FAIL;
		int end;//VALUE
		JSON_BUFFER value;
		if (CheckBoolean(buffer, pos, end, value)) pos = end;
		else if (CheckInteger(buffer, pos, end, value)) pos = end;
		else if (CheckString(buffer, pos, end, value)) pos = end;
		else if (CheckArray(buffer, pos, end, value)) pos = end;
		else if (CheckJson(buffer, pos, end, value)) pos = end;
		else TRANSLATE_FAIL;
		KeysValues.push_back({ key, value });
		if (SkipSpace(buffer, pos)) TRANSLATE_FAIL;
	} while (buffer[pos] == ',');//结束后应该停留在下一个有效字符处
	if (buffer[pos] != '}') TRANSLATE_FAIL;
	if (SkipSpace(buffer, ++pos) == false) TRANSLATE_FAIL;
	//检查KEY是否有重复
	bool flag = false;
	std::sort(KeysValues.begin(), KeysValues.end(), [&](const auto& a, const auto& b) {
		int i = 0;
		while (a.first.size() > i && b.first.size() > i) {
			if (a.first[i] != b.first[i]) return a.first[i] < b.first[i];
			i++;
		}
		if (a.first.size() > i) return true;
		else if (b.first.size() > i) return false;
		else flag = true;
	});
	if (flag) TRANSLATE_FAIL;
}

JSON_BUFFER Json::Jsontostring() {
	JSON_BUFFER ans = "{";
	for (int i = 0; i < KeysValues.size(); ++i) {
		ans += KeysValues[i].first + ':' + KeysValues[i].second;
		if (i != KeysValues.size() - 1) ans += ',';
	}
	ans += '}';
	return ans;
}

void Json::Askkey(const JSON_KEY_STRING keystr, JSON_BUFFER& value) {//只把buffer查找出来
	value = "";
	JSON_KEY_STRING key = '"' + keystr + '"';
	for (int i = 0; i < KeysValues.size(); ++i) {
		if (key == KeysValues[i].first) {
			value = KeysValues[i].second;
			return;
		}
	}
}

void Json::Translatevector(const JSON_VALUE_STRING values, std::vector<JSON_BUFFER>& value) {
	std::stack<char> layer;
	JSON_BUFFER tmp;
	for (int i = 1; i < values.size(); ++i) {
		if ((values[i] == ',' || values[i] == ']') && layer.size() == 0) {
			value.push_back(tmp); tmp = ""; continue;
		}
		tmp += values[i];
		if (values[i] == '[' || values[i] == '{') layer.emplace(values[i]);
		else if (values[i] == ']' || values[i] == '}') layer.pop();
	}
}

/*	测试debug
int main() {
	Json json("{\"4\":[{\"1\":[[2],[3],[\"114\"]]},{}]}");
	std::string b = "fuck you";
	int a = 123;
	std::vector<Json> c; c.push_back(json); c.push_back(json);
	json.Addkey("hello world\n", c);
	std::cout << json.Jsontostring() << std::endl;
	std::vector<std::string> v;
	std::string value;
	json.Askkey("\"hello world\n\"", value);
	json.Translatevector(value, v);
	for (int i = 0; i < v.size(); ++i) std::cout << v[i] << '\t';
	return 0;
}
*/