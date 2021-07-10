#ifndef Error_h
#define Error_h

#include<string>
using std::string;

class Error {
public:
	Error();
	Error(int line, char type, string description);
	bool operator < (const Error& a) const
	{
		return (line < a.line);
	}
	int getLine();
	char getType();
	string getDescription();
	void setLine(int line);
	void setType(char type);
	void setDescription(string description);

private:
	int line;
	char type;
	string description;
};

#endif // !Error_h