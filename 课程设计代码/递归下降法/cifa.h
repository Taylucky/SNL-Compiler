#pragma once
#define _CRT_SECURE_NO_DEPRECATE
#include <string>
#include<vector>
#define MAXRESERVED 21
using namespace std;
typedef enum {
	//���Ƿ�
	ENDFILE, ERROR,

	//������
	PROGRAM, PROCEDURE, TYPE, VAR, IF,
	THEN, ELSE, FI, WHILE, DO,
	ENDWH, BEGIN, END, READ, WRITE,
	ARRAY, OF, RECORD, RETURN,

	//������
	INTEGER, CHAR1,

	//���ַ�����
	ID, INTC, CHARC,

	//�������
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
	int Lineshow;//��¼�õ�����Դ�����е�����
	LexType Lex;//��¼�õ��ʵĴʷ���Ϣ������ LexType Ϊ���ʵ�����ö��
	string Sem;//�õ��ʵ�������Ϣ
};
vector<Token> getTokenList();//�ֳ�tokens
void chainToFile(vector<Token>);//��tokens���
