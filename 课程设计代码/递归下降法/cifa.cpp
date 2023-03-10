#include "cifa.h"
#include <cstdio>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include<fstream>
using namespace std;
FILE* source;
//fstream f1("D:\\bubble.txt", ios::in);//冒泡排序代码
fstream f1("D:\\SNLsource.txt", ios::in);//教材上的代码
char ch;
vector<char> ch1;
int p = 0;
bool flag = true;
static map<string, LexType> reservedWords;
static map<char, LexType> specialChars;
void init() {
	reservedWords["program"] = PROGRAM;
	reservedWords["type"] = TYPE;
	reservedWords["var"] = VAR;
	reservedWords["procedure"] = PROCEDURE;
	reservedWords["begin"] = BEGIN;
	reservedWords["end"] = END;
	reservedWords["array"] = ARRAY;
	reservedWords["of"] = OF;
	reservedWords["record"] = RECORD;
	reservedWords["if"] = IF;
	reservedWords["then"] = THEN;
	reservedWords["else"] = ELSE;
	reservedWords["fi"] = FI;
	reservedWords["while"] = WHILE;
	reservedWords["do"] = DO;
	reservedWords["endwh"] = ENDWH;
	reservedWords["read"] = READ;
	reservedWords["write"] = WRITE;
	reservedWords["return"] = RETURN;
	reservedWords["integer"] = INTEGER;
	reservedWords["char"] = CHAR1;

	reservedWords["PROGRAM"] = PROGRAM;
	reservedWords["TYPE"] = TYPE;
	reservedWords["VAR"] = VAR;
	reservedWords["PROCEDURE"] = PROCEDURE;
	reservedWords["BEGIN"] = BEGIN;
	reservedWords["END"] = END;
	reservedWords["ARRAY"] = ARRAY;
	reservedWords["OF"] = OF;
	reservedWords["RECORD"] = RECORD;
	reservedWords["IF"] = IF;
	reservedWords["THEN"] = THEN;
	reservedWords["ELSE"] = ELSE;
	reservedWords["FI"] = FI;
	reservedWords["WHILE"] = WHILE;
	reservedWords["DO"] = DO;
	reservedWords["ENDWH"] = ENDWH;
	reservedWords["READ"] = READ;
	reservedWords["WRITE"] = WRITE;
	reservedWords["RETURN"] = RETURN;
	reservedWords["INTEGER"] = INTEGER;
	reservedWords["CHAR"] = CHAR1;
	specialChars['+'] = PLUS;
	specialChars['-'] = MINUS;
	specialChars['*'] = TIMES;
	specialChars['/'] = OVER;
	specialChars['('] = LPAREN;
	specialChars[')'] = RPAREN;
	specialChars[';'] = SEMI;
	specialChars['['] = LMIDPAREN;
	specialChars[']'] = RMIDPAREN;
	specialChars['='] = EQUAL;
	specialChars['<'] = LESSTHEN;
	specialChars[' '] = SPACE;
	specialChars[','] = COMMA;
}
bool isNum(char x) {
	if (x >= '0' && x <= '9')
		return true;
	return false;
}
bool isLetter(char x) {
	if (x >= 'a' && x <= 'z')
		return true;
	if (x >= 'A' && x <= 'Z')
		return true;
	return false;
}
char getNextChar() {
	if (flag == true)
	{
		while ((ch = f1.get()) != EOF)
		{
			ch1.push_back(ch);
		}
		flag = false;
	}
	if (p < ch1.size())
		return ch1[p++];
	else return 0;
}
void undoGetNextChar() {
	p--;
}
bool reservedLookup(string x) {//是否是保留字
	if (reservedWords.count(x) == 1) return true;
	return false;
}
void chainToFile(vector<Token>arr) {
	string key[] = {
		"ENDFILE","ERROR","PROGRAM","PROCEDURE","TYPE","VAR","IF",
		"THEN","ELSE","FI","WHILE","DO","ENDWH","BEGIN","END",
		"READ","WRITE","ARRAY","OF","RECORD","RETURN","INTEGER",
		"CHAR1","ID","INTC","CHARC","ASSIGN","EQUAL","LESSTHEN",
		"PLUS","MINUS","TIMES","OVER","LPAREN","RPAREN","DOT",
		"COLON","SEMI","COMMA","LMIDPAREN","RMIDPAREN","UNDERANGE",
		"SPACE"
	};
	for (int i = 0; i < (int)arr.size(); i++) {
		cout << arr[i].Lineshow << '\t';
		cout << key[arr[i].Lex] << '\t';
		cout << arr[i].Sem << '\n';
	}
}
vector<Token> getTokenList() {
	init();
	string curr;
	vector<Token> Tokens;
	char ch;
	State state = S0;
	OutKind outKind;
	int l = 1;
	while (ch = getNextChar()) {
		state = S0;
		bool flag = true;
		while (flag) {
			switch (state) {
			case S0:
				curr = "";
				if (isLetter(ch)) state = S1;
				else if (isNum(ch)) state = S2;
				else if (specialChars.count(ch) == 1) state = S3;
				else if (ch == ':') state = S4;
				else if (ch == '{') state = S6;
				else if (ch == '.') state = S7;
				else if (ch == '\'') state = S9;
				else if (ch == '\n') {
					l++;
					ch = getNextChar();
				}
				else if (ch == '\t') ch = getNextChar();
				else state = S12;
				break;
			case S1:
				while (isNum(ch) || isLetter(ch)) {
					curr += ch;
					ch = getNextChar();
				}
				undoGetNextChar();
				state = S13;
				outKind = id;
				break;
			case S2:
				while (isNum(ch)) {
					curr += ch;
					ch = getNextChar();
				}
				undoGetNextChar();
				state = S13;
				outKind = num;
				break;
			case S3:
				outKind = sch;
				state = S13;
				break;
			case S4:
				ch = getNextChar();
				if (ch == '=') {
					state = S5;
				}
				else {
					state = S13;
					outKind = error;
				}
				break;
			case S5:
				outKind = dch;
				state = S13;
				break;
			case S6:
				while (ch != '}') ch = getNextChar();
				state = S13;
				outKind = text;
				break;
			case S7:
				ch = getNextChar();
				if (ch == '.') {
					state = S8;
				}
				else {
					state = S13;
					outKind = endf;
				}
				break;
			case S8:
				state = S13;
				outKind = underange;
				break;
			case S9:
				ch = getNextChar();
				if (isNum(ch) || isLetter(ch)) {
					while (isNum(ch) || isLetter(ch)) {
						curr += ch;
						ch = getNextChar();
					}
					if (ch == '\'') {
						state = S13;
						outKind = charc;
					}
					else {
						state = S13;
						outKind = error;
					}
				}
				else {
					state = S13;
					outKind = error;
				}
				break;
			case 12:
				state = S13;
				outKind = error;
				break;
			case 13:
				if (outKind == error) {
					cout << "error in line " << l << endl;
				}
				else {
					LexType lex;
					string sem;
					switch (outKind) {
					case id:
						if (reservedLookup(curr)) {
							lex = reservedWords[curr];
							sem = "";
						}
						else {
							lex = ID;
							sem = curr;
						}
						break;
					case num:
						lex = INTC;
						sem = curr;
						break;
					case sch:
						lex = specialChars[ch];
						sem = "";
						break;
					case dch:
						lex = ASSIGN;
						sem = "";
						break;
					case endf:
						lex = DOT;
						sem = "";
						break;
					case underange:
						lex = UNDERANGE;
						sem = "";
						break;
					case charc:
						lex = CHARC;
						sem = curr;
						break;
					}
					if (lex != SPACE && outKind != text) Tokens.push_back({ l,lex,sem });
				}
				flag = false;
				break;
			}
		}
	}
	Tokens.push_back({ ++l,ENDFILE,"" });
	return Tokens;
}
