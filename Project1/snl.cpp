#define _CRT_SECURE_NO_WARNINGS
//#define enumToStr(WEEEK) 
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
#include <fstream> 
#include <string>
using namespace std;

//输入输出路径
char in[100] = "D:\\月月\\大三下\\必修课\\编译原理课设\\Project1\\source1.txt";
char out[100] = "D:\\月月\\大三下\\必修课\\编译原理课设\\Project1\\token.txt";
char out1[100]= "D:\\月月\\大三下\\必修课\\编译原理课设\\Project1\\tree1.txt";
char out2[100]= "D:\\月月\\大三下\\必修课\\编译原理课设\\Project1\\tree2.txt";
//设置类型
typedef enum
{
	INIT,ENDFILE, ERROR,
	PROGRAM, PROCEDURE, TYPE, VAR, IF,
	THEN, ELSE, FI, WHILE, DO, ENDWH,
	BEGIN, END1, READ, WRITE, ARRAY, OF,
	RECORD, RETURN1,
	INTEGER, CHAR1, 
	ID, INTC, CHARC,
	ASSIGN, EQ, LT, PLUS, MINUS, MULTIPLY, DIVIDE,OVER, LPAREN, RPAREN, DOT, LBRACKET, RBRACKET,
	COLON, SEMI, COMMA, 
	UNDERANGE
}LexType;
string lextype[44] = { "INIT","ENDFILE", "ERROR",
	"PROGRAM", "PROCEDURE", "TYPE", "VAR", "IF",
	"THEN", "ELSE", "FI", "WHILE", "DO", "ENDWH",
	"BEGIN", "END1", "READ", "WRITE", "ARRAY", "OF",
	"RECORD", "RETURN1",
	"INTEGER", "CHAR1",
	"ID", "INTC", "CHARC",
	"ASSIGN", "EQ","LT","PLUS","MINUS","MULTIPLY","DIVIDE","OVER","LPAREN","RPAREN","DOT","LBRACKET", "RBRACKET",
	"COLON","SEMI","COMMA",
	"UNDERANGE" };
//保留字表
typedef struct Word
{
	string str;//含义
	LexType tok;//类型
}word;
word reservedWords[21] =
{
	{"program",PROGRAM},
	{"type",TYPE},
	{"var",VAR},
	{"procedure",PROCEDURE},
	{"begin",BEGIN},
	{"end",END1},
	{"array",ARRAY},
	{"of",OF},
	{"record",RECORD},
	{"if",IF},{"then",THEN},
	{"else",ELSE},
	{"fi",FI},
	{"while",WHILE},
	{"do",DO},
	{"endwh",ENDWH},
	{"read",READ},
	{"write",WRITE},
	{"return",RETURN1},
	{"integer",INTEGER},
	{"char",CHAR1}
};
typedef struct {
	int line;//行数
	word content;//内容
}token;
token tokenlist[1000];//形成一个表
//判断函数们
int isReservedWords(string word);//是不是保留字
bool isLetter(char ch);//是不是字母
bool isDigit(char ch);//是不是数字
bool isSingle(char ch);//是不是界限符
void lexAnalyse(FILE* fp);
void printTokenlist();



int isReservedWords(string s) {
	for (int i = 0; i < 21; i++) {
		if (s == reservedWords[i].str)
			return i;
	}
	return -1;
}
bool isLetter(char ch) {
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
		return true;
	else return false;
}

bool isDigit(char ch) {
	if (ch <= '9' && ch >= '0')
		return true;
	else return false;
}
bool isOperator(char ch) {
	if (ch == '+' || ch == '-' || ch == '*' || ch == '/' 
		|| ch == '<' || ch == '>')//(){}[];
		return true;
	else return false;
}

bool isSingle(char ch) {
	if (ch == '+'|| ch == '-' || ch == '*' || ch == '/'
		|| ch == '(' || ch == ')' || ch == ';' || ch == '['
		|| ch == ']' || ch == '=' || ch == '<' || ch == ' '
		|| ch == '\n' || ch == '\t' || ch == '\r' || ch == '\0')//没有逗号|| ch == ','
		return true;
	else return false;
}
int index = -1;
void lexAnalyse(FILE* fp) {
	
	string buffer = "";
	char ch = fgetc(fp);
	cout << ch;
	int curline = 1;
START://开始
		index++;
		//是否到达文件末尾
		if (ch == EOF)
			goto DONE;////????

		// 检查是不是字母
		if (isLetter(ch))
			goto INID;

		else if (isDigit(ch))
			goto INNUM;

		else if (isSingle(ch))
			goto INSINGLE;

		else if (ch == ':')
			goto INASSIGN;

		else if (ch == '{')
			goto INCOMMENT;

		else if (ch == '.')
			goto INRANGE;

		else if (ch == '\'')
			goto INCHAR;
		else goto INERROR;

INID: {//标识符
		buffer = ""; // 初始化字符串
		buffer = buffer + ch; // 先把第一个字符存进来


		//取字符串
		ch = fgetc(fp);
		while (isLetter(ch) || isDigit(ch))
		{
			buffer = buffer + ch;
			ch = fgetc(fp);
		}
		int result = isReservedWords(buffer);
		if (result != -1) {
			tokenlist[index].line = curline;
			tokenlist[index].content.str = reservedWords[result].str;//删掉？
			tokenlist[index].content.tok = reservedWords[result].tok;
		}
		else {
			tokenlist[index].line = curline;
			tokenlist[index].content.str = buffer; // 记录字符串
			tokenlist[index].content.tok = ID; // 
		}
		goto START;
		}

INNUM: {//数字
		buffer = "";
		buffer += ch;
		ch = fgetc(fp);
		while (isDigit(ch))
		{
			buffer += ch;
			ch = fgetc(fp);
		}
		if (isLetter(ch)) goto INERROR;
		tokenlist[index].line = curline;
		tokenlist[index].content.tok = INTC;
		tokenlist[index].content.str = buffer;
		//index++;//?
		goto START;
}
INSINGLE: {//
		
		if (ch == '+')
			tokenlist[index].content.tok = PLUS;
		else if (ch == '-')
			tokenlist[index].content.tok = MINUS;
		else if (ch == '*')
			tokenlist[index].content.tok = MULTIPLY;
		else if (ch == '/')
			tokenlist[index].content.tok = DIVIDE;
		else if (ch == '(')
			tokenlist[index].content.tok = LPAREN;
		else if (ch == ')')
			tokenlist[index].content.tok = RPAREN;
		else if (ch == ';')
			tokenlist[index].content.tok = SEMI;
		else if (ch == '[')
			tokenlist[index].content.tok = LBRACKET;
		else if (ch == ']')
			tokenlist[index].content.tok = RBRACKET;
		else if (ch == '=') {
			tokenlist[index].content.tok = EQ;
			goto INASSIGN;
		}
		else if (ch == '<')
			tokenlist[index].content.tok = LT;
		else if (ch == ' ' || ch == '\t' || ch == '\r') {
			index--;
			ch = fgetc(fp);
			goto START;
		}
		else if (ch == '\n') {
			index--;
			curline++;
			ch = fgetc(fp);
			goto START;
		}

		tokenlist[index].line = curline;
		ch = fgetc(fp);
		goto START;
}
INASSIGN: {//赋值//没有只有冒号的情况吧？？？
	if (ch == '=') {
		tokenlist[index].content.tok = ASSIGN;
		tokenlist[index].line = curline;
		
	}
	else {//:
		ch = fgetc(fp);
		if (ch == '=') {
			tokenlist[index].content.tok = ASSIGN;
			tokenlist[index].line = curline;
		}
		//
		else goto INERROR;
	}
	ch = fgetc(fp);
	goto START;
}
INCOMMENT: {//注释
	index--;
	while (ch != '}') {
		ch = getc(fp);
		if (ch == '\n') {
			curline++;
		}
	}
	ch = fgetc(fp);
	goto START;
	}

INRANGE: {//数组下标
	ch = fgetc(fp);
	if (ch == '.') {//有很多...
		tokenlist[index].line = curline;
		tokenlist[index].content.tok = UNDERANGE ;
	}
	else {
		if (ch == EOF) {
			tokenlist[index].line = curline;
			tokenlist[index].content.tok = DOT;
		}
		else goto INERROR;
	}

	ch = fgetc(fp);
	goto START;
	}

INCHAR: {
	tokenlist[index].content.tok = CHARC;
	string buffer = "";
	while (ch != '\'') {
		if (isDigit(ch) || isLetter(ch))
			buffer = buffer + ch;
		else goto INERROR;
	}
	tokenlist[index].content.str = buffer;
	tokenlist[index].line = curline;
	ch = fgetc(fp);
	goto START;
}
INERROR: {//错误
	cout << tokenlist[index-1].content.tok<<"ERROR!" << endl;
	tokenlist[index].line = curline;
	tokenlist[index].content.tok = ERROR;
	ch = fgetc(fp);
	goto START;
}
DONE://完成状态
	tokenlist[index].line = curline;
	tokenlist[index].content.str = ch;
	tokenlist[index].content.tok = ENDFILE;
	}


//打印 tokenlist
	void printTokenlist() {
		ofstream output(out);
		int i = 0;
		while (tokenlist[i].content.tok != ENDFILE) {
			output << tokenlist[i].line<<'\t'<< lextype[tokenlist[i].content.tok]<< '\t' << tokenlist[i].content.str << endl;
			i++;
		}
		output << tokenlist[i].line<<'\t'<< lextype[tokenlist[i].content.tok] << '\t' << tokenlist[i].content.str << endl;
		output.close();
}

//************递归下降建立语法树************
	int Rcurline = 0;//当前行
	token* Rptr=NULL;//指针指向tokenlist
	class Treenode//树节点
	{
	public:
		string name;
		token* tk;
		Treenode* child[32];//
		int linenum;//有无必要再说
		int childIndex;
		Treenode(string names)
		{
			name = names;
			tk = NULL;
			for (int i = 0; i < 31; i++)
			{
				child[i] = NULL;
			}
			linenum = 0;//初始化
			childIndex = 0;
		}
		void add(Treenode* t) {
			child[childIndex] = t;
			if (t != NULL){//???
				child[childIndex]->linenum = linenum + 1;//this->linenum
				childIndex++;
			}

		}
	};
	//读入当前token
	void Readtoken() {
		Rptr = &tokenlist[Rcurline];
		Rcurline++;
	}
	void Match(LexType TOK) {
		Rcurline++;
		Rptr = &tokenlist[Rcurline];
	}
	void Nextoken() {
		Rcurline++;
		Rptr = &tokenlist[Rcurline];
	}
	Treenode* Parse(FILE* );
	Treenode* Program();
	Treenode* ProgramHead();
	Treenode* DeclarePart();
	Treenode* ProcDec();
	Treenode* ProcDeclaration();
	Treenode* ProcDecMore();
	Treenode* ProcDecPart();
	Treenode* ProcBody();
	Treenode* ParamList();
	Treenode* ParamDecList();
	Treenode* ParamMore();
	Treenode* Param();
	Treenode* FormList();
	Treenode* FidMore();
	Treenode* VarDec();
	Treenode* VarDeclaration();
	Treenode* VarDecList();
	Treenode* VarIdMore();
	Treenode* VarIdList();
	Treenode* TypeDecMore();
	Treenode* StmList();
	Treenode* StmMore();
	Treenode* Stm();
	Treenode* AssCall();
	Treenode* AssignmentRest();
	Treenode* ConditionalStm();
	Treenode* LoopStm();
	Treenode* InputStm();
	Treenode* OutputStm();
	Treenode* ReturnStm();
	Treenode* CallStmRest();
	Treenode* ActParamList();
	Treenode* ActParamMore();
	Treenode* RelExp();
	Treenode* OtherRelE();
	Treenode* Exp();
	Treenode* OtherTerm();
	Treenode* Term();
	Treenode* Factor();
	Treenode* OtherFactor();
	Treenode* Variable();
	Treenode* VariMore();
	Treenode* FieldVar();
	Treenode* FieldVarMore();
	Treenode* CmpOp();
	Treenode* AddOp();
	Treenode* MultOp();
	Treenode* ProgramBody();
	Treenode* ProgramName();
	Treenode* VarDecMore();
	Treenode* TypeDec();
	Treenode* TypeDeclaration();
	Treenode* TypeDecList();
	Treenode* TypeId();
	Treenode* TypeDef();
	Treenode* BaseType();
	Treenode* StructureType();
	Treenode* ArrayType();
	Treenode* RecType();
	Treenode* FieldDecList();
	Treenode* IdList();
	Treenode* IdMore();
	Treenode* FieldDecMore();

	
	Treenode* Parse() {
		Readtoken();
		Treenode* t = Program();
		if (tokenlist[Rcurline].content.tok != ENDFILE) {
			cout << "PARSE ERROR!!!";//错误处理
		}
		else return t;
	}

	Treenode* Program() {
		Treenode* t1 = ProgramHead();
		Treenode* q1 = DeclarePart();
		Treenode* s1 = ProgramBody();
		Treenode* t = new Treenode("Program");//生成节点root
		if (t == NULL)
			cout << "Creating Tree falls near line";//<< Rcurline;////
		else {
			t->add(t1);//调用程序头部分分析函数
			t->add(q1);//调用程序声明部分分析函数
			t->add(s1);//调用程序体部分分析函数

			if (t1 == NULL)
				cout << "MissingProgramHead near line";//<< Rcurline;////
			if (q1 == NULL)
				cout << "MissingDeclarePart near line"; //<< Rcurline;////
			if (s1 == NULL)
				cout << "MissingProgramBody near line";// << Rcurline;////

		}
		//匹配最后的match???
		if (Rptr->content.tok != DOT) cout << "MissingEnd";
		else Match(DOT);
		return t;
	}

	Treenode* ProgramHead() {	
		Treenode* t = new Treenode("ProgramHead");
		Match(PROGRAM);
		if (Rptr->content.tok == ID) {
			
		}
		Match(ID);
		return t;
	}
	Treenode* DeclarePart() {
		Treenode* t = new Treenode("DeclarePart");
		t->add(TypeDec());
		t->add(VarDec());
		t->add(ProcDec());
		return t;
	}
	
	Treenode* ProcDec();
	Treenode* ProcDeclaration();
	Treenode* ProcDecMore();
	Treenode* ProcDecPart();
	Treenode* ProcBody();
	Treenode* ParamList();
	Treenode* ParamDecList();
	Treenode* ParamMore();
	Treenode* Param();
	Treenode* FormList();
	Treenode* FidMore();
	Treenode* VarDec();
	Treenode* VarDeclaration();
	Treenode* VarDecList();
	Treenode* VarIdMore();
	Treenode* VarIdList();
	Treenode* TypeDecMore();
	Treenode* StmList();
	Treenode* StmMore();
	Treenode* Stm();
	Treenode* AssCall();
	Treenode* AssignmentRest();
	Treenode* ConditionalStm();
	Treenode* LoopStm();
	Treenode* InputStm();
	Treenode* OutputStm();
	Treenode* ReturnStm();
	Treenode* CallStmRest();
	Treenode* ActParamList();
	Treenode* ActParamMore();
	Treenode* RelExp();
	Treenode* OtherRelE();
	Treenode* Exp();
	Treenode* OtherTerm();
	Treenode* Term();
	Treenode* Factor();
	Treenode* OtherFactor();
	Treenode* Variable();
	Treenode* VariMore();
	Treenode* FieldVar();
	Treenode* FieldVarMore();
	Treenode* CmpOp();
	Treenode* AddOp();
	Treenode* MultOp();
	Treenode* ProgramBody();
	Treenode* ProgramName();
	Treenode* VarDecMore();
	Treenode* TypeDec(){
		Treenode* t = NULL;
		LexType temp = tokenlist[Rcurline].content.tok;
		if (temp== TYPE) {
			t = TypeDeclaration();
			return t;
		}else {
			if (temp == VAR || temp == PROCEDURE || temp == BEGIN) {  }
				
			else Nextoken(); 
		}
		return t;
	}

	Treenode* TypeDeclaration() {
		Match(TYPE);
		Treenode* t = TypeDecList();
		if (t == NULL) cout << "typedeclaration fails";
		else return t;
	}

	Treenode* TypeDecList() {
		Treenode* t = new Treenode("TypeDecLIst");
		if (t != NULL) {
			t->add(TypeId());
			Match(EQ);
			t->add(TypeDef());
			Match(SEMI);
			t->add(TypeDecMore());
		}
		return t;
	}
	Treenode* TypeId() {
	}
	Treenode* TypeDef();
	Treenode* BaseType();
	Treenode* StructureType();
	Treenode* ArrayType();
	Treenode* RecType();
	Treenode* FieldDecList();
	Treenode* IdList();
	Treenode* IdMore();
	Treenode* FieldDecMore();

//******主程序************* 
	int main() {
	for (int i = 0; i < 1000; i++) {
		tokenlist[i].line = 0;
		tokenlist[i].content.str = '\0';
		tokenlist[i].content.tok = INIT;
	}
	FILE* fp;
	fp = fopen(in, "r");
	lexAnalyse(fp);
	fp = fopen(out, "r");
	Treenode *t=Parse(fp);
	printTokenlist();//要改为写入文件
}