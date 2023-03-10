#pragma once

#include "globals.h"	
using namespace std;
class production//用来存储一条产生式
{
public:
	int sequenceNum;
	string productionLeft;
	vector<string> productionRight;
public:
	bool operator<(const production &p) const //重载运算符，使之能作为map的key值
	{
		return (this->sequenceNum<p.sequenceNum);
	}
};

class productionList//产生式链表
{
public:
	list<production> producList;//产生式链表
	set<string> nonTerminalSet;//非终极符集
	set<string> TerminalSet;//非终极符集

	map<string, set<string>> firstMap;//所有非终极符的First集
	map<string, set<string>> followMap;//所有非终极符的Follow集
	map<production, set<string>> predictMap;//每条产生式对应的Predict集

	map<string, LexType> LexTypeMap;
	map<string, NontmlType> NontmlTypeMap; 
public:
	productionList(string dir);//参数为产生式文件所在的目录路径+文件名

	//求三个集合
	set<string>first_str(string str);
	void first_1();
	set<string> follow_str(string str, string old);
	void follow_1();
	void predict();

	//将三个集合保存至文件
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

	void readProducList(string dir);//读取产生式文件，填充productionList和非终极符set
	void fillTerminalSet();//填充终极符集，便于之后的人工构造String到LexType的映射
	void makeMapFromStringToLexType();//实现从string到enum类型LexType的映射，方便构造LL1分析表
	void makeMapFromStringToNontmlType();//实现从string到enum类型NontmlType的映射，方便构造LL1分析表
	void fillLL1Table();//根据求出的predict集，填充公有全局数组LL1分析表

	production& FindAProduction(int num);//根据产生式编号，找到某个产生式
	
};