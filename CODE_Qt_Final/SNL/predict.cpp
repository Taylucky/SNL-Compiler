#define _CRT_SECURE_NO_WARNINGS
#include "predict.h"
#include"parseanalyse.h"
#include <cstdio>
#include <stdio.h>
#include <iostream>
#include <set>
#include <map>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

string grammarfile = "D:/CODE_Qt/txt/UI/grammarfinal.txt";
char predictfile[] = "D:/CODE_Qt/txt/UI/predicts.txt";
string grammarfile1 = "D:/CODE_Qt/txt/UI/grammarfinal1.txt";

ifstream in(grammarfile);
gramm sentence[104];

set<TokenType> Predict[ProductExpressionNum+5];
static bool isInitPredict=false;
/////
static set<string> Vn{
    "Program", "ProgramHead", "ProgramName", "DeclarePart",
    "TypeDec", "TypeDeclaration", "TypeDecList", "TypeDecMore",
    "TypeId", "TypeName", "BaseType", "StructureType",
    "ArrayType", "Low", "Top", "RecType",
    "FieldDecList", "FieldDecMore", "IdList", "IdMore",
    "VarDec","VarDeclaration","VarDecList", "VarDecMore",
    "VarIdList", "VarIdMore", "ProcDec", "ProcDeclaration",
    "ProcDecMore", "ProcName","ParamList", "ParamDecList",
    "ParamMore", "Param", "FormList", "FidMore",
    "ProcDecPart", "ProcBody","ProgramBody", "StmList",
    "StmMore", "Stm","AssCall","AssignmentRest",
    "ConditionalStm", "StmL", "LoopStm", "InputStm",
    "InVar", "OutputStm", "ReturnStm", "CallStmRest",
    "ActParamList", "ActParamMore", "RelExp", "OtherRelE",
    "Exp", "OtherTerm", "Term", "OtherFactor",
    "Factor","Variable", "VariMore", "FieldVar",
    "FieldVarMore", "CmpOp", "AddOp", "MultOp" };

/*set<string> Vt{
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
    "UNDERANGErf" };*/

static set<string> Vt{
    "ENDFILE", "ERROR",
    "PROGRAM", "PROCEDURE", "TYPE", "VAR", "IF",
    "THEN", "ELSE", "FI", "WHILE", "DO", "ENDWH",
    "BEGIN", "END","READ", "WRITE", "ARRAY[low..top]", "OF",
    "RECORD", "RETURN",
    "INTEGER", "CHAR",
    "ID", "INTC", "CHARC",
    ":=","=", "<", "+", "-",
    "*", "/", "(", ")", ".",
    ":", ";", ",", "[", "]",
    "UNDERANGErf" };
//����ʽ���ؽṹ

map<string, set<vector<string>>> grammars;
map<string, set<string>> first;//first(x)
map<string, set<string>> follow;
map<int, set<string>> first1;//first(beta)
map<int, set<string>> predict;//ÿ������ʽ
set<string> getFirst(string str);
void getallFirst();
void get_grammars();
vector<string> Splitsen(string tem);
void getFollow();
void getFirst1();
void getPredict();
void init();
int testnum = 0;
void outputPredict(const char predictFileName[]);


void  getAllPredict() {
    //cout<<isTerminal("RMIDPAREN");
    get_grammars();
    getallFirst();
    //init();
    getFollow();
    getFollow();
    getFollow();
    getFollow();
    getFollow();
    getFollow();
    getFollow();
    getFirst1();

    getPredict();
    map<string, string> transfer = {
        {":=","ASSIGN"}, {"=","EQ"}, {"<","LT"} ,{"+","PLUS"},
        { "-","SUB"},{"*","TIMES"}, {"/","OVER"}, {"(","LPAREN"}, {")","RPAREN"}, {".","DOT"},{":",""},
        { ";","SEMI"}, {",","COMMA"}, {"[","L"}, {"]",""},
    };
    outputPredict(predictfile);

    if(isInitPredict) return ;
    isInitPredict=true;

    for (int i = 0; i < 104; i++) {
            for (set<string>::iterator ii = sentence[i].predictset.begin(); ii != sentence[i].predictset.end(); ii++){
                cout<<(*ii).c_str()<<endl;
               Predict[i+1].insert(strToToken[(*ii)]);}
     }
}

void init() {
    for (set<string>::iterator it = Vn.begin(); it != Vn.end(); it++) {
        follow[*it] = { {} };
    }
}

int isTerminal(string str) {
    if (Vt.find(str) != Vt.end())
        return 1;
    else {
        if (Vn.find(str) != Vn.end())
            return 0;
        else return -1;
    }
}
vector<string> Splitsen(string tem) {
    int num = count(tem.begin(), tem.end(), ' ');
    //cout<<"number"<<num<<endl;
    vector<string> splitsen;
    if (num == 0) {

    }
    else {
        string stemp;
        for (int i = 0; i < num; i++) {
            int position = tem.find(' ');
            stemp = tem.substr(0, position);
            //cout<<"stemp:"<<stemp<<endl;
            splitsen.insert(splitsen.end(), stemp);
            tem = tem.substr(position + 1);
            //cout<<"tem:"<<tem<<endl;
        }

    }
    string stemp2 = tem.substr(0, tem.length());
    splitsen.insert(splitsen.end(), stemp2);

    return splitsen;
}

void get_grammars() {
    for (int i = 0; i < 67; i++) {
        string temp;
        getline(in, temp);

        int position = temp.find('::==');
        string s = temp.substr(0, position - 2);
        //cout<<s<<endl;
        string remaining = temp.substr(position + 1);
        //cout<<"*********************"<<endl;
        //cout<<remaining<<endl;
        set<vector<string>> split;
        string tem = "";
        for (int j = 0; j < remaining.length(); j++) {
            if (remaining[j] == '|') {
                vector<string> tempp = Splitsen(tem);

                split.insert(tempp);
                tem = "";
            }
            else {
                tem.append(remaining.substr(j, 1));
            }
        }
        // cout<<"#"<<tem<<endl;
        vector<string> tempp = Splitsen(tem);
        split.insert(tempp);
        grammars.insert(pair<string, set<vector<string>>>(s, split));
    }
}

set<string> getFirst(string str) {

    if (isTerminal(str)) {
        first[str].insert(str);
    }
    else {
        if (grammars.find(str) == grammars.end())
        {
            return { {} };
        }

        set<vector<string>> temp = grammars[str];
        for (set<vector<string>>::iterator it = temp.begin(); it != temp.end(); it++) {

            vector<string> temp1 = *it;

            if (*(temp1.begin()) == "$") {
                first[str].insert("$");
            }
            else {
                if (isTerminal(*(temp1.begin())))
                    first[str].insert(*(temp1.begin()));
                else {
                    set<string>temp2 = getFirst(*(temp1.begin()));
                    temp2 = first[*(temp1.begin())];
                    for (set<string>::iterator i = temp2.begin(); i != temp2.end(); i++) {
                       // cout << (*i)<<endl;
                        if ((*i).compare("$") == 0)
                            // cout<<"��"<<endl;
                            ;
                        else
                            first[str].insert((*i));
                    }


                    set<string>temp3 = temp2;
                    vector<string>::iterator tem1 = temp1.begin()+1;
                    while (temp3.find("$") != temp3.end()    &&     tem1!=temp1.end()) {
                        temp3 = getFirst(*(tem1));
                        for (set<string>::iterator iii = temp3.begin(); iii != temp3.end(); iii++) {
                           // cout << (*iii) << endl;
                            if ((*iii).compare("$") == 0)
                                //cout << "��" << endl;
                                ;
                            else
                                first[str].insert((*iii));
                        }

                        tem1++;
                    }
                    if (temp3.find("$") != temp3.end() && tem1 == temp1.end()) {//��Ϊ����������������
                        first[str].insert("$");
                    }
                }

            }

        }

    }
    return first[str];
}
//��first����
void getallFirst() {

    for (set<string>::iterator it = Vn.begin(); it != Vn.end(); it++) {
        first[*it] = getFirst(*it);

    }
    for (set<string>::iterator it = Vt.begin(); it != Vt.end(); it++) {
        first[*it] = getFirst(*it);
    }

    first["$"] = getFirst("$");
}

void getFollow(){
    for(map<string, set<vector<string>>>::iterator it = grammars.begin(); it != grammars.end(); it++){//����ÿ���󲿷��ռ���
        //cout << "1" << " ";
        string left=it->first;


        set<vector<string>> right=it->second;
        for(set<vector<string>>::iterator ii=right.begin();ii!=right.end();ii++){
            //����ÿ������ʽ
            vector<string> temp=*ii;
            for(vector<string>::iterator iit=temp.begin();iit!=temp.end();iit++){
             //ÿ������
                if (left == "VariMore" && testnum == 2 && (*iit)== "FieldVar") {

                    cout << "left: " << left << " ";
                }
               if(isTerminal(*iit)||(*iit)=="$") {
                continue;
               }else{

                   vector<string>::iterator tem1 = iit + 1;
                   while (1){
                       if (tem1 != temp.end()) {
                           if (first[*tem1].find("$") != first[*tem1].end()) {//��һ���ַ����� ��first���п�
                               for (set<string>::iterator i = first[*tem1].begin(); i != first[*tem1].end(); i++) {
                                   if ((*i) != "$")
                                       follow[(*iit)].insert((*i));//���벻�ǿյģ���Ҫ���ж�
                               }
                           }
                           else {
                               for (set<string>::iterator i = first[*tem1].begin(); i != first[*tem1].end(); i++) {
                                       follow[(*iit)].insert((*i));//���벻�ǿյģ���Ҫ���ж�
                               }
                               break;
                           }
                               tem1++;
                               continue;
                       }
                       else {


                           for (set<string>::iterator i = follow[left].begin(); i != follow[left].end(); i++) {
                               //cout <<"��"<< * i << endl;
                              follow[(*iit)].insert(*i);

                           } break;

                       }
                   }
                }
            }
        }
    }


}

void getFirst1() {
        ifstream in(grammarfile1);

        for (int i = 0; i < 104; i++) {//����ÿ���ս���ʽ��
            string temp;
            getline(in, temp);
            int position = temp.find('::==');
            string s = temp.substr(0, position - 2);//�����ռ�ʽ��
            string remaining = temp.substr(position + 1);
            sentence[i].production = Splitsen(remaining);
            sentence[i].num = i + 1;
            sentence[i].vn = s;
        }
        for (int i=0; i < 104; i++) {//����ÿ���ķ�

            vector<string>::iterator tem=sentence[i].production.begin();//��һ������
            set<string> temfirst = first[(*tem)];
            while (tem!= sentence[i].production.end()) {//
                temfirst = first[*tem];
                for (set<string>::iterator ii = temfirst.begin(); ii != temfirst.end(); ii++) {
                    if ((*ii) != "$")
                        sentence[i].firstset.insert(*ii);
                }
                    if (temfirst.find("$") != temfirst.end()) {//���п�
                        tem++;
                    }
                    else break;
                }
            if(tem == sentence[i].production.end()&& temfirst.find("$")!=temfirst.end())//����һ��Ҳ����Ϊ��
                sentence[i].firstset.insert("$");
            }

       /*for (int i = 0; i < 40; i++) {
            //cout << sentence[i].num << " " << sentence[i].vn << ":";
            for (set<string>::iterator ii = sentence[i].firstset.begin(); ii != sentence[i].firstset.end(); ii++)
                cout << *ii << "  ";
            cout << endl;
        }*/

        }


void getPredict(){

    for (int i = 0; i < 104; i++) {
        /*if (i = 73) {
            set<string> aaaa = follow["ActParamMore"];
            for (set<string>::iterator aa = aaaa.begin(); aa != aaaa.end(); aa++) {
                cout << *aa << " ";
             }
            cout << sentence[27].vn << "!" << endl;
        }*/

        if (sentence[i].firstset.find("$") == sentence[i].firstset.end()) {//first�����ռ�
            //sentence[i].predictset = sentence[i].firstset;������ֱ�Ӹ�ֵ��
            for (set<string>::iterator it = sentence[i].firstset.begin(); it != sentence[i].firstset.end(); it++) {
                sentence[i].predictset.insert(*it);
            }
        }
        else {
            //���пռ��ϣ�predict����Ϊfollow����+�����յ�first����
            for (set<string>::iterator it = sentence[i].firstset.begin(); it != sentence[i].firstset.end(); it++) {
                if ((*it) != "$")
                    sentence[i].predictset.insert(*it);
            }//first���˿ն����ƽ�ȥ
            for (set<string>::iterator it = follow[sentence[i].vn].begin(); it != follow[sentence[i].vn].end(); it++)
                sentence[i].predictset.insert(*it);
            //followҲ���ӽ�ȥ
        }
    }
}
void outputPredict(const char predictFileName[]) {
    FILE* fn = fopen(predictFileName, "w");
    fprintf(fn, "Number\tTokenType\tPredicts\n");
    for (int i = 0; i < 104; i++) {
        fprintf(fn,"%d\t%s  \t", sentence[i].num, sentence[i].vn.c_str());
        for (set<string>::iterator ii = sentence[i].predictset.begin(); ii != sentence[i].predictset.end(); ii++)
            fprintf(fn, "%s\t", ( * ii).c_str());
        fprintf(fn, "\n");

    }
    fclose(fn);
}
