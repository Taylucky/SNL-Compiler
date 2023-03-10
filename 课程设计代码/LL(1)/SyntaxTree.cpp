#include "SyntaxTree.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include <iomanip>

vector<production> vecTree;
vector<string> TokenValueVec;
vector<string> TokenTypeVec;

 AnalysisTreeNode* root;//�﷨�������ĸ��ڵ�
 int global_id;//DOT Language��ÿ���ڵ��id
 unsigned int global_token_index;//���ڽ�IDת��Ϊ���������ߺ�������
 unsigned int treeIndex;// ָʾvTree���±�

void SNLCompiler()
{
	LexicalAnalysis();
	string str("D:\\wenfa.txt");
	productionList* ptrProducList = new productionList(str);
	SyntaxAnalysis(ptrProducList);
	CreateSyntaxTree(ptrProducList);//�����﷨������
}


void LexicalAnalysis()
{
	ifstream ifile("D:\\SNLsource.txt");//�̲��ϵ�SNL����
	//ifstream ifile("D:\\bubble.txt");//ð������Դ����
	ostringstream buf;
	char ch;
	while (buf && ifile.get(ch))
		buf.put(ch);
	InputS = buf.str();
	TokenChain = new Token();
	CurChain = TokenChain;
	InputHandle();
	DisplayToken();//չʾToken���д�
}


void SyntaxAnalysis(productionList* ptrProducList)
{
	fstream outputFile;
	outputFile.open("D:\\output.csv");
	if (!outputFile)
	{
		cout << "�ļ���ʧ�ܣ�" << endl;
	}
	else
	{
		outputFile <<"���"<<","<< "����ջջ��" << ","<< "��ǰ������" << ","<< "����" << endl;
	}
	//��Token���е�ĩβ���һ��Token--"#"
	while (CurChain->next != NULL)//�ҵ�Token���е����һ��Token
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

	//�������ջ�������ú��ʼ��ջ��Ԫ�ء�#��+���ķ���ʼ���š�
	list<string> AnalysisStack;//��һ���������䵱����ջ
	AnalysisStack.push_back("#");
	AnalysisStack.push_back("Program");
	Token* ReadToken = TokenChain->next;
	int count = 0;//ƥ����־���
	while (!AnalysisStack.empty())
	{
		int seqNum = 0;
		string InputStr = ReadToken->Value;//������(�ķ�����)
		if (ReadToken->Type == "Reserved word")
		{
			//Ϊ�˷��㴦���ѱ�����ת��Ϊ��д(�����ʽһ�£�������һ��ƥ��)
			transform(InputStr.begin(), InputStr.end(), InputStr.begin(), ::toupper);
		}
		string StackTopStr = AnalysisStack.back();//ջ������

		count++;//ÿһ��ѭ������־��ŵ���
		outputFile  << count <<","<< StackTopStr << ","<< InputStr <<",";

		//�ж�ջ���ķ��������ռ������Ƿ��ռ���
		bool NonTerminalFlag = false;
		if (ptrProducList->nonTerminalSet.find(StackTopStr) != ptrProducList->nonTerminalSet.end())
		{
			NonTerminalFlag = true;
		}
		else
		{
			NonTerminalFlag = false;
		}

		if (!NonTerminalFlag)//���ջ���������ռ���
		{
			if (StackTopStr == "#" && InputStr == "#")
			{
				outputFile << "�﷨�����ɹ�!" << endl;
				break;//ƥ��ɹ�������ѭ��
			}
			else if (StackTopStr == InputStr)
			{
				//match�ɹ�
				AnalysisStack.pop_back();//����ջ��Ԫ��
				TokenValueVec.push_back(InputStr);//�洢��ÿһ��Token��Value��Ϊ�����﷨��������׼��
				TokenTypeVec.push_back(ReadToken->Type);//�洢��ÿһ��Token��Type��Ϊ�����﷨��������׼��
				ReadToken = ReadToken->next;//ָ����һ���������ķ�����
				outputFile << "ƥ��" << endl;
				continue;
			}
			else if (StackTopStr == "@")
			{
				AnalysisStack.pop_back();
				outputFile << "����ջ�������ַ� " << endl;
			}
			else
			{
				outputFile << "�����﷨����ʧ�ܣ�" << endl;
				break;//ƥ��ʧ�ܣ�����ѭ��
			}
		}
		else//ջ�������Ƿ��ռ���
		{
			//����LL1�������ҵ��˶�Ӧ����ʽ�����
			if (ptrProducList->NontmlTypeMap.find(StackTopStr) != ptrProducList->NontmlTypeMap.end() && ptrProducList->LexTypeMap.find(InputStr) != ptrProducList->LexTypeMap.end())//���ҵ��Ÿ�ֵ
			{
				seqNum = LL1Table[ptrProducList->NontmlTypeMap.find(StackTopStr)->second][ptrProducList->LexTypeMap.find(InputStr)->second];
				outputFile << "������ʽ" << seqNum << "�Ƶ������ķ�����";
			}
			else//�Ҳ�����Ӧö�����;ͱ���
			{
				outputFile << "�����﷨����ʧ�ܣ�" << endl;
				break;//ƥ��ʧ�ܣ�����ѭ��
			}

			if (seqNum == 0)//����ʽ���Ϊ0��ʾû���ҵ���Ӧ�Ĳ���ʽ��˵��LL1�������첻��ȷ
			{
				outputFile << "��������LL1���������Ƿ���ȷ�� " << endl;
				break;
			}
			AnalysisStack.pop_back();//����ջ������
			//�����������ʽ�Ҳ���������Щ�ķ�����ѹ��ջ��
			vector<string>::reverse_iterator vecIt;
			for (vecIt = ptrProducList->FindAProduction(seqNum).productionRight.rbegin(); vecIt != ptrProducList->FindAProduction(seqNum).productionRight.rend(); ++vecIt)
			{
				outputFile << *vecIt << "     ";
				AnalysisStack.push_back(*vecIt);
			}
			outputFile << "���뵽����ջ��" << endl;
			vecTree.push_back(ptrProducList->FindAProduction(seqNum));//���õ��Ĳ���ʽ��¼������Ϊ��һ�������﷨��������׼��
			continue;
		}
	}
	outputFile.close();
}


void CreateSyntaxTree(productionList* ptrProducList)
{
	global_id = 1;
	//���ڵ㼰��ֱ���ӽڵ㹹�����
	root = new AnalysisTreeNode(vecTree[0].productionLeft, global_id++);
	for (vector<string>::iterator strVecIter = vecTree[0].productionRight.begin(); strVecIter != vecTree[0].productionRight.end(); strVecIter++)
	{
		root->children.emplace_back(new AnalysisTreeNode(*strVecIter,global_id++));
	}

	//�����õݹ�ķ����������¹����﷨������
	 treeIndex = 1;
	 global_token_index = 0;
	for (unsigned int i = 0; i < root->children.size(); i++) 
	{
		if (ptrProducList->TerminalSet.find(root->children[i]->TokenValue) != ptrProducList->TerminalSet.end())
		{
			//���ռ���������Ҫ�������¹���ڵ�,������һ��ѭ��
			continue;
		}
		else
		{
			//�Ƿ��ռ�������Ҫ�������½���
			dfsBuildTree(root->children[i], ptrProducList);
		}
	}
//����Dot�ű�����txt��ʽ����
	stringstream ss;//��DOT Language��ʾ���﷨�����ַ����ŵ������
	ss.clear();
	ss << "digraph GrammarTree {" << endl;
	queue<AnalysisTreeNode *> q;
	q.push(root);
	while (!q.empty()) 
	{
		AnalysisTreeNode *c = q.front();
		q.pop();

		// �ýڵ����ʽ������
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
			// Ҷ�ӽ��
			continue;
		}
		// �������ڵ�Ĺ�ϵ
		string children = "";
		for (unsigned int i = 0; i < c->children.size(); i++) 
		{
			children += "\"" + to_string(c->children[i]->id) + "\"; "; // "id; "
		}
		ss << "\"" << c->id << "\" -> {" << children << "}" << endl;
		ss << "{rank=same; " << children << "}" << endl;

		// ��ջ
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
		cout << "�ļ���ʧ�ܣ�" << endl;
	}
	outFile<<ss.str();
	outFile.close();
}


void dfsBuildTree(AnalysisTreeNode*& node, productionList* ptrProducList)
{
	// �����ӽڵ�
	for (vector<string>::iterator strVecIter = vecTree[treeIndex].productionRight.begin(); strVecIter != vecTree[treeIndex].productionRight.end(); strVecIter++)
	{
		node->children.emplace_back(new AnalysisTreeNode(*strVecIter, global_id++));
	}
	
	// �������Ƶ�ʽ��,�±�ͼӼ�
	treeIndex++;
	for (unsigned int i = 0; i < node->children.size(); i++)
	{
		if (ptrProducList->TerminalSet.find(node->children[i]->TokenValue) != ptrProducList->TerminalSet.end())
		{
			//���ռ���
			if (node->children[i]->TokenValue == "@") 
			{
			//��������
			}
			else
			{
				// ����ID��INTC��CHARC��Token,����Type��ֵΪ��ʵ��ֵ
				node->children[i]->TokenType = TokenTypeVec[global_token_index];
				global_token_index++;
			}
			continue;
			
		}
		dfsBuildTree(node->children[i], ptrProducList);
	}

}




