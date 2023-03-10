#include "SyntaxTree.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include <iomanip>

vector<production> vecTree;
vector<string> TokenValueVec;
vector<string> TokenTypeVec;

 AnalysisTreeNode* root;//语法分析树的根节点
 int global_id;//DOT Language中每个节点的id
 unsigned int global_token_index;//用于将ID转化为变量名后者函数名等
 unsigned int treeIndex;// 指示vTree的下标

void SNLCompiler()
{
	LexicalAnalysis();
	string str("D:\\wenfa.txt");
	productionList* ptrProducList = new productionList(str);
	SyntaxAnalysis(ptrProducList);
	CreateSyntaxTree(ptrProducList);//构建语法树生成
}


void LexicalAnalysis()
{
	ifstream ifile("D:\\SNLsource.txt");//教材上的SNL程序
	//ifstream ifile("D:\\bubble.txt");//冒泡排序源程序
	ostringstream buf;
	char ch;
	while (buf && ifile.get(ch))
		buf.put(ch);
	InputS = buf.str();
	TokenChain = new Token();
	CurChain = TokenChain;
	InputHandle();
	DisplayToken();//展示Token序列串
}


void SyntaxAnalysis(productionList* ptrProducList)
{
	fstream outputFile;
	outputFile.open("D:\\output.csv");
	if (!outputFile)
	{
		cout << "文件打开失败！" << endl;
	}
	else
	{
		outputFile <<"序号"<<","<< "分析栈栈顶" << ","<< "当前输入流" << ","<< "动作" << endl;
	}
	//在Token序列的末尾添加一个Token--"#"
	while (CurChain->next != NULL)//找到Token序列的最后一个Token
	{
		CurChain = CurChain->next;
	}
	Token* tempT = new Token;
	tempT->Line = 0;
	tempT->Type = "Sharp";
	tempT->Value = "#";
	tempT->next = NULL;
	CurChain->next = tempT;
	CurChain = CurChain->next;

	//构造分析栈，并设置好最开始的栈底元素“#”+“文法开始符号”
	list<string> AnalysisStack;//用一个链表来充当分析栈
	AnalysisStack.push_back("#");
	AnalysisStack.push_back("Program");
	Token* ReadToken = TokenChain->next;
	int count = 0;//匹配日志编号
	while (!AnalysisStack.empty())
	{
		int seqNum = 0;
		string InputStr = ReadToken->Value;//输入流(文法符号)
		if (ReadToken->Type == "Reserved word")
		{
			//为了方便处理，把保留字转化为大写(与产生式一致，方便下一步匹配)
			transform(InputStr.begin(), InputStr.end(), InputStr.begin(), ::toupper);
		}
		string StackTopStr = AnalysisStack.back();//栈顶符号

		count++;//每一次循环，日志序号递增
		outputFile  << count <<","<< StackTopStr << ","<< InputStr <<",";

		//判断栈顶文法符号是终极符还是非终极符
		bool NonTerminalFlag = false;
		if (ptrProducList->nonTerminalSet.find(StackTopStr) != ptrProducList->nonTerminalSet.end())
		{
			NonTerminalFlag = true;
		}
		else
		{
			NonTerminalFlag = false;
		}

		if (!NonTerminalFlag)//如果栈顶符号是终极符
		{
			if (StackTopStr == "#" && InputStr == "#")
			{
				outputFile << "语法分析成功!" << endl;
				break;//匹配成功，跳出循环
			}
			else if (StackTopStr == InputStr)
			{
				//match成功
				AnalysisStack.pop_back();//弹出栈顶元素
				TokenValueVec.push_back(InputStr);//存储下每一个Token的Value，为建立语法分析树做准备
				TokenTypeVec.push_back(ReadToken->Type);//存储下每一个Token的Type，为建立语法分析树做准备
				ReadToken = ReadToken->next;//指向下一个输入流文法符号
				outputFile << "匹配" << endl;
				continue;
			}
			else if (StackTopStr == "@")
			{
				AnalysisStack.pop_back();
				outputFile << "分析栈弹出空字符 " << endl;
			}
			else
			{
				outputFile << "错误！语法分析失败！" << endl;
				break;//匹配失败，跳出循环
			}
		}
		else//栈顶符号是非终极符
		{
			//根据LL1分析表找到了对应产生式的序号
			if (ptrProducList->NontmlTypeMap.find(StackTopStr) != ptrProducList->NontmlTypeMap.end() && ptrProducList->LexTypeMap.find(InputStr) != ptrProducList->LexTypeMap.end())//能找到才赋值
			{
				seqNum = LL1Table[ptrProducList->NontmlTypeMap.find(StackTopStr)->second][ptrProducList->LexTypeMap.find(InputStr)->second];
				outputFile << "将产生式" << seqNum << "推导出的文法符号";
			}
			else//找不到对应枚举类型就报错
			{
				outputFile << "错误！语法分析失败！" << endl;
				break;//匹配失败，跳出循环
			}

			if (seqNum == 0)//产生式序号为0表示没有找到对应的产生式，说明LL1分析表构造不正确
			{
				outputFile << "错误！请检查LL1分析表构造是否正确！ " << endl;
				break;
			}
			AnalysisStack.pop_back();//弹出栈顶符号
			//倒序遍历产生式右部，并将这些文法符号压入栈中
			vector<string>::reverse_iterator vecIt;
			for (vecIt = ptrProducList->FindAProduction(seqNum).productionRight.rbegin(); vecIt != ptrProducList->FindAProduction(seqNum).productionRight.rend(); ++vecIt)
			{
				outputFile << *vecIt << "     ";
				AnalysisStack.push_back(*vecIt);
			}
			outputFile << "加入到分析栈中" << endl;
			vecTree.push_back(ptrProducList->FindAProduction(seqNum));//将用到的产生式记录下来，为下一步构造语法分析树做准备
			continue;
		}
	}
	outputFile.close();
}


void CreateSyntaxTree(productionList* ptrProducList)
{
	global_id = 1;
	//根节点及其直接子节点构造完毕
	root = new AnalysisTreeNode(vecTree[0].productionLeft, global_id++);
	for (vector<string>::iterator strVecIter = vecTree[0].productionRight.begin(); strVecIter != vecTree[0].productionRight.end(); strVecIter++)
	{
		root->children.emplace_back(new AnalysisTreeNode(*strVecIter,global_id++));
	}

	//下面用递归的方法继续向下构造语法分析树
	 treeIndex = 1;
	 global_token_index = 0;
	for (unsigned int i = 0; i < root->children.size(); i++) 
	{
		if (ptrProducList->TerminalSet.find(root->children[i]->TokenValue) != ptrProducList->TerminalSet.end())
		{
			//是终极符，不需要继续向下构造节点,进入下一个循环
			continue;
		}
		else
		{
			//是非终极符，需要继续向下建树
			dfsBuildTree(root->children[i], ptrProducList);
		}
	}
//生成Dot脚本并以txt形式保存
	stringstream ss;//把DOT Language表示的语法树的字符串放到这里边
	ss.clear();
	ss << "digraph GrammarTree {" << endl;
	queue<AnalysisTreeNode *> q;
	q.push(root);
	while (!q.empty()) 
	{
		AnalysisTreeNode *c = q.front();
		q.pop();

		// 该节点的样式和内容
		if (ptrProducList->TerminalSet.find(c->TokenValue) != ptrProducList->TerminalSet.end())
		{
			if (c->TokenValue == "@") 
			{
				ss << "\"" << c->id << "\" [shape=square; style=filled; fillcolor=lightpink; label=\""
					<< "blank" << "\"];" << endl;
			}
			else if (c->TokenValue == "ID"|| c->TokenValue == "INTC")
			{
				ss << "\"" << c->id << "\" [shape=square; style=filled; fillcolor=yellow; label=\""
					<< c->TokenType << "\"];" << endl;
			}
			else 
			{
				ss << "\"" << c->id << "\" [shape=square; style=filled; fillcolor=blue; label=\""
					<< c->TokenValue << "\"];" << endl;
			}
		}
		else 
		{
			ss << "\"" << c->id << "\" [style=filled; fillcolor=green; label=\""
				<< c->TokenValue << "\"];" << endl;
		}
		if (c->children.size() == 0) 
		{
			// 叶子结点
			continue;
		}
		// 跟其他节点的关系
		string children = "";
		for (unsigned int i = 0; i < c->children.size(); i++) 
		{
			children += "\"" + to_string(c->children[i]->id) + "\"; "; // "id; "
		}
		ss << "\"" << c->id << "\" -> {" << children << "}" << endl;
		ss << "{rank=same; " << children << "}" << endl;

		// 入栈
		for (AnalysisTreeNode *nd : c->children) 
		{
			q.push(nd);
		}

	}
	ss << "}" << endl;
	fstream outFile;
	outFile.open("D:\\Graphviz.txt");
	if (!outFile)
	{
		cout << "文件打开失败！" << endl;
	}
	outFile<<ss.str();
	outFile.close();
}


void dfsBuildTree(AnalysisTreeNode*& node, productionList* ptrProducList)
{
	// 构造子节点
	for (vector<string>::iterator strVecIter = vecTree[treeIndex].productionRight.begin(); strVecIter != vecTree[treeIndex].productionRight.end(); strVecIter++)
	{
		node->children.emplace_back(new AnalysisTreeNode(*strVecIter, global_id++));
	}
	
	// 构造完推导式后,下标就加加
	treeIndex++;
	for (unsigned int i = 0; i < node->children.size(); i++)
	{
		if (ptrProducList->TerminalSet.find(node->children[i]->TokenValue) != ptrProducList->TerminalSet.end())
		{
			//是终极符
			if (node->children[i]->TokenValue == "@") 
			{
			//不作处理
			}
			else
			{
				// 对于ID、INTC、CHARC的Token,将其Type赋值为其实际值
				node->children[i]->TokenType = TokenTypeVec[global_token_index];
				global_token_index++;
			}
			continue;
			
		}
		dfsBuildTree(node->children[i], ptrProducList);
	}

}




