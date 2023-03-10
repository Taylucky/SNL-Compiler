 #include"Syntax.h"

productionList::productionList(string dir)
{
	//填充产生式list和非终极符set
	readProducList(dir);

	//填充终极符set
	fillTerminalSet();
	first_1();
	follow_1();
	predict();
	//实现从string到enum类型LexType的映射，方便构造LL1分析表
	makeMapFromStringToLexType();

	//实现从string到enum类型NontmlType的映射，方便构造LL1分析表
	makeMapFromStringToNontmlType();

	fillLL1Table();

	//存储相关信息
	storeNonTerminalSet();
	storeTerminalSet();
	storeFirstMap();
	storeFollowMap();
	storePredictMap();
}


void productionList::readProducList(string dir)
{
	//填充产生式list和非终极符set
	ifstream infile;
	infile.open(dir);
	if (!infile)
	{
		cout << "文件打开失败！" << endl;
	}
	string line;//从文件中读取一行产生式
	string substr;//承载分割后的字符串
	production* ptrProduc;//指向一个产生式
	bool equalFlag = false;//标志是否已读到"::="这个符号
	int seqNum;//当前行号，也即产生式的编号


	while (getline(infile, line))
	{
		istringstream stream(line);//用于分割字符
		ptrProduc = new production();//一行对应一个产生式


		stream >> substr;//每行的第一个元素必定是行号
		substr.pop_back();
		istringstream toInt(substr);
		toInt >> seqNum;
		ptrProduc->sequenceNum = seqNum;
		//cout << "Sequence:" << seqNum << endl;

		while (stream >> substr)
		{
			if (substr == "::=")
			{
				equalFlag = true;
				//cout << "::=" << endl;
				continue;//对等号不做任何处理
			}

			if (!equalFlag)
			{
				ptrProduc->productionLeft = substr;
				//cout << "Left:" << substr << endl;
				nonTerminalSet.insert(substr);//从产生式中提出非终极符，放入非终极符集
			}
			else
			{
				ptrProduc->productionRight.push_back(substr);
				//cout << "Right:" << substr << endl;
			}
		}


		producList.push_back(*ptrProduc);//将这个产生式链接入producList中
		equalFlag = false;//一行内容结束，将标志归位
		line.clear();
		substr.clear();
		//cout <<"This line is over!\n\n\n\n"<< endl;
	}
	infile.close();
}


void productionList::fillTerminalSet()
{
	list<production>::iterator iter;
	vector<string>::iterator it;
	for (iter = producList.begin(); iter != producList.end(); iter++)
	{
		for (it = (*iter).productionRight.begin(); it != (*iter).productionRight.end(); it++)
		{
			if (nonTerminalSet.find(*it) != nonTerminalSet.end())//该文法符号在非终极符集中
			{

			}
			else//该文法符号应该在终极符集中
			{
				TerminalSet.insert(*it);
			}
		}
	}
}


void productionList::storeFirstMap()
{

	fstream outputFile;
	outputFile.open("D:\\FirstSet.txt");
	if (!outputFile)
	{
		cout << "文件打开失败！" << endl;
		return;
	}
	else
	{
		outputFile << "First Map: " << endl << endl;
	}

	for (map<string, set<string>>::iterator firstMapIter = firstMap.begin(); firstMapIter != firstMap.end(); firstMapIter++)
	{
		outputFile << firstMapIter->first << ": { ";
		for (set<string>::iterator setIter = firstMapIter->second.begin(); setIter != firstMapIter->second.end(); setIter++)
		{
			outputFile << *setIter << " ";
		}
		outputFile << " }" << endl;
	}
	outputFile << endl << endl;
	outputFile.close();

}


void productionList::storeFollowMap()
{
	fstream outputFile;
	outputFile.open("D:\\FollowSet.txt");
	if (!outputFile)
	{
		cout << "文件打开失败！" << endl;
		return;
	}
	else
	{
		outputFile << "Follow Map: " << endl << endl;
	}

	for (map<string, set<string>>::iterator followMapIter = followMap.begin(); followMapIter != followMap.end(); followMapIter++)
	{
		outputFile << followMapIter->first << ": { ";
		for (set<string>::iterator setIter = followMapIter->second.begin(); setIter != followMapIter->second.end(); setIter++)
		{
			outputFile << *setIter << " ";
		}
		outputFile << " }" << endl;
	}
	
	outputFile << endl << endl;
	outputFile.close();
}


void productionList::storePredictMap()
{
	fstream outputFile;
	outputFile.open("D:\\PredictSet.txt");
	if (!outputFile)
	{
		cout << "文件打开失败！" << endl;
		return;
	}
	else
	{
		outputFile << "Predict Map: " << endl << endl;
	}

	for (map<production, set<string>>::iterator predictMapIter = predictMap.begin(); predictMapIter != predictMap.end(); predictMapIter++)
	{
		outputFile << predictMapIter->first.sequenceNum << ")    ";
		outputFile << " { ";
		for (set<string>::iterator setIter = predictMapIter->second.begin(); setIter != predictMapIter->second.end(); setIter++)
		{
			outputFile << *setIter << " ";
		}
		outputFile << " }" << endl;
	}
	outputFile << endl << endl;

	outputFile.close();
}


void  productionList::storeNonTerminalSet()
{

	fstream outputFile;
	outputFile.open("D:\\NonTerminalSet.txt");
	if (!outputFile)
	{
		cout << "文件打开失败！" << endl;
		return;
	}
	else
	{
		outputFile << "NonTerminal Set: " << endl << endl;
	}

	for (set<string>::iterator iter = nonTerminalSet.begin(); iter != nonTerminalSet.end(); iter++)
	{
		outputFile << *iter << endl;
	}

	outputFile << endl << endl;
	outputFile.close();

}


void  productionList::storeTerminalSet()
{
	fstream outputFile;
	outputFile.open("D:\\TerminalSet.txt");
	if (!outputFile)
	{
		cout << "文件打开失败！" << endl;
		return;
	}
	else
	{
		outputFile << "Terminal Set: " << endl << endl;
	}

	for (set<string>::iterator iter = TerminalSet.begin(); iter != TerminalSet.end(); iter++)
	{
		outputFile << *iter << endl;
	}

	outputFile << endl << endl;
	outputFile.close();
}


//---------------------------------------------------------------------------------------------------------
//--------------------------------------------上分界线-------------------------------------------------------

set<string> productionList::first_str(string str)
{
	set<string> tempSet;//存储每个str的first集合，也是返回值
	for (int j = 0; j < producList.size(); j++)//遍历每一个文法
	{
		if (this->get_i(j).productionLeft == str)//如果左部等于该求字符串
		{
			for (int k = 0; k < this->get_i(j).productionRight.size(); k++)//如果取到右边的第k个字符，那么前k-1个字符均能推出空串
			{
				//1、如果当前字符是空串并且是左后一个字符
				if (this->get_i(j).productionRight[k] == "@" && k + 1 == this->get_i(j).productionRight.size())
				{
					tempSet.insert("@");
					break;
				}
				//2、如果当前字符是终极符且不是空串
				else if ((nonTerminalSet.find(this->get_i(j).productionRight[k]) == nonTerminalSet.end() && this->get_i(j).productionRight[k] != "@"))
				{
					tempSet.insert(this->get_i(j).productionRight[k]);
					break;//加入后考虑下一个产生式
				}
				//3、如果当前字符是非终极符，考虑它的first集合
				else if (nonTerminalSet.find(this->get_i(j).productionRight[k]) != nonTerminalSet.end())
				{
					set<string> tempSet1 = first_str(this->get_i(j).productionRight[k]);//递归调用求解
					tempSet.insert(tempSet1.begin(), tempSet1.end());

					if (tempSet1.find("@") == tempSet1.end() || k + 1 == this->get_i(j).productionRight.size())//如果不含有空串或者  有空串但到达右边字符最后一个
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
void productionList::first_1()//调用求单个first集的函数求出所有的first集
{
	set<string>::iterator it;
	for (it = nonTerminalSet.begin(); it != nonTerminalSet.end(); it++)
	{
		set<string> s = first_str(*it);
		firstMap.insert(pair<string, set<string> >(*it, s));
	}
}
set<string> productionList::follow_str(string str, string old)//old用来保存谁递归调用了，确定是否产生闭包，以免形成死锁
{
	set<string> tempSet;//保存当前求的follow集
	for (int i = 0; i < producList.size(); i++)//遍历每一个文法产生式
	{
		for (int k = 0; k < this->get_i(i).productionRight.size(); k++)//取到第i个产生式右边的第k个字符
		{
			//如果发现当前字符出现在产生式右边
			if (this->get_i(i).productionRight[k] == str)
			{
				//接下来讨论当前字符是不是在产生式最右边
				//一、如果发现当前字符出现在产生式最右边，那么讨论它的left的follow集合
				if ((this->get_i(i).productionRight.size() - 1) == k)
				{
					//1、做递归时、还有左边跟最右边相等时的验证；可以验证是个闭包，不会添加新元素，所以直接跳过：避免死锁
					if (old == this->get_i(i).productionLeft || this->get_i(i).productionRight[k] == this->get_i(i).productionLeft)
					{
						break;
					}
					//2、出现在产生式最右边,且左边推出的不等于它自身（即不是形成一个不添加新元素的闭包）；
					//那么此时观察left的follow集合
					else
					{
						set<string> tempSet1 = follow_str(this->get_i(i).productionLeft, str);
						if (tempSet1.size() != 0)
						{
							tempSet.insert(tempSet1.begin(), tempSet1.end());
						}
					}
				}
				//二、不是出现在最右边，那么观察紧邻它的下一个字符的first集合
				int j = 1;//j-1个符号可推出空串
				string str_next;
				while (k + j < this->get_i(i).productionRight.size())//k+j代表下一个字符，如果字符的first集中有空集，则j++直到right的末尾字符
				{
					str_next = this->get_i(i).productionRight[k + j];
					//1、是终极符直接添加
					if (nonTerminalSet.find(str_next) == nonTerminalSet.end())
					{
						tempSet.insert(str_next);
						break;
					}
					// 2、是非终极符：需要讨论是否能推出空串
					else
					{
						tempSet.insert(firstMap[str_next].begin(), firstMap[str_next].end());//并上紧随其后的FIRST集合	
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
							if (k + j == this->get_i(i).productionRight.size())//表明没有下一个字符的下一个字符，即尾部可以推出空串则，所以并上left集
							{
								set<string> tempSet1 = follow_str(this->get_i(i).productionLeft, str);	//递归调用
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
void productionList::follow_1()
{
	followMap["Program"].insert("#");//初始化开始字符的follow集合
	set<string>::iterator it_F;
	for (it_F = nonTerminalSet.begin(); it_F != nonTerminalSet.end(); it_F++)
	{
		set<string> s = follow_str(*it_F, "");
		followMap.insert(pair<string, set<string> >(*it_F, s));
	}
}
void productionList::predict()
{
	//初始化predictMap
	set<string> nullstrset;  //一个空的string的set集
	list<production>::iterator produciter;
	for (produciter = producList.begin(); produciter != producList.end(); produciter++)
	{
		predictMap.insert(pair<production, set<string >>((*produciter), nullstrset));
	}
	vector<string>::iterator it;
	for (int i = 0; i < producList.size(); i++)//遍历每一个文法产生式
	{
		int k = 0;//判断右部第k个字符
		while (k < this->get_i(i).productionRight.size())
		{
			//如果是终结符但不是空字符
			if (nonTerminalSet.find(this->get_i(i).productionRight[k]) == nonTerminalSet.end() && this->get_i(i).productionRight[k] != "@")
			{
				predictMap[this->get_i(i)].insert(this->get_i(i).productionRight[k]);
				break;
			}
			//如果是空字符
			else if (this->get_i(i).productionRight[k] == "@")
			{
				predictMap[this->get_i(i)].insert(followMap[this->get_i(i).productionLeft].begin(), followMap[this->get_i(i).productionLeft].end());
				break;
			}
			//如果不是终结符:判断是否能推出空串
			set<string> s = firstMap[this->get_i(i).productionRight[k]];
			set<string>::iterator iter = find(s.begin(), s.end(), "@");

			if (iter == s.end())//产生式右部第k个字符first集合不含空
			{

				predictMap[this->get_i(i)] = firstMap[this->get_i(i).productionRight[k]];
				break;
			}
			else if (iter != s.end()) // 产生式右部第k个字符first集合含空：继续向后判断
			{
				s.erase("@");//删除空字符串
				predictMap[this->get_i(i)].insert(s.begin(), s.end());	//并上该字符的first集合之后往后看
				k++;
				if (k == this->get_i(i).productionRight.size())
				{
					//插入左边的follow集合
					predictMap[this->get_i(i)].insert(followMap[this->get_i(i).productionLeft].begin(), followMap[this->get_i(i).productionLeft].end());
					break;
				}
			}
		}
	}
}

//--------------------------------------------下分界线-------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------


void productionList::fillLL1Table()
{
	//默认先全部置零
	for (int i = 0; i < TABLESIZE; i++)
	{
		for (int j = 0; j < TABLESIZE; j++)
		{
			LL1Table[i][j] = 0;
		}
	}
	set<string>::iterator setIter;
	map<production, set<string>>::iterator predictMapIter;
	for (predictMapIter = predictMap.begin(); predictMapIter != predictMap.end(); predictMapIter++)
	{
		for (setIter = predictMapIter->second.begin(); setIter != predictMapIter->second.end(); setIter++)
		{
			if (NontmlTypeMap.find(predictMapIter->first.productionLeft) != NontmlTypeMap.end() && LexTypeMap.find(*setIter) != LexTypeMap.end())//能找到，才赋值
			{
				LL1Table[NontmlTypeMap.find(predictMapIter->first.productionLeft)->second][LexTypeMap.find(*setIter)->second] = predictMapIter->first.sequenceNum;
			}
		}
	}
}


void productionList::makeMapFromStringToLexType()
{
	LexTypeMap.insert({ "PROGRAM",PROGRAM});
	LexTypeMap.insert({ "PROCEDURE",PROCEDURE});
	LexTypeMap.insert({ "TYPE", TYPE});
	LexTypeMap.insert({ "VAR", VAR});
	LexTypeMap.insert({ "IF", IF});
	LexTypeMap.insert({ "THEN", THEN});
	LexTypeMap.insert({ "ELSE", ELSE});
	LexTypeMap.insert({ "FI", FI});
	LexTypeMap.insert({ "WHILE", WHILE});
	LexTypeMap.insert({ "DO", DO});
	LexTypeMap.insert({ "ENDWH", ENDWH});
	LexTypeMap.insert({ "BEGIN", BEGIN});
	LexTypeMap.insert({ "END", END});
	LexTypeMap.insert({ "READ", READ});
	LexTypeMap.insert({ "WRITE", WRITE});
	LexTypeMap.insert({ "ARRAY", ARRAY});
	LexTypeMap.insert({ "OF", OF});
	LexTypeMap.insert({ "RECORD", RECORD});
	LexTypeMap.insert({ "RETURN", RETURN});
	LexTypeMap.insert({ "INTEGER", INTEGER});
	LexTypeMap.insert({ "CHAR", CHAR});
	LexTypeMap.insert({ "ID",ID });
	LexTypeMap.insert({ "INTC", INTC});
	LexTypeMap.insert({ "'", CHARC});//实际上对应的是  '  这个符号，用于词法分析中的读取字符
	LexTypeMap.insert({ ":=", ASSIGN});
	LexTypeMap.insert({ "=", EQ});
	LexTypeMap.insert({ "<", LT});
	LexTypeMap.insert({ "+",PLUS});
	LexTypeMap.insert({ "-", MINUS});
	LexTypeMap.insert({ "*", TIMES});
	LexTypeMap.insert({ "/", OVER});
	LexTypeMap.insert({ "(", LPAREN});
	LexTypeMap.insert({ ")", RPAREN});
	LexTypeMap.insert({ ".", DOT});
	LexTypeMap.insert({ ":", COLON});//实际上对应的是  :  这个符号，用于词法分析中的读取字符,在SNL语言中，没有单独使用：的情况，通常用在::=和:=中
	LexTypeMap.insert({ ";", SEMI});
	LexTypeMap.insert({ ",", COMMA});
	LexTypeMap.insert({ "[", LMIDPAREN});
	LexTypeMap.insert({ "]", RMIDPAREN});
	LexTypeMap.insert({ "..", UNDERANGE});


}


void productionList::makeMapFromStringToNontmlType()
{
	NontmlTypeMap.insert({"Program",Program });
	NontmlTypeMap.insert({ "ProgramHead", ProgramHead });
	NontmlTypeMap.insert({ "ProgramName", ProgramName });
	NontmlTypeMap.insert({ "DeclarePart", DeclarePart});
	NontmlTypeMap.insert({ "TypeDec", TypeDec});
	NontmlTypeMap.insert({ "TypeDeclaration", TypeDeclaration });
	NontmlTypeMap.insert({ "TypeDecList", TypeDecList});
	NontmlTypeMap.insert({ "TypeDecMore", TypeDecMore});
	NontmlTypeMap.insert({ "TypeId", TypeId});
	NontmlTypeMap.insert({ "TypeName", TypeName});
	NontmlTypeMap.insert({ "BaseType", BaseType});
	NontmlTypeMap.insert({ "StructureType", StructureType});
	NontmlTypeMap.insert({ "ArrayType", ArrayType});
	NontmlTypeMap.insert({ "Low", Low});
	NontmlTypeMap.insert({ "Top", Top});
	NontmlTypeMap.insert({ "RecType", RecType});
	NontmlTypeMap.insert({ "FieldDecList", FieldDecList});
	NontmlTypeMap.insert({ "FieldDecMore", FieldDecMore });
	NontmlTypeMap.insert({ "IdList",  IdList});
	NontmlTypeMap.insert({ "IdMore", IdMore});
	NontmlTypeMap.insert({ "VarDec", VarDec});
	NontmlTypeMap.insert({ "VarDeclaration", VarDeclaration});
	NontmlTypeMap.insert({ "VarDecList", VarDecList});
	NontmlTypeMap.insert({ "VarDecMore", VarDecMore});
	NontmlTypeMap.insert({ "VarIdList", VarIdList});
	NontmlTypeMap.insert({ "VarIdMore", VarIdMore});
	NontmlTypeMap.insert({ "ProcDec", ProcDec});
	NontmlTypeMap.insert({ "ProcDeclaration", ProcDeclaration});
	NontmlTypeMap.insert({ "ProcDecMore", ProcDecMore});
	NontmlTypeMap.insert({ "ProcName", ProcName});
	NontmlTypeMap.insert({ "ParamList", ParamList});
	NontmlTypeMap.insert({ "ParamDecList", ParamDecList});
	NontmlTypeMap.insert({ "ParamMore", ParamMore});
	NontmlTypeMap.insert({ "Param", Param });
	NontmlTypeMap.insert({ "FormList", FormList});
	NontmlTypeMap.insert({ "FidMore", FidMore});
	NontmlTypeMap.insert({ "ProcDecPart", ProcDecPart});
	NontmlTypeMap.insert({ "ProcBody", ProcBody});
	NontmlTypeMap.insert({ "ProgramBody", ProgramBody});
	NontmlTypeMap.insert({ "StmList", StmList});
	NontmlTypeMap.insert({ "StmMore", StmMore});
	NontmlTypeMap.insert({ "Stm", Stm});
	NontmlTypeMap.insert({ "AssCall", AssCall});
	NontmlTypeMap.insert({ "AssignmentRest", AssignmentRest});
	NontmlTypeMap.insert({ "ConditionalStm", ConditionalStm});
	//NontmlTypeMap.insert({ "", StmL});
	NontmlTypeMap.insert({ "LoopStm", LoopStm});
	NontmlTypeMap.insert({ "InputStm", InputStm});
	NontmlTypeMap.insert({ "Invar", InVar});
	NontmlTypeMap.insert({ "OutputStm", OutputStm});
	NontmlTypeMap.insert({ "ReturnStm", ReturnStm});
	NontmlTypeMap.insert({ "CallStmRest", CallStmRest});
	NontmlTypeMap.insert({ "ActParamList", ActParamList});
	NontmlTypeMap.insert({ "ActParamMore", ActParamMore});
	NontmlTypeMap.insert({ "RelExp", RelExp});
	NontmlTypeMap.insert({ "OtherRelE", OtherRelE});
	NontmlTypeMap.insert({ "Exp", Exp});
	NontmlTypeMap.insert({ "OtherTerm", OtherTerm});
	NontmlTypeMap.insert({ "Term", Term});
	NontmlTypeMap.insert({ "OtherFactor", OtherFactor});
	NontmlTypeMap.insert({ "Factor", Factor});
	NontmlTypeMap.insert({ "Variable", Variable});
	NontmlTypeMap.insert({ "VariMore", VariMore});
	NontmlTypeMap.insert({ "FieldVar", FieldVar});
	NontmlTypeMap.insert({ "FieldVarMore", FieldVarMore});
	NontmlTypeMap.insert({ "CmpOp", CmpOp});
	NontmlTypeMap.insert({ "AddOp", AddOp});
	NontmlTypeMap.insert({ "MultOp", MultOp});
}


production& productionList::FindAProduction(int num)
{
	list<production>::iterator iter;
	for (iter = producList.begin(); iter != producList.end(); iter++)
	{
		if (iter->sequenceNum == num)
		{
			return *iter;
		}
	}
}
