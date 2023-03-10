#pragma once
#include<string>
#include<vector>
#include<map>
#include<algorithm>
#include <cstdlib>
#include<set>
#include<iostream>
#include<fstream>
#include<stack>
using namespace std;
class TreeNode {
public:
	string Lex;//记录节点类型
	string Sem;//记录该单词的语义信息
	vector<TreeNode*> child;//子节点
	TreeNode()
	{
		Lex = "";//存储函数名跟保留字
		Sem = "";//如果是ID或者VAR额外存储值
	}
	TreeNode(string lex)
	{
		Lex = lex;
		Sem = "";
	}
	TreeNode(string lex, string sem)
	{
		Lex = lex;
		Sem = sem;
	}
};
string  trim(string s);//删除字符两头的空格
string remove_surplus_spaces(const string& s);//去除字符s中多余的空格
void change_to_rule();
set<string>first_str(string str);//生成字符串str的first集
void first_1();//循环调用first_str生成文法的first集合
void showFirstSet();
set<string> follow_str(string str, string old);//old用来保存谁递归调用了，确定是否产生闭包，以免形成死锁
void follow_1();//循环调用follow_str生成文法的first集合
void showFollowSet();
void predict();//直接求出所有文法产生式的first集
void showPredictSet();
void Initial();//初始化即生成
string LexToString(LexType lex);//将LEX即token类型转化为字符串
bool judgeTerminalVocabulary(string J_T_V);//判断是不是终极符
void out_tree(TreeNode* root);//打印树结构的dot语言代码
TreeNode* Program();
TreeNode* ProgramHead();
TreeNode* ProgramBody();
TreeNode* ProgramName();
TreeNode* DeclarePart();
TreeNode* TypeDec();
TreeNode* TypeDeclaration();
TreeNode* TypeDecList();
TreeNode* TypeDecMore();
TreeNode* Typeld();
TreeNode* TypeName();
TreeNode* BaseType();
TreeNode* StructureType();
TreeNode* ArrayType();
TreeNode* Low();
TreeNode* Top();
TreeNode* RecType();
TreeNode* FieldDecList();
TreeNode* FieldDecMore();
TreeNode* IdList();
TreeNode* IdMore();
TreeNode* VarDec();
TreeNode* VarDeclaration();
TreeNode* VarDecList();
TreeNode* FieldDecMore();
TreeNode* IdList();
TreeNode* IdMore();
TreeNode* VarDec();
TreeNode* VarDeclaration();
TreeNode* VarDecList();
TreeNode* VarDecMore();
TreeNode* VarIdList();
TreeNode* VarIdMore();
TreeNode* ProcDec();
TreeNode* ProcDeclaration();
TreeNode* ProcDecMore();
TreeNode* ProcName();
TreeNode* ParamList();
TreeNode* ParamDecList();
TreeNode* ParamMore();
TreeNode* Param();
TreeNode* FormList();
TreeNode* FidMore();
TreeNode* ProcDecPart();
TreeNode* ProcBody();
TreeNode* StmList();
TreeNode* StmMore();
TreeNode* Stm();
TreeNode* AssCall();
TreeNode* AssignmentRest();
TreeNode* ConditionalStm();
TreeNode* LoopStm();
TreeNode* InputStm();
TreeNode* Invar();
TreeNode* OutputStm();
TreeNode* ReturnStm();
TreeNode* CallStmRest();
TreeNode* ActParamList();
TreeNode* ActParamMore();
TreeNode* RelExp();
TreeNode* OtherRelE();
TreeNode* Exp();
TreeNode* OtherTerm();
TreeNode* Term();
TreeNode* OtherFactor();
TreeNode* Factor();
TreeNode* Variable();
TreeNode* VariMore();
TreeNode* FieldVar();
TreeNode* FieldVarMore();
TreeNode* CmpOp();
TreeNode* AddOp();
TreeNode* MultOp();