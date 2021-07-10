#ifndef Token_h
#define Token_h

#include<string>
using std::string;

class Token {
public:
	Token();
	Token(string token, string lowtoken, string symbol, int line);
	string getToken();
	string getLowToken();
	string getSymbol();
	int getLine();
	void setToken(string token);
	void setLowToken(string lowtoken);
	void setSymbol(string symbol);
	void setLine(int line);

private:
	string token;
	string lowtoken;
	string symbol;
	int line;
};

#endif // !Token_hpp
