#include <cstdio>
#include <stdio.h>
#include <iostream>
#include <set>
#include <map>
#include <fstream>
#include <string>
#include <Windows.h>
#include <vector>
#include <algorithm>
using namespace std;
//输入文件
string grammarfile="D:\\月月\\大三下\\必修课\\编译原理课设\\grammarfinal.txt";
string predictfile="D:\\月月\\大三下\\必修课\\编译原理课设\\predicts.txt";
//非终极符
ifstream in(grammarfile);

set<string> Vn{
    "Program", "ProgramHead", "ProgramName", "DeclarePart",
	"TypeDec", "TypeDeclaration", "TypeDecList", "TypeDecMore",
	"TypeId", "TypeName", "BaseType", "StructureType",
	"ArrayType", "Low", "Top", "RecType",
	"FieldDecList", "FieldDecMore", "IdList", "IdMore",
	"VarDec","VarDeclaration"," VarDecList", "VarDecMore",
	"VarIdList", "VarIdMore", "ProcDec", "ProcDeclaration",
	"ProcDecMore, ProcName, ParamList", "ParamDecList",
	"ParamMore, Param, FormList", "FidMore",
	"ProcDecPart, ProcBody","ProgramBody", "StmList",
	"StmMore", "Stm","AssCall","AssignmentRest",
	"ConditionalStm", "StmL", "LoopStm", "InputStm",
	"InVar", "OutputStm", "ReturnStm", "CallStmRest",
	"ActParamList", "ActParamMore", "RelExp", "OtherRelE",
	"Exp", "OtherTerm", "Term", "OtherFactor",
	"Factor","Variable", "VariMore", "FieldVar",
	"FieldVarMore", "CmpOp", "AddOp", "MultOp"};

set<string> Vt{
    "ENDFILE", "ERROR",
	"PROGRAM", "PROCEDURE", "TYPE", "VAR", "IF",
	"THEN", "ELSE", "FI", "WHILE", "DO", "ENDWH",
	"BEGIN", "END1","READ", "WRITE", "ARRAY", "OF",
	"RECORD", "RETURN1",
	"INTEGER", "CHAR1",
	"ID", "INTC", "CHARC",
	"ASSIGN","EQ", "LT", "PLUS", "MINUS",
	"TIMES", "OVER", "LPAREN", "RPAREN", "DOT",
	"COLON", "SEMI", "COMMA", "LMIDPAREN", "RMIDPAREN",
	"UNDERANGErf"};
map<string,set<vector<string>>> grammars;
map<string,set<string>> first;//first(x)
map<string,set<string>> follow;
map<int,set<string>> first1;//first(beta)
map<int,set<string>> predict;//每个产生式
set<string> getFirst(string str);
void get_grammars();
vector<string> Splitsen(string tem);

int main(){
    //cout<<isTerminal("RMIDPAREN");
    get_grammars();
    set<string> re=getFirst("Program");
    for(set<string>::iterator i=re.begin();i!=re.end();i++){
        cout<<"!"<<*i<<endl;
    }

}


void init(){

}

int isTerminal(string str){
    if(Vt.find(str)!=Vt.end())
        return 1;
    else{
            if(Vn.find(str)!=Vn.end())
                return 0;
            else return -1;}
}
vector<string> Splitsen(string tem){
    //先分割为一个一个单词，放入集合splitsen里面
    int num=count(tem.begin(),tem.end(),' ');
    //cout<<"number"<<num<<endl;
    vector<string> splitsen;
    if(num==0){//就一个词，没有空格

    }else{//有很多词
        string stemp;
        for(int i=0;i<num;i++){
            int position=tem.find(' ');
                stemp=tem.substr(0,position);
                //cout<<"stemp:"<<stemp<<endl;
                splitsen.insert(splitsen.end(),stemp);
                tem=tem.substr(position+1);
                //cout<<"tem:"<<tem<<endl;
        }

    }
    string stemp2=tem.substr(0,tem.length());//最后一个是\n
    splitsen.insert(splitsen.end(),stemp2);

    return splitsen;
}

void get_grammars(){
    for(int i=0;i<67;i++){
        string temp;
        getline(in,temp);

            int position=temp.find('::==');
            string s=temp.substr(0,position-2);
            //cout<<s<<endl;
            string remaining=temp.substr(position+1);
            //cout<<"*********************"<<endl;
            //cout<<remaining<<endl;
            set<vector<string>> split;
            string tem="";
            for(int j=0;j<remaining.length();j++){
                if(remaining[j]=='|'){
                    vector<string> tempp=Splitsen(tem);
                    //cout<<"!"<<tem<<endl;
                    split.insert(tempp);
                    tem="";
                }else{
                    tem.append(remaining.substr(j,1));//&remaining[j]不对？？？
                }
            }
           // cout<<"#"<<tem<<endl;
            vector<string> tempp=Splitsen(tem);
            split.insert(tempp);
            /*for(set<string>::iterator k=split.begin();k!=split.end();k++)
                cout<<*k<<' ';*/

        grammars.insert(pair<string,set<vector<string>>>(s,split));

    }
    for(map<string, set<vector<string>>>::iterator it = grammars.begin(); it != grammars.end(); it++){
        cout <<it->first <<':'<<endl;
       for(set<vector<string>>::iterator k = it->second.begin(); k != it->second.end(); k++){
             for(auto k1 = (*k).begin(); k1!=(*k).end(); k1++)
               cout<<*k1<<"  ";
            cout<<endl;
       }
        cout<<endl;
    }
}

set<string> getFirst(string str){
    if(isTerminal(str)){//终极符
        first[str].insert(str);
    }else{//非终结符
    set<vector<string>> temp=grammars[str];//得到好几条文法集合
    for(set<vector<string>>::iterator it=temp.begin();it!=temp.end();it++){//对于每条集合
      vector<string> temp1=*it;//每个展开的单词集合
      if(*(temp1.begin())=="$"){
        first[str].insert("$");
      }else{
          if(isTerminal(*(temp1.begin())))
            first[str].insert(*(temp1.begin()));
          else{//第一个元素是非终极符
                //int flag=0;//凡是可以推出空就+0，退不出空就+1；当flag不为0时就代表该取这个东西的first了
                 set<string>temp2=getFirst(*(temp1.begin()));
                  for(set<string>::iterator i=temp2.begin();i!=temp2.end();i++){
                    if((*i)!="$") first[str].insert((*i));//插入不是空的，空要再判断
                  }

                if(temp2.find("$")==temp2.end()){//第一个元素非终极符可以推出空
                   //flag+=0；
                   if(temp1.begin()!=temp1.end())//是不是最后一个了
                        getFirst(*(temp1.begin()+1));//看下一个非终极符
                    else first[str].insert("$");//最后一个可以为空的，说明前面都为空，不然不会到这一步
                }

          }

      }

    }

}
    return first[str];
}
//求first集合
map<string,set<string>> getallFirst(){


}

/*set<string> getFollow(){
    for(map<string, set<vector<string>>>::iterator it = grammars.begin(); it != grammars.end(); it++){
        string left=it->first;
        set<vector<string>> right=it->second;
        for(set<vector<string>>::iterator ii=right.begin();ii!=right.end();ii++){
            //对于每条产生式
            vector<string> temp=*ii;
            for(vector<string>::iterator iit=temp.begin();iit!=temp.end();iit++){
             //每个单词
               if(isTerminal(*iit)){//是终极符就跳过，只有非终极符有follow集
                continue;
               }else{
                if((iit+1)!=temp.end()){//后一个存在

                        string temp2=*(iit+1);
                        if(first[temp2].find('$')){//有空
                             for(set<string>::iterator i=first[temp2].begin();i!=first[temp2].end();i++){
                                if((*i)!='$') follow[(*iit)].insert((*i));//插入不是空的，空要再判断
                                }

                                //插入后一个的follow集合
                                follow[(*iit)].insert(follow[*(iit+1)]);//初始化所有follow为空，有的follow集肯定不完整，多运行几次这个函数就收敛了
                        }else{
                            follow[(*iit)].insert(first[temp2]);
                        }



                   }else{//后面不存在，是left的follow集

                     follow[(*iit)].insert(follow[*(iit+1)]);

                   }
               }
            }
        }
    }
}
*/
/*void getFirst1(int i){
       for(map<string, set<vector<string>>>::iterator it = grammars.begin(); it != grammars.end(); it++){

       }


}*/
/*void getPredict(int i){
    if(first1[i].find('$')!+first1[i].end()){
       for()
            predict[i].insert(first[i]);

    }else{

    }

}*/
