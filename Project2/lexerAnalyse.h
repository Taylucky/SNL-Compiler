#ifndef LEXERANALYSE_H_INCLUDED
#define LEXERANALYSE_H_INCLUDED

#include<vector>
#include<string>
#include<map>
using namespace std;

enum TokenType   // 前半部分为 终极符 , 后半部分为非终极符
{
    /* 单词符号 */
    ENDFILE=0,ERROR,

    /* 保留字 */
    PROGRAM,	PROCEDURE,	TYPE,	VAR,		IF,
    THEN,		ELSE,		FI,		WHILE,		DO,
    ENDWH,		BEGIN,		END,	READ,		WRITE,
    ARRAY,		OF,			RECORD,	RETURN,

    INTEGER,	CHAR,

    /* 多字符单词符号 */
    ID,			INTC,		CHARC,

    /*特殊符号 */
    ASSIGN/** := **/,		EQ /** = **/,	LT/** < **/,	PLUS/** + **/,		MINUS/** - **/,
    TIMES/** * **/,		OVER/** / **/,		LPAREN/**(**/,	RPAREN/**)**/,		DOT/**.**/,
    COLON/**:**/,		SEMI/**;**/,		COMMA/**,**/,	LMIDPAREN/**[**/,	RMIDPAREN/**]**/,
    UNDERANGE/**..**/,

    LAMBDA,  // 空 

    /*** 非终极符  ***/
    Program,        ProgramHead,	    ProgramName,	DeclarePart,
    TypeDec,        TypeDeclaration,	TypeDecList,	TypeDecMore,
    TypeId,         TypeName,			BaseType,	    StructureType,
    ArrayType,      Low,	            Top,            RecType,
    FieldDecList,   FieldDecMore,	    IdList,	        IdMore,
    VarDec,         VarDeclaration, 	VarDecList,		VarDecMore,
    VarIdList,      VarIdMore,		    ProcDec,		ProcDeclaration,
    ProcDecMore,    ProcName,		    ParamList,		ParamDecList,
    ParamMore,      Param,		        FormList,		FidMore,
    ProcDecPart,    ProcBody,	    	ProgramBody,	StmList,
    StmMore,        Stm,				AssCall,		AssignmentRest,
    ConditionalStm, StmL,			    LoopStm,		InputStm,
    InVar,          OutputStm,		    ReturnStm,		CallStmRest,
    ActParamList,   ActParamMore,		RelExp,			OtherRelE,
    Exp,			OtherTerm,		    Term,           OtherFactor,
    Factor,         Variable,			VariMore,		FieldVar,
    FieldVarMore,   CmpOp,			    AddOp,          MultOp,
} ;

typedef struct Token
{
    int  lineNum;//行号
    string name;//名称
    TokenType type;//标识符类型
    Token(int ln=0):lineNum(ln),type(TokenType::ERROR) {}
    Token(int ln,const char ch[],TokenType tp):lineNum(ln),name(ch),type(tp) {}
  
} Token;

// 对外接口, 以便语法分析程序获取token序列
vector<Token>& getTokenList();
void initReadOneToken();
bool ReadOneToken(Token & tk);

#endif