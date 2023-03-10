#include "cifa.h"
#include"parse.h"
#include<vector>
#include<map>
#include<set>
#include<string>
#include<iostream>
#include<fstream>
#include<sstream>
#include<queue>
#include<utility>
using namespace std;
vector<string> LexToS
{
	//���Ƿ�
"ENDFILE", "ERROR",

//������
"PROGRAM", "PROCEDURE", "TYPE", "VAR", "IF",
"THEN", "ELSE", "FI", "WHILE", "DO",
"ENDWH", "BEGIN", "END", "READ", "WRITE",
"ARRAY", "OF", "RECORD", "RETURN",

//������
"INTEGER", "CHAR",

//���ַ�����
"ID", "INTC", "CHARC",

//�������
":=", "=", "<", "+", "-",
"*", "/", "(", ")", ".",
":", ";", ",", "[", "]",
"..", " "
};
struct Rule//��������ʽ
{
	string left;
	vector<string> right;
	set<string> Predict;
};
vector<Rule> Grammar;//����ʽ�ļ���
map<string, set<string> > First;
map<string, set<string> > Follow;
string start;    //��ʼ�ַ���
set<string> not_terminal_chars =//���ս�����ϣ��ֶ����칲67��
{
	"Program", "ProgramHead", "ProgramName", "DeclarePart",
	"TypeDec", "TypeDeclaration", "TypeDecList", "TypeDecMore",
	"TypeId", "TypeName", "BaseType", "StructureType",
	"ArrayType", "Low", "Top", "RecType",
	"FieldDecList", "FieldDecMore", "IdList", "IdMore",
	"VarDec", "VarDeclaration", "VarDecList", "VarDecMore",
	"VarIdList", "VarIdMore", "ProcDec", "ProcDeclaration",
	"ProcDecMore", "ProcName", "ParamList", "ParamDecList",
	"ParamMore", "Param", "FormList", "FidMore",
	"ProcDecPart", "ProcBody", "ProgramBody", "StmList",
	"StmMore", "Stm", "AssCall", "AssignmentRest",
	"ConditionalStm", "LoopStm", "InputStm",
	"Invar","OutputStm","ReturnStm", "CallStmRest",
	"ActParamList", "ActParamMore", "RelExp", "OtherRelE",
	"Exp", "OtherTerm", "Term", "OtherFactor",
	"Factor", "Variable", "VariMore", "FieldVar",
	"FieldVarMore","CmpOp", "AddOp", "MultOp"
};
bool judgeTerminalVocabulary(string J_T_V)//�ж��Ƿ����ս��
{
	if (not_terminal_chars.find(J_T_V) == not_terminal_chars.end())
		return true;
	return false;
}
/*****************************************************************************
******************************************************************************
*                                                                    *********
*                   ��ȡ�ķ��ļ���ִ���Grammar�ڣ�                  *********
*                                                                    *********
******************************************************************************
******************************************************************************/
//ɾ���ַ���ͷ�Ŀո�
string  trim(string s) {
	if (s.empty()) {//����ǿմ�
		return s;
	}
	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
	return s;
}
//ȥ���ַ�s�ж���Ŀո�
string remove_surplus_spaces(const string& s)
{
	string src = trim(s);
	string result = "";
	for (int i = 0; src[i] != '\0'; i++)// \0=�ַ�����β
	{
		if (src[i] != ' ')
		{
			result.append(1, src[i]);
		}
		else if (src[i + 1] != ' ')
			result.append(1, src[i]);
	}
	return result;
}
void change_to_rule()
{
	string line;//��ȡ�ķ��ļ�ÿһ��
	string left, right;//ÿ���ķ�����߸��ұ�
	fstream file_wenfa("D:\\wenfa2020.txt", ios::in);
	if (file_wenfa.is_open() == false)
	{
		cout << "�ķ��ļ���ʧ��" << endl;
		return;
	}
	while (getline(file_wenfa, line))
	{
		Rule c_rule;//��ǰ�����ķ���������ɺ����grammar��
		string x;//�洢ÿһ������ĸ��
		for (int i = 0; i < line.length(); i++)//�����е��ַ����е��Ʊ��\tת��Ϊ�ո�
			if (line[i] == '\t')
				line = line.replace(i, 1, " ");

		int split = line.find("::=");//�ҵ��ָ��ַ����ڵ�λ��

		if (split != -1)//���� | ���ڵ���
		{
			//����ÿ����������Ҳ���
			left = line.substr(0, split);
			right = line.substr(split + 3, line.length());


			//��Ϊ�󲿺��� ��18������������Ų��֣��ʷ�������ս��
			split = line.find(")");
			left = left.substr(split + 1, left.length());//ȥ�����ż����
			left = remove_surplus_spaces(left);//��ȥǰ���ͺ󵼿ո񣬱���֮��Ĵ���
			right = remove_surplus_spaces(right);

			c_rule.left = left;

			int j = 0;//ǰһ����ĸ����ĩβ
			int z = 0;//��ǰ��ĸ��ĩβ
			int flag = 0;//�жϵ�ǰ��������Ӣ�Ļ��������ķ���

			for (int i = 0; i < right.length(); i++, z++)
			{
				if ((right[i] <= 'Z' && right[i] >= 'A') || (right[i] <= 'z' && right[i] >= 'a'))//�Ƿ���Ӣ��
				{
					if (flag != 0)//����Ӣ�ģ�Ӧ�ý�֮ǰ�����б���
					{
						x = right.substr(j, z - j);//����ǰһ��Ӧ�ñ������ĸ
						if (!x.empty())
							c_rule.right.push_back(x);//����ṹ��
						j = i;//�任ĩβ
						z = i;//�趨��ǰ��ĸĩβ
						flag = 0;//�任��ʽ
					}
					else
						continue;//�����Ȼ��Ӣ�ľͼ���
				}
				else
				{
					if (flag != 1)//���Ƿ��ţ�Ӧ�ý�֮ǰ�����б���
					{
						x = right.substr(j, z - j);
						if (!x.empty())
							c_rule.right.push_back(x);

						if (right[i] == '\0' || right[i] == 32)//�ǿո����ַ���Ӧ����������ֱ������
						{
							j = i + 1;
							z = i;
							flag = 3;
						}
						else
						{
							j = i;
							z = i;
							flag = 1;
						}

						continue;

					}
					else
						if (right[i] == '\0' || right[i] == 32)//�����������ַ����пո�Ӧ�ý���ĸĩβ����Ϊ�ո�ǰһ�ַ���λ�ã�����z-1
						{
							z--;
							flag = 3;
						}
					continue;
				}
			}
			x = right.substr(j, 999);//�Ե�ĩβ����ĸ���д���

			if (!x.empty())
				c_rule.right.push_back(x);

			Grammar.push_back(c_rule);
		}
		else
		{
			c_rule.left = left;
			split = line.find("|");
			right = line.substr(split + 1, 9999);
			right = remove_surplus_spaces(right);
			int j = 0, z = 0;
			int flag = 0;
			for (int i = 0; i < right.length(); i++, z++)
			{

				if ((right[i] <= 'Z' && right[i] >= 'A') || (right[i] <= 'z' && right[i] >= 'a'))//�Ƿ���Ӣ��
				{
					if (flag != 0)//����Ӣ�ģ�Ӧ�ý�֮ǰ�����б���
					{
						x = right.substr(j, z - j);//����ǰһ��Ӧ�ñ������ĸ
						if (!x.empty())
							c_rule.right.push_back(x);//����ṹ��
						j = i;//�任ĩβ
						z = i;//�趨��ǰ��ĸĩβ
						flag = 0;//�任��ʽ
					}
					else
						continue;//�����Ȼ��Ӣ�ľͼ���
				}


				else
				{
					if (flag != 1)//���Ƿ��ţ�Ӧ�ý�֮ǰ�����б���
					{
						x = right.substr(j, z - j);
						if (!x.empty())
							c_rule.right.push_back(x);

						if (right[i] == '\0' || right[i] == 32)//�ǿո����ַ���Ӧ����������ֱ������
						{
							j = i + 1;
							z = i;
							flag = 3;
						}
						else
						{

							j = i;
							z = i;
							flag = 1;

						}

						continue;

					}
					else
						if (right[i] == '\0' || right[i] == 32)//�����������ַ����пո�Ӧ�ý���ĸĩβ����Ϊ�ո�ǰһ�ַ���λ�ã�����z-1
						{
							z--;
							flag = 3;
						}
					continue;
				}
			}
			x = right.substr(j, 999);//�Ե�ĩβ����ĸ���д���
			if (!x.empty())
				c_rule.right.push_back(x);
			Grammar.push_back(c_rule);
		}
	}
	file_wenfa.close();
}
/*****************************************************************************
******************************************************************************
**********                                                           *********
**********                    ����FIRST����                          *********
**********                                                           *********
******************************************************************************
******************************************************************************/

set<string>first_str(string str)
{
	set<string> tempSet;//�洢ÿ��str��first���ϣ�Ҳ�Ƿ���ֵ
	for (int j = 0; j < Grammar.size(); j++)//����ÿһ���ķ�
	{
		if (Grammar[j].left == str)//����󲿵��ڸ����ַ���
		{
			for (int k = 0; k < Grammar[j].right.size(); k++)//���ȡ���ұߵĵ�k���ַ�����ôǰk-1���ַ������Ƴ��մ�
			{
				//1�������ǰ�ַ��ǿմ����������һ���ַ�
				if (Grammar[j].right[k] == "@" && k + 1 == Grammar[j].right.size())
				{
					tempSet.insert("@");
					break;
				}
				//2�������ǰ�ַ����ռ����Ҳ��ǿմ�
				else if ((not_terminal_chars.find(Grammar[j].right[k]) == not_terminal_chars.end() && Grammar[j].right[k] != "@"))
				{
					tempSet.insert(Grammar[j].right[k]);
					break;//���������һ������ʽ
				}
				//3�������ǰ�ַ��Ƿ��ռ�������������first����
				else if (not_terminal_chars.find(Grammar[j].right[k]) != not_terminal_chars.end())
				{
					set<string> tempSet1 = first_str(Grammar[j].right[k]);//�ݹ�������
					tempSet.insert(tempSet1.begin(), tempSet1.end());

					if (tempSet1.find("@") == tempSet1.end() || k + 1 == Grammar[j].right.size())//��������пմ������пմ��������ұ��ַ����һ��
					{
						break;//ֱ�ӿ�����һ������ʽ
					}
				}
				//��ǰ�ַ����Ƶ����մ�����������ַ�
				tempSet.erase("@");//ɾ�������ȥ�Ŀմ�,��ʼ������һ��ѭ��
			}
		}
	}
	return tempSet;
}
void first_1()
{
	set<string>::iterator it;
	for (it = not_terminal_chars.begin(); it != not_terminal_chars.end(); it++)
	{
		set<string> s = first_str(*it);
		First.insert(pair<string, set<string> >(*it, s));
	}
}

void showFirstSet()
{
	ofstream outFile;
	outFile.open("D:\\firstSetlast.csv", ios::out);
	outFile << "�ķ�����" << "," << "First��" << endl;
	for (map<string, set<string> >::iterator it1 = First.begin(); it1 != First.end(); it1++)
	{
		outFile << it1->first << "," << "{ ";
		for (set<string>::iterator it2 = it1->second.begin(); it2 != it1->second.end(); it2++)
		{
			if (*it2 == ",")
			{
				outFile << "COMMA ";
			}
			else outFile << *it2 << " ";
		}
		outFile << "}" << endl;
	}
	outFile.close();
}
/*****************************************************************************
******************************************************************************
**********                                                           *********
**********                    ����FOLLOW����                         *********
**********                                                           *********
******************************************************************************
******************************************************************************/

set<string> follow_str(string str, string old)//old��������˭�ݹ�����ˣ�ȷ���Ƿ�����հ��������γ�����
{
	set<string> tempSet;//���浱ǰ���follow��
	for (int i = 0; i < Grammar.size(); i++)//����ÿһ���ķ�����ʽ
	{
		for (int k = 0; k < Grammar[i].right.size(); k++)//ȡ����i������ʽ�ұߵĵ�k���ַ�
		{
			//������ֵ�ǰ�ַ������ڲ���ʽ�ұ�
			if (Grammar[i].right[k] == str)
			{
				//���������۵�ǰ�ַ��ǲ����ڲ���ʽ���ұ�
				//һ��������ֵ�ǰ�ַ������ڲ���ʽ���ұߣ���ô��������left��follow����
				if ((Grammar[i].right.size() - 1) == k)
				{
					//1�����ݹ�ʱ��������߸����ұ����ʱ����֤��������֤�Ǹ��հ������������Ԫ�أ�����ֱ����������������
					if (old == Grammar[i].left || Grammar[i].right[k] == Grammar[i].left)
					{
						break;
					}
					//2�������ڲ���ʽ���ұ�,������Ƴ��Ĳ������������������γ�һ���������Ԫ�صıհ�����
					//��ô��ʱ�۲�left��follow����
					else
					{
						set<string> tempSet1 = follow_str(Grammar[i].left, str);
						if (tempSet1.size() != 0)
						{
							tempSet.insert(tempSet1.begin(), tempSet1.end());
						}
					}
				}
				//�������ǳ��������ұߣ���ô�۲����������һ���ַ���first����
				int j = 1;//j-1�����ſ��Ƴ��մ�
				string str_next;
				while (k + j < Grammar[i].right.size())//k+j������һ���ַ�������ַ���first�����пռ�����j++ֱ��right��ĩβ�ַ�
				{
					str_next = Grammar[i].right[k + j];
					//1�����ռ���ֱ�����
					if (not_terminal_chars.find(str_next) == not_terminal_chars.end())
					{
						tempSet.insert(str_next);
						break;
					}
					// 2���Ƿ��ռ�������Ҫ�����Ƿ����Ƴ��մ�
					else
					{
						tempSet.insert(First[str_next].begin(), First[str_next].end());//���Ͻ�������FIRST����	
						set<string>::iterator s;
						s = tempSet.find("@");
						if (s == tempSet.end())//����һ���ַ���first������û�г��ֿռ�
						{
							break;//�������ÿ��Ǻ�ߵ��ַ�
						}
						else//����һ���ַ���first�����г��ֿռ�
						{
							tempSet.erase(s);//����ɾ��@
							j++;//�۲���һ���ַ�����һ���ַ�
							if (k + j == Grammar[i].right.size())//����û����һ���ַ�����һ���ַ�����β�������Ƴ��մ������Բ���left��
							{
								set<string> tempSet1 = follow_str(Grammar[i].left, str);	//�ݹ����
								if (tempSet1.size() != 0)
								{
									tempSet.insert(tempSet1.begin(), tempSet1.end());
								}
							}
						}
					}
				}
			}
		}
	}
	return tempSet;
}
void follow_1()
{
	Follow[start].insert("#");//��ʼ����ʼ�ַ���follow����
	set<string>::iterator it_F;
	for (it_F = not_terminal_chars.begin(); it_F != not_terminal_chars.end(); it_F++)
	{
		set<string> s = follow_str(*it_F, "");
		Follow.insert(pair<string, set<string> >(*it_F, s));
	}
}
void showFollowSet()
{
	ofstream outFile;
	outFile.open("D:\\followSet9.csv", ios::out);
	outFile << "�ķ�����" << "," << "Follow��" << endl;
	for (map<string, set<string> >::iterator it1 = Follow.begin(); it1 != Follow.end(); it1++)
	{
		outFile << it1->first << "," << "{ ";
		for (set<string>::iterator it2 = it1->second.begin(); it2 != it1->second.end(); it2++)
		{
			if (*it2 == ",")
			{
				outFile << "COMMA ";
			}
			else outFile << *it2 << " ";
		}
		outFile << "}" << endl;
	}
	outFile.close();
}
/*****************************************************************************
******************************************************************************
**********                                                           *********
**********                    ����PREDICT����                        *********
**********                                                           *********
******************************************************************************
******************************************************************************/
void predict()
{
	vector<string>::iterator it;
	for (int i = 0; i < Grammar.size(); i++)//����ÿһ���ķ�����ʽ
	{
		int k = 0;//�ж��Ҳ���k���ַ�
		while (k < Grammar[i].right.size())
		{
			//������ս�������ǿ��ַ�
			if (not_terminal_chars.find(Grammar[i].right[k]) == not_terminal_chars.end() && Grammar[i].right[k] != "@")
			{
				Grammar[i].Predict.insert(Grammar[i].right[k]);
				break;
			}
			//����ǿ��ַ�
			else if (Grammar[i].right[k] == "@")
			{
				Grammar[i].Predict.insert(Follow[Grammar[i].left].begin(), Follow[Grammar[i].left].end());//������ߵ�follow����

				break;
			}

			//��������ս��:�ж��Ƿ����Ƴ��մ�
			set<string> s = First[Grammar[i].right[k]];
			set<string>::iterator iter = find(s.begin(), s.end(), "@");

			if (iter == s.end())//����ʽ�Ҳ���k���ַ�first���ϲ�����
			{
				Grammar[i].Predict = First[Grammar[i].right[k]];
				break;
			}
			else if (iter != s.end()) // ����ʽ�Ҳ���k���ַ�first���Ϻ��գ���������ж�
			{
				s.erase("@");//ɾ�����ַ���
				Grammar[i].Predict.insert(s.begin(), s.end());//���ϸ��ַ���first����֮������
				k++;
				if (k == Grammar[i].right.size())
				{
					Grammar[i].Predict.insert(Follow[Grammar[i].left].begin(), Follow[Grammar[i].left].end());//������ߵ�follow����
					break;
				}
			}
		}
	}
}
void showPredictSet()
{
	ofstream outFile;
	outFile.open("D:\\predictSet1.csv", ios::out);
	outFile << "����ʽ" << "," << "Predict��" << endl;
	for (int i = 0; i < Grammar.size(); i++)
	{
		outFile << Grammar[i].left << "->";
		for (int j = 0; j < Grammar[i].right.size(); j++)
		{
			if (Grammar[i].right[j] == ",")
			{
				outFile << "COMMA";
			}
			else outFile << Grammar[i].right[j];
			if (j < Grammar[i].right.size() - 1)
			{
				outFile << " ";
			}
			else
			{
				outFile << ",";
			}
		}
		outFile << "{ ";
		set<string>::iterator j = Grammar[i].Predict.begin();
		while (j != Grammar[i].Predict.end())
		{
			if (*j == ",")
			{
				outFile << "COMMA ";
			}
			else outFile << *j << " ";
			j++;
		}
		outFile << "}" << endl;
	}

	outFile.close();
}
vector<Token> TL;
string LexToString(LexType lex)
{
	return LexToS[lex];
}
void Initial()
{

	change_to_rule();
	start = Grammar[0].left;
	first_1();
	showFirstSet();
	follow_1();
	showFollowSet();
	predict();
	showPredictSet();
	TL = getTokenList();
	chainToFile(TL);
}

int sum = -1;
Token cur_token;
bool signal = true;//Ϊ�����ʾ��Ҫread_token()

void read_token()
{
	sum = sum + 1;
	cur_token = TL[sum];
}
/*****************************************************************************
******************************************************************************
**********                                                           *********
**********                    �ݹ��½���                             *********
**********                                                           *********
******************************************************************************
******************************************************************************/
//�����﷨����Dot���Դ���
void out_tree(TreeNode* root)
{
	fstream outFile("D:\\S_tree.txt", ios::out);
	queue<pair<TreeNode*, int> > q;//��Ҫ�洢�ڵ��Լ����
	int node_num = 1;//�ڵ���
	stringstream ss;//��DOT Language��ʾ���﷨�����ַ����ŵ������
	ss.clear();
	/**************************************************************************/
	/****************** ��ʼ����DOT�����ļ� ***********************************/
	/**************************************************************************/
	ss << "digraph GrammarTree {" << endl;
	if (root)
	{
		q.push(pair<TreeNode*, int>(root, node_num));//���ڵ����
	}

	while (!q.empty())
	{
		pair<TreeNode*, int> t = q.front();
		q.pop();
		if (t.first->Lex == "ID" || t.first->Lex == "INTC" || t.first->Lex == "CHARC")//�����id֮��ĸı���״����ɫ
		{
			ss << "\"" << t.second << "\"[shape=square; style=filled; fillcolor=yellow; label=\"" << t.first->Lex;
		}

		else if (!judgeTerminalVocabulary(t.first->Lex))//����Ƿ��ս���ı���ɫ
		{
			ss << "\"" << t.second << "\"[style=filled; fillcolor=green; label=\"" << t.first->Lex;
		}

		else//����token����ɫ
		{
			ss << "\"" << t.second << "\"[style=filled; fillcolor=lightpink; label=\"" << t.first->Lex;//��ǰ�ĸ��ڵ㣺�����õ��﷨��������
		}


		if (!t.first->Sem.empty())//�ж��Ƿ���Ҫ���ֵsem
		{
			ss << ":" << t.first->Sem << "\"];" << endl;
		}
		else
		{
			ss << "\"];" << endl;
		}

//1.1��ȷ����ǰ��Ҫָ��˭
		if (t.first->child.size())
		{

			ss << "\"" << t.second << "\"" << "->" << "\{";//�������ǰ�ڵ�

			for (int j = 1; j <= t.first->child.size(); j++)
			{
				ss << "\"" << ++node_num << "\";";
				q.push(pair<TreeNode*, int>(t.first->child[j - 1], node_num));//��ǰ�ڵ�������ӽڵ��Լ�������
			}

			ss << "\}" << endl;

//1.2�����ӽڵ㶼����ͬһ�߶�
			ss << "{rank=same; ";
			for (int j = t.first->child.size() - 1; j >= 0; j--)
			{
				ss << "\"" << node_num - j << "\";";
			}
			ss << "\}" << endl;
//1.3Ȼ�����ѭ�������ӽڵ�
		}
	}
	ss << "}";
	outFile << ss.str();
	outFile.close();
}
TreeNode* Program()
{
	TreeNode* node = new TreeNode("Program");//�½����ڵ�,���ؼ��Ǹ��ڵ�
	read_token();
	set<string>::iterator iter = find(Grammar[0].Predict.begin(), Grammar[0].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[0].Predict.end())
	{
		TreeNode* tempnode = new TreeNode();//�½���ʱ���ڵ㣬���ڴ洢���ص������ڵ�
		if (tempnode = ProgramHead())//�ж��Ƿ��������ڵ㣺���Ƿ���һ�������Ĵ��벻����
		{
			node->child.push_back(tempnode);
			read_token();
			TreeNode* tempnode1 = new TreeNode();
			if (tempnode1 = DeclarePart())
			{
				node->child.push_back(tempnode1);
				if (signal)
				{
					read_token();
				}
				else
				{
					signal = true;
				}
				TreeNode* tempnode2 = new TreeNode();
				if (tempnode2 = ProgramBody())
				{
					node->child.push_back(tempnode2);
					read_token();//�������Ľ�������һ����
					if (cur_token.Lex == DOT)
					{
						node->child.push_back(new TreeNode("."));
						return node;
					}
					cout << "ERROR:" << cur_token.Lex<<"	."<<endl;
					return nullptr;
				}
			}
		}
	}
	return nullptr;
}
TreeNode* ProgramHead()
{
	TreeNode* node = new TreeNode("ProgramHead");//�½����ڵ�
	if (cur_token.Lex == PROGRAM)
	{
		TreeNode* tempnode = new TreeNode("PROGRAM");
		node->child.push_back(tempnode);
		read_token();
		TreeNode* tempnode1 = new TreeNode();//�½���ʱ���ڵ㣬���ڴ洢���ص������ڵ�
		if (tempnode1 = ProgramName())
		{
			node->child.push_back(tempnode1);
			return node;
		}
	}
	cout << "ERROR:" << cur_token.Lineshow << "		PROGRAM" << endl;
	return  nullptr;
	return nullptr;
}
TreeNode* ProgramName()
{
	TreeNode* node = new TreeNode("ProgramName");//�½����ڵ�
	if (cur_token.Lex == ID)
	{
		TreeNode* tempnode = new TreeNode("ID",cur_token.Sem);
		node->child.push_back(tempnode);
		return node;
	}
	cout << "ERROR:" << cur_token.Lineshow << "		ID" << endl;
	return nullptr;
}
TreeNode* DeclarePart()
{
	TreeNode* node = new TreeNode("DeclarePart");//�½����ڵ�
	set<string>::iterator iter = find(Grammar[3].Predict.begin(), Grammar[3].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[3].Predict.end())
	{
		TreeNode* tempnode = new TreeNode();
		if (tempnode = TypeDec())
		{
			node->child.push_back(tempnode);
			if (signal)
				read_token();
			else
			{
				signal = true;
			}
			TreeNode* tempnode1 = new TreeNode();
			if (tempnode1 = VarDec())
			{
				node->child.push_back(tempnode1);
				if (signal)
					read_token();
				else
					signal = true;
				TreeNode* tempnode2 = new TreeNode();
				if ((tempnode2 = ProcDec()))
				{
					node->child.push_back(tempnode2);
					return node;
				}
			}
		}
	}
	return nullptr;
}
TreeNode* TypeDec()
{
	TreeNode* node = new TreeNode("TypeDec");//�½����ڵ�
	TreeNode* tempnode = new TreeNode();
	set<string>::iterator iter = find(Grammar[4].Predict.begin(), Grammar[4].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[4].Predict.end())
	{
		node->child.push_back(new TreeNode("@"));
		signal = false;
		return node;
	}
	iter = find(Grammar[5].Predict.begin(), Grammar[5].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[5].Predict.end())
	{
		if (tempnode = TypeDeclaration())
		{
			node->child.push_back(tempnode);
			return node;
		}

	}
	return nullptr;
}
TreeNode* TypeDeclaration()
{
	TreeNode* node = new TreeNode("TypeDeclaration");//�½����ڵ�
	TreeNode* tempnode = new TreeNode();
	if (cur_token.Lex == TYPE)
	{
		node->child.push_back(new TreeNode("TYPE"));
		read_token();
		if (tempnode = TypeDecList())
			node->child.push_back(tempnode);
		return node;
	}
	cout << "ERROR:" << cur_token.Lineshow << "		TYPE" << endl;
	return  nullptr;
}
TreeNode* TypeDecList()
{
	TreeNode* node = new TreeNode("TypeDecList");//�½����ڵ�
	set<string>::iterator iter = find(Grammar[7].Predict.begin(), Grammar[7].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[7].Predict.end())
	{
		TreeNode* tempnode = new TreeNode();
		if (tempnode = Typeld())
		{
			node->child.push_back(tempnode);
			read_token();
			if (cur_token.Lex == EQUAL)
			{
				node->child.push_back(new TreeNode("EQUAL"));
				read_token();
				TreeNode* tempnode2 = new TreeNode();
				if (tempnode2 = TypeName())
				{
					node->child.push_back(tempnode2);
					read_token();
					if (cur_token.Lex == SEMI)
					{
						node->child.push_back(new TreeNode("SEMI"));
						read_token();
						TreeNode* tempnode3 = new TreeNode();
						if (tempnode3 = TypeDecMore())
						{
							node->child.push_back(tempnode3);
							return node;
						}

					}
					cout << "ERROR:" << cur_token.Lineshow << "		;" << endl;
					return  nullptr;
				}
			}
			cout << "ERROR:" << cur_token.Lineshow << "		=" << endl;
			return  nullptr;
		}
	}
	return  nullptr;
}
TreeNode* TypeDecMore()
{
	TreeNode* node = new TreeNode("TypeDecMore");//�½����ڵ�
	TreeNode* tempnode = new TreeNode();
	set<string>::iterator iter = find(Grammar[8].Predict.begin(), Grammar[8].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[8].Predict.end())
	{
		node->Lex = "@";
		signal = false;
		return node;
	}
	iter = find(Grammar[9].Predict.begin(), Grammar[9].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[9].Predict.end())
	{
		if (tempnode = TypeDecList())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	return  nullptr;
}
TreeNode* Typeld()
{
	TreeNode* node = new TreeNode("TypeId");//�½����ڵ�
	if (cur_token.Lex == ID)
	{
		TreeNode* tempnode = new TreeNode();
		tempnode->Lex = "ID";
		node->child.push_back(tempnode);
		return node;
	}
	cout << "ERROR:" << cur_token.Lineshow << "		ID" << endl;
	return  nullptr;
}
TreeNode* TypeName()
{
	TreeNode* node = new TreeNode("TypeName");//�½����ڵ�
	TreeNode* tempnode = new TreeNode();
	set<string>::iterator iter = find(Grammar[11].Predict.begin(), Grammar[11].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[11].Predict.end())
	{

		if (tempnode = BaseType())
		{
			node->child.push_back(tempnode);
		}
		return node;
	}

	iter = find(Grammar[12].Predict.begin(), Grammar[12].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[12].Predict.end())
	{
		if (tempnode = StructureType())
		{
			node->child.push_back(tempnode);
		}
		return node;
	}

	if (cur_token.Lex == ID)
	{
		node->child.push_back(new TreeNode("ID", cur_token.Sem));
		return node;
	}
	cout << "ERROR:" << cur_token.Lineshow << "		ID" << endl;
	return  nullptr;
}
TreeNode* BaseType()
{
	TreeNode* node = new TreeNode("BaseType");//�½����ڵ�
	if (cur_token.Lex == INTEGER)
	{
		node->child.push_back(new TreeNode("INTEGER"));
		return node;
	}
	if (cur_token.Lex == CHAR1)
	{
		node->child.push_back(new TreeNode("CHAR1", cur_token.Sem));
		return node;
	}
	cout << "ERROR:" << cur_token.Lineshow << "INTEGER/CHAR" << endl;
	return  nullptr;
}
TreeNode* StructureType()
{
	TreeNode* node = new TreeNode("StructureType");//�½����ڵ�
	TreeNode* tempnode = new TreeNode();
	set<string>::iterator iter = find(Grammar[16].Predict.begin(), Grammar[16].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[16].Predict.end())
	{
		if (tempnode = ArrayType())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}

	iter = find(Grammar[17].Predict.begin(), Grammar[17].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[17].Predict.end())
	{
		if (tempnode = RecType())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	return  nullptr;
}
TreeNode* ArrayType()
{
	TreeNode* node = new TreeNode("ArrayType");//�½����ڵ�
	if (cur_token.Lex == ARRAY)
	{
		node->child.push_back(new TreeNode("ARRAY"));
		read_token();

		if (cur_token.Lex == LMIDPAREN)
		{
			node->child.push_back(new TreeNode("["));
			read_token();
			TreeNode* tempnode = new TreeNode();
			if (tempnode = Low())
			{
				node->child.push_back(tempnode);
				read_token();
				if (cur_token.Lex == UNDERANGE)
				{
					node->child.push_back(new TreeNode(".."));
					read_token();
					TreeNode* tempnode1 = new TreeNode();
					if (tempnode1 = Top())
					{
						node->child.push_back(tempnode1);
						read_token();
						if (cur_token.Lex == RMIDPAREN)
						{
							node->child.push_back(new TreeNode("]"));
							read_token();
							if (cur_token.Lex == OF)
							{
								node->child.push_back(new TreeNode("OF"));
								read_token();
								TreeNode* tempnode2 = new TreeNode();
								if (tempnode2 = BaseType())
								{
									node->child.push_back(tempnode2);
									return node;
								}
							}
							cout << "ERROR:" << cur_token.Lineshow << "		OF" << endl;
							return  nullptr;

						}
						cout << "ERROR:" << cur_token.Lineshow << "		]" << endl;
						return  nullptr;
					}
				}
				cout << "ERROR:" << cur_token.Lineshow << "		.." << endl;
				return  nullptr;
			}
		}
		cout << "ERROR:" << cur_token.Lineshow << "		[" << endl;
		return  nullptr;
	}
	cout << "ERROR:" << cur_token.Lineshow << "		ARRAY" << endl;
	return  nullptr;
}
TreeNode* Low()
{
	TreeNode* node = new TreeNode("Low");//�½����ڵ�
	if (cur_token.Lex == INTC)
	{
		node->child.push_back(new TreeNode("INTC", cur_token.Sem));
		return node;
	}
	cout << "ERROR:" << cur_token.Lineshow << "		INTC" << endl;
	return  nullptr;
}
TreeNode* Top()
{
	TreeNode* node = new TreeNode("Top");//�½����ڵ�
	if (cur_token.Lex == INTC)
	{
		node->child.push_back(new TreeNode("INTC", cur_token.Sem));
		return node;
	}
	cout << "ERROR:" << cur_token.Lineshow << "		INTC" << endl;
	return  nullptr;
}
TreeNode* RecType()
{
	TreeNode* node = new TreeNode("RecType");//�½����ڵ�
	if (cur_token.Lex == RECORD)
	{
		node->child.push_back(new TreeNode("RECORD"));
		read_token();
		TreeNode* tempnode = new TreeNode();
		if (tempnode = FieldDecList())
		{
			node->child.push_back(tempnode);
			if (signal)
				read_token();
			else
				signal = false;
			if (cur_token.Lex == END)
			{
				node->child.push_back(new TreeNode("END"));
				return node;
			}
			cout << "ERROR:" << cur_token.Lineshow << "		END" << endl;
			return  nullptr;
		}
	}
	cout << "ERROR:" << cur_token.Lineshow << "		RECORD" << endl;
	return  nullptr;
}
TreeNode* FieldDecList()
{
	TreeNode* node = new TreeNode("FieldDecList");
	set<string>::iterator iter = find(Grammar[22].Predict.begin(), Grammar[22].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[22].Predict.end())
	{
		TreeNode* tempnode = new TreeNode();
		if (tempnode = BaseType())
		{
			node->child.push_back(tempnode);
			read_token();
			TreeNode* tempnode1 = new TreeNode();
			if (IdList())
			{
				node->child.push_back(tempnode1);
				if (signal)
					read_token();
				else
					signal = true;
				if (cur_token.Lex == SEMI)
				{
					node->child.push_back(new TreeNode(";"));
					read_token();
					TreeNode* tempnode2 = new TreeNode();
					if (tempnode = FieldDecMore())
					{
						node->child.push_back(tempnode);
						return node;
					}
				}
				cout << "ERROR:" << cur_token.Lineshow << "		;" << endl;
				return  nullptr;
			}
		}
	}
	iter = find(Grammar[23].Predict.begin(), Grammar[23].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[23].Predict.end())
	{
		TreeNode* tempnode3 = new TreeNode();
		if (tempnode3 = ArrayType())
		{
			node->child.push_back(tempnode3);
			read_token();
			TreeNode* tempnode4 = new TreeNode();
			if (tempnode4 = IdList())
			{
				node->child.push_back(tempnode4);
				read_token();
				if (cur_token.Lex == SEMI)
				{
					node->child.push_back(new TreeNode(";"));
					read_token();
					TreeNode* tempnode5 = new TreeNode();
					if (tempnode5 = FieldDecMore())
					{
						node->child.push_back(tempnode5);
						return node;
					}
				}
				cout << "ERROR:" << cur_token.Lineshow << "		;" << endl;
				return  nullptr;
			}
		}
	}
	return  nullptr;
}
TreeNode* FieldDecMore()
{
	TreeNode* node = new TreeNode("FieldDecMore");
	TreeNode* tempnode = new TreeNode();
	set<string>::iterator iter = find(Grammar[24].Predict.begin(), Grammar[24].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[24].Predict.end())
	{
		signal = false;
		node->child.push_back(new TreeNode("@"));
		return node;
	}
	iter = find(Grammar[25].Predict.begin(), Grammar[25].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[25].Predict.end())
	{

		if (tempnode = FieldDecList())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	return  nullptr;
}
TreeNode* IdList()
{
	TreeNode* node = new TreeNode("IdList");

	if (cur_token.Lex == ID)
	{
		TreeNode* tempnode = new TreeNode("ID", cur_token.Sem);
		node->child.push_back(tempnode);
		read_token();
		TreeNode* tempnode1 = new TreeNode();
		if (tempnode1 = IdMore())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	cout << "ERROR:" << cur_token.Lineshow << "		ID" << endl;
	return  nullptr;
}
TreeNode* IdMore()
{
	TreeNode* node = new TreeNode("IdMore");
	set<string>::iterator iter = find(Grammar[27].Predict.begin(), Grammar[27].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[27].Predict.end())
	{
		signal = false;
		node->child.push_back(new TreeNode("@"));
		return node;
	}
	if (cur_token.Lex == COMMA)
	{
		node->child.push_back(new TreeNode("COMMA"));
		read_token();
		TreeNode* tempnode = new TreeNode();
		if (tempnode = IdList())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	cout << "ERROR:" << cur_token.Lineshow << "		," << endl;
	return  nullptr;
}
TreeNode* VarDec()
{
	TreeNode* node = new TreeNode("VarDec");
	TreeNode* tempnode = new TreeNode();
	set<string>::iterator iter = find(Grammar[29].Predict.begin(), Grammar[29].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[29].Predict.end())
	{
		signal = false;
		node->child.push_back(new TreeNode("@"));
		return node;
	}
	iter = find(Grammar[30].Predict.begin(), Grammar[30].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[30].Predict.end())
	{
		if (tempnode = VarDeclaration())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	return  nullptr;
}
TreeNode* VarDeclaration()
{
	TreeNode* node = new TreeNode("VarDeclaration");

	if (cur_token.Lex == VAR)
	{
		node->child.push_back(new TreeNode("VAR"));
		read_token();
		TreeNode* tempnode = new TreeNode();
		if (tempnode = VarDecList())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	cout << "ERROR:" << cur_token.Lineshow << "		VAR" << endl;
	return  nullptr;
}
TreeNode* VarDecList()
{
	TreeNode* node = new TreeNode("VarDecList");
	set<string>::iterator iter = find(Grammar[32].Predict.begin(), Grammar[32].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[32].Predict.end())
	{
		TreeNode* tempnode = new TreeNode();
		if (tempnode = TypeName())
		{
			node->child.push_back(tempnode);
			read_token();
			TreeNode* tempnode1 = new TreeNode();
			if (tempnode1 = VarIdList())
			{
				node->child.push_back(tempnode1);
				if (signal)
					read_token();
				else
					signal = true;
				if (cur_token.Lex == SEMI)
				{
					node->child.push_back(new TreeNode(";"));
					read_token();
					TreeNode* tempnode2 = new TreeNode();
					if (tempnode2 = VarDecMore())
					{
						node->child.push_back(tempnode2);
						return node;
					}
				}
				cout << "ERROR:" << cur_token.Lineshow << "		;" << endl;
				return  nullptr;
			}
		}
	}
	return nullptr;
}
TreeNode* VarDecMore()
{
	TreeNode* node = new TreeNode("VarDecMore");
	set<string>::iterator iter = find(Grammar[33].Predict.begin(), Grammar[33].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[33].Predict.end())
	{
		signal = false;
		node->child.push_back(new TreeNode("@"));
		return node;
	}

	iter = find(Grammar[34].Predict.begin(), Grammar[34].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[34].Predict.end())
	{
		TreeNode* tempnode = new TreeNode;
		if (tempnode = VarDecList())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	return  nullptr;
}
TreeNode* VarIdList()
{
	TreeNode* node = new TreeNode("VarIdList");
	if (cur_token.Lex == ID)
	{
		node->child.push_back(new TreeNode("ID", cur_token.Sem));
		read_token();
		TreeNode* tempnode = new TreeNode();
		if (tempnode = VarIdMore())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	cout << "ERROR:" << cur_token.Lineshow << "		ID" << endl;
	return  nullptr;
}
TreeNode* VarIdMore()
{
	TreeNode* node = new TreeNode("VarIdMore");
	set<string>::iterator iter = find(Grammar[36].Predict.begin(), Grammar[36].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[36].Predict.end())
	{
		signal = false;
		node->child.push_back(new TreeNode("@"));
		return node;
	}
	if (cur_token.Lex == COMMA)
	{
		node->child.push_back(new TreeNode(","));
		read_token();
		TreeNode* tempnode = new TreeNode;
		if (tempnode = VarIdList())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	cout << "ERROR:" << cur_token.Lineshow << "		," << endl;
	return  nullptr;
}
TreeNode* ProcDec()
{
	TreeNode* node = new TreeNode("ProcDec");
	set<string>::iterator iter = find(Grammar[38].Predict.begin(), Grammar[38].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[38].Predict.end())
	{
		signal = false;
		node->child.push_back(new TreeNode("@"));
		return node;
	}

	iter = find(Grammar[39].Predict.begin(), Grammar[39].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[39].Predict.end())
	{
		TreeNode* tempnode = new TreeNode();
		if (tempnode = ProcDeclaration())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	return  nullptr;
}
TreeNode* ProcDeclaration()
{
	TreeNode* node = new TreeNode("ProcDeclaration");
	set<string>::iterator iter = find(Grammar[40].Predict.begin(), Grammar[40].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[40].Predict.end())
	{
		if (cur_token.Lex == PROCEDURE)
		{
			node->child.push_back(new TreeNode("PROCEDURE"));
			read_token();
			TreeNode* tempnode = new TreeNode();
			if (tempnode = ProcName())
			{
				node->child.push_back(tempnode);
				read_token();
				if (cur_token.Lex == LPAREN)
				{
					node->child.push_back(new TreeNode("("));
					read_token();
					TreeNode* tempnode1 = new TreeNode();
					if (tempnode1 = ParamList())
					{
						node->child.push_back(tempnode1);
						if (signal)
							read_token();
						else
							signal = true;
						if (cur_token.Lex == RPAREN)
						{
							node->child.push_back(new TreeNode(")"));
							read_token();
							if (cur_token.Lex == SEMI)
							{
								node->child.push_back(new TreeNode(";"));
								read_token();
								TreeNode* tempnode2 = new TreeNode();
								if (tempnode2 = ProcDecPart())
								{
									node->child.push_back(tempnode2);
									if (signal)
										read_token();
									else
										signal = true;
									TreeNode* tempnode3 = new TreeNode();
									if (tempnode3 = ProcBody())
									{
										node->child.push_back(tempnode3);
										read_token();
										TreeNode* tempnode4 = new TreeNode();
										if (tempnode4 = ProcDecMore())
										{
											node->child.push_back(tempnode4);
											return node;
										}

									}
								}
							}
							cout << "ERROR:" << cur_token.Lineshow << "		;" << endl;
							return  nullptr;
						}
						cout << "ERROR:" << cur_token.Lineshow << "		)" << endl;
						return  nullptr;
					}				
				}	cout << "ERROR:	" << cur_token.Lineshow << "		(" << endl;
				return  nullptr;
			}
		}
		cout << "ERROR:" << cur_token.Lineshow<<"		PROCEDURE" << endl;
		return  nullptr;
	}
	return  nullptr;
}
TreeNode* ProcDecMore()
{
	TreeNode* node = new TreeNode("ProcDecMore");
	set<string>::iterator iter = find(Grammar[41].Predict.begin(), Grammar[41].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[41].Predict.end())
	{
		signal = false;
		node->child.push_back(new TreeNode("@"));
		return node;
	}
	iter = find(Grammar[42].Predict.begin(), Grammar[42].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[42].Predict.end())
	{
		TreeNode* tempnode = new TreeNode();
		if (tempnode = ProcDeclaration())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	return  nullptr;
}
TreeNode* ProcName()
{
	TreeNode* node = new TreeNode("ProcName");
	if (cur_token.Lex == ID)
	{
		node->child.push_back(new TreeNode("ID", cur_token.Sem));
		return node;
	}
	cout << "ERROR:" << cur_token.Lineshow << "		ID" << endl;
	return  nullptr;
}
TreeNode* ParamList()
{
	TreeNode* node = new TreeNode("ParamList");
	set<string>::iterator iter = find(Grammar[44].Predict.begin(), Grammar[44].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[44].Predict.end())
	{
		signal = false;
		node->child.push_back(new TreeNode("@"));
		return node;
	}
	iter = find(Grammar[45].Predict.begin(), Grammar[45].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[45].Predict.end())
	{
		TreeNode* tempnode = new TreeNode();
		if (tempnode = ParamDecList())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	return  nullptr;
}
TreeNode* ParamDecList()
{
	TreeNode* node = new TreeNode("ParamDecList");
	set<string>::iterator iter = find(Grammar[46].Predict.begin(), Grammar[46].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[46].Predict.end())
	{
		TreeNode* tempnode = new TreeNode();
		if (tempnode = Param())
		{
			node->child.push_back(tempnode);
			if (signal)
				read_token();
			else
				signal = true;
			TreeNode* tempnode1 = new TreeNode();
			if (tempnode1 = ParamMore())
			{
				node->child.push_back(tempnode1);
				return node;
			}
		}
	}
	return  nullptr;
}
TreeNode* ParamMore()
{
	TreeNode* node = new TreeNode("ParamMore");
	set<string>::iterator iter = find(Grammar[47].Predict.begin(), Grammar[47].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[47].Predict.end())
	{
		signal = false;
		node->child.push_back(new TreeNode("@"));
		return node;
	}
	if (cur_token.Lex == SEMI)
	{
		node->child.push_back(new TreeNode(";"));
		read_token();
		TreeNode* tempnode = new TreeNode();
		if (tempnode = ParamDecList())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	cout << "ERROR:" << cur_token.Lineshow << "		INTC" << endl;
	return  nullptr;
}
TreeNode* Param()
{
	TreeNode* node = new TreeNode("Param");
	TreeNode* tempnode = new TreeNode();
	TreeNode* tempnode1 = new TreeNode();
	set<string>::iterator iter = find(Grammar[49].Predict.begin(), Grammar[49].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[49].Predict.end())
	{
		if (tempnode = TypeName())
		{
			node->child.push_back(tempnode);
			read_token();
			if (tempnode1 = FormList())
			{
				node->child.push_back(tempnode1);
				return node;
			}
		}
	}
	if (cur_token.Lex == VAR)
	{
		read_token();
		if (tempnode = TypeName())
		{
			node->child.push_back(tempnode);
			read_token();
			if (tempnode1 = FormList())
			{
				node->child.push_back(tempnode1);
				return node;
			}
		}
	}
	cout << "ERROR:" << cur_token.Lineshow << "		VAR" << endl;
	return  nullptr;
}
TreeNode* FormList()
{
	TreeNode* node = new TreeNode("FormList");
	if (cur_token.Lex == ID)
	{
		node->child.push_back(new TreeNode("ID", cur_token.Sem));
		read_token();
		TreeNode* tempnode = new TreeNode();
		if (tempnode = FidMore())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	cout << "ERROR:" << cur_token.Lineshow << "		ID" << endl;
	return  nullptr;
}
TreeNode* FidMore()
{
	TreeNode* node = new TreeNode("FidMore");
	set<string>::iterator iter = find(Grammar[52].Predict.begin(), Grammar[52].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[52].Predict.end())
	{
		signal = false;
		node->child.push_back(new TreeNode("@"));
		return node;
	}
	if (cur_token.Lex == COMMA)
	{
		node->child.push_back(new TreeNode(","));
		read_token();
		TreeNode* tempnode = new TreeNode;
		if (tempnode = FormList())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	cout << "ERROR:" << cur_token.Lineshow << "		," << endl;
	return  nullptr;
}
TreeNode* ProcDecPart()
{
	TreeNode* node = new TreeNode("ProcDecPart");
	set<string>::iterator iter = find(Grammar[54].Predict.begin(), Grammar[54].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[54].Predict.end())
	{
		TreeNode* tempnode = new TreeNode;
		if (tempnode = DeclarePart())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	return  nullptr;
}



TreeNode* ProcBody()
{
	TreeNode* node = new TreeNode("ProcBody");
	set<string>::iterator iter = find(Grammar[55].Predict.begin(), Grammar[55].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[55].Predict.end())
	{
		TreeNode* tempnode = new TreeNode;
		if (tempnode = ProgramBody())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	return  nullptr;
}
TreeNode* ProgramBody()
{
	TreeNode* node = new TreeNode("ProgramBody");
	if (cur_token.Lex == BEGIN)
	{
		node->child.push_back(new TreeNode("BEGIN"));
		read_token();
		TreeNode* tempnode = new TreeNode;
		if (tempnode = StmList())
		{
			node->child.push_back(tempnode);
			if (signal)
				read_token();
			else
				signal = true;
			if (cur_token.Lex == END)
			{
				node->child.push_back(new TreeNode("END"));
				return node;
			}
			cout << "ERROR:" << cur_token.Lineshow << "		END" << endl;
			return  nullptr;
		}
	}
	cout << "ERROR:" << cur_token.Lineshow << "		BEGIN" << endl;
	return  nullptr;
}
TreeNode* StmList()
{
	TreeNode* node = new TreeNode("StmList");
	set<string>::iterator iter = find(Grammar[57].Predict.begin(), Grammar[57].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[57].Predict.end())
	{
		TreeNode* tempnode = new TreeNode();
		if (tempnode = Stm())
		{
			node->child.push_back(tempnode);
			if (signal)
				read_token();
			else
				signal = true;
			TreeNode* tempnode1 = new TreeNode();
			if (tempnode1 = StmMore())
			{
				node->child.push_back(tempnode1);
				return node;
			}
		}
	}
	return  nullptr;
}
TreeNode* StmMore()
{
	TreeNode* node = new TreeNode("StmMore");
	set<string>::iterator iter = find(Grammar[58].Predict.begin(), Grammar[58].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[58].Predict.end())
	{
		signal = false;
		node->child.push_back(new TreeNode("@"));
		return node;
	}

	if (cur_token.Lex == SEMI)
	{
		node->child.push_back(new TreeNode(";"));
		if (signal)
			read_token();
		else
			signal = true;
		TreeNode* tempnode = new TreeNode();
		if (tempnode = StmList())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	cout << "ERROR:" << cur_token.Lineshow << "		;" << endl;
	return  nullptr;
}
TreeNode* Stm()
{
	TreeNode* node = new TreeNode("Stm");
	TreeNode* tempnode = new TreeNode();
	set<string>::iterator iter = find(Grammar[60].Predict.begin(), Grammar[60].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[60].Predict.end())
	{
		if (tempnode = ConditionalStm())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	iter = find(Grammar[61].Predict.begin(), Grammar[61].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[61].Predict.end())
	{
		if (tempnode = LoopStm())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	iter = find(Grammar[62].Predict.begin(), Grammar[62].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[62].Predict.end())
	{
		if (tempnode = InputStm())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	iter = find(Grammar[63].Predict.begin(), Grammar[63].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[63].Predict.end())
	{
		if (tempnode = OutputStm())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	iter = find(Grammar[64].Predict.begin(), Grammar[64].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[64].Predict.end())
	{
		if (tempnode = ReturnStm())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	if (cur_token.Lex == ID)
	{
		node->child.push_back(new TreeNode("ID", cur_token.Sem));
		read_token();

		if (tempnode = AssCall())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	cout << "ERROR:" << cur_token.Lineshow << "		ID" << endl;
	return  nullptr;
}
TreeNode* AssCall()
{
	TreeNode* node = new TreeNode("AssCall");
	TreeNode* tempnode = new TreeNode();
	set<string>::iterator iter = find(Grammar[66].Predict.begin(), Grammar[66].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[66].Predict.end())
	{
		if (tempnode = AssignmentRest())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	iter = find(Grammar[67].Predict.begin(), Grammar[67].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[67].Predict.end())
	{
		if (tempnode = CallStmRest())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	return  nullptr;
}
TreeNode* AssignmentRest()
{
	TreeNode* node = new TreeNode("AssignmentRest");

	set<string>::iterator iter = find(Grammar[68].Predict.begin(), Grammar[68].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[68].Predict.end())
	{
		TreeNode* tempnode = new TreeNode();
		if (tempnode = VariMore())
		{
			node->child.push_back(tempnode);
			if (signal)
				read_token();
			else
				signal = true;
			if (cur_token.Lex == ASSIGN)
			{
				node->child.push_back(new TreeNode("ASSIGN"));
				read_token();
				TreeNode* tempnode1 = new TreeNode();
				if (tempnode1 = Exp())
				{
					node->child.push_back(tempnode1);
					return node;
				}
			}
			cout << "ERROR:" << cur_token.Lineshow << "		ASSIGN" << endl;
			return  nullptr;
		}
	}
	return  nullptr;
}
TreeNode* ConditionalStm()
{
	TreeNode* node = new TreeNode(" ConditionalStm");

	if (cur_token.Lex == IF)
	{
		node->child.push_back(new TreeNode("IF"));
		read_token();
		TreeNode* tempnode = new TreeNode();
		if (tempnode = RelExp())
		{
			node->child.push_back(tempnode);
			if (signal)
				read_token();
			else
				signal = true;

			if (cur_token.Lex == THEN)
			{
				node->child.push_back(new TreeNode("THEN"));
				read_token();
				TreeNode* tempnode1 = new TreeNode();
				if (tempnode1 = StmList())
				{
					node->child.push_back(tempnode1);
					if (signal)
						read_token();
					else
						signal = true;
					if (cur_token.Lex == ELSE)
					{
						node->child.push_back(new TreeNode("ELSE"));
						read_token();
						TreeNode* tempnode2 = new TreeNode();
						if (tempnode2 = StmList())
						{
							node->child.push_back(tempnode2);
							if (signal)
								read_token();
							else
								signal = true;
							if (cur_token.Lex == FI)
							{
								node->child.push_back(new TreeNode("FI"));
								return node;
							}
							cout << "ERROR:" << cur_token.Lineshow << "		FI" << endl;
							return  nullptr;
						}
					}
					cout << "ERROR:" << cur_token.Lineshow << "		ELSE" << endl;
					return  nullptr;
				}
			}
			cout << "ERROR:" << cur_token.Lineshow << "		THEN" << endl;
			return  nullptr;
		}
	}
	cout << "ERROR:" << cur_token.Lineshow << "		IF" << endl;
	return  nullptr;
}
TreeNode* LoopStm()
{
	TreeNode* node = new TreeNode(" LoopStm");
	if (cur_token.Lex == WHILE)
	{
		node->child.push_back(new TreeNode("WHILE"));
		read_token();
		TreeNode* tempnode = new TreeNode();
		if (tempnode = RelExp())
		{
			node->child.push_back(tempnode);
			if (signal)
				read_token();
			else
				signal = true;
			if (cur_token.Lex == DO)
			{
				node->child.push_back(new TreeNode("DO"));
				read_token();
				TreeNode* tempnode1 = new TreeNode();
				if (tempnode1 = StmList())
				{
					node->child.push_back(tempnode1);
					if (signal)
						read_token();
					else
						signal = true;
					if (cur_token.Lex == ENDWH)
					{
						node->child.push_back(new TreeNode("ENDWH"));
						return node;
					}
					cout << "ERROR:" << cur_token.Lineshow << "		ENDWH" << endl;
					return  nullptr;
				}
			}
			cout << "ERROR:" << cur_token.Lineshow << "		DO" << endl;
			return  nullptr;
		}
	}
	cout << "ERROR:" << cur_token.Lineshow << "		WHILE" << endl;
	return  nullptr;
}
TreeNode* InputStm()
{
	TreeNode* node = new TreeNode("InputStm");
	if (cur_token.Lex == READ)
	{
		node->child.push_back(new TreeNode("READ"));
		read_token();
		if (cur_token.Lex == LPAREN)
		{
			node->child.push_back(new TreeNode("("));
			read_token();
			TreeNode* tempnode = new TreeNode();
			if (tempnode = Invar())
			{
				node->child.push_back(tempnode);
				read_token();
				if (cur_token.Lex == RPAREN)
				{
					node->child.push_back(new TreeNode(")"));
					return node;
				}
				cout << "ERROR:" << cur_token.Lineshow << "		)" << endl;
				return  nullptr;
			}
		}
		cout << "ERROR:" << cur_token.Lineshow << "		)" << endl;
		return  nullptr;
	}
	cout << "ERROR:" << cur_token.Lineshow << "		READ" << endl;
	return  nullptr;
}
TreeNode* Invar()
{
	TreeNode* node = new TreeNode("Invar");
	if (cur_token.Lex == ID)
	{
		node->child.push_back(new TreeNode("ID", cur_token.Sem));
		return node;
	}
	cout << "ERROR:" << cur_token.Lineshow << "		ID" << endl;
	return  nullptr;
}
TreeNode* OutputStm()
{
	TreeNode* node = new TreeNode("OutputStm");
	if (cur_token.Lex == WRITE)
	{
		node->child.push_back(new TreeNode("WRITE"));
		read_token();
		if (cur_token.Lex == LPAREN)
		{
			node->child.push_back(new TreeNode("("));
			read_token();
			TreeNode* tempnode = new TreeNode();
			if (tempnode = Exp())
			{
				node->child.push_back(tempnode);
				if (signal)
					read_token();
				else
					signal = true;
				if (cur_token.Lex == RPAREN)
				{
					node->child.push_back(new TreeNode(")"));
					return node;
				}
				cout << "ERROR:" << cur_token.Lineshow << "		)" << endl;
				return  nullptr;
			}
		}
		cout << "ERROR:" << cur_token.Lineshow << "		(" << endl;
		return  nullptr;
	}
	cout << "ERROR:" << cur_token.Lineshow << "		WRITE" << endl;
	return  nullptr;
}
TreeNode* ReturnStm()
{
	TreeNode* node = new TreeNode("ReturnStm");
	if (cur_token.Lex == RETURN)
	{
		node->child.push_back(new TreeNode("RETURN"));
		read_token();
		if (cur_token.Lex == LPAREN)
		{
			node->child.push_back(new TreeNode("("));
			read_token();
			TreeNode* tempnode = new TreeNode();
			if (tempnode = Exp())
			{
				if (signal)
					read_token();
				else
					signal = true;
				if (cur_token.Lex == RPAREN)
				{
					node->child.push_back(new TreeNode(")"));
					return node;
				}
				cout << "ERROR:" << cur_token.Lineshow << "		)" << endl;
				return  nullptr;
			}
		}
		cout << "ERROR:" << cur_token.Lineshow << "		(" << endl;
		return  nullptr;
	}
	cout << "ERROR:" << cur_token.Lineshow << "		RETURN" << endl;
	return  nullptr;
}
TreeNode* CallStmRest()
{
	TreeNode* node = new TreeNode("CallStmRest");
	if (cur_token.Lex == LPAREN)
	{
		node->child.push_back(new TreeNode("("));
		read_token();
		TreeNode* tempnode = new TreeNode();
		if (tempnode = ActParamList())
		{
			node->child.push_back(tempnode);
			if (signal)
				read_token();
			else
				signal = true;
			if (cur_token.Lex == RPAREN)
			{
				node->child.push_back(new TreeNode(")"));
				return node;
			}
			cout << "ERROR:" << cur_token.Lineshow << "		)" << endl;
			return  nullptr;
		}
	}
	cout << "ERROR:" << cur_token.Lineshow << "		(" << endl;
	return  nullptr;
}
TreeNode* ActParamList()
{
	TreeNode* node = new TreeNode(" ActParamList");
	set<string>::iterator iter = find(Grammar[76].Predict.begin(), Grammar[76].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[76].Predict.end())
	{
		signal = false;
		node->child.push_back(new TreeNode("@"));
		return node;
	}
	iter = find(Grammar[77].Predict.begin(), Grammar[77].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[77].Predict.end())
	{
		TreeNode* tempnode = new TreeNode();
		if (tempnode = Exp())
		{
			node->child.push_back(tempnode);
			if (signal)
				read_token();
			else
				signal = true;
			TreeNode* tempnode1 = new TreeNode();
			if (tempnode = ActParamMore())
			{
				node->child.push_back(tempnode);
				return node;
			}
		}
	}
	return  nullptr;
}
TreeNode* ActParamMore()
{
	TreeNode* node = new TreeNode("ActParamMore");
	set<string>::iterator iter = find(Grammar[78].Predict.begin(), Grammar[78].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[78].Predict.end())
	{
		signal = false;
		node->child.push_back(new TreeNode("@"));
		return node;
	}
	if (cur_token.Lex == COMMA)
	{
		node->child.push_back(new TreeNode("COMMA"));
		read_token();
		TreeNode* tempnode = new TreeNode();
		if (tempnode = ActParamList())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	cout << "ERROR:" << cur_token.Lineshow << "		," << endl;
	return  nullptr;
}
TreeNode* RelExp()
{
	TreeNode* node = new TreeNode("RelExp");
	set<string>::iterator iter = find(Grammar[80].Predict.begin(), Grammar[80].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[80].Predict.end())
	{
		TreeNode* tempnode = new TreeNode();
		if (tempnode = Exp())
		{
			if (signal)
				read_token();
			else
				signal = true;
			TreeNode* tempnode1 = new TreeNode();
			if (tempnode1 = OtherRelE())
			{
				node->child.push_back(tempnode1);
				return node;
			}
		}
	}
	return  nullptr;
}
TreeNode* OtherRelE()
{
	TreeNode* node = new TreeNode("OtherRelE");
	set<string>::iterator iter = find(Grammar[81].Predict.begin(), Grammar[81].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[81].Predict.end())
	{
		TreeNode* tempnode = new TreeNode();
		if (tempnode=CmpOp())
		{
			node->child.push_back(tempnode);
			read_token();
			TreeNode* tempnode1 = new TreeNode();
			if (tempnode1 = Exp())
			{
				node->child.push_back(tempnode1);
				return node;
			}
		}
	}
	return  nullptr;
}
TreeNode* Exp()
{
	TreeNode* node = new TreeNode("Exp");
	set<string>::iterator iter = find(Grammar[82].Predict.begin(), Grammar[82].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[82].Predict.end())
	{
		TreeNode* tempnode = new TreeNode();
		if (tempnode=Term())
		{
			node->child.push_back(tempnode);
			if (signal)
				read_token();
			else
				signal = true;
			TreeNode* tempnode1 = new TreeNode();
			if (tempnode1 = OtherTerm())
			{
				node->child.push_back(tempnode1);
				return node;
			}
		}
	}
	return  nullptr;
}
TreeNode* OtherTerm()
{
	TreeNode* node = new TreeNode("OtherTerm");
	set<string>::iterator iter = find(Grammar[83].Predict.begin(), Grammar[83].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[83].Predict.end())
	{
		signal = false;
		node->child.push_back(new TreeNode("@"));
		return node;
	}
	iter = find(Grammar[84].Predict.begin(), Grammar[84].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[84].Predict.end())
	{
		TreeNode* tempnode = new TreeNode();
		if (tempnode=AddOp())
		{
			node->child.push_back(tempnode);
			read_token();
			TreeNode* tempnode1 = new TreeNode();
			if (tempnode1 = Exp())
			{
				node->child.push_back(tempnode1);
				return node;
			}
		}
	}
	return  nullptr;
}
TreeNode* Term()
{
	TreeNode* node = new TreeNode("Term");
	set<string>::iterator iter = find(Grammar[85].Predict.begin(), Grammar[85].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[85].Predict.end())
	{
		TreeNode* tempnode = new TreeNode();
		if (tempnode=Factor())
		{
			node->child.push_back(tempnode);
			if (signal)
				read_token();
			else
				signal = true;
			TreeNode* tempnode1 = new TreeNode();
			if (tempnode1=OtherFactor())
			{
				node->child.push_back(tempnode1);
				return node;
			}
		}
	}
	return  nullptr;
}
TreeNode* OtherFactor()
{
	TreeNode* node = new TreeNode("OtherFactor");
	set<string>::iterator iter = find(Grammar[86].Predict.begin(), Grammar[86].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[86].Predict.end())
	{
		signal = false;
		node->child.push_back(new TreeNode("@"));
		return node;
	}
	iter = find(Grammar[87].Predict.begin(), Grammar[87].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[87].Predict.end())
	{
		TreeNode* tempnode = new TreeNode();
		if (tempnode=MultOp())
		{
			node->child.push_back(tempnode);
			read_token();
			TreeNode* tempnode1 = new TreeNode();
			if (tempnode1=Term())
			{
				node->child.push_back(tempnode1);
				return node;
			}
		}
	}
	return  nullptr;
}
TreeNode* Factor()
{
	TreeNode* node = new TreeNode("Factor");
	TreeNode* tempnode = new TreeNode();
	if (cur_token.Lex == LPAREN)
	{
		node->child.push_back(new TreeNode("("));
		read_token();
		if (tempnode=Exp())
		{
			node->child.push_back(tempnode);
			if (signal)
				read_token();
			else
				signal = true;
			if (cur_token.Lex == RPAREN)
			{
				node->child.push_back(new TreeNode(")"));
				return node;
			}
			cout << "ERROR:" << cur_token.Lineshow << "		)" << endl;
			return  nullptr;
		}
	}
	if (cur_token.Lex == INTC)
	{
		node->child.push_back(new TreeNode("INTC",cur_token.Sem));
		return node;
	}
	set<string>::iterator iter = find(Grammar[90].Predict.begin(), Grammar[90].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[90].Predict.end())
	{
		if (tempnode = Variable())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	cout << "ERROR:" << cur_token.Lineshow << endl;
	return  nullptr;
}
TreeNode* Variable()
{
	TreeNode* node = new TreeNode("Variable");
	if (cur_token.Lex == ID)
	{
		node->child.push_back(new TreeNode("ID", cur_token.Sem));
		read_token();
		TreeNode* tempnode = new TreeNode();
		if (tempnode=VariMore())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	cout << "ERROR:" << cur_token.Lineshow << "		ID" << endl;
	return  nullptr;
}
TreeNode* VariMore()
{
	TreeNode* node = new TreeNode("VariMore");
	set<string>::iterator iter = find(Grammar[92].Predict.begin(), Grammar[92].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[92].Predict.end())
	{
		signal = false;
		node->child.push_back(new TreeNode("@"));
		return node;
	}
	if (cur_token.Lex == LMIDPAREN)
	{
		node->child.push_back(new TreeNode("["));
		read_token();
		TreeNode* tempnode = new TreeNode();
		if (tempnode=Exp())
		{
			node->child.push_back(tempnode);
			if (signal)
				read_token();
			else
				signal = true;
			if (cur_token.Lex == RMIDPAREN)
			{
				node->child.push_back(new TreeNode("]"));
				return node;
			}
		}
	}
	if (cur_token.Lex == DOT)
	{
		node->child.push_back(new TreeNode("."));
		read_token();
		TreeNode* tempnode = new TreeNode();
		if (tempnode=FieldVar())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	cout << "ERROR:" << cur_token.Lineshow << endl;
	return  nullptr;
}
TreeNode* FieldVar()
{
	TreeNode* node = new TreeNode("FieldVar");
	if (cur_token.Lex == ID)
	{
		node->child.push_back(new TreeNode("ID",cur_token.Sem));
		read_token();
		TreeNode* tempnode = new TreeNode();
		if (tempnode = FieldVarMore())
		{
			node->child.push_back(tempnode);
			return node;
		}
	}
	cout << "ERROR:" << cur_token.Lineshow << "		ID" << endl;
	return  nullptr;
}
TreeNode* FieldVarMore()
{
	TreeNode* node = new TreeNode("FieldVarMore");
	set<string>::iterator iter = find(Grammar[96].Predict.begin(), Grammar[96].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[96].Predict.end())
	{
		signal = false;
		node->child.push_back(new TreeNode("@"));
		return node;
	}
	if (cur_token.Lex == LMIDPAREN)
	{
		node->child.push_back(new TreeNode("["));
		read_token();
		TreeNode* tempnode = new TreeNode();
		if (tempnode=Exp())
		{
			node->child.push_back(tempnode);
			read_token();
			if (cur_token.Lex == RMIDPAREN)
			{
				node->child.push_back(new TreeNode("]"));
				return node;
			}
			cout << "ERROR:" << cur_token.Lineshow << "		]" << endl;
			return  nullptr;
		}
	}
	cout << "ERROR:" << cur_token.Lineshow << "		[" << endl;
	return  nullptr;
}
TreeNode* CmpOp()
{
	TreeNode* node = new TreeNode("CmpOp");
	if (cur_token.Lex == LESSTHEN || cur_token.Lex == EQUAL)
	{
		node->child.push_back(new TreeNode(LexToString(cur_token.Lex)));
		return node;
	}
	cout << "ERROR:" << cur_token.Lineshow << "		<=" << endl;
	return  nullptr;
}
TreeNode* AddOp()
{
	TreeNode* node = new TreeNode("AddOp");
	if (cur_token.Lex == PLUS || cur_token.Lex == MINUS)
	{
		node->child.push_back(new TreeNode(LexToString(cur_token.Lex)));
		return node;
	}
	cout << "ERROR:" << cur_token.Lineshow << "	    + -" << endl;
	return  nullptr;
}
TreeNode* MultOp()
{
	TreeNode* node = new TreeNode("MultOp");
	if (cur_token.Lex == TIMES || cur_token.Lex == OVER)
	{
		node->child.push_back(new TreeNode(LexToString(cur_token.Lex)));
		return node;
	}
	cout << "ERROR:" << cur_token.Lineshow << "		* /" << endl;
	return  nullptr;
}
