#include"Token.h"

Token::Token() {}
Token::Token(string token, string lowtoken, string symbol, int line) {
	Token::token = token;
	Token::lowtoken = lowtoken;
	Token::symbol = symbol;
	Token::line = line;
}
string Token::getToken() {
	return token;
}
string Token::getLowToken() {
	return lowtoken;
}
string Token::getSymbol() {
	return symbol;
}
int Token::getLine() {
	return line;
}
void Token::setToken(string token) {
	Token::token = token;
}
void Token::setLowToken(string lowtoken) {
	Token::lowtoken = lowtoken;
}
void Token::setSymbol(string symbol) {
	Token::symbol = symbol;
}
void Token::setLine(int line) {
	Token::line = line;
}