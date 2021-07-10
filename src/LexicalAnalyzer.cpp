#include"LexicalAnalyzer.h"

#define debug 0
#include<iostream>
using std::cout;
using std::endl;
void LexicalAnalyzer::error(int line, char type, const char* description) {
	Error err(line, type, description);
	errorVec.push_back(err);
	if (debug) {
		cout << "debug: something wrong.";
		cout << "line " << line;
		cout << "type " << type;
		cout << " currchar: " << currchar;
		cout << " token: " << curtoken;
		cout << " symbol " << cursymbol << " " << curlineno << " " << curcharno << endl;
	}
}

bool LexicalAnalyzer::isLetter() {
	return (currchar == '_' || (currchar >= 'a' && currchar <= 'z') || (currchar >= 'A' && currchar <= 'Z')) ? true : false;
}
bool LexicalAnalyzer::isDigit() {
	return (currchar >= '0' && currchar <= '9') ? true : false;
}
bool LexicalAnalyzer::isChar() {
	return (currchar == '+' || currchar == '-' || currchar == '*' || currchar == '/' || isLetter() || isDigit()) ? true : false;
}
bool LexicalAnalyzer::isString() {
	return (currchar == 32 || currchar == 33 || (currchar >= 35 && currchar <= 126)) ? true : false;
}

void LexicalAnalyzer::catToken() {
	curtoken.push_back(currchar);
	curlowtoken.push_back(currchar);
}
void LexicalAnalyzer::clearToken() {
	curtoken.clear();
	curlowtoken.clear();
}
void LexicalAnalyzer::clearSymbol() {
	cursymbol.clear();
}
void LexicalAnalyzer::retract() {
	if (currchar == '\n') {
		curlineno--;
	}
	else {
		curcharno--;
	}
	idx--;
}
void LexicalAnalyzer::nextChar() {
	idx++;
	if (idx < inputVec.size()) {
		currchar = inputVec[idx];
	}
	else {
		currchar = '\0';
	}
	if (currchar == '\n') {
		curlineno++;
		curcharno = 1;
	}
	else {
		curcharno++;
	}
}
string LexicalAnalyzer::reserver() {
	curlowtoken.clear();
	int dist = 'a' - 'A';
	for (int i = 0; i < (int)curtoken.size(); i++) {
		if (curtoken[i] >= 'A' && curtoken[i] <= 'Z') {
			curlowtoken.push_back(curtoken[i] + dist);
		}
		else {
			curlowtoken.push_back(curtoken[i]);
		}
	}
	auto it = reserveMap.find(curlowtoken);
	string retSym;
	if (it == reserveMap.end()) {
		retSym = "";
	}
	else {
		retSym = it->second;
	}
	return retSym;
}
int LexicalAnalyzer::transNum() {
	int num = 0;
	for (int i = 0; i < (int)curtoken.size(); i++) {
		num *= 10;
		num += curtoken[i];
	}
	return num;
}
bool LexicalAnalyzer::getsym() {
	nextChar();
	while (isspace(currchar)) {
		nextChar();
	}
	if (isLetter()) {
		catToken();
		nextChar();
		while (isLetter() || isDigit()) {
			catToken();
			nextChar();
		}
		retract();
		string resultstr = reserver();
		if (resultstr == "") {
			cursymbol = "IDENFR";
		}
		else {
			cursymbol = resultstr;
		}
	}
	else if (isDigit()) {
		while (isDigit()) {
			catToken();
			nextChar();
		}
		retract();
		int num = transNum();
		cursymbol = "INTCON";
	}
	else if (currchar == '\'') {
		nextChar();
		if (isChar()) {
			catToken();
			cursymbol = "CHARCON";
			nextChar();
			if (!(currchar == '\'')) {
				error(curlineno, 'a', "too much element between ' '");
			}
			else {
				curtoken = "'" + curtoken + "'";
				curlowtoken = "'" + curlowtoken + "'";
			}
		}
		else if (currchar == '\'') {
			error(curlineno, 'a', "empty char");
		}
		else {
			error(curlineno, 'a', "unknown character between ' '");
			nextChar();
			if (!(currchar == '\'')) {
				error(curlineno, 'a', "too much element between ' '");
			}
		}
	}
	else if (currchar == '"') {		// "hello world"
		nextChar();
		if (!isString()) {
			error(curlineno, 'a', "unknown character in string constant");
			//return false;
		}
		while (!(currchar == '"')) {
			catToken();
			nextChar();
			if (!isString() && !(currchar == '"')) {
				error(curlineno, 'a', "unknown character in string constant");
				//return false;
			}
		}
		curtoken = "\"" + curtoken + "\"";
		curlowtoken = "\"" + curlowtoken + "\"";
		cursymbol = "STRCON";
	}
	else if (currchar == '+') {
		catToken();
		cursymbol = "PLUS";	// +
	}
	else if (currchar == '-') {
		catToken();
		cursymbol = "MINU";	// -
	}
	else if (currchar == '*') {
		catToken();
		cursymbol = "MULT";	// *
	}
	else if (currchar == '/') {
		catToken();
		cursymbol = "DIV";		// /
	}
	else if (currchar == '<') {
		catToken();
		nextChar();
		if (currchar == '=') {
			catToken();
			cursymbol = "LEQ";	// <=
		}
		else {
			retract();
			cursymbol = "LSS";	// <
		}
	}
	else if (currchar == '>') {
		catToken();
		nextChar();
		if (currchar == '=') {
			catToken();
			cursymbol = "GEQ";	// >=
		}
		else {
			retract();
			cursymbol = "GRE";	// >
		}
	}
	else if (currchar == '=') {
		catToken();
		nextChar();
		if (currchar == '=') {
			catToken();
			cursymbol = "EQL";	// ==
		}
		else {
			retract();
			cursymbol = "ASSIGN";	// =
		}
	}
	else if (currchar == '!') {
		catToken();
		nextChar();
		if (currchar == '=') {
			catToken();
			cursymbol = "NEQ";	// !=
		}
		else {
			retract();
			//error();
			return false;
		}
	}
	else if (currchar == ':') {
		catToken();
		cursymbol = "COLON";	// :
	}
	else if (currchar == ';')
	{
		catToken();
		cursymbol = "SEMICN";	// ;
	}
	else if (currchar == ',') {
		catToken();
		cursymbol = "COMMA";	// ,
	}
	else if (currchar == '(') {
		catToken();
		cursymbol = "LPARENT"; 	// (
	}
	else if (currchar == ')') {
		catToken();
		cursymbol = "RPARENT";	// )
	}
	else if (currchar == '[') {
		catToken();
		cursymbol = "LBRACK";	// [
	}
	else if (currchar == ']') {
		catToken();
		cursymbol = "RBRACK";	// ]
	}
	else if (currchar == '{') {
		catToken();
		cursymbol = "LBRACE";	// {
	}
	else if (currchar == '}') {
		catToken();
		cursymbol = "RBRACE";	// }
	}
	else {
		//error();
		return false;
	}
	return true;
}
void LexicalAnalyzer::createReserveMap() {
	reserveMap["const"] = "CONSTTK";
	reserveMap["int"] = "INTTK";
	reserveMap["char"] = "CHARTK";
	reserveMap["void"] = "VOIDTK";
	reserveMap["main"] = "MAINTK";
	reserveMap["if"] = "IFTK";
	reserveMap["else"] = "ELSETK";
	reserveMap["switch"] = "SWITCHTK";
	reserveMap["case"] = "CASETK";
	reserveMap["default"] = "DEFAULTTK";
	reserveMap["while"] = "WHILETK";
	reserveMap["for"] = "FORTK";
	reserveMap["scanf"] = "SCANFTK";
	reserveMap["printf"] = "PRINTFTK";
	reserveMap["scanf"] = "SCANFTK";
	reserveMap["return"] = "RETURNTK";
}

LexicalAnalyzer::LexicalAnalyzer(vector<char>& input) {
	createReserveMap();
	inputVec = input;
	idx = -1;
	curlineno = 1;
	int len = input.size();
	while (idx < len) {
		clearSymbol();
		clearToken();
		if (getsym()) {
			tokenVec.push_back(Token(curtoken, curlowtoken, cursymbol, curlineno));
		}
	}
}