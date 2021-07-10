#include"Error.h"
Error::Error() {

}
Error::Error(int line, char type, string description) {
	Error::line = line;
	Error::type = type;
	Error::description = description;
}
int Error::getLine() {
	return line;
}
char Error::getType() {
	return type;
}
string Error::getDescription() {
	return description;
}
void Error::setLine(int line) {
	Error::line = line;
}
void Error::setType(char type) {
	Error::type = type;
}
void Error::setDescription(string description) {
	Error::description = description;
}