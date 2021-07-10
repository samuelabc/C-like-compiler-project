#include"SyntaxAnalyzer.h"
#include<iostream>

using std::cout;
using std::endl;

#define debug 0

void SyntaxAnalyzer::updateRegCount() {
	if (reg_type == "$t") {
		reg_count++;
		if (reg_count >= 10) {
			reg_count = (reg_count % 10) + 2;
		}
	}
	int last_count = reg_count;
	
	while (find(terms.begin(), terms.end(), "$t" + to_string(reg_count)) != terms.end() ||
		find(pushVec.begin(), pushVec.end(), "$t" + to_string(reg_count)) != pushVec.end()) //element in vector
	{
		reg_count++;
		if (reg_count >= 10) {
			reg_count = (reg_count % 10) + 2;
		}
		if (last_count == reg_count) {
			reg_type = "#temp";
			break;
		}
	}
	
	if (reg_type == "#temp") {
		bool reuse = false;
		for (int i = 0; i <= temp_max; i++) {
			if (find(terms.begin(), terms.end(), "#temp" + to_string(i)) == terms.end() &&
				find(pushVec.begin(), pushVec.end(), "#temp" + to_string(i)) == pushVec.end()) {
				reuse = true;
				temp_count = i;
			}
		}
		if (reuse == false) {
			temp_max++;
			temp_count = temp_max;
		}
	}
}
bool SyntaxAnalyzer::isLocalVar(string attrname) {
	vector<vector<string>> localVarVec = symbolTable.funcAttrs[symbolTable.currentFuncName];
	vector<vector<string>> localParamVec = symbolTable.funcParams[symbolTable.currentFuncName];
	for (auto v : localVarVec) {
		if (v[0] == attrname) {
			return true;
		}
	}
	for (auto v : localParamVec) {
		if (v[0] == attrname) {
			return true;
		}
	}
	return false;
}

string SyntaxAnalyzer::isConstant(string attrname) {
	vector<vector<string>> localConstVec = symbolTable.funcConst[symbolTable.currentFuncName];
	string constValue = "";
	for (auto v : localConstVec) {
		if (v[0] == attrname) {
			constValue = v[4];
		}
	}
	if (constValue == "" && !isLocalVar(attrname)) {
		vector<vector<string>> globalConstVec = symbolTable.funcConst[symbolTable.globalName];
		for (auto v : globalConstVec) {
			if (v[0] == attrname) {
				constValue = v[4];
			}
		}
	}
	return constValue;
}
void SyntaxAnalyzer::expressionAnalyze() {
	/*for (auto str : exprAnalyze) {
		cout << str << endl;
	}*/
	vector<string> storeArrInter;
	
	string left;
	string right;
	string op;
	string interCode;
	string smallop;
	int len = exprAnalyze.size();
	for (int i = 0; i < len; i++) {
		if (exprAnalyze[i] == "PLUS" || exprAnalyze[i] == "MINU" || exprAnalyze[i] == "MULT" || exprAnalyze[i] == "DIV") {
			op = exprAnalyze[i];
			if (op == "PLUS") {
				smallop = "+";
			}
			else if (op == "MINU") {
				smallop = "-";
			}
			else if (op == "MULT") {
				smallop = "*";
			}
			else if (op == "DIV") {
				smallop = "/";
			}
			if (terms.size() < 2) {
				right = terms.back();
				terms.pop_back();
				string x;
				if ((x = isConstant(right)) != "") {
					right = x;
				}
				left = "$0";
			}
			else {
				right = terms.back();
				terms.pop_back();

				string x;
				if ((x = isConstant(right)) != "") {
					right = x;
				}

				left = terms.back();
				terms.pop_back();

				if ((x = isConstant(left)) != "") {
					left = x;
				}
			}
			vector<string> inter;
			string reg;
			if (reg_type == "$t") {
				reg = "$t" + to_string(reg_count);
			}
			else {
				reg = "#temp" + to_string(temp_count);
			}
			inter.push_back(op);
			inter.push_back(reg);
			inter.push_back("=");
			inter.push_back(left);
			inter.push_back(smallop);
			inter.push_back(right);
			intermediateCodeVec.push_back(inter);

			terms.push_back(reg);

			updateRegCount();
		}
		else if (exprAnalyze[i] == "HEADMINU") {
			string op = "MINU";
			string smallop = "-";
			right = terms.back();
			terms.pop_back();
			left = "$0";

			string reg;
			if (reg_type == "$t") {
				reg = "$t" + to_string(reg_count);
			}
			else {
				reg = "#temp" + to_string(temp_count);
			}
			vector<string> inter;
			inter.push_back(op);
			inter.push_back(reg);
			inter.push_back("=");
			inter.push_back(left);
			inter.push_back(smallop);
			inter.push_back(right);
			intermediateCodeVec.push_back(inter);

			terms.push_back(reg);

			updateRegCount();
		}
		else if (exprAnalyze[i] == "#dim") {
			vector<string> inter;
			inter.push_back("DIM");
			inter.push_back(terms.back());
			terms.pop_back();
			intermediateCodeVec.push_back(inter);
		}
		else if (exprAnalyze[i] == "#arr") {
			vector<string> inter;
			if (terms.size() == 1) {	//store value to arr, the only element on terms is arrName
				//do nothing
				//later will be assign through symbol "="
			}
			else {	//load value from arr
				string reg;
				if (reg_type == "$t") {
					reg = "$t" + to_string(reg_count);
				}
				else {
					reg = "#temp" + to_string(temp_count);
				}
				inter.push_back("LOADARR");
				inter.push_back(reg);
				inter.push_back(terms.back());
				terms.pop_back();
				terms.push_back(reg);
				intermediateCodeVec.push_back(inter);

				updateRegCount();
			}
		}
		else if (exprAnalyze[i] == "#para") {
			vector<string> inter;
			inter.push_back("PUSH");
			inter.push_back(terms.back());

			pushVec.push_back(terms.back());
			terms.pop_back();
			intermediateCodeVec.push_back(inter);
		}
		else if (exprAnalyze[i] == "#func") {
			vector<string> inter;
			inter.push_back("CALL");
			inter.push_back(terms.back());
			int paramNum = symbolTable.funcParams[terms.back()].size();
			for (int i = 0; i < paramNum; i++) {
				pushVec.pop_back();
			}
			terms.pop_back();
			intermediateCodeVec.push_back(inter);

			string reg;
			if (reg_type == "$t") {
				reg = "$t" + to_string(reg_count);
			}
			else {
				reg = "#temp" + to_string(temp_count);
			}
			inter.clear();
			inter.push_back("ASSIGN");
			inter.push_back(reg);
			inter.push_back("=");
			inter.push_back("$v0");
			intermediateCodeVec.push_back(inter);

			terms.push_back(reg);

			updateRegCount();
		}
		else if (exprAnalyze[i] == "=") {
			vector<string> inter;
			string right = terms.back();
			terms.pop_back();
			string left = terms.back();
			terms.pop_back();
			inter.push_back("ASSIGN");
			inter.push_back(left);
			inter.push_back("=");
			inter.push_back(right);
			intermediateCodeVec.push_back(inter);
		}
		else {
			terms.push_back(exprAnalyze[i]);
		}
	}
	symbolTable.updateFuncTempMax(temp_max);
	terms.clear();
	pushVec.clear();
	reg_count = 2;
	reg_type = "$t";
	temp_max = -1;
	temp_count = -1;
	exprAnalyze.clear();
}
void SyntaxAnalyzer::error(int line, char type, const char* description) {
	/*if (errorVec.size()>0 && errorVec.back().getLine() == line) {
		return;
	}*/
	Error err(line, type, description);
	errorVec.push_back(err);
	if (debug) {
		cout << "error" << " " << description << endl;
	}
}
bool SyntaxAnalyzer::checkDupIdenfr(string name, int level) {
	if (symbolTable.nameExistInSameLev(name, level)) {
		error(getCurLineNo(), 'b', "duplicate definition");
		return true;
	}
	else {
		return false;
	}
}
bool SyntaxAnalyzer::checkIdenfrHasDefined(string name) {
	if (symbolTable.nameExist(name)) {
		return true;
	}
	else {
		error(getCurLineNo(), 'c', "name is not declared");
		return false;
	}
}
string SyntaxAnalyzer::getType(string name) {
	string type = symbolTable.getType(name);
	if (type.length() == 0) {	//name is not declared
		error(getCurLineNo(), 'c', "name is not declared");
	}
	return type;
}
string SyntaxAnalyzer::getKind(string name) {
	string type = symbolTable.getKind(name);
	if (type.length() == 0) {	//name is not declared
		error(getCurLineNo(), 'c', "name is not declared");
	}
	return type;
}
void SyntaxAnalyzer::skip(vector<string> sym) {
	bool flag = true;
	while (flag) {
		for (auto s : sym) {
			if (symbol == s) {
				flag = false;
				break;
			}
		}
		if (flag) {
			nextSymbol();
		}
	}
}
bool SyntaxAnalyzer::checkSEMICN() {
	if (symbol == "SEMICN") {
		return true;
	}
	else {
		error(getLastLineNo(), 'k', "missing SEMICN");
		retractSymbol();
		return true;
	}
}
bool SyntaxAnalyzer::checkRPARENT() {
	if (symbol == "RPARENT") {
		return true;
	}
	else {
		error(getCurLineNo(), 'l', "missing RPARENT");
		retractSymbol();
		//skip(vector<string>{string("SEMICN"), string("ASSIGN"), string("RBRACE")});
		return true;
	}
}
bool SyntaxAnalyzer::checkRBRACK() {
	if (symbol == "RBRACK") {
		return true;
	}
	else {
		error(getCurLineNo(), 'm', "missing RBRACK");
		retractSymbol();
		//skip(vector<string>{string("SEMICN"), string("ASSIGN")});
		return true;
	}
}
bool SyntaxAnalyzer::checkRBRACE() {
	if (symbol == "RBRACE") {
		return true;
	}
	else {
		//error(getLastLineNo(), '\0', "missing RBRACE");
		retractSymbol();
		return false;
	}
}
void SyntaxAnalyzer::insertToOutputVec(const char* str) {
	if (debug) {
		cout << str << endl;
	}
	outputVec.push_back(str);
}

void SyntaxAnalyzer::nextSymbol() {
	idx++;
	if (idx < tokenVec.size()) {
		symbol = tokenVec[idx].getSymbol();
		token = tokenVec[idx].getToken();
		lowtoken = tokenVec[idx].getLowToken();
		outputVec.push_back(symbol + " " + token);
	}
	else {
		//Error("no more symbol");
	}
}
void SyntaxAnalyzer::retractSymbol() {
	idx--;
	symbol = tokenVec[idx].getSymbol();
	token = tokenVec[idx].getToken();
	lowtoken = tokenVec[idx].getLowToken();
	outputVec.pop_back();
}
int SyntaxAnalyzer::getLastLineNo() {
	return tokenVec[idx - 1].getLine();
}
int SyntaxAnalyzer::getCurLineNo() {
	return tokenVec[idx].getLine();
}
void SyntaxAnalyzer::charString() {
	if (symbol == "STRCON") {
		insertToOutputVec("<字符串>");
	}
	else {
		//Error("missing STRCON");
	}
}
int SyntaxAnalyzer::unsignedInteger() {
	if (symbol == "INTCON") {
		insertToOutputVec("<无符号整数>");
		return stoi(token);
	}
	else {
		return -1;
		//Error("missing unsigned integer");
	}
}
string SyntaxAnalyzer::integer() {
	string retstr;
	if (symbol == "PLUS" || symbol == "MINU") {
		if (symbol == "MINU") {
			retstr += token;
		}
		nextSymbol();
	}
	retstr += token;
	unsignedInteger();
	insertToOutputVec("<整数>");
	return retstr;
}
string SyntaxAnalyzer::constant(string type) {
	string retstr;
 	if (symbol == "INTCON" || symbol == "PLUS" || symbol == "MINU") {
		retstr += integer();
		if (type == "CHARTK") {
			error(getCurLineNo(), 'o', "constant type incompatible");
		}
		else {
			insertToOutputVec("<常量>");
		}
	}
	else if (symbol == "CHARCON") {
		retstr += token;
		if (type == "INTTK") {
			error(getCurLineNo(), 'o', "constant type incompatible");
		}
		else {
			insertToOutputVec("<常量>");
		}
	}
	else {
		//Error("missing constant integer or character in constant");
	}
	return retstr;
}
int SyntaxAnalyzer::stepLength() {
	int ret = unsignedInteger();
	insertToOutputVec("<步长>");
	return ret;
}
void SyntaxAnalyzer::paramValueTable(vector<string> param) {
	int i = 0;
	int len = param.size();
	string type;
	type = expression();

	exprAnalyze.push_back("#para");

	if (i > len - 1) {
		error(getCurLineNo(), 'd', "number of parameter incompatible");
	}
	else if (type != param[i]) {
		error(getCurLineNo(), 'e', "type of parameter incompatible");
	}
	nextSymbol();
	while (symbol == "COMMA") {
		nextSymbol();
		type = expression();

		exprAnalyze.push_back("#para");

		i++;
		if (i <= len-1 && type != param[i]) {
			error(getCurLineNo(), 'e', "type of parameter incompatible");
		}
		nextSymbol();
	}
	retractSymbol();
	if(i != len-1) {
		error(getCurLineNo(), 'd', "number of parameter incompatible");
	}
	else {
		insertToOutputVec("<值参数表>");
	}
}
string SyntaxAnalyzer::callFunction() {
	if (symbol == "IDENFR") {
		string functionName = lowtoken;
		bool enter_from_factor = true;
		if (exprAnalyze.size() == 0) {	//else the function name is already inserted in factor(function)
			enter_from_factor = false;
			exprAnalyze.push_back(functionName);
		}

		string retStr = symbolTable.findFunction(functionName);
		if (retStr == "FunctionNotFound") {
			error(getCurLineNo(), 'c', "calling an undefined function");
			//skip(vector<string>{string("RPARENT")});
		}
		else {
			nextSymbol();
			if (symbol == "LPARENT") {
				nextSymbol();
				if (symbol == "RPARENT") {	//参数为空
					if (symbolTable.getParam(functionName).size() != 0) {
						error(getCurLineNo(), 'd', "number of parameter incompatible");
					}
					retractSymbol();
					insertToOutputVec("<值参数表>");
					nextSymbol();
				}
				else if (symbol == "SEMICN") { //missing RPARENT
				
				}
				else {
					paramValueTable(symbolTable.getParam(functionName));
					nextSymbol();
				}

				exprAnalyze.push_back("#func");
				if (!enter_from_factor) {
					expressionAnalyze();
					exprAnalyze.clear();
				}

				if (checkRPARENT()) {
					if (retStr == "RETURNFUNCTION") {
						insertToOutputVec("<有返回值函数调用语句>");
						string type = symbolTable.getFuncReturnType(functionName);
						return type;
					}
					else if (retStr == "VOIDFUNCTION") {
						insertToOutputVec("<无返回值函数调用语句>");
						return "VOIDTK";
					}
				}
			}
		}
	}
	return "";
}
string SyntaxAnalyzer::factor() {
	if (symbol == "IDENFR") {
		exprAnalyze.push_back(lowtoken);
 		string type = getType(lowtoken);
		if (type == "") {
			skip(vector<string>{string("RPARENT"),string("SEMICN"),string("COMMA"),string("PLUS"),string("MINU"), string("MULT"), string("DIV"),
				string("LSS"),string("LEQ"),string("GRE"),string("GEQ"),string("EQL"),string("NEQ")});
			retractSymbol();
			return "";
		}
		nextSymbol();
		if (symbol == "LBRACK") {
			nextSymbol();
			string type1 = expression();
			exprAnalyze.push_back("#dim");

			if (type1 != "INTTK") {
				error(getCurLineNo(), 'i', "index of array should be of type INTTK");
			}
			nextSymbol();
			if (checkRBRACK()) {
				nextSymbol();
				if (symbol == "LBRACK") {
					nextSymbol();
					string type2 = expression();
					exprAnalyze.push_back("#dim");
					exprAnalyze.push_back("#arr");

					if (type2 != "INTTK") {
						error(getCurLineNo(), 'i', "index of array should be of type INTTK");
					}
					nextSymbol();
					if (checkRBRACK()) {
						insertToOutputVec("<因子>");
						return type;
					}
				}
				else {
					exprAnalyze.push_back("#arr");
					retractSymbol();
					insertToOutputVec("<因子>");
					return type;
				}
			}
		}
		else if (symbol == "LPARENT") {
			retractSymbol();
			type = callFunction();
			insertToOutputVec("<因子>");
			return type;
		}
		else {
			retractSymbol();
			insertToOutputVec("<因子>");
			return type;
		}
	}
	else if (symbol == "LPARENT") {
		nextSymbol();
		expression();
		nextSymbol();
		if (checkRPARENT()) {
			insertToOutputVec("<因子>");
			return "INTTK";
		}
	}
	else if (symbol == "INTCON" || symbol == "PLUS" || symbol == "MINU") {
		string intstr = integer();
		exprAnalyze.push_back(intstr);
		insertToOutputVec("<因子>");
		return "INTTK";
	}
	else if (symbol == "CHARCON") {
		exprAnalyze.push_back(lowtoken);
		insertToOutputVec("<因子>");
		return "CHARTK";
	}
	else {
		//Error("");
	}
	return "";
}
string SyntaxAnalyzer::term() {
	string type = factor();
	nextSymbol();
	if (symbol == "MULT" || symbol == "DIV") {
		while (symbol == "MULT" || symbol == "DIV") {
			string op = symbol;
			nextSymbol();
			factor();
			exprAnalyze.push_back(op);
			nextSymbol();
		}
		retractSymbol();
		insertToOutputVec("<项>");
		return "INTTK";
	}
	else {
		retractSymbol();
		insertToOutputVec("<项>");
		return type;
	}
}
string SyntaxAnalyzer::expression() {
	string type = "";
	bool headminus = false;
	if (symbol == "PLUS" || symbol == "MINU") {
		if (symbol == "MINU") {
			headminus = true;
		}
		nextSymbol();
		type = "INTTK";
	}
	string type1 = term();
	if (headminus) {
		exprAnalyze.push_back("HEADMINU");
	}
	if (type1 == "CHARTK" && type == "") {
		type = "CHARTK";
	}
	else {
		type = "INTTK";
	}
	nextSymbol();
	while (symbol == "PLUS" || symbol == "MINU") {
		string op = symbol;
		nextSymbol();
		term();
		exprAnalyze.push_back(op);
		type = "INTTK";
		nextSymbol();
	}
	retractSymbol();
	insertToOutputVec("<表达式>");
	return type;
}
void SyntaxAnalyzer::assignStatement() {
	if (symbol == "IDENFR") {
		string name = lowtoken;
		exprAnalyze.push_back(name);
		string kind = getKind(lowtoken);
		if (kind == "") {
			skip(vector<string>{string("SEMICN")});
			retractSymbol();
			return;
		}
		string type1;
		nextSymbol();
		if (symbol == "ASSIGN") {
			if (kind == "CONSTTK") {
				error(getCurLineNo(), 'j', "not allowed to assign to constant element");
			}
			nextSymbol();
			expression();
		}
		else if (symbol == "LBRACK") {
			nextSymbol();
			type1 = expression();
			exprAnalyze.push_back("#dim");

			if (type1 != "INTTK") {
				error(getCurLineNo(), 'i', "index of array should be integer");
			}
			nextSymbol();
			if (checkRBRACK()) {
				nextSymbol();
				if (symbol == "ASSIGN") {
					exprAnalyze.push_back("#arr");

					nextSymbol();
					expression();
				}
				else if (symbol == "LBRACK") {
					nextSymbol();
					type1 = expression();
					exprAnalyze.push_back("#dim");
					exprAnalyze.push_back("#arr");

					if (type1 != "INTTK") {
						error(getCurLineNo(), 'i', "index of array should be integer");
					}
					nextSymbol();
					if (checkRBRACK()) {
						nextSymbol();
						if (symbol == "ASSIGN") {
							nextSymbol();
							expression();
						}
					}
				}
			}
		}
		exprAnalyze.push_back("=");
		expressionAnalyze();
		insertToOutputVec("<赋值语句>");
	}
}
void SyntaxAnalyzer::condition() {
	exprAnalyze.push_back("#expr1");
	string type1 = expression();
	exprAnalyze.push_back("=");
	expressionAnalyze();

	nextSymbol();
	if (symbol == "LSS" || symbol == "LEQ" || symbol == "GRE" || symbol == "GEQ" || symbol == "EQL" || symbol == "NEQ") {
		vector<string> v;
		v.push_back(symbol);
		v.push_back("#expr1");
		v.push_back("#expr2");

		nextSymbol();
		exprAnalyze.push_back("#expr2");
		string type2 = expression(); 
		exprAnalyze.push_back("=");
		expressionAnalyze();

		intermediateCodeVec.push_back(v);

		if (type1 == "INTTK" && type2 == "INTTK") {
			insertToOutputVec("<条件>");
		}
		else {
			error(getCurLineNo(), 'f', "incompatible elements in condition statement");
		}
	}
	else {
		error(getLastLineNo(), '\0', "missing compare operator");
	}
}
void SyntaxAnalyzer::conditionalStatement() {
	if (symbol == "IFTK") {
		nextSymbol();
		if (symbol == "LPARENT") {
			nextSymbol();
			condition();

			intermediateCodeVec.back().push_back("BF");
			intermediateCodeVec.back().push_back("LABEL" + to_string(labelcnt));

			nextSymbol();
			if (checkRPARENT()) {
				nextSymbol();
				int lastlabelcnt = labelcnt;
				labelcnt += 2;
				statement();

				vector<string> v;
				v.push_back("GOTO");
				v.push_back("LABEL" + to_string(lastlabelcnt + 1));
				intermediateCodeVec.push_back(v);

				nextSymbol();

				v.clear();
				v.push_back("SET");
				v.push_back("LABEL" + to_string(lastlabelcnt));
				intermediateCodeVec.push_back(v);

				if (symbol == "ELSETK") {
					nextSymbol();
					statement();
				}
				else {
					retractSymbol();
				}

				v.clear();
				v.push_back("SET");
				v.push_back("LABEL" + to_string(lastlabelcnt + 1));
				intermediateCodeVec.push_back(v);

				insertToOutputVec("<条件语句>");
			}
		}
	}
}
void SyntaxAnalyzer::loopStatement() {
	if (symbol == "WHILETK") {
		nextSymbol();
		if (symbol == "LPARENT") {
			nextSymbol();

			vector<string> inter;
			inter.push_back("SET");
			inter.push_back("LABEL" + to_string(labelcnt));
			intermediateCodeVec.push_back(inter);

			condition();

			intermediateCodeVec.back().push_back("BF");
			intermediateCodeVec.back().push_back("LABEL" + to_string(labelcnt + 1));

			nextSymbol();
			if (checkRPARENT()) {
				int lastlabelcnt = labelcnt;
				labelcnt += 2;
				nextSymbol();
				statement();

				inter.clear();
				inter.push_back("GOTO");
				inter.push_back("LABEL" + to_string(lastlabelcnt));
				intermediateCodeVec.push_back(inter);

				inter.clear();
				inter.push_back("SET");
				inter.push_back("LABEL" + to_string(lastlabelcnt + 1));
				intermediateCodeVec.push_back(inter);

				insertToOutputVec("<循环语句>");
			}
		}
	}
	else if (symbol == "FORTK") {
		nextSymbol();
		if (symbol == "LPARENT") {
			nextSymbol();
			if (symbol == "IDENFR") {
				exprAnalyze.push_back(lowtoken);
				
				checkIdenfrHasDefined(lowtoken);
				nextSymbol();
				if (symbol == "ASSIGN") {
					nextSymbol();
					expression();

					exprAnalyze.push_back("=");
					expressionAnalyze();

					nextSymbol();
					if (checkSEMICN()) {
						nextSymbol();

						vector<string> inter;
						inter.push_back("SET");
						inter.push_back("LABEL" + to_string(labelcnt));
						intermediateCodeVec.push_back(inter);

						condition();

						intermediateCodeVec.back().push_back("BF");
						intermediateCodeVec.back().push_back("LABEL" + to_string(labelcnt + 1));

						nextSymbol();
						if (checkSEMICN()) {
							nextSymbol();
							if (symbol == "IDENFR") {
								string steplname = lowtoken;

								checkIdenfrHasDefined(lowtoken);
								nextSymbol();
								if (symbol == "ASSIGN") {
									nextSymbol();
									if (symbol == "IDENFR") {
										string steprname = lowtoken;

										checkIdenfrHasDefined(lowtoken);
										nextSymbol();
										if (symbol == "PLUS" || symbol == "MINU") {
											string op = symbol;
											nextSymbol();
											int steplen = stepLength();

											vector<string> step1;
											step1.push_back(op);
											step1.push_back("$t2");
											step1.push_back("=");
											step1.push_back(steprname);
											if (op == "PLUS") {
												step1.push_back("+");
											}
											else {
												step1.push_back("-");
											}
											step1.push_back(to_string(steplen));
											vector<string> step2;
											step2.push_back("ASSIGN");
											step2.push_back(steprname);
											step2.push_back("=");
											step2.push_back("$t2");

											nextSymbol();
											if (checkRPARENT()) {
												int lastlabelcnt = labelcnt;
												labelcnt += 2;

												nextSymbol();
												statement();

												intermediateCodeVec.push_back(step1);
												intermediateCodeVec.push_back(step2);

												inter.clear();
												inter.push_back("GOTO");
												inter.push_back("LABEL" + to_string(lastlabelcnt));
												intermediateCodeVec.push_back(inter);

												inter.clear();
												inter.push_back("SET");
												inter.push_back("LABEL" + to_string(lastlabelcnt + 1));
												intermediateCodeVec.push_back(inter);

												//labelcnt += 2;

												insertToOutputVec("<循环语句>");
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
void SyntaxAnalyzer::defaultChildStatement() {
	if (symbol == "DEFAULTTK") {
		nextSymbol();
		if (symbol == "COLON") {
			nextSymbol();

			vector<string> inter;
			inter.push_back("GOTO");
			inter.push_back("LABEL" + to_string(labelcnt));
			tempIntermediateCodeVec.push_back(inter);

			inter.clear();
			inter.push_back("SET");
			inter.push_back("LABEL" + to_string(labelcnt));
			intermediateCodeVec.push_back(inter);
			
			labelcnt++;

			statement();

			insertToOutputVec("<缺省>");
		}
	}
	else {
		error(getCurLineNo(), 'p', "missing DEFAULTTK");
		//skip(vector<string>{string("RBRACE")});
		retractSymbol();
	}
}
void SyntaxAnalyzer::caseChildStatement(string type) {
	if (symbol == "CASETK") {
		vector<string> inter;

		nextSymbol();
		string constr = constant(type);

		inter.clear();
		inter.push_back("EQL");
		inter.push_back("#expr1");
		inter.push_back(constr);
		inter.push_back("BT");
		inter.push_back("LABEL" + to_string(labelcnt));
		tempIntermediateCodeVec.push_back(inter);

		nextSymbol();
		if (symbol == "COLON") {
			nextSymbol();

			inter.clear();
			inter.push_back("SET");
			inter.push_back("LABEL" + to_string(labelcnt));
			intermediateCodeVec.push_back(inter);
			labelcnt++;

			statement();

			inter.clear();
			inter.push_back("GOTO");
			inter.push_back("LABEL" + to_string(caselabel0));
			intermediateCodeVec.push_back(inter);

			insertToOutputVec("<情况子语句>");
		}
	}
}
void SyntaxAnalyzer::caseTable(string type) {
	caseChildStatement(type);
	nextSymbol();

	while (symbol == "CASETK") {
		caseChildStatement(type);
		nextSymbol();
	}

	retractSymbol();
	insertToOutputVec("<情况表>");
}
void SyntaxAnalyzer::caseStatement() {
	if (symbol == "SWITCHTK") {
		nextSymbol();
		if (symbol == "LPARENT") {
			nextSymbol();

			exprAnalyze.push_back("#expr1");

			string type = expression();

			exprAnalyze.push_back("=");
			expressionAnalyze();

			nextSymbol();
			if (checkRPARENT()) {
				nextSymbol();
				if (symbol == "LBRACE") {
					nextSymbol();

					caselabel0 = labelcnt;
					labelcnt++;
					tempIndex = intermediateCodeVec.size();

					caseTable(type);
					nextSymbol();
					defaultChildStatement();

					vector<string> inter;
					inter.push_back("SET");
					inter.push_back("LABEL" + to_string(caselabel0));
					intermediateCodeVec.push_back(inter);

					intermediateCodeVec.insert(intermediateCodeVec.begin() + tempIndex, tempIntermediateCodeVec.begin(), tempIntermediateCodeVec.end());
					tempIntermediateCodeVec.clear();

					nextSymbol();
					if (checkRBRACE()) {
						insertToOutputVec("<情况语句>");
					}
				}
			}
		}
	}
}
void SyntaxAnalyzer::readStatement() {
	if (symbol == "SCANFTK") {
		nextSymbol();
		if (symbol == "LPARENT") {
			nextSymbol();
			if (symbol == "IDENFR") {
				string name = lowtoken;
				vector<string> inter;
				inter.push_back("SCANF");
				inter.push_back(name);
				intermediateCodeVec.push_back(inter);
				string kind = getKind(lowtoken);
				if (kind == "CONSTTK") {
					error(getCurLineNo(), 'j', "cannot scanf element to const");
				}
				nextSymbol();
				if (checkRPARENT()) {
					insertToOutputVec("<读语句>");
				}
			}
		}
	}
}
void SyntaxAnalyzer::writeStatement() {
	if (symbol == "PRINTFTK") {
		nextSymbol();
		if (symbol == "LPARENT") {
			nextSymbol();
			if (symbol == "STRCON") {
				string str = token;
				charString();
				nextSymbol();
				if (symbol == "COMMA") {
					nextSymbol();

					exprAnalyze.push_back("#expr1");

					string type = expression();

					exprAnalyze.push_back("=");
					expressionAnalyze();

					vector<string> v;
					v.push_back("PRINTF");
					v.push_back(str);
					v.push_back("#expr1");
					v.push_back(type);
					intermediateCodeVec.push_back(v);
					nextSymbol();
				}
				else {
					vector<string> v;
					v.push_back("PRINTF");
					v.push_back(str);
					intermediateCodeVec.push_back(v);
				}
				if (checkRPARENT()) {
					insertToOutputVec("<写语句>");
				}
			}
			else {
				exprAnalyze.push_back("#expr1");
				string type = expression();
				exprAnalyze.push_back("=");
				expressionAnalyze();
				vector<string> v;
				v.push_back("PRINTF");
				v.push_back("#expr1");
				v.push_back(type);
				intermediateCodeVec.push_back(v);
				nextSymbol();
				if (checkRPARENT()) {
					insertToOutputVec("<写语句>");
				}
			}
		}
	}
}
void SyntaxAnalyzer::returnStatement() {
	if (symbol == "RETURNTK") {
		nextSymbol();
		if (symbol == "LPARENT") {
			if (funcType == "VOIDTK") {
				error(getCurLineNo(), 'g', "return statement with expression in void function");
				skip(vector<string>{string("SEMICN")});
				retractSymbol();
				return;
			}
			nextSymbol();
			if (symbol == "RPARENT") {
				error(getCurLineNo(), 'h', "RETURNFUNCTION expecting return expression");
			}
			else {
				exprAnalyze.clear();
				exprAnalyze.push_back("#expr1");

				string type1 = expression();

				exprAnalyze.push_back("=");
				expressionAnalyze();

				vector<string> inter;
				inter.push_back("RET");
				inter.push_back("#expr1");
				intermediateCodeVec.push_back(inter);

				if (funcType != type1) {	//functype is INTTK / CHARTK
					error(getCurLineNo(), 'h', "incompatible return type");
				}
				nextSymbol();
				checkRPARENT();
			}
		}
		else {
			if (funcType != "VOIDTK") {
				error(getCurLineNo(), 'h', "RETURNFUNCTION expecting return expression");
			}

			vector<string> inter;
			inter.push_back("RET");
			intermediateCodeVec.push_back(inter);

			retractSymbol();
		}

		returned = true;
		insertToOutputVec("<返回语句>");
	}
}
void SyntaxAnalyzer::statementList() {
	while (symbol != "RBRACE") {
		statement();
		nextSymbol();
	}
	retractSymbol();
	insertToOutputVec("<语句列>");
}
void SyntaxAnalyzer::statement() {
	if (symbol == "WHILETK" || symbol == "FORTK") {
		loopStatement();
		insertToOutputVec("<语句>");
	}
	else if (symbol == "IFTK") {
		conditionalStatement();
		insertToOutputVec("<语句>");
	}
	else if (symbol == "SWITCHTK") {
		caseStatement();
		insertToOutputVec("<语句>");
	}
	else if (symbol == "IDENFR") {
		nextSymbol();
		if (symbol == "LPARENT") {
			retractSymbol();
			callFunction();
		}
		else if (symbol == "ASSIGN") {
			retractSymbol();	//retract assign
			assignStatement();
		}
		else {
			retractSymbol();	//retract assign
			assignStatement();	//maybe its assignment of array
		}
		nextSymbol();
		if (checkSEMICN()) {
			insertToOutputVec("<语句>");
		}
	}
	else if (symbol == "SCANFTK") {
		readStatement();
		nextSymbol();
		if (checkSEMICN()) {
			insertToOutputVec("<语句>");
		}
	}
	else if (symbol == "PRINTFTK") {
		writeStatement();
		nextSymbol();
		if (checkSEMICN()) {
			insertToOutputVec("<语句>");
		}
	}
	else if (symbol == "RETURNTK") {
		returnStatement();
		nextSymbol();
		if (checkSEMICN()) {
			insertToOutputVec("<语句>");
		}
	}
	else if (symbol == "LBRACE") {
		nextSymbol();
		statementList();
		nextSymbol();
		if (checkRBRACE()) {
			insertToOutputVec("<语句>");
		}
	}
	else if (symbol == "SEMICN") {
		insertToOutputVec("<语句>");
	}
	else {
		error(getCurLineNo(), '\0', "not a proper statement");
	}
}
void SyntaxAnalyzer::varDeclare() {
	varDefine();
	nextSymbol();
	if (checkSEMICN()) {
		nextSymbol();
		while (symbol == "INTTK" || symbol == "CHARTK") {
			nextSymbol();
			if (symbol == "IDENFR") {
				nextSymbol();
				if (symbol == "LPARENT") {
					retractSymbol();	//retract (
					retractSymbol();	//retract idenfr
					break;
				}
				else {
					retractSymbol(); // retract ; or =
					retractSymbol();	//retract IDENFR
					varDefine();
					nextSymbol();
					if (checkSEMICN()) {
						nextSymbol();
					}
				}
			}
		}
		retractSymbol();
		insertToOutputVec("<变量说明>");
	}
}
void SyntaxAnalyzer::varDefineNoInit(string type) {
	while (symbol == "COMMA") {
		nextSymbol();
		if (symbol == "IDENFR") {
			string name = lowtoken;
			bool dup = checkDupIdenfr(name, level);
			nextSymbol();
			if (symbol == "LBRACK") {
				nextSymbol();
				int dim1 = unsignedInteger();
				nextSymbol();
				if (checkRBRACK()) {
					nextSymbol();
					if (symbol == "LBRACK") {
						nextSymbol();
						int dim2 = unsignedInteger();

						vector<string> inter;	//2 dimension array
						inter.push_back("ARRAY");
						inter.push_back("2");
						inter.push_back("NOTINIT");
						inter.push_back(type);
						inter.push_back(name);
						inter.push_back(to_string(dim1));
						inter.push_back(to_string(dim2));
						intermediateCodeVec.push_back(inter);

						if (!dup) {
							symbolTable.insert(name, "VAR", "ARRAY", level);
							symbolTable.setArrayAttr(type, vector<int>{dim1, dim2});
						}
						nextSymbol();
						if (checkRBRACK()) {
							nextSymbol();
						}
					}
					else {
						vector<string> inter;	//one dimension array
						inter.push_back("ARRAY");
						inter.push_back("1");
						inter.push_back("NOTINIT");
						inter.push_back(type);
						inter.push_back(name);
						inter.push_back(to_string(dim1));
						intermediateCodeVec.push_back(inter);
						if (!dup) {
							symbolTable.insert(name, "VAR", "ARRAY", level);
							symbolTable.setArrayAttr(type, vector<int>{dim1});
						}
					}
				}
			}
			else {
				vector<string> inter;
				inter.push_back("VAR");
				inter.push_back(type);
				inter.push_back(name);
				intermediateCodeVec.push_back(inter);
				if (!dup) {
					symbolTable.insert(name, "VAR", type, level);
				}
			}
		}
	}
	retractSymbol();
	insertToOutputVec("<变量定义无初始化>");
}
void SyntaxAnalyzer::varDefineInit() {
	if (symbol == "INTTK" || symbol == "CHARTK") {
		string type = symbol;	//attribute in symbol table
		nextSymbol();
		if (symbol == "IDENFR") {
			string name = lowtoken;
			bool dup = checkDupIdenfr(name, level);
			nextSymbol();
			if (symbol == "ASSIGN") {
				if (!dup) {
					symbolTable.insert(name, "VAR", type, level);
				}
				nextSymbol();
				string constStr = constant(type);
				vector<string> inter;
				inter.push_back("VAR");
				inter.push_back(type);
				inter.push_back(name);
				inter.push_back("=");
				inter.push_back(constStr);
				intermediateCodeVec.push_back(inter);
				insertToOutputVec("<变量定义及初始化>");
			}
			else if (symbol == "LBRACK") {
				nextSymbol();
				int dim1 = unsignedInteger();
				nextSymbol();
				if (checkRBRACK()) {
					nextSymbol();
					if (symbol == "ASSIGN") {
						vector<string> inter;	//one dimension array
						inter.push_back("ARRAY");
						inter.push_back("1");
						inter.push_back("INIT");
						inter.push_back(type);
						inter.push_back(name);
						inter.push_back(to_string(dim1));

						if (!dup) {
							symbolTable.insert(name, "VAR", "ARRAY", level);
							symbolTable.setArrayAttr(type, vector<int>{dim1});
						}
						nextSymbol();
						if (symbol == "LBRACE") {
							int cnt = 0;
							nextSymbol();
							string cons = constant(type);
							inter.push_back(cons);

							cnt++;
							nextSymbol();
							while (symbol == "COMMA") {
								nextSymbol();
								cons = constant(type);
								inter.push_back(cons);

								cnt++;
								nextSymbol();
								if (cnt > dim1) {
									error(getCurLineNo(), 'n', "number of elements initialized in array not compatible with number of elements declared");
									skip(vector<string>{string("SEMICN")});
									retractSymbol();
									break;
								}
							}
							if (cnt < dim1) {
								error(getCurLineNo(), 'n', "number of elements initialized in array not compatible with number of elements declared");
							}
							if (checkRBRACE()) {

								intermediateCodeVec.push_back(inter);

								insertToOutputVec("<变量定义及初始化>");
							}
						}
					}
					else if (symbol == "LBRACK") {
						nextSymbol();
						int dim2 = unsignedInteger();
						nextSymbol();
						if (checkRBRACK()) {
							nextSymbol();
							if (symbol == "ASSIGN") {
								vector<string> inter;	//2 dimension array
								inter.push_back("ARRAY");
								inter.push_back("2");
								inter.push_back("INIT");
								inter.push_back(type);
								inter.push_back(name);
								inter.push_back(to_string(dim1));
								inter.push_back(to_string(dim2));

								if (!dup) {
									symbolTable.insert(name, "VAR", "ARRAY", level);
									symbolTable.setArrayAttr(type, vector<int>{dim1, dim2});
								}
								nextSymbol();
								if (symbol == "LBRACE") {
									int cnt1 = 0;
									nextSymbol();
									while (symbol == "LBRACE") {
										cnt1++;
										int cnt2 = 1;
										nextSymbol();
										string cons = constant(type);
										inter.push_back(cons);

										nextSymbol();
										while (symbol == "COMMA") {
											nextSymbol();
											cons = constant(type);
											inter.push_back(cons);

											cnt2++;
											nextSymbol();
											if (cnt2 > dim2) {
												error(getCurLineNo(), 'n', "number of elements initialized in array not compatible with number of elements declared");
												skip(vector<string>{string("SEMICN")});
												retractSymbol();
												break;
											}
										}
										if (checkRBRACE()) {
											nextSymbol();
											if (symbol == "COMMA") {
												if (cnt2 != dim2) {
													error(getCurLineNo(), 'n', "number of elements initialized in array not compatible with number of elements declared");
													skip(vector<string>{string("SEMICN")});
													retractSymbol();
												}
												else {
													nextSymbol();
												}
											}
											else if (checkRBRACE()) {
												if (cnt2 != dim2 || cnt1 != dim1) {
													error(getCurLineNo(), 'n', "number of elements initialized in array not compatible with number of elements declared");
												}
												else {
													intermediateCodeVec.push_back(inter);
													insertToOutputVec("<变量定义及初始化>");
												}
											}
										}
									}
								}
							}
							else {
								vector<string> inter;	//2 dimension array
								inter.push_back("ARRAY");
								inter.push_back("2");
								inter.push_back("NOTINIT");
								inter.push_back(type);
								inter.push_back(name);
								inter.push_back(to_string(dim1));
								inter.push_back(to_string(dim2));
								intermediateCodeVec.push_back(inter);
								if (!dup) {
									symbolTable.insert(name, "VAR", "ARRAY", level);
									symbolTable.setArrayAttr(type, vector<int>{dim1, dim2});
								}
								varDefineNoInit(type);
							}
						}
					}
					else {
						vector<string> inter;	//1 dimension array
						inter.push_back("ARRAY");
						inter.push_back("1");
						inter.push_back("NOTINIT");
						inter.push_back(type);
						inter.push_back(name);
						inter.push_back(to_string(dim1));
						intermediateCodeVec.push_back(inter);
						if (!dup) {
							symbolTable.insert(name, "VAR", "ARRAY", level);
							symbolTable.setArrayAttr(type, vector<int>{dim1});
						}
						varDefineNoInit(type);
					}
				}
			}
			else {
				vector<string> inter;
				inter.push_back("VAR");
				inter.push_back(type);
				inter.push_back(name);
				intermediateCodeVec.push_back(inter);
				if (!dup) {
					symbolTable.insert(name, "VAR", type, level);
				}
				varDefineNoInit(type);
			}
		}
	}
}
void SyntaxAnalyzer::varDefine() {
	varDefineInit();
	insertToOutputVec("<变量定义>");
}
void SyntaxAnalyzer::constDefine() {
	bool dup;
	if (symbol == "INTTK") {
		do {
			nextSymbol();
			if (symbol == "IDENFR") {
				string name = lowtoken;
				dup = checkDupIdenfr(lowtoken, level);
				
				nextSymbol();
				if (symbol == "ASSIGN") {
					nextSymbol();
					string intStr = integer();
					if (!dup) {
						symbolTable.insert(name, "CONSTTK", "INTTK", level, intStr);
					}
					vector<string> inter;
					inter.push_back("CONST");
					inter.push_back("INTTK");
					inter.push_back(name);
					inter.push_back("=");
					inter.push_back(intStr);
					intermediateCodeVec.push_back(inter);
					nextSymbol();
				}
			}
		} while (symbol == "COMMA");
		retractSymbol();
		insertToOutputVec("<常量定义>");
	}
	else if (symbol == "CHARTK") {
		do {
			nextSymbol();
			if (symbol == "IDENFR") {
				string name = lowtoken;
				dup = checkDupIdenfr(lowtoken, level);
				
				nextSymbol();
				if (symbol == "ASSIGN") {
					nextSymbol();
					if (symbol == "CHARCON") {
						if (!dup) {
							symbolTable.insert(name, "CONSTTK", "CHARTK", level, lowtoken);
						}
						vector<string> inter;
						inter.push_back("CONST");
						inter.push_back("CHARTK");
						inter.push_back(name);
						inter.push_back("=");
						inter.push_back(lowtoken);
						intermediateCodeVec.push_back(inter);
						nextSymbol();
					}
				}
			}
		} while (symbol == "COMMA");
		retractSymbol();
		insertToOutputVec("<常量定义>");
	}
}
void SyntaxAnalyzer::constDeclare() {
	if (symbol == "CONSTTK") {
		nextSymbol();
		constDefine();
		nextSymbol();
		if (checkSEMICN()) {
			nextSymbol();
			while (symbol == "CONSTTK") {
				nextSymbol();
				constDefine();
				nextSymbol();
				checkSEMICN();
				nextSymbol();
			}
			retractSymbol();
			insertToOutputVec("<常量说明>");
		}
	}
}
void SyntaxAnalyzer::parameterTable(string funcName, bool funcDup) {
	vector<string> param;
	bool dup;
	if (symbol == "INTTK" || symbol == "CHARTK") {
 		string type = symbol;
		param.push_back(symbol);
		nextSymbol();
		if (symbol == "IDENFR") {
			string paraName = lowtoken;
			dup = checkDupIdenfr(lowtoken, 1);
			if (!dup) {
				symbolTable.insert(lowtoken, "PARA", type, 1);
			}
			vector<string> inter;
			inter.push_back("PARA");
			inter.push_back(type);
			inter.push_back(paraName);
			intermediateCodeVec.push_back(inter);

			nextSymbol();
			while (symbol == "COMMA") {
				nextSymbol();
				if (symbol == "INTTK" || symbol == "CHARTK") {
					type = symbol;
					param.push_back(symbol);
					nextSymbol();
					if (symbol == "IDENFR") {
						paraName = lowtoken;
						dup = checkDupIdenfr(lowtoken, 1);
						if (!dup) {
							symbolTable.insert(lowtoken, "PARA", type, 1);
						}

						vector<string> inter;
						inter.push_back("PARA");
						inter.push_back(type);
						inter.push_back(paraName);
						intermediateCodeVec.push_back(inter);

						nextSymbol();
					}
				}
			}
			retractSymbol();
			insertToOutputVec("<参数表>");
		}
		if (!funcDup) {
			symbolTable.setFuncParam(funcName, param);
		}
	}
	else {	//参数表为空
		if (symbol == "RPARENT") {
			retractSymbol();
			insertToOutputVec("<参数表>");
		}
		else {
			retractSymbol();
		}
	}
}
void SyntaxAnalyzer::compoundStatement() {
	if (symbol == "CONSTTK") {
		constDeclare();
		nextSymbol();
	}
	if (symbol == "INTTK" || symbol == "CHARTK") {
		varDeclare();
		nextSymbol();
	}
	statementList();
	insertToOutputVec("<复合语句>");
}
void SyntaxAnalyzer::functionReturnDefine() {
	string funcName;
	bool dup = false;
	if (symbol == "INTTK" || symbol == "CHARTK") {	//declareHead
		returned = false;
		level = 1;
		funcType = symbol;	//functype(global var), used in return statement
		nextSymbol();
		if (symbol == "IDENFR") {
			funcName = lowtoken;

			vector<string> inter;
			inter.push_back("FUNCTION");
			inter.push_back(funcName);
			inter.push_back(funcType);
			intermediateCodeVec.push_back(inter);

			dup = checkDupIdenfr(funcName, 0);
			symbolTable.insert(funcName, "RETURNFUNCTION", funcType, 0);
			insertToOutputVec("<声明头部>");
		}
	}
	nextSymbol();
	if (symbol == "LPARENT") {
		nextSymbol();
		if (symbol == "LBRACE") {	//parameter is empty and missing RPARENT
			checkRPARENT();	//give and error on missing RPARENT
			nextSymbol(); //symbol is now LBRACE
		}
		else {
			parameterTable(funcName, dup);
			nextSymbol();
			checkRPARENT();
			nextSymbol();
		}
		if (symbol == "LBRACE") {
			nextSymbol();
			compoundStatement();
			nextSymbol();
			if (checkRBRACE()) {
				insertToOutputVec("<有返回值函数定义>");
			}
		}
		
	}
	if (returned == false) {
		error(getCurLineNo(), 'h', "missing RETURNTK");
	}
	symbolTable.clearFuncScope(funcName, dup);
}
void SyntaxAnalyzer::functionNoReturnDefine() {
	string funcName;
	bool dup = false;
	if (symbol == "VOIDTK") {
		string type = symbol;
		funcType = type;
		nextSymbol();
		if (symbol == "IDENFR") {
			funcName = lowtoken;

			vector<string> inter;
			inter.push_back("FUNCTION");
			inter.push_back(funcName);
			inter.push_back(funcType);
			intermediateCodeVec.push_back(inter);

			dup = checkDupIdenfr(funcName, 0);
			symbolTable.insert(funcName, "VOIDFUNCTION", "VOIDTK", 0);
			level = 1;
			nextSymbol();
			if (symbol == "LPARENT") {
				nextSymbol();
				if (symbol == "LBRACE") {	//parameter is empty and missing RPARENT
 					checkRPARENT();	//get an missing RPARENT error
					nextSymbol(); //symbol is now LBRACE
				}
				else {
					parameterTable(funcName, dup);
					nextSymbol();
					checkRPARENT();
					nextSymbol();
				}
				if (symbol == "LBRACE") {
					nextSymbol();
					compoundStatement();
					nextSymbol();
					if (checkRBRACE()) {
						insertToOutputVec("<无返回值函数定义>");
					}
				}
			}
		}
	}
	symbolTable.clearFuncScope(funcName, dup);
}
void SyntaxAnalyzer::mainFunction() {
	if (symbol == "VOIDTK") {
		string type = symbol;
		funcType = type;
		nextSymbol();
		if (symbol == "MAINTK") {
			vector<string> inter;
			inter.push_back("FUNCTION");
			inter.push_back("main");
			inter.push_back("VOIDTK"); //return type
			intermediateCodeVec.push_back(inter);

 			symbolTable.insert(lowtoken, "VOIDFUNCTION", "VOIDTK", 0);
			level = 1;
			nextSymbol();
			if (symbol == "LPARENT") {
				nextSymbol();
				if (checkRPARENT()) {
					nextSymbol();
					if (symbol == "LBRACE") {
						nextSymbol();
						compoundStatement();
						nextSymbol();
						if (checkRBRACE()) {
							insertToOutputVec("<主函数>");
							symbolTable.clearFuncScope("main", 0);
						}
					}
				}
			}
		}
	}
}
void SyntaxAnalyzer::program() {
	if (symbol == "CONSTTK") {
		constDeclare();
		nextSymbol();
	}
	if (symbol == "CHARTK" || symbol == "INTTK") {
		nextSymbol();
		if (symbol == "IDENFR") {
			nextSymbol();
			if (symbol != "LPARENT") {	//if its lparent, then this is a function
				retractSymbol();
				retractSymbol();
				varDeclare();
				nextSymbol();
			}
			else {
				retractSymbol();
				retractSymbol();
			}
		}
	}
	while (symbol == "CHARTK" || symbol == "INTTK" || symbol == "VOIDTK") {
		if (symbol == "CHARTK" || symbol == "INTTK") {
			functionReturnDefine();
			nextSymbol();
		}
		if (symbol == "VOIDTK") {
			nextSymbol();
			if (symbol == "MAINTK") {
				retractSymbol(); //retract maintk
				mainFunction();
			}
			else {
				retractSymbol();	//retract idenfr
				functionNoReturnDefine();
				nextSymbol();
			}
		}
	}
	insertToOutputVec("<程序>");
}

SyntaxAnalyzer::SyntaxAnalyzer(vector<Token>& Tokens) {
	idx = -1;
	level = 0;
	tokenVec = Tokens;
	nextSymbol();
	program();
}