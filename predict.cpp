#include <cstdio>
#include <iostream>
using namespace std;
//输入文件
char grammar[100]="D:\\月月\\大三下\\必修课\\编译原理课设\\grammars.txt";
char predict[100]="D:\\月月\\大三下\\必修课\\编译原理课设\\predicts.txt";
//非终极符
typedef enum
{
	Program, ProgramHead, ProgramName, DeclarePart,
	TypeDec, TypeDeclaration, TypeDecList, TypeDecMore,
	TypeId, TypeName, BaseType, StructureType,
	ArrayType, Low, Top, RecType,
	FieldDecList, FieldDecMore, IdList, IdMore,
	VarDec, VarDeclaration, VarDecList, VarDecMore,
	VarIdList, VarIdMore, ProcDec, ProcDeclaration,
	ProcDecMore, ProcName, ParamList, ParamDecList,
	ParamMore, Param, FormList, FidMore,
	ProcDecPart, ProcBody, ProgramBody, StmList,
	StmMore, Stm, AssCall, AssignmentRest,
	ConditionalStm, StmL, LoopStm, InputStm,
	InVar, OutputStm, ReturnStm, CallStmRest,
	ActParamList, ActParamMore, RelExp, OtherRelE,
	Exp, OtherTerm, Term, OtherFactor,
	Factor, Variable, VariMore, FieldVar,
	FieldVarMore, CmpOp, AddOp, MultOp,
//NonTerminal;
//终极符

	ENDFILE, ERROR,
	PROGRAM, PROCEDURE, TYPE, VAR, IF,
	THEN, ELSE, FI, WHILE, DO, ENDWH,
	BEGIN, END1, READ, WRITE, ARRAY, OF,
	RECORD, RETURN1,
	INTEGER, CHAR1,
	ID, INTC, CHARC,
	ASSIGN, EQ, LT, PLUS, MINUS,
	TIMES, OVER, LPAREN, RPAREN, DOT,
	COLON, SEMI, COMMA, LMIDPAREN, RMIDPAREN,
	UNDERANGErf
}Symbol;
//输出predict集合
bool isTerminal(Symbol s){
    if(s>=68) return 1;
    else return 0;
}
typedef struct {
    int num;
    Symbol left;
    Symbol* rhead;
}Node;
Node node[104];
//处理文件
string readString(FILE* fp){
    char ch=getc(fp);
    while(ch==':'||ch==' '||ch=='='||ch=='\t'){
        ch=getc(fp);
    }
    string buffer="";
    char ch=fgetc(fp);
    while(ch!=':'&&ch!='\n'&&ch!='\t'&&ch!=' ')
        buffer+=ch;
        ch = fgetc(fp);
    }
    return buffer;
}
void transfer(FILE* fp){
    for(int i;i<104;i++){
        node[i].left=readString();
    }

}
//求first集合
void getFirst(int num){
//    if()
}
int main(){
    FILE* fp=fopen(grammar,"r");
    transfer(fp);
    cout<<isTerminal(Exp);
}
