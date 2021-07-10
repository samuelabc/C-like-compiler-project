#ifndef SyntaxAnalyzer_h
#define SyntaxAnalyzer_h

#include "Token.h"
#include"SymbolTable.h"
#include"Error.h"

#include<vector>
#include<string>
#include<algorithm>
using std::vector;
using std::string;
using std::to_string;

class SyntaxAnalyzer {
public:
	SyntaxAnalyzer(vector<Token>& Tokens);
	vector<string> outputVec;
	vector<Error> errorVec;
	SymbolTable symbolTable;
	vector<vector<string>> intermediateCodeVec;

private:
	int idx;
	int level;
	vector<Token> tokenVec;
	string symbol;
	string token;
	string lowtoken;
	string funcType;
	bool returned;
	vector<string> exprAnalyze;
	int labelcnt = 0;
	int caselabel0 = 0;
	vector<vector<string>> tempIntermediateCodeVec;
	int tempIndex;
	int reg_count = 2;
	string reg_type = "$t";
	int temp_count = -1;
	int temp_max = -1;
	void updateRegCount();
	vector<string> terms;
	vector<string> pushVec;

	void expressionAnalyze();
	string isConstant(string attrname);
	bool isLocalVar(string attrname);

	void error(int line, char type, const char* description);
	bool checkDupIdenfr(string name, int level);
	bool checkIdenfrHasDefined(string name);
	string getType(string name);
	string getKind(string name);
	void skip(vector<string> sym);
	bool checkSEMICN(); 
	bool checkRPARENT();
	bool checkRBRACK();
	bool checkRBRACE();

	void insertToOutputVec(const char* str);
	void nextSymbol();
	void retractSymbol();
	int getLastLineNo();
	int getCurLineNo();

	void charString();	//＜字符串＞
	void program();	//＜程序＞
	void constDeclare(); //＜常量说明＞
	void constDefine(); //＜常量定义＞
	int unsignedInteger(); //＜无符号整数＞
	string integer(); //＜整数＞
	//string declareHead(); //<声明头部> moved to functionReturnDefine()
	string constant(string type); //<常量>
	void varDeclare(); //＜变量说明＞
	void varDefine();	//＜变量定义＞
	void varDefineNoInit(string type); //＜变量定义无初始化＞
	void varDefineInit(); //＜变量定义及初始化＞
	void functionReturnDefine(); //＜有返回值函数定义＞
	void functionNoReturnDefine(); //＜无返回值函数定义＞
	void compoundStatement(); //＜复合语句＞
	void parameterTable(string funcName, bool dup);	//＜参数表＞
	void mainFunction(); //＜主函数＞
	string expression(); //＜表达式＞
	string term(); //＜项＞
	string factor(); //＜因子＞
	void statement(); //＜语句＞
	void assignStatement(); //＜赋值语句＞
	void conditionalStatement(); //＜条件语句＞
	void condition(); //＜条件＞
	void loopStatement(); //＜循环语句＞
	int stepLength(); //＜步长＞
	void caseStatement(); //＜情况语句＞
	void caseTable(string type); //＜情况表＞
	void caseChildStatement(string type);  //＜情况子语句＞
	void defaultChildStatement(); //＜缺省＞
	//void callFunctionReturn(); //＜有返回值函数调用语句＞	moved to callFunction
	//void callFunctionNoReturn(); //＜无返回值函数调用语句＞  moved to callFunction
	string callFunction(); //＜有返回值函数调用语句＞ and <无返回值函数调用语句＞
	void paramValueTable(vector<string> param); //＜值参数表＞
	void statementList(); //＜语句列＞
	void readStatement(); //＜读语句＞
	void writeStatement(); //＜写语句＞
	void returnStatement(); //＜返回语句＞
};
#endif // !SyntaxAnalyzer_hpp
