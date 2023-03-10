#define _CRT_SECURE_NO_WARNINGS
//#define enumToStr(WEEEK) 
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
#include <fstream> 
#include <string>
using namespace std;

//�������·��
char in[100] = "D:\\����\\������\\���޿�\\����ԭ�����\\Project1\\source1.txt";
char out[100] = "D:\\����\\������\\���޿�\\����ԭ�����\\Project1\\token.txt";
char out1[100]= "D:\\����\\������\\���޿�\\����ԭ�����\\Project1\\tree1.txt";
char out2[100]= "D:\\����\\������\\���޿�\\����ԭ�����\\Project1\\tree2.txt";
//��������
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
//�����ֱ�
typedef struct Word
{
	string str;//����
	LexType tok;//����
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
	int line;//����
	word content;//����
}token;
token tokenlist[1000];//�γ�һ����
//�жϺ�����
int isReservedWords(string word);//�ǲ��Ǳ�����
bool isLetter(char ch);//�ǲ�����ĸ
bool isDigit(char ch);//�ǲ�������
bool isSingle(char ch);//�ǲ��ǽ��޷�
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
		|| ch == '\n' || ch == '\t' || ch == '\r' || ch == '\0')//û�ж���|| ch == ','
		return true;
	else return false;
}
int index = -1;
void lexAnalyse(FILE* fp) {
	
	string buffer = "";
	char ch = fgetc(fp);
	cout << ch;
	int curline = 1;
START://��ʼ
		index++;
		//�Ƿ񵽴��ļ�ĩβ
		if (ch == EOF)
			goto DONE;////????

		// ����ǲ�����ĸ
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

INID: {//��ʶ��
		buffer = ""; // ��ʼ���ַ���
		buffer = buffer + ch; // �Ȱѵ�һ���ַ������


		//ȡ�ַ���
		ch = fgetc(fp);
		while (isLetter(ch) || isDigit(ch))
		{
			buffer = buffer + ch;
			ch = fgetc(fp);
		}
		int result = isReservedWords(buffer);
		if (result != -1) {
			tokenlist[index].line = curline;
			tokenlist[index].content.str = reservedWords[result].str;//ɾ����
			tokenlist[index].content.tok = reservedWords[result].tok;
		}
		else {
			tokenlist[index].line = curline;
			tokenlist[index].content.str = buffer; // ��¼�ַ���
			tokenlist[index].content.tok = ID; // 
		}
		goto START;
		}

INNUM: {//����
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
INASSIGN: {//��ֵ//û��ֻ��ð�ŵ�����ɣ�����
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
INCOMMENT: {//ע��
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

INRANGE: {//�����±�
	ch = fgetc(fp);
	if (ch == '.') {//�кܶ�...
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
INERROR: {//����
	cout << tokenlist[index-1].content.tok<<"ERROR!" << endl;
	tokenlist[index].line = curline;
	tokenlist[index].content.tok = ERROR;
	ch = fgetc(fp);
	goto START;
}
DONE://���״̬
	tokenlist[index].line = curline;
	tokenlist[index].content.str = ch;
	tokenlist[index].content.tok = ENDFILE;
	}


//��ӡ tokenlist
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

//************�ݹ��½������﷨��************
	int Rcurline = 0;//��ǰ��
	token* Rptr=NULL;//ָ��ָ��tokenlist
	class Treenode//���ڵ�
	{
	public:
		string name;
		token* tk;
		Treenode* child[32];//
		int linenum;//���ޱ�Ҫ��˵
		int childIndex;
		Treenode(string names)
		{
			name = names;
			tk = NULL;
			for (int i = 0; i < 31; i++)
			{
				child[i] = NULL;
			}
			linenum = 0;//��ʼ��
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
	//���뵱ǰtoken
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
			cout << "PARSE ERROR!!!";//������
		}
		else return t;
	}

	Treenode* Program() {
		Treenode* t1 = ProgramHead();
		Treenode* q1 = DeclarePart();
		Treenode* s1 = ProgramBody();
		Treenode* t = new Treenode("Program");//���ɽڵ�root
		if (t == NULL)
			cout << "Creating Tree falls near line";//<< Rcurline;////
		else {
			t->add(t1);//���ó���ͷ���ַ�������
			t->add(q1);//���ó����������ַ�������
			t->add(s1);//���ó����岿�ַ�������

			if (t1 == NULL)
				cout << "MissingProgramHead near line";//<< Rcurline;////
			if (q1 == NULL)
				cout << "MissingDeclarePart near line"; //<< Rcurline;////
			if (s1 == NULL)
				cout << "MissingProgramBody near line";// << Rcurline;////

		}
		//ƥ������match???
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

//******������************* 
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
	printTokenlist();//Ҫ��Ϊд���ļ�
}