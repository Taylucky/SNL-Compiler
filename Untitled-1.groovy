if(grammars.count(str)>0){
        set<vector<string>> temp=grammars[str];
        for(set<vector<string>>::iterator it=temp.begin();it!=temp.end();it++){
            vector<string> temp1=*it;
            if(*(temp1.begin()）=='$'){
                first[s].insert('$');
            }else{

                 //先分割为一个一个单词，放入集合splitsen里面
            for(set<string>::iterator iit=temp.begin();itt!=temp.end();itt++){
                string temp3=*iit;
                if(!isTerminal(temp3)){//非终极符
                        set<string>temp2=getFirst(temp3)
                        for (set<string>::iterator ii = temp3.begin(); ii != temp3.end(); ii++) {
                            if(*ii!='$')
                                first[str].insert(*ii);
                            else{
                             if(*iit)//如果不是最后一个展开式非终极符，就求下一个非终极符的first，否则就是空   ？？？
                            }
                        }
                    }else{
                        first[s].insert(stemp);
                    }
                }

            }
        }
        return first[str];
    }

    void FF::getFollow() {
	for (map<string, set<string>>::iterator it = split_productions.begin(); it != split_productions.end(); it++) {
		string left = it->first;
		set<string>right = it->second;
		for (set<string>::iterator ii = right.begin(); ii != right.end(); ii++) {
			string temp = *ii;
			
			for (int i = 0; i < temp.length(); i++) {
				if (isVt(temp.substr(i, 1))) {//终结符
					continue;
				}
				else {//不带’的非终结符
					
					if (i+1<temp.length() && isVt(temp.substr(i + 1, 1))) {//非终结符后面是终结符
						follow[temp.substr(i, 1)].insert(temp.substr(i + 1, 1));
						i++;
					}
					else {//非终结符后面是非终结符s
						//把后面非终结符的first集ff加入follow集中
						string s;
						
						s = temp.substr(i + 1, 1);
						
						set<string> ff = first[s];
						for (set<string>::iterator nn = ff.begin(); nn != ff.end(); nn++) {
							if (*nn != "#")
								follow[temp.substr(i, 1)].insert(*nn);
						}
					}
				}
			}
		}
	}
	//这一个需要多进行几次，因为follow是不断增长的
	SSS();
	SSS();

	cout << "Follow集" << endl;
	for (map<string, set<string>>::iterator it = follow.begin(); it != follow.end(); it++) {
		cout << it->first << "  :  ";
		for (set<string>::iterator ii = it->second.begin(); ii != it->second.end(); ii++)
		{
			cout << *ii << "    ";
		}
		cout << endl;
	}
}
