#ifndef SymbolTable_h
#define SymbolTable_h

#include<string>
#include<vector>
#include<map>
#include<utility>
#include<algorithm>
#include<cctype>
#include<iostream>
using std::tolower;
using std::transform;
using std::map;
using std::pair;
using std::make_pair;
using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::to_string;
using std::reverse;

class Attributes {
public:
	Attributes();
	Attributes(string name, string kind, string type, int level);
	Attributes(string name, string kind, string type, int level, string value);
	string getName();
	string getKind();
	string getType();
	string getValue();
	int getLevel();
	void setName(string name);
	void setKind(string kind);
	void setType(string type);
	void setLevel(int level);
	void setValue(string value);

private:
	string name;
	string kind;
	string type;
	int level;
	string value;
};

class SymbolTable {
public:
	SymbolTable();
	void insert(string name, string kind, string type, int level);
	void insert(string name, string kind, string type, int level, string value);
	void setArrayAttr(string type, vector<int> dim);
	void setFuncParam(string name, vector<string> param);
	string getType(string name);	//return type of idenfr
	string getKind(string name);	//CONSTTK VAR RETURNFUNCTION VOIDFUNCTION
	string findFunction(string name);
	vector<string> getParam(string name);
	string getFuncReturnType(string name);
	bool nameExistInSameLev(string name, int level);
	bool nameExist(string name);
	bool clearFuncScope(string name, bool dup);
	void updateFuncTempMax(int temp_max);

	map<string, vector<vector<string>>> funcAttrs;	//(funcname, vector(attrname, attrtype, attrsize))
	map<string, vector<vector<string>>> funcParams;	//(funcname, vector(paramname, paramtype))
	map<string, vector<pair<string, int>>> funcCalls; //(funcname, vector(funcname, paramcount))
	map<string, int> funcTempMax; //(funcName, max temp memory needed)
	map<string, vector<vector<string>>> funcConst; //(funcname, vector(attrname, attrType, attrValue)), not included array
	string globalName = "$global";
	string currentFuncName = globalName;


private:
	vector<Attributes> AttrVec;
	map<pair<string, int>, pair<string, vector<int>>> arrayAttrMap; //array attribute for a certain array name and level
	map<string, vector<string>> funcParamMap;
	//map <pair< string, int > , string> constValueMap;

	
};

#endif // !SymbolTable_h