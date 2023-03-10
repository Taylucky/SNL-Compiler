#include <cstdio>
#include <iostream>
#include <set>
#include <map>
#include <fstream>
#include <string>
#include <Windows.h>
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
map<string,set<string>> grammars;
map<string,set<string>> first;
map<string,set<string>> follow;

int isTerminal(string str){
    if(Vt.find(str)!=Vt.end())
        return 1;
    else{
            if(Vn.find(str)!=Vn.end())
                return 0;
            else return -1;}
}
void get_grammars(){
    for(int i=0;i<67;i++){
        string temp;
        getline(in,temp);

            int position=temp.find('::==');
            string s=temp.substr(0,position-2);
            //cout<<s<<endl;
            string remaining=temp.substr(position+1);
            cout<<"*********************"<<endl;
            cout<<remaining<<endl;
            set<string> split;
            string tem="";
            for(int j=0;j<remaining.length();j++){
                if(remaining[j]=='|'){
                        cout<<"!"<<tem<<endl;
                    split.insert(tem);
                    tem="";
                }else{
                    tem.append(remaining.substr(j,1));//&remaining[j]���ԣ�����
                }
            }
            cout<<"#"<<tem<<endl;
            split.insert(tem);
            /*for(set<string>::iterator k=split.begin();k!=split.end();k++)
                cout<<*k<<' ';*/

        grammars.insert(pair<string,set<string>>(s,split));
        //cout<<endl;
    }
    for(map<string, set<string>>::iterator it = grammars.begin(); it != grammars.end(); it++){
        cout <<it->first <<':';
       for(set<string>::iterator k = it->second.begin(); k != it->second.end(); k++)
            cout<<*k<<'/';
        cout<<endl;
    }
}
/*set<string> getFirst(string str){
    if(grammars.count(str)>0){
        set<string> temp=grammars[str];
        for(set<string>::iterator it=temp.begin();it!=temp.end();it++){
            string temp1=*it;
            if(it=='$'){
                first[s].insert('$');
            }else{

                 //�ȷָ�Ϊһ��һ�����ʣ����뼯��splitsen����
                int num=temp1.count(' ');
                set<string> splitsen;
                if(num==0){//��һ���ʣ�û�пո�

                }else{//�кܶ��
                    string stemp;
                    for(int i=0;i<num;i++){
                        int position=temp1.find(' ');
                            stemp=temp1.substr(0,position-1);
                            //cout<<s<<endl;
                            splitsen.insert(stemp);
                            string temp1=temp1.substr(position+1);
                    }
                    //���һ����\n
                    stemp=temp1.substr(0,temp1.length()-1);
                    splitsen.insert(stemp);
                }
            for(set<string>::iterator iit=temp.begin();itt!=temp.end();itt++){
                string temp3=*iit;
                if(!isTerminal(temp3)){//���ռ���
                        set<string>temp2=getFirst(temp3)
                        for (set<string>::iterator ii = temp3.begin(); ii != temp3.end(); ii++) {
                            if(*ii!='$')
                                first[str].insert(*ii);
                            else{
                             if(*iit)//����������һ��չ��ʽ���ռ�����������һ�����ռ�����first��������ǿ�   ������
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
    return first[str];
}*/
//��first����
map<string,set<string>> getallFirst(){

}

set<string> getFollow(){

}
int main(){
    //cout<<isTerminal("RMIDPAREN");
    get_grammars();

}
