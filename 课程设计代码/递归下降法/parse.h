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
	string Lex;//��¼�ڵ�����
	string Sem;//��¼�õ��ʵ�������Ϣ
	vector<TreeNode*> child;//�ӽڵ�
	TreeNode()
	{
		Lex = "";//�洢��������������
		Sem = "";//�����ID����VAR����洢ֵ
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
string  trim(string s);//ɾ���ַ���ͷ�Ŀո�
string remove_surplus_spaces(const string& s);//ȥ���ַ�s�ж���Ŀո�
void change_to_rule();
set<string>first_str(string str);//�����ַ���str��first��
void first_1();//ѭ������first_str�����ķ���first����
void showFirstSet();
set<string> follow_str(string str, string old);//old��������˭�ݹ�����ˣ�ȷ���Ƿ�����հ��������γ�����
void follow_1();//ѭ������follow_str�����ķ���first����
void showFollowSet();
void predict();//ֱ����������ķ�����ʽ��first��
void showPredictSet();
void Initial();//��ʼ��������
string LexToString(LexType lex);//��LEX��token����ת��Ϊ�ַ���
bool judgeTerminalVocabulary(string J_T_V);//�ж��ǲ����ռ���
void out_tree(TreeNode* root);//��ӡ���ṹ��dot���Դ���
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