#pragma once

#include "globals.h"
#include "Lexical.h"
#include "Syntax.h"


extern vector<production> vecTree;//用于记录匹配过程中使用到的产生式，为下一步建造语法分析树做准备
extern vector<string> TokenValueVec;
extern vector<string> TokenTypeVec;

struct AnalysisTreeNode 
{
	int id;//方便在画dot图时区分节点
	string TokenValue;
	string TokenType;
	vector<AnalysisTreeNode *> children;
	AnalysisTreeNode(string s,int id) : TokenValue(s), id(id) {}
};

extern AnalysisTreeNode* root;//语法分析树的根节点
extern int global_id;//DOT Language中每个节点的id
extern unsigned int global_token_index;//用于将ID转化为变量名后者函数名等
extern unsigned int treeIndex;// 指示vTree的下标


void SNLCompiler();//主函数
void LexicalAnalysis();
void SyntaxAnalysis(productionList* ptrProducList);
void CreateSyntaxTree(productionList* ptrProducList);
void dfsBuildTree(AnalysisTreeNode*& node, productionList* ptrProducList);//深度优先遍历，建语法分析树树

