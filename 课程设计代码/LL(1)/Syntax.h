#pragma once

#include "globals.h"	
using namespace std;
class production//�����洢һ������ʽ
{
public:
	int sequenceNum;
	string productionLeft;
	vector<string> productionRight;
public:
	bool operator<(const production &p) const //�����������ʹ֮����Ϊmap��keyֵ
	{
		return (this->sequenceNum<p.sequenceNum);
	}
};

class productionList//����ʽ����
{
public:
	list<production> producList;//����ʽ����
	set<string> nonTerminalSet;//���ռ�����
	set<string> TerminalSet;//���ռ�����

	map<string, set<string>> firstMap;//���з��ռ�����First��
	map<string, set<string>> followMap;//���з��ռ�����Follow��
	map<production, set<string>> predictMap;//ÿ������ʽ��Ӧ��Predict��

	map<string, LexType> LexTypeMap;
	map<string, NontmlType> NontmlTypeMap; 
public:
	productionList(string dir);//����Ϊ����ʽ�ļ����ڵ�Ŀ¼·��+�ļ���

	//����������
	set<string>first_str(string str);
	void first_1();
	set<string> follow_str(string str, string old);
	void follow_1();
	void predict();

	//���������ϱ������ļ�
	void storeFirstMap();
	void storeFollowMap();
	void storePredictMap();
	void storeNonTerminalSet();
	void storeTerminalSet();


	production get_i(int i)
	{
		list<production>::iterator temp = producList.begin();
		int count = 0;
		while (count < i)
		{
			temp++;
			count++;
		}
		return *temp;
	}

	void readProducList(string dir);//��ȡ����ʽ�ļ������productionList�ͷ��ռ���set
	void fillTerminalSet();//����ռ�����������֮����˹�����String��LexType��ӳ��
	void makeMapFromStringToLexType();//ʵ�ִ�string��enum����LexType��ӳ�䣬���㹹��LL1������
	void makeMapFromStringToNontmlType();//ʵ�ִ�string��enum����NontmlType��ӳ�䣬���㹹��LL1������
	void fillLL1Table();//���������predict������乫��ȫ������LL1������

	production& FindAProduction(int num);//���ݲ���ʽ��ţ��ҵ�ĳ������ʽ
	
};