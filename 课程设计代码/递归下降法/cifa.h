#pragma once
#define _CRT_SECURE_NO_DEPRECATE
#include <string>
#include<vector>
#define MAXRESERVED 21
using namespace std;
typedef enum {
	//助记符
	ENDFILE, ERROR,

	//保留字
	PROGRAM, PROCEDURE, TYPE, VAR, IF,
	THEN, ELSE, FI, WHILE, DO,
	ENDWH, BEGIN, END, READ, WRITE,
	ARRAY, OF, RECORD, RETURN,

	//类型名
	INTEGER, CHAR1,

	//多字符单词
	ID, INTC, CHARC,

	//特殊符号
	ASSIGN, EQUAL, LESSTHEN, PLUS, MINUS,
	TIMES, OVER, LPAREN, RPAREN, DOT,
	COLON, SEMI, COMMA, LMIDPAREN, RMIDPAREN,
	UNDERANGE, SPACE
}LexType;
typedef enum {
	S0, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13
}State;
typedef enum {
	id, num, error, sch, dch, endf, underange, charc, text
}OutKind;
struct Token 
{
	int Lineshow;//记录该单词在源程序中的行数
	LexType Lex;//记录该单词的词法信息，其中 LexType 为单词的类型枚举
	string Sem;//该单词的语义信息
};
vector<Token> getTokenList();//分成tokens
void chainToFile(vector<Token>);//将tokens输出
