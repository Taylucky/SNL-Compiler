#include "globals.h"


//LL1������
int LL1Table[TABLESIZE][TABLESIZE];

//�����ֱ�
string ReservedList[21] = 
{ "program","type","var","procedure","begin","end","array","of",
"record","if","then","else","fi","while","do","endwh",
"read","write","return","integer","char" };


//ȫ�ֱ��� 
int LineNumber = 1, CurIndex = 0;
string InputS;//�������ַ��� 
Token *TokenChain;//Token�ṹ��ͷָ�� 
Token *CurChain; //Tonken�ṹ�嵱ǰָ��

