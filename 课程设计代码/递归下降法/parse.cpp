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
	//助记符
"ENDFILE", "ERROR",

//保留字
"PROGRAM", "PROCEDURE", "TYPE", "VAR", "IF",
"THEN", "ELSE", "FI", "WHILE", "DO",
"ENDWH", "BEGIN", "END", "READ", "WRITE",
"ARRAY", "OF", "RECORD", "RETURN",

//类型名
"INTEGER", "CHAR",

//多字符单词
"ID", "INTC", "CHARC",

//特殊符号
":=", "=", "<", "+", "-",
"*", "/", "(", ")", ".",
":", ";", ",", "[", "]",
"..", " "
};
struct Rule//单个产生式
{
	string left;
	vector<string> right;
	set<string> Predict;
};
vector<Rule> Grammar;//产生式的集合
map<string, set<string> > First;
map<string, set<string> > Follow;
string start;    //起始字符串
set<string> not_terminal_chars =//非终结符集合，手动构造共67个
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
bool judgeTerminalVocabulary(string J_T_V)//判断是否是终结符
{
	if (not_terminal_chars.find(J_T_V) == not_terminal_chars.end())
		return true;
	return false;
}
/*****************************************************************************
******************************************************************************
*                                                                    *********
*                   读取文法文件拆分存入Grammar内；                  *********
*                                                                    *********
******************************************************************************
******************************************************************************/
//删除字符两头的空格
string  trim(string s) {
	if (s.empty()) {//如果是空串
		return s;
	}
	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
	return s;
}
//去除字符s中多余的空格
string remove_surplus_spaces(const string& s)
{
	string src = trim(s);
	string result = "";
	for (int i = 0; src[i] != '\0'; i++)// \0=字符串结尾
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
	string line;//读取文法文件每一行
	string left, right;//每条文法的左边跟右边
	fstream file_wenfa("D:\\wenfa2020.txt", ios::in);
	if (file_wenfa.is_open() == false)
	{
		cout << "文法文件打开失败" << endl;
		return;
	}
	while (getline(file_wenfa, line))
	{
		Rule c_rule;//当前处理文法，处理完成后存入grammar中
		string x;//存储每一个‘字母’
		for (int i = 0; i < line.length(); i++)//将所有的字符串中的制表符\t转换为空格
			if (line[i] == '\t')
				line = line.replace(i, 1, " ");

		int split = line.find("::=");//找到分割字符所在的位置

		if (split != -1)//不是 | 所在的列
		{
			//分离每条规则的左右部分
			left = line.substr(0, split);
			right = line.substr(split + 3, line.length());


			//因为左部含有 “18）”这样的序号部分，故分离出非终结符
			split = line.find(")");
			left = left.substr(split + 1, left.length());//去掉括号及左侧
			left = remove_surplus_spaces(left);//除去前导和后导空格，便于之后的处理
			right = remove_surplus_spaces(right);

			c_rule.left = left;

			int j = 0;//前一个字母的最末尾
			int z = 0;//当前字母的末尾
			int flag = 0;//判断当前是连续的英文还是连续的符号

			for (int i = 0; i < right.length(); i++, z++)
			{
				if ((right[i] <= 'Z' && right[i] >= 'A') || (right[i] <= 'z' && right[i] >= 'a'))//是否是英文
				{
					if (flag != 0)//不是英文，应该将之前的序列保存
					{
						x = right.substr(j, z - j);//计算前一个应该保存的字母
						if (!x.empty())
							c_rule.right.push_back(x);//放入结构体
						j = i;//变换末尾
						z = i;//设定当前字母末尾
						flag = 0;//变换格式
					}
					else
						continue;//如果仍然是英文就继续
				}
				else
				{
					if (flag != 1)//不是符号，应该将之前的序列保存
					{
						x = right.substr(j, z - j);
						if (!x.empty())
							c_rule.right.push_back(x);

						if (right[i] == '\0' || right[i] == 32)//是空格或空字符，应该无视他，直接跳过
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
						if (right[i] == '\0' || right[i] == 32)//遇到连续的字符中有空格，应该将字母末尾设置为空格前一字符的位置，所以z-1
						{
							z--;
							flag = 3;
						}
					continue;
				}
			}
			x = right.substr(j, 999);//对到末尾的字母进行处理

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

				if ((right[i] <= 'Z' && right[i] >= 'A') || (right[i] <= 'z' && right[i] >= 'a'))//是否是英文
				{
					if (flag != 0)//不是英文，应该将之前的序列保存
					{
						x = right.substr(j, z - j);//计算前一个应该保存的字母
						if (!x.empty())
							c_rule.right.push_back(x);//放入结构体
						j = i;//变换末尾
						z = i;//设定当前字母末尾
						flag = 0;//变换格式
					}
					else
						continue;//如果仍然是英文就继续
				}


				else
				{
					if (flag != 1)//不是符号，应该将之前的序列保存
					{
						x = right.substr(j, z - j);
						if (!x.empty())
							c_rule.right.push_back(x);

						if (right[i] == '\0' || right[i] == 32)//是空格或空字符，应该无视他，直接跳过
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
						if (right[i] == '\0' || right[i] == 32)//遇到连续的字符中有空格，应该将字母末尾设置为空格前一字符的位置，所以z-1
						{
							z--;
							flag = 3;
						}
					continue;
				}
			}
			x = right.substr(j, 999);//对到末尾的字母进行处理
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
**********                    生成FIRST集合                          *********
**********                                                           *********
******************************************************************************
******************************************************************************/

set<string>first_str(string str)
{
	set<string> tempSet;//存储每个str的first集合，也是返回值
	for (int j = 0; j < Grammar.size(); j++)//遍历每一个文法
	{
		if (Grammar[j].left == str)//如果左部等于该求字符串
		{
			for (int k = 0; k < Grammar[j].right.size(); k++)//如果取到右边的第k个字符，那么前k-1个字符均能推出空串
			{
				//1、如果当前字符是空串并且是最后一个字符
				if (Grammar[j].right[k] == "@" && k + 1 == Grammar[j].right.size())
				{
					tempSet.insert("@");
					break;
				}
				//2、如果当前字符是终极符且不是空串
				else if ((not_terminal_chars.find(Grammar[j].right[k]) == not_terminal_chars.end() && Grammar[j].right[k] != "@"))
				{
					tempSet.insert(Grammar[j].right[k]);
					break;//加入后考虑下一个产生式
				}
				//3、如果当前字符是非终极符，考虑它的first集合
				else if (not_terminal_chars.find(Grammar[j].right[k]) != not_terminal_chars.end())
				{
					set<string> tempSet1 = first_str(Grammar[j].right[k]);//递归调用求解
					tempSet.insert(tempSet1.begin(), tempSet1.end());

					if (tempSet1.find("@") == tempSet1.end() || k + 1 == Grammar[j].right.size())//如果不含有空串或者有空串但到达右边字符最后一个
					{
						break;//直接考虑下一个产生式
					}
				}
				//当前字符可推导出空串但后边仍有字符
				tempSet.erase("@");//删除误填进去的空串,开始进入下一次循环
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
	outFile << "文法符号" << "," << "First集" << endl;
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
**********                    生成FOLLOW集合                         *********
**********                                                           *********
******************************************************************************
******************************************************************************/

set<string> follow_str(string str, string old)//old用来保存谁递归调用了，确定是否产生闭包，以免形成死锁
{
	set<string> tempSet;//保存当前求的follow集
	for (int i = 0; i < Grammar.size(); i++)//遍历每一个文法产生式
	{
		for (int k = 0; k < Grammar[i].right.size(); k++)//取到第i个产生式右边的第k个字符
		{
			//如果发现当前字符出现在产生式右边
			if (Grammar[i].right[k] == str)
			{
				//接下来讨论当前字符是不是在产生式最右边
				//一、如果发现当前字符出现在产生式最右边，那么讨论它的left的follow集合
				if ((Grammar[i].right.size() - 1) == k)
				{
					//1、做递归时、还有左边跟最右边相等时的验证；可以验证是个闭包，不会添加新元素，所以直接跳过：避免死锁
					if (old == Grammar[i].left || Grammar[i].right[k] == Grammar[i].left)
					{
						break;
					}
					//2、出现在产生式最右边,且左边推出的不等于它自身（即不是形成一个不添加新元素的闭包）；
					//那么此时观察left的follow集合
					else
					{
						set<string> tempSet1 = follow_str(Grammar[i].left, str);
						if (tempSet1.size() != 0)
						{
							tempSet.insert(tempSet1.begin(), tempSet1.end());
						}
					}
				}
				//二、不是出现在最右边，那么观察紧邻它的下一个字符的first集合
				int j = 1;//j-1个符号可推出空串
				string str_next;
				while (k + j < Grammar[i].right.size())//k+j代表下一个字符，如果字符的first集中有空集，则j++直到right的末尾字符
				{
					str_next = Grammar[i].right[k + j];
					//1、是终极符直接添加
					if (not_terminal_chars.find(str_next) == not_terminal_chars.end())
					{
						tempSet.insert(str_next);
						break;
					}
					// 2、是非终极符：需要讨论是否能推出空串
					else
					{
						tempSet.insert(First[str_next].begin(), First[str_next].end());//并上紧随其后的FIRST集合	
						set<string>::iterator s;
						s = tempSet.find("@");
						if (s == tempSet.end())//在下一个字符的first集合中没有出现空集
						{
							break;//跳出不用考虑后边的字符
						}
						else//在下一个字符的first集合中出现空集
						{
							tempSet.erase(s);//从中删除@
							j++;//观察下一个字符的下一个字符
							if (k + j == Grammar[i].right.size())//表明没有下一个字符的下一个字符，即尾部可以推出空串则，所以并上left集
							{
								set<string> tempSet1 = follow_str(Grammar[i].left, str);	//递归调用
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
	Follow[start].insert("#");//初始化开始字符的follow集合
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
	outFile << "文法符号" << "," << "Follow集" << endl;
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
**********                    生成PREDICT集合                        *********
**********                                                           *********
******************************************************************************
******************************************************************************/
void predict()
{
	vector<string>::iterator it;
	for (int i = 0; i < Grammar.size(); i++)//遍历每一个文法产生式
	{
		int k = 0;//判断右部第k个字符
		while (k < Grammar[i].right.size())
		{
			//如果是终结符但不是空字符
			if (not_terminal_chars.find(Grammar[i].right[k]) == not_terminal_chars.end() && Grammar[i].right[k] != "@")
			{
				Grammar[i].Predict.insert(Grammar[i].right[k]);
				break;
			}
			//如果是空字符
			else if (Grammar[i].right[k] == "@")
			{
				Grammar[i].Predict.insert(Follow[Grammar[i].left].begin(), Follow[Grammar[i].left].end());//插入左边的follow集合

				break;
			}

			//如果不是终结符:判断是否能推出空串
			set<string> s = First[Grammar[i].right[k]];
			set<string>::iterator iter = find(s.begin(), s.end(), "@");

			if (iter == s.end())//产生式右部第k个字符first集合不含空
			{
				Grammar[i].Predict = First[Grammar[i].right[k]];
				break;
			}
			else if (iter != s.end()) // 产生式右部第k个字符first集合含空：继续向后判断
			{
				s.erase("@");//删除空字符串
				Grammar[i].Predict.insert(s.begin(), s.end());//并上该字符的first集合之后往后看
				k++;
				if (k == Grammar[i].right.size())
				{
					Grammar[i].Predict.insert(Follow[Grammar[i].left].begin(), Follow[Grammar[i].left].end());//插入左边的follow集合
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
	outFile << "产生式" << "," << "Predict集" << endl;
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
bool signal = true;//为真则表示需要read_token()

void read_token()
{
	sum = sum + 1;
	cur_token = TL[sum];
}
/*****************************************************************************
******************************************************************************
**********                                                           *********
**********                    递归下降法                             *********
**********                                                           *********
******************************************************************************
******************************************************************************/
//生成语法树的Dot语言代码
void out_tree(TreeNode* root)
{
	fstream outFile("D:\\S_tree.txt", ios::out);
	queue<pair<TreeNode*, int> > q;//需要存储节点以及标号
	int node_num = 1;//节点标号
	stringstream ss;//把DOT Language表示的语法树的字符串放到这里边
	ss.clear();
	/**************************************************************************/
	/****************** 开始生成DOT代码文件 ***********************************/
	/**************************************************************************/
	ss << "digraph GrammarTree {" << endl;
	if (root)
	{
		q.push(pair<TreeNode*, int>(root, node_num));//根节点入队
	}

	while (!q.empty())
	{
		pair<TreeNode*, int> t = q.front();
		q.pop();
		if (t.first->Lex == "ID" || t.first->Lex == "INTC" || t.first->Lex == "CHARC")//如果是id之类的改变形状，颜色
		{
			ss << "\"" << t.second << "\"[shape=square; style=filled; fillcolor=yellow; label=\"" << t.first->Lex;
		}

		else if (!judgeTerminalVocabulary(t.first->Lex))//如果是非终结符改变颜色
		{
			ss << "\"" << t.second << "\"[style=filled; fillcolor=green; label=\"" << t.first->Lex;
		}

		else//其他token的颜色
		{
			ss << "\"" << t.second << "\"[style=filled; fillcolor=lightpink; label=\"" << t.first->Lex;//当前的根节点：即调用的语法分析函数
		}


		if (!t.first->Sem.empty())//判断是否需要输出值sem
		{
			ss << ":" << t.first->Sem << "\"];" << endl;
		}
		else
		{
			ss << "\"];" << endl;
		}

//1.1、确定当前需要指向谁
		if (t.first->child.size())
		{

			ss << "\"" << t.second << "\"" << "->" << "\{";//先输出当前节点

			for (int j = 1; j <= t.first->child.size(); j++)
			{
				ss << "\"" << ++node_num << "\";";
				q.push(pair<TreeNode*, int>(t.first->child[j - 1], node_num));//当前节点的所有子节点以及编号入队
			}

			ss << "\}" << endl;

//1.2、把子节点都放在同一高度
			ss << "{rank=same; ";
			for (int j = t.first->child.size() - 1; j >= 0; j--)
			{
				ss << "\"" << node_num - j << "\";";
			}
			ss << "\}" << endl;
//1.3然后出队循环考虑子节点
		}
	}
	ss << "}";
	outFile << ss.str();
	outFile.close();
}
TreeNode* Program()
{
	TreeNode* node = new TreeNode("Program");//新建树节点,返回即是根节点
	read_token();
	set<string>::iterator iter = find(Grammar[0].Predict.begin(), Grammar[0].Predict.end(), LexToString(cur_token.Lex));
	if (iter != Grammar[0].Predict.end())
	{
		TreeNode* tempnode = new TreeNode();//新建临时树节点，用于存储返回的子树节点
		if (tempnode = ProgramHead())//判断是否有子树节点：即是否下一步分析的代码不出错
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
					read_token();//读出最后的结束符：一个点
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
	TreeNode* node = new TreeNode("ProgramHead");//新建树节点
	if (cur_token.Lex == PROGRAM)
	{
		TreeNode* tempnode = new TreeNode("PROGRAM");
		node->child.push_back(tempnode);
		read_token();
		TreeNode* tempnode1 = new TreeNode();//新建临时树节点，用于存储返回的子树节点
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
	TreeNode* node = new TreeNode("ProgramName");//新建树节点
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
	TreeNode* node = new TreeNode("DeclarePart");//新建树节点
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
	TreeNode* node = new TreeNode("TypeDec");//新建树节点
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
	TreeNode* node = new TreeNode("TypeDeclaration");//新建树节点
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
	TreeNode* node = new TreeNode("TypeDecList");//新建树节点
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
	TreeNode* node = new TreeNode("TypeDecMore");//新建树节点
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
	TreeNode* node = new TreeNode("TypeId");//新建树节点
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
	TreeNode* node = new TreeNode("TypeName");//新建树节点
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
	TreeNode* node = new TreeNode("BaseType");//新建树节点
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
	TreeNode* node = new TreeNode("StructureType");//新建树节点
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
	TreeNode* node = new TreeNode("ArrayType");//新建树节点
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
	TreeNode* node = new TreeNode("Low");//新建树节点
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
	TreeNode* node = new TreeNode("Top");//新建树节点
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
	TreeNode* node = new TreeNode("RecType");//新建树节点
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
