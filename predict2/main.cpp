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
//�����ļ�
string grammarfile="D:\\����\\������\\���޿�\\����ԭ�����\\grammarfinal.txt";
string predictfile="D:\\����\\������\\���޿�\\����ԭ�����\\predicts.txt";
//���ռ���
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
map<int,set<string>> predict;//ÿ������ʽ
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
    //�ȷָ�Ϊһ��һ�����ʣ����뼯��splitsen����
    int num=count(tem.begin(),tem.end(),' ');
    //cout<<"number"<<num<<endl;
    vector<string> splitsen;
    if(num==0){//��һ���ʣ�û�пո�

    }else{//�кܶ��
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
    string stemp2=tem.substr(0,tem.length());//���һ����\n
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
                    tem.append(remaining.substr(j,1));//&remaining[j]���ԣ�����
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
    if(isTerminal(str)){//�ռ���
        first[str].insert(str);
    }else{//���ս��
    set<vector<string>> temp=grammars[str];//�õ��ü����ķ�����
    for(set<vector<string>>::iterator it=temp.begin();it!=temp.end();it++){//����ÿ������
      vector<string> temp1=*it;//ÿ��չ���ĵ��ʼ���
      if(*(temp1.begin())=="$"){
        first[str].insert("$");
      }else{
          if(isTerminal(*(temp1.begin())))
            first[str].insert(*(temp1.begin()));
          else{//��һ��Ԫ���Ƿ��ռ���
                //int flag=0;//���ǿ����Ƴ��վ�+0���˲����վ�+1����flag��Ϊ0ʱ�ʹ����ȡ���������first��
                 set<string>temp2=getFirst(*(temp1.begin()));
                  for(set<string>::iterator i=temp2.begin();i!=temp2.end();i++){
                    if((*i)!="$") first[str].insert((*i));//���벻�ǿյģ���Ҫ���ж�
                  }

                if(temp2.find("$")==temp2.end()){//��һ��Ԫ�ط��ռ��������Ƴ���
                   //flag+=0��
                   if(temp1.begin()!=temp1.end())//�ǲ������һ����
                        getFirst(*(temp1.begin()+1));//����һ�����ռ���
                    else first[str].insert("$");//���һ������Ϊ�յģ�˵��ǰ�涼Ϊ�գ���Ȼ���ᵽ��һ��
                }

          }

      }

    }

}
    return first[str];
}
//��first����
map<string,set<string>> getallFirst(){


}

/*set<string> getFollow(){
    for(map<string, set<vector<string>>>::iterator it = grammars.begin(); it != grammars.end(); it++){
        string left=it->first;
        set<vector<string>> right=it->second;
        for(set<vector<string>>::iterator ii=right.begin();ii!=right.end();ii++){
            //����ÿ������ʽ
            vector<string> temp=*ii;
            for(vector<string>::iterator iit=temp.begin();iit!=temp.end();iit++){
             //ÿ������
               if(isTerminal(*iit)){//���ռ�����������ֻ�з��ռ�����follow��
                continue;
               }else{
                if((iit+1)!=temp.end()){//��һ������

                        string temp2=*(iit+1);
                        if(first[temp2].find('$')){//�п�
                             for(set<string>::iterator i=first[temp2].begin();i!=first[temp2].end();i++){
                                if((*i)!='$') follow[(*iit)].insert((*i));//���벻�ǿյģ���Ҫ���ж�
                                }

                                //�����һ����follow����
                                follow[(*iit)].insert(follow[*(iit+1)]);//��ʼ������followΪ�գ��е�follow���϶��������������м������������������
                        }else{
                            follow[(*iit)].insert(first[temp2]);
                        }



                   }else{//���治���ڣ���left��follow��

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
