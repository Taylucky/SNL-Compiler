#include "globals.h"


//LL1分析表
int LL1Table[TABLESIZE][TABLESIZE];

//保留字表
string ReservedList[21] = 
{ "program","type","var","procedure","begin","end","array","of",
"record","if","then","else","fi","while","do","endwh",
"read","write","return","integer","char" };


//全局变量 
int LineNumber = 1, CurIndex = 0;
string InputS;//输入流字符串 
Token *TokenChain;//Token结构体头指针 
Token *CurChain; //Tonken结构体当前指针

