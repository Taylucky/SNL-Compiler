#ifndef PREDICT_H
#define PREDICT_H

#include "lexeranalyse.h"
#include <set>
#include <map>
#include <string>

#define ProductExpressionNum  105   // 共 104 条产生式, 下标从 1开始
extern set<TokenType> Predict[ProductExpressionNum+5];

//产生式相关结构
typedef struct node {
    int num;//产生式号
    string vn;//左边的非终极符
    vector<string> production;//单词组合
    set<string> firstset;
    set<string> predictset;
}gramm;



void getAllPredict();

static map<string,TokenType> strToToken={
{"Program",Program},{"ProgramHead",ProgramHead}, {"ProgramName",ProgramName},
{"DeclarePart",DeclarePart}, {"TypeDec",TypeDec}, { "TypeDeclaration",TypeDeclaration },
{ "TypeDecList",TypeDecList}, { "TypeDecMore",TypeDecMore},
{"TypeId",TypeId}, {"TypeName",TypeName}, {"BaseType",BaseType }, { "StructureType",StructureType },
{"ArrayType",ArrayType},{ "Low",Low },{ "Top",Top}, {"RecType",RecType},
{"FieldDecList", FieldDecList},{"FieldDecMore",FieldDecMore},{ "IdList", IdList},{"IdMore",IdMore},
{"VarDec",VarDec},{"VarDeclaration",VarDeclaration},{"VarDecList",VarDecList},{ "VarDecMore",VarDecMore},
{"VarIdList",VarIdList}, {"VarIdMore",VarIdMore}, {"ProcDec",ProcDec}, {"ProcDeclaration",ProcDeclaration},
{"ProcDecMore",ProcDecMore}, {"ProcName",ProcName},{"ParamList",ParamList},{ "ParamDecList",ParamDecList},
{"ParamMore",ParamMore}, {"Param",Param}, {"FormList",FormList}, {"FidMore",FidMore},
{"ProcDecPart",ProcDecPart},{ "ProcBody",ProcBody},{"ProgramBody",ProgramBody}, {"StmList",StmList},
{"StmMore",StmMore}, {"Stm",Stm},{"AssCall",AssCall},{"AssignmentRest",AssignmentRest},
{"ConditionalStm",ConditionalStm}, {"StmL",StmL}, {"LoopStm",LoopStm}, {"InputStm",InputStm},{"InVar",InVar}, {"OutputStm",OutputStm},{ "ReturnStm",ReturnStm}, {"CallStmRest",CallStmRest},
{"ActParamList",ActParamList}, {"ActParamMore",ActParamMore}, {"RelExp",RelExp}, {"OtherRelE",OtherRelE},
{"Exp",Exp}, {"OtherTerm",OtherTerm},{"Term",Term}, {"OtherFactor",OtherFactor},
{"Factor",Factor},{"Variable",Variable},{"VariMore",VariMore}, {"FieldVar",FieldVar},
{"FieldVarMore",FieldVarMore}, {"CmpOp",CmpOp}, {"AddOp",AddOp}, {"MultOp",MultOp},
{"ENDFILE",ENDFILE}, {"ERROR",ERROR},
{"PROGRAM",PROGRAM}, { "PROCEDURE",PROCEDURE}, { "TYPE",TYPE }, { "VAR",VAR }, { "IF",IF },
{"THEN",THEN}, {"ELSE",ELSE},{ "FI",FI},{"WHILE",WHILE}, {"DO",DO}, {"ENDWH",ENDWH},
{"BEGIN",BEGIN}, {"END",END},{"READ",READ},{"WRITE",WRITE}, {"ARRAY[low..top]",ARRAY}, {"OF",OF},
{"RECORD",RECORD}, {"RETURN",RETURN},
{"INTEGER",INTEGER},{"CHAR",CHAR},
{"ID",ID}, {"INTC",INTC}, {"CHARC",CHARC},
{":=",ASSIGN}, {"=",EQ}, {"<",LT} ,{"+",PLUS},
{ "-",MINUS},{"*",TIMES}, {"/",OVER}, {"(",LPAREN}, {")",RPAREN}, {".",DOT},{":",COLON},
{ ";",SEMI}, {",",COMMA}, {"[",LMIDPAREN}, {"]",RMIDPAREN}};

#endif
// PREDICT_H
