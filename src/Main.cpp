#include "LexicalAnalyzer.h"
#include "SyntaxAnalyzer.h"
#include"Error.h"
#include"CodeGen.h"
#include"SymbolTable.h"

#include<iostream>
#include<cstdio>
#include<vector>
#include<algorithm>
#include<set>
using std::set;
using std::sort;
using std::vector;
using std::cin;
using std::cout;
using std::endl;

void OutputLex(LexicalAnalyzer& lex, FILE *out) {
	for (auto Token : lex.tokenVec) {
		string token = Token.getToken();
		string symbol = Token.getSymbol();
		for (int i = 0; i < token.size(); i++) {
			putc(token[i], out);
		}
		putc(' ', out);
		for (int i = 0; i < symbol.size(); i++) {
			putc(symbol[i], out);
		}
		putc('\n', out);
	}
}
void OutputSyn(vector<string> output, FILE* out) {
	for (auto str : output) {
		for (auto ch : str) {
			putc(ch, out);
		}
		putc('\n', out);
	}
}
void OutputErr(vector<Error> errVec, FILE* out) {
	int lastline = -1;
	char lasttype = '0';
	for (auto err : errVec) {
		if (err.getLine() == lastline && err.getType() == lasttype) {
			continue;
		}
		else {
			fprintf(out, "%d %c\n", err.getLine(), err.getType());
			lastline = err.getLine();
			lasttype = err.getType();
			for (auto ch : err.getDescription()) {
				putc(ch, out);
			}
			putc('\n', out);
		}
	}
}
void OutputIntermediate(vector<vector<string>> interVec, FILE* inter) {
	for (auto v : interVec) {
		for (auto str : v) {
			for (auto ch : str) {
				putc(ch, inter);
			}
			putc(' ', inter);
		}
		putc('\n', inter);
	}
}
void OutputMipsCode(vector<string> mipsCode, FILE* codegen) {
	for (auto str : mipsCode) {
		for (auto ch : str) {
			putc(ch, codegen);
		}
		putc('\n', codegen);
	}
}
int main() {
	FILE *in, *out, *err, *inter, *codegen;
	in = fopen("testfile.txt", "r");
	out = fopen("output.txt", "w");
	err = fopen("error.txt", "w");
	inter = fopen("intermediate.txt", "w");
	codegen = fopen("mips.txt", "w");

	vector<char> input;
	char ch;
	while ((ch = getc(in)) != EOF) {
		input.push_back(ch);
	}
	fclose(in);

	LexicalAnalyzer lex = LexicalAnalyzer(input);
	//OutputLex(lex, out);

	SyntaxAnalyzer syn = SyntaxAnalyzer(lex.tokenVec);

	vector<Error> ascErrorVec;
	ascErrorVec.reserve(lex.errorVec.size() + syn.errorVec.size());
	ascErrorVec.insert(ascErrorVec.end(), lex.errorVec.begin(), lex.errorVec.end());
	ascErrorVec.insert(ascErrorVec.end(), syn.errorVec.begin(), syn.errorVec.end());
	sort(ascErrorVec.begin(), ascErrorVec.end());
	if (ascErrorVec.size() > 0) {
		OutputErr(ascErrorVec, err);
	}
	else {
		OutputIntermediate(syn.intermediateCodeVec, inter);
		fclose(inter);
		OutputSyn(syn.outputVec, out);
		CodeGen CG = CodeGen(syn.intermediateCodeVec, syn.symbolTable);
		OutputMipsCode(CG.dataSegment, codegen);
		OutputMipsCode(CG.textSegment, codegen);
	}
	
	fclose(out);
	fclose(err);
	fclose(codegen);
	return 0;
}