#include"SymbolTable.h"

SymbolTable::SymbolTable() {
	
}
void SymbolTable::insert(string name, string kind, string type, int level) {
	AttrVec.push_back(Attributes(name, kind, type, level));
	if (kind == "RETURNFUNCTION" || kind == "VOIDFUNCTION") {
		currentFuncName = name;
		funcCalls[name] = vector<pair<string, int>>();
	}
	else if (kind == "PARA") {
		vector<string> v;
		v.push_back(name);
		v.push_back(type);
		v.push_back(kind);
		v.push_back("1");	//size
		v.push_back("0"); //const value
		funcParams[currentFuncName].push_back(v);
	}
	else if (type != "ARRAY") {
		vector<string> v;
		v.push_back(name);
		v.push_back(type);
		v.push_back(kind);
		v.push_back("1");	//size
		v.push_back("0"); //const value
		funcAttrs[currentFuncName].push_back(v);
	}

}
void SymbolTable::insert(string name, string kind, string type, int level, string value) {
	AttrVec.push_back(Attributes(name, kind, type, level, value));
	if (kind == "RETURNFUNCTION" || kind == "VOIDFUNCTION") {
		currentFuncName = name;
		funcCalls[name] = vector<pair<string, int>>();
	}
	else if (kind == "CONSTTK") {
		vector<string> v;
		v.push_back(name);
		v.push_back(type);
		v.push_back(kind);
		v.push_back("1");	//size
		v.push_back(value); //for constant
		funcConst[currentFuncName].push_back(v);
	}
}
	
void SymbolTable::setArrayAttr(string type, vector<int> dimVec) {
	string name = AttrVec.back().getName();
	int level = AttrVec.back().getLevel();
	arrayAttrMap[make_pair(name, level)] = make_pair(type, dimVec);
	int size = 1;
	for (auto x : dimVec) {
		size *= x;
	}
	vector<string> v;
	v.push_back(name);
	v.push_back(type);
	v.push_back("ARRAY");
	v.push_back(to_string(size));
	funcAttrs[currentFuncName].push_back(v);
}
void SymbolTable::setFuncParam(string name, vector<string> param) {
	funcParamMap[name] = param;
	for (map<string, vector<string>>::iterator it = funcParamMap.begin(); it != funcParamMap.end(); it++) {
		cout << it->first << " ";
		vector<string> v = it->second;
		for (int i = 0; i < v.size(); i++) {
			cout << v[i] << " ";
		}
	}
	cout << endl;
}
string SymbolTable::getType(string name) {
	for (vector<Attributes>::reverse_iterator i = AttrVec.rbegin(); i != AttrVec.rend(); i++) {
		if (i->getName() == name) {
			if (i->getType() == "ARRAY") {
				return arrayAttrMap[{name, i->getLevel()}].first;	//get type from arrayAttrMap for array
			}
			else {
				return i->getType();
			}
		}
	}
	return "";
}
string SymbolTable::getKind(string name) {
	for (vector<Attributes>::reverse_iterator i = AttrVec.rbegin(); i != AttrVec.rend(); i++) {
		if (i->getName() == name) {
			return i->getKind();
		}
	}
	return "";
}
bool SymbolTable::nameExistInSameLev(string name, int level) {
	for (vector<Attributes>::reverse_iterator i = AttrVec.rbegin(); i != AttrVec.rend(); i++) {
		if (i->getName() == name && i->getLevel() == level) {
			return true;
		}
	}
	return false;
}
bool SymbolTable::nameExist(string name) {
	for (vector<Attributes>::reverse_iterator i = AttrVec.rbegin(); i != AttrVec.rend(); i++) {
		if (i->getName() == name) {
			return true;
		}
	}
	return false;
}
string SymbolTable::findFunction(string name) {
	for (vector<Attributes>::reverse_iterator i = AttrVec.rbegin(); i != AttrVec.rend(); i++) {
		if (i->getName() == name) {
			int paramCount = funcParamMap[name].size();
			funcCalls[currentFuncName].push_back(make_pair(name, paramCount));
			return i->getKind();//"RETURNFUNCTION" or "VOIDFUNCTION"
		}
	}
	return "FunctionNotFound";	//not found
}
vector<string> SymbolTable::getParam(string name) {
	auto t = funcParamMap.find(name);
	if (t != funcParamMap.end()) {
		return t->second; //vector<string> of param
	}
	else {
		return vector<string>();
	}
}
string SymbolTable::getFuncReturnType(string name) {
	for (vector<Attributes>::reverse_iterator i = AttrVec.rbegin(); i != AttrVec.rend(); i++) {
		if (i->getName() == name) {
			return i->getType(); //INTTK or CHARTK
		}
	}
	return "FunctionNotFound";	//not found
}
void SymbolTable::updateFuncTempMax(int temp_max) {
	int current_max = funcTempMax[currentFuncName];
	if (current_max < temp_max) {
		funcTempMax[currentFuncName] = temp_max;
	}
}

bool SymbolTable::clearFuncScope(string name, bool dup) {
	vector<vector<string>> attrs;
	vector<vector<string>> params;
	int paramSize = 0;
	auto t = funcParamMap.find(name);
	if (t != funcParamMap.end()) {
		paramSize = t->second.size();
	}

	while (AttrVec.back().getName() != name) {
		vector<string> v;
		int size = 1;
		if (AttrVec.back().getType() == "ARRAY") {
			arrayAttrMap.erase(make_pair(AttrVec.back().getName(), 1));
		}

		AttrVec.pop_back();
	}

	if (dup && AttrVec.back().getName() == name) {
		AttrVec.pop_back();
	}

	map<string, int>::iterator it = funcTempMax.find(name);
	if (it == funcTempMax.end()) {
		funcTempMax[name] = -1;
	}
	return true;
}




Attributes::Attributes() {}
Attributes::Attributes(string name, string kind, string type, int level) {
	Attributes::name = name;
	Attributes::kind = kind;
	Attributes::type = type;
	Attributes::level = level;
	Attributes::value = "0";
}
Attributes::Attributes(string name, string kind, string type, int level, string value) {
	Attributes::name = name;
	Attributes::kind = kind;
	Attributes::type = type;
	Attributes::level = level;
	Attributes::value = value;
}
string Attributes::getName() {
	return name;
}
string Attributes::getKind() {
	return kind;
}
string Attributes::getType() {
	return type;
}string Attributes::getValue() {
	return value;
}
int Attributes::getLevel() {
	return level;
}
void Attributes::setValue(string value) {
	Attributes::value = value;
}
void Attributes::setName(string name) {
	Attributes::name = name;
}
void Attributes::setKind(string kind) {
	Attributes::kind = kind;
}
void Attributes::setType(string type) {
	Attributes::type = type;
}
void Attributes::setLevel(int level) {
	Attributes::level = level;
}