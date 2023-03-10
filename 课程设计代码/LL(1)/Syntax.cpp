 #include"Syntax.h"

productionList::productionList(string dir)
{
	//������ʽlist�ͷ��ռ���set
	readProducList(dir);

	//����ռ���set
	fillTerminalSet();
	first_1();
	follow_1();
	predict();
	//ʵ�ִ�string��enum����LexType��ӳ�䣬���㹹��LL1������
	makeMapFromStringToLexType();

	//ʵ�ִ�string��enum����NontmlType��ӳ�䣬���㹹��LL1������
	makeMapFromStringToNontmlType();

	fillLL1Table();

	//�洢�����Ϣ
	storeNonTerminalSet();
	storeTerminalSet();
	storeFirstMap();
	storeFollowMap();
	storePredictMap();
}


void productionList::readProducList(string dir)
{
	//������ʽlist�ͷ��ռ���set
	ifstream infile;
	infile.open(dir);
	if (!infile)
	{
		cout << "�ļ���ʧ�ܣ�" << endl;
	}
	string line;//���ļ��ж�ȡһ�в���ʽ
	string substr;//���طָ����ַ���
	production* ptrProduc;//ָ��һ������ʽ
	bool equalFlag = false;//��־�Ƿ��Ѷ���"::="�������
	int seqNum;//��ǰ�кţ�Ҳ������ʽ�ı��


	while (getline(infile, line))
	{
		istringstream stream(line);//���ڷָ��ַ�
		ptrProduc = new production();//һ�ж�Ӧһ������ʽ


		stream >> substr;//ÿ�еĵ�һ��Ԫ�رض����к�
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
				continue;//�ԵȺŲ����κδ���
			}

			if (!equalFlag)
			{
				ptrProduc->productionLeft = substr;
				//cout << "Left:" << substr << endl;
				nonTerminalSet.insert(substr);//�Ӳ���ʽ��������ռ�����������ռ�����
			}
			else
			{
				ptrProduc->productionRight.push_back(substr);
				//cout << "Right:" << substr << endl;
			}
		}


		producList.push_back(*ptrProduc);//���������ʽ������producList��
		equalFlag = false;//һ�����ݽ���������־��λ
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
			if (nonTerminalSet.find(*it) != nonTerminalSet.end())//���ķ������ڷ��ռ�������
			{

			}
			else//���ķ�����Ӧ�����ռ�������
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
		cout << "�ļ���ʧ�ܣ�" << endl;
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
		cout << "�ļ���ʧ�ܣ�" << endl;
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
		cout << "�ļ���ʧ�ܣ�" << endl;
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
		cout << "�ļ���ʧ�ܣ�" << endl;
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
		cout << "�ļ���ʧ�ܣ�" << endl;
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
//--------------------------------------------�Ϸֽ���-------------------------------------------------------

set<string> productionList::first_str(string str)
{
	set<string> tempSet;//�洢ÿ��str��first���ϣ�Ҳ�Ƿ���ֵ
	for (int j = 0; j < producList.size(); j++)//����ÿһ���ķ�
	{
		if (this->get_i(j).productionLeft == str)//����󲿵��ڸ����ַ���
		{
			for (int k = 0; k < this->get_i(j).productionRight.size(); k++)//���ȡ���ұߵĵ�k���ַ�����ôǰk-1���ַ������Ƴ��մ�
			{
				//1�������ǰ�ַ��ǿմ����������һ���ַ�
				if (this->get_i(j).productionRight[k] == "@" && k + 1 == this->get_i(j).productionRight.size())
				{
					tempSet.insert("@");
					break;
				}
				//2�������ǰ�ַ����ռ����Ҳ��ǿմ�
				else if ((nonTerminalSet.find(this->get_i(j).productionRight[k]) == nonTerminalSet.end() && this->get_i(j).productionRight[k] != "@"))
				{
					tempSet.insert(this->get_i(j).productionRight[k]);
					break;//���������һ������ʽ
				}
				//3�������ǰ�ַ��Ƿ��ռ�������������first����
				else if (nonTerminalSet.find(this->get_i(j).productionRight[k]) != nonTerminalSet.end())
				{
					set<string> tempSet1 = first_str(this->get_i(j).productionRight[k]);//�ݹ�������
					tempSet.insert(tempSet1.begin(), tempSet1.end());

					if (tempSet1.find("@") == tempSet1.end() || k + 1 == this->get_i(j).productionRight.size())//��������пմ�����  �пմ��������ұ��ַ����һ��
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
void productionList::first_1()//�����󵥸�first���ĺ���������е�first��
{
	set<string>::iterator it;
	for (it = nonTerminalSet.begin(); it != nonTerminalSet.end(); it++)
	{
		set<string> s = first_str(*it);
		firstMap.insert(pair<string, set<string> >(*it, s));
	}
}
set<string> productionList::follow_str(string str, string old)//old��������˭�ݹ�����ˣ�ȷ���Ƿ�����հ��������γ�����
{
	set<string> tempSet;//���浱ǰ���follow��
	for (int i = 0; i < producList.size(); i++)//����ÿһ���ķ�����ʽ
	{
		for (int k = 0; k < this->get_i(i).productionRight.size(); k++)//ȡ����i������ʽ�ұߵĵ�k���ַ�
		{
			//������ֵ�ǰ�ַ������ڲ���ʽ�ұ�
			if (this->get_i(i).productionRight[k] == str)
			{
				//���������۵�ǰ�ַ��ǲ����ڲ���ʽ���ұ�
				//һ��������ֵ�ǰ�ַ������ڲ���ʽ���ұߣ���ô��������left��follow����
				if ((this->get_i(i).productionRight.size() - 1) == k)
				{
					//1�����ݹ�ʱ��������߸����ұ����ʱ����֤��������֤�Ǹ��հ������������Ԫ�أ�����ֱ����������������
					if (old == this->get_i(i).productionLeft || this->get_i(i).productionRight[k] == this->get_i(i).productionLeft)
					{
						break;
					}
					//2�������ڲ���ʽ���ұ�,������Ƴ��Ĳ������������������γ�һ���������Ԫ�صıհ�����
					//��ô��ʱ�۲�left��follow����
					else
					{
						set<string> tempSet1 = follow_str(this->get_i(i).productionLeft, str);
						if (tempSet1.size() != 0)
						{
							tempSet.insert(tempSet1.begin(), tempSet1.end());
						}
					}
				}
				//�������ǳ��������ұߣ���ô�۲����������һ���ַ���first����
				int j = 1;//j-1�����ſ��Ƴ��մ�
				string str_next;
				while (k + j < this->get_i(i).productionRight.size())//k+j������һ���ַ�������ַ���first�����пռ�����j++ֱ��right��ĩβ�ַ�
				{
					str_next = this->get_i(i).productionRight[k + j];
					//1�����ռ���ֱ�����
					if (nonTerminalSet.find(str_next) == nonTerminalSet.end())
					{
						tempSet.insert(str_next);
						break;
					}
					// 2���Ƿ��ռ�������Ҫ�����Ƿ����Ƴ��մ�
					else
					{
						tempSet.insert(firstMap[str_next].begin(), firstMap[str_next].end());//���Ͻ�������FIRST����	
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
							if (k + j == this->get_i(i).productionRight.size())//����û����һ���ַ�����һ���ַ�����β�������Ƴ��մ������Բ���left��
							{
								set<string> tempSet1 = follow_str(this->get_i(i).productionLeft, str);	//�ݹ����
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
	followMap["Program"].insert("#");//��ʼ����ʼ�ַ���follow����
	set<string>::iterator it_F;
	for (it_F = nonTerminalSet.begin(); it_F != nonTerminalSet.end(); it_F++)
	{
		set<string> s = follow_str(*it_F, "");
		followMap.insert(pair<string, set<string> >(*it_F, s));
	}
}
void productionList::predict()
{
	//��ʼ��predictMap
	set<string> nullstrset;  //һ���յ�string��set��
	list<production>::iterator produciter;
	for (produciter = producList.begin(); produciter != producList.end(); produciter++)
	{
		predictMap.insert(pair<production, set<string >>((*produciter), nullstrset));
	}
	vector<string>::iterator it;
	for (int i = 0; i < producList.size(); i++)//����ÿһ���ķ�����ʽ
	{
		int k = 0;//�ж��Ҳ���k���ַ�
		while (k < this->get_i(i).productionRight.size())
		{
			//������ս�������ǿ��ַ�
			if (nonTerminalSet.find(this->get_i(i).productionRight[k]) == nonTerminalSet.end() && this->get_i(i).productionRight[k] != "@")
			{
				predictMap[this->get_i(i)].insert(this->get_i(i).productionRight[k]);
				break;
			}
			//����ǿ��ַ�
			else if (this->get_i(i).productionRight[k] == "@")
			{
				predictMap[this->get_i(i)].insert(followMap[this->get_i(i).productionLeft].begin(), followMap[this->get_i(i).productionLeft].end());
				break;
			}
			//��������ս��:�ж��Ƿ����Ƴ��մ�
			set<string> s = firstMap[this->get_i(i).productionRight[k]];
			set<string>::iterator iter = find(s.begin(), s.end(), "@");

			if (iter == s.end())//����ʽ�Ҳ���k���ַ�first���ϲ�����
			{

				predictMap[this->get_i(i)] = firstMap[this->get_i(i).productionRight[k]];
				break;
			}
			else if (iter != s.end()) // ����ʽ�Ҳ���k���ַ�first���Ϻ��գ���������ж�
			{
				s.erase("@");//ɾ�����ַ���
				predictMap[this->get_i(i)].insert(s.begin(), s.end());	//���ϸ��ַ���first����֮������
				k++;
				if (k == this->get_i(i).productionRight.size())
				{
					//������ߵ�follow����
					predictMap[this->get_i(i)].insert(followMap[this->get_i(i).productionLeft].begin(), followMap[this->get_i(i).productionLeft].end());
					break;
				}
			}
		}
	}
}

//--------------------------------------------�·ֽ���-------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------


void productionList::fillLL1Table()
{
	//Ĭ����ȫ������
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
			if (NontmlTypeMap.find(predictMapIter->first.productionLeft) != NontmlTypeMap.end() && LexTypeMap.find(*setIter) != LexTypeMap.end())//���ҵ����Ÿ�ֵ
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
	LexTypeMap.insert({ "'", CHARC});//ʵ���϶�Ӧ����  '  ������ţ����ڴʷ������еĶ�ȡ�ַ�
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
	LexTypeMap.insert({ ":", COLON});//ʵ���϶�Ӧ����  :  ������ţ����ڴʷ������еĶ�ȡ�ַ�,��SNL�����У�û�е���ʹ�ã��������ͨ������::=��:=��
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
