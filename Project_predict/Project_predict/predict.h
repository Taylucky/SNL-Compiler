#pragma once
#include <cstdio>
#include <stdio.h>
#include <iostream>
#include <set>
#include <map>
#include <fstream>
#include <string>
#include <Windows.h>
#include <vector>
#include <algorithm>
//#include"lexerAnalyse.h"
enum TokenType   // 前半部分为 终极符 , 后半部分为非终极符
{
    /* 单词符号 */
    ENDFILE = 0, ERROR,

    /* 保留字 */
    PROGRAM, PROCEDURE, TYPE, VAR, IF,
    THEN, ELSE, FI, WHILE, DO,
    ENDWH, BEGIN, END, READ, WRITE,
    ARRAY, OF, RECORD, RETURN,

    INTEGER, CHAR,

    /* 多字符单词符号 */
    ID, INTC, CHARC,

    /*特殊符号 */
    ASSIGN/** := **/, EQ /** = **/, LT/** < **/, PLUS/** + **/, MINUS/** - **/,
    TIMES/** * **/, OVER/** / **/, LPAREN/**(**/, RPAREN/**)**/, DOT/**.**/,
    COLON/**:**/, SEMI/**;**/, COMMA/**,**/, LMIDPAREN/**[**/, RMIDPAREN/**]**/,
    UNDERANGE/**..**/,

    LAMBDA,  // 空 

    /*** 非终极符  ***/
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
};

//非终极符
set<string> Vn{
    "Program", "ProgramHead", "ProgramName", "DeclarePart",
    "TypeDec", "TypeDeclaration", "TypeDecList", "TypeDecMore",
    "TypeId", "TypeName", "BaseType", "StructureType",
    "ArrayType", "Low", "Top", "RecType",
    "FieldDecList", "FieldDecMore", "IdList", "IdMore",
    "VarDec","VarDeclaration","VarDecList", "VarDecMore",
    "VarIdList", "VarIdMore", "ProcDec", "ProcDeclaration",
    "ProcDecMore", "ProcName","ParamList", "ParamDecList",
    "ParamMore", "Param", "FormList", "FidMore",
    "ProcDecPart", "ProcBody","ProgramBody", "StmList",
    "StmMore", "Stm","AssCall","AssignmentRest",
    "ConditionalStm", "StmL", "LoopStm", "InputStm",
    "InVar", "OutputStm", "ReturnStm", "CallStmRest",
    "ActParamList", "ActParamMore", "RelExp", "OtherRelE",
    "Exp", "OtherTerm", "Term", "OtherFactor",
    "Factor","Variable", "VariMore", "FieldVar",
    "FieldVarMore", "CmpOp", "AddOp", "MultOp" };
//终极符
/*set<string> Vt{
    "ENDFILE", "ERROR",
    "PROGRAM", "PROCEDURE", "TYPE", "VAR", "IF",
    "THEN", "ELSE", "FI", "WHILE", "DO", "ENDWH",
    "BEGIN", "END1","READ", "WRITE", "ARRAY", "OF",
    "RECORD", "RETURN1",
    "INTEGER", "CHAR1",
    "ID", "INTC", "CHARC",
    "ASSIGN","EQ", "LT", "PLUS", "MINUS",
    "TIMES", "OVER", "LPAREN", "RPAREN", "DOT",
    "COLON", "SEMI", "COMMA", "LMIDPAREN", "RMIDPAREN",
    "UNDERANGErf" };*/

set<string> Vt{
    "ENDFILE", "ERROR",
    "PROGRAM", "PROCEDURE", "TYPE", "VAR", "IF",
    "THEN", "ELSE", "FI", "WHILE", "DO", "ENDWH",
    "BEGIN", "END","READ", "WRITE", "ARRAY[low..top]", "OF",
    "RECORD", "RETURN",
    "INTEGER", "CHAR",
    "ID", "INTC", "CHARC",
    ":=","=", "<", "+", "-",
    "*", "/", "(", ")", ".",
    ":", ";", ",", "[", "]",
    "UNDERANGErf" };


/*map<string, enum TokenType> StrToToken = {

    {"Program",Program}, {"ProgramHead",ProgramHead}, {"ProgramName",ProgramName}, {"DeclarePart",DeclarePart},
    {"TypeDec",TypeDec}, { "TypeDeclaration",TypeDeclaration }, { "TypeDecList",TypeDecList}, { "TypeDecMore",TypeDecMore},
    {"TypeId",TypeId}, {"TypeName",TypeName}, {"BaseType",BaseType }, { "StructureType",StructureType },
    {"ArrayType",ArrayType},{ "Low",Low },{ "Top",Top}, {"RecType",RecType},
    {"FieldDecList", FieldDecList},{"FieldDecMore",FieldDecMore},{ "IdList", IdList},{"IdMore",IdMore},
    {"VarDec",VarDec},{"VarDeclaration",VarDeclaration},{"VarDecList",VarDecList},{ "VarDecMore",VarDecMore},
    {"VarIdList",VarIdList}, {"VarIdMore",VarIdMore}, {"ProcDec",ProcDec}, {"ProcDeclaration",ProcDeclaration},
    {"ProcDecMore",ProcDecMore}, {"ProcName",ProcName},{"ParamList",ParamList},{ "ParamDecList",ParamDecList},
    {"ParamMore",ParamMore}, {"Param",Param}, {"FormList",FormList}, {"FidMore",FidMore},
    {"ProcDecPart",ProcDecPart},{ "ProcBody",ProcBody},{"ProgramBody",ProgramBody}, {"StmList",StmList},
    {"StmMore",StmMore}, {"Stm",Stm},{"AssCall",AssCall},{"AssignmentRest",AssignmentRest},
    {"ConditionalStm",ConditionalStm}, {"StmL",StmL}, {"LoopStm",LoopStm}, {"InputStm",InputStm},
    {"InVar",InVar}, {"OutputStm",OutputStm},{ "ReturnStm",ReturnStm}, {"CallStmRest",CallStmRest},
    {"ActParamList",ActParamList}, {"ActParamMore",ActParamMore}, {"RelExp",RelExp}, {"OtherRelE",OtherRelE},
    {"Exp",Exp}, {"OtherTerm",OtherTerm},{"Term",Term}, {"OtherFactor",OtherFactor},
    {"Factor",Factor},{"Variable",Variable},{"VariMore",VariMore}, {"FieldVar",FieldVar},
    {"FieldVarMore",FieldVarMore}, {"CmpOp",CmpOp}, {"AddOp",AddOp}, {"MultOp",MultOp},
    {"ENDFILE",ENDFILE}, {"ERROR",ERROR},
    {"PROGRAM",PROGRAM}, { "PROCEDURE",PROCEDURE}, { "TYPE",TYPE }, { "VAR",VAR }, { "IF",IF },
    {"THEN",THEN}, {"ELSE",ELSE},{ "FI",FI},{"WHILE",WHILE}, {"DO",DO}, {"ENDWH",ENDWH},
    {"BEGIN",BEGIN}, {"END",END},{"READ",READ},{"WRITE",WRITE}, {"ARRAY[low..top]",ARRAY}, {"OF",OF},
    {"RECORD"，RECORD}, {"RETURN",RETURN},
    {"INTEGER",INTEGER},
    {"ID",ID}, {"INTC",INTC}, {"CHARC",CHARC},
    {":=",ASSIGN}, {"=",EQ}, {"<",LT} ,{"+",PLUS},
    { "-",MINUS},{"*",TIMES}, {"/",OVER}, {"(",LPAREN}, {")",RPAREN}, {".",DOT},{":",COLON},
    { ";",SEMI}, {",",COMMA}, {"[",LMIDPAREN}, {"]",RMIDPAREN},
};//{ "CHAR",CHAR},
*/