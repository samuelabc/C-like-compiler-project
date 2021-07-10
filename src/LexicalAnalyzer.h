#ifndef Tokenizer_h
#define Tokenizer_h

#include"Token.h"
#include <string>
#include <vector>
#include<unordered_map>
#include<cctype>
#include"Error.h"

using std::unordered_map;
using std::string;
using std::vector;

class LexicalAnalyzer {
public:
    vector<Token> tokenVec;
    vector<Error> errorVec;
    LexicalAnalyzer(vector<char>& input);

private:
    unordered_map<string, string> reserveMap;
    string curtoken;
    string curlowtoken;
    string cursymbol;
    char currchar;
    int curlineno;
    int curcharno;
    int idx;	//index of inputVec
    vector<char> inputVec;

    void error(int line, char type, const char* description);
    bool isLetter();
    bool isDigit();
    bool isChar();
    bool isString();
    void catToken();
    void clearToken();
    void clearSymbol();
    void retract();
    void nextChar();
    string reserver();
    int transNum();
    bool getsym();
    void createReserveMap();
};

#endif