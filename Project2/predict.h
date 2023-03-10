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
#include"lexerAnalyse.h"

//·ÇÖÕ¼«·û
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
//ÖÕ¼«·û
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

map<string, enum TokenType> StrToToken = {

    {"Program",Program}, {"ProgramHead",ProgramHead}, {"ProgramName",ProgramName}, {"DeclarePart",DeclarePart},
    {"TypeDec",TypeDec}, { "TypeDeclaration", }, { "TypeDecList" }, { "TypeDecMore"},
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
    {"Factor"},"Variable", "VariMore", {"FieldVar",},
    {"FieldVarMore",FieldVarMore}, {"CmpOp",CmpOp}, {"AddOp",AddOp}, {"MultOp",MultOp},
        {":=",ASSIGN}, {"=",EQ}, {"<",LT} ,{"+",PLUS},
        { "-",MINUS},{"*",TIMES}, {"/",OVER}, {"(",LPAREN}, {")",RPAREN}, {".",DOT},{":",COLON},
        { ";",SEMI}, {",",COMMA}, {"[",LMIDPAREN}, {"]",RMIDPAREN},
};