#define _CRT_SECURE_NO_WARNINGS
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
string grammarfile = "D:\\Qt\\txt\\grammarfinal.txt";
char predictfile[] = "D:\\Qt\\txt\\predicts.txt";
string grammarfile1 = "D:\\Qt\\txt\\grammarfinal1.txt";
//非终极符
ifstream in(grammarfile);
set<string> Vn{
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

set<string> Vt{
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
//产生式相关结构
typedef struct node {
    int num;//产生式号
    string vn;//左边的非终极符
    vector<string> production;//单词组合
    set<string> firstset;
    set<string> predictset;
}gramm;
gramm sentence[104];

map<string, set<vector<string>>> grammars;
map<string, set<string>> first;//first(x)
map<string, set<string>> follow;
map<int, set<string>> first1;//first(beta)
map<int, set<string>> predict;//每个产生式
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
int main() {
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

    /*set<string> aaaa = follow["fieldvar"];
    for (set<string>::iterator aa = aaaa.begin(); aa != aaaa.end(); aa++) {
        cout << "!!!--" << *aa << "\n";
    }


        cout << "vn"<<sentence[57].vn << endl;
         set<string> it = sentence[57].firstset;
        for (set<string>::iterator k = it.begin(); k != it.end(); k++) {
            cout << "array:" << *k << "  ";
        }
        cout << "oppp" << endl;*/
    getPredict();
    map<string, string> transfer = {
        {":=","ASSIGN"}, {"=","EQ"}, {"<","LT"} ,{"+","PLUS"},
        { "-","SUB"},{"*","TIMES"}, {"/","OVER"}, {"(","LPAREN"}, {")","RPAREN"}, {".","DOT"},{":",""},
        { ";","SEMI"}, {",","COMMA"}, {"[","L"}, {"]",""},
    };
    outputPredict(predictfile);
}

void init() {
    for (set<string>::iterator it = Vn.begin(); it != Vn.end(); it++) {
        follow[*it] = { {} };//初始化吗？？？
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
    //先分割为一个一个单词，放入集合splitsen里面
    int num = count(tem.begin(), tem.end(), ' ');
    //cout<<"number"<<num<<endl;
    vector<string> splitsen;
    if (num == 0) {//就一个词，没有空格

    }
    else {//有很多词
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
    string stemp2 = tem.substr(0, tem.length());//最后一个是\n
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
                tem.append(remaining.substr(j, 1));//&remaining[j]不对？？？
            }
        }
        // cout<<"#"<<tem<<endl;
        vector<string> tempp = Splitsen(tem);
        split.insert(tempp);
        /*for(set<string>::iterator k=split.begin();k!=split.end();k++)
            cout<<*k<<' ';*/

        grammars.insert(pair<string, set<vector<string>>>(s, split));

    }
   /* for (map<string, set<vector<string>>>::iterator it = grammars.begin(); it != grammars.end(); it++) {
        cout << it->first << ':' << endl;
        for (set<vector<string>>::iterator k = it->second.begin(); k != it->second.end(); k++) {
            for (auto k1 = (*k).begin(); k1 != (*k).end(); k1++)
                cout << *k1 << "  ";
            cout << endl;
            }
        cout << endl;
    }*/
}

set<string> getFirst(string str) {

    if (isTerminal(str)) {//终极符
        first[str].insert(str);
    }
    else {
        if (grammars.find(str) == grammars.end())
        {
            return { {} };
        }

        //非终结符
        set<vector<string>> temp = grammars[str];//得到好几条文法集合
        for (set<vector<string>>::iterator it = temp.begin(); it != temp.end(); it++) {//对于每条集合

            vector<string> temp1 = *it;//每个展开的单词集合

            if (*(temp1.begin()) == "$") {
                first[str].insert("$");
            }
            else {
                if (isTerminal(*(temp1.begin())))
                    first[str].insert(*(temp1.begin()));
                else {//第一个元素是非终极符
                      //int flag=0;//凡是可以推出空就+0，退不出空就+1；当flag不为0时就代表该取这个东西的first了
                    set<string>temp2 = getFirst(*(temp1.begin()));
                    temp2 = first[*(temp1.begin())];
                    for (set<string>::iterator i = temp2.begin(); i != temp2.end(); i++) {
                       // cout << (*i)<<endl;
                        if ((*i).compare("$") == 0)
                            // cout<<"空"<<endl;
                            ;
                        else
                            first[str].insert((*i));//插入不是空的，空要再判断
                    }


                    set<string>temp3 = temp2;
                    vector<string>::iterator tem1 = temp1.begin()+1;
                    while (temp3.find("$") != temp3.end()    &&     tem1!=temp1.end()) {
                        temp3 = getFirst(*(tem1));
                        for (set<string>::iterator iii = temp3.begin(); iii != temp3.end(); iii++) {
                           // cout << (*iii) << endl;
                            if ((*iii).compare("$") == 0)
                                //cout << "空" << endl;
                                ;
                            else
                                first[str].insert((*iii));//插入不是空的，空要再判断
                        }

                        tem1++;
                    }

                   /* if (temp3.find("$") == temp2.end() && tem1 != temp1.end()) {//因为不是空跳出
                        ;//什么都不用做，已经都加到first里面了
                    }*/
                    if (temp3.find("$") != temp3.end() && tem1 == temp1.end()) {//因为到最后了所以跳出
                        first[str].insert("$");
                    }


                }

            }

        }

    }
    return first[str];
}
//求first集合
void getallFirst() {

    for (set<string>::iterator it = Vn.begin(); it != Vn.end(); it++) {
        first[*it] = getFirst(*it);

    }
    for (set<string>::iterator it = Vt.begin(); it != Vt.end(); it++) {
        first[*it] = getFirst(*it);
    }

    first["$"] = getFirst("$");
   // cout << grammars.size() << endl;
    for (map<string, set<string>>::iterator it = first.begin(); it != first.end(); it++) {
        cout << endl<<it->first << ':' ;
        for (set<string>::iterator k = it->second.begin(); k != it->second.end(); k++) {
            cout << *k << "  ";

        }
        //cout << endl;
    }
}

void getFollow(){
    for(map<string, set<vector<string>>>::iterator it = grammars.begin(); it != grammars.end(); it++){//对于每个左部非终极符
        //cout << "1" << " ";
        string left=it->first;


        set<vector<string>> right=it->second;
        for(set<vector<string>>::iterator ii=right.begin();ii!=right.end();ii++){
            //对于每条产生式
            vector<string> temp=*ii;
            for(vector<string>::iterator iit=temp.begin();iit!=temp.end();iit++){
             //每个单词
                if (left == "VariMore" && testnum == 2 && (*iit)== "FieldVar") {

                    cout << "left: " << left << " ";
                }
               if(isTerminal(*iit)||(*iit)=="$") {//是终极符就跳过，只有非终极符有follow集
                continue;
               }else{

                   vector<string>::iterator tem1 = iit + 1;
                   while (1){
                       if (tem1 != temp.end()) {
                           if (first[*tem1].find("$") != first[*tem1].end()) {//下一个字符存在 且first含有空
                               for (set<string>::iterator i = first[*tem1].begin(); i != first[*tem1].end(); i++) {
                                   if ((*i) != "$")
                                       follow[(*iit)].insert((*i));//插入不是空的，空要再判断
                               }
                           }
                           else {
                               for (set<string>::iterator i = first[*tem1].begin(); i != first[*tem1].end(); i++) {
                                       follow[(*iit)].insert((*i));//插入不是空的，空要再判断
                               }
                               break;
                           }
                               tem1++;
                               continue;
                       }
                       else {


                           for (set<string>::iterator i = follow[left].begin(); i != follow[left].end(); i++) {
                               //cout <<"左"<< * i << endl;
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

        for (int i = 0; i < 104; i++) {//对于每个终结符式子
            string temp;
            getline(in, temp);
            int position = temp.find('::==');
            string s = temp.substr(0, position - 2);//左边终极式子
            string remaining = temp.substr(position + 1);
            sentence[i].production = Splitsen(remaining);
            sentence[i].num = i + 1;
            sentence[i].vn = s;
        }
        for (int i=0; i < 104; i++) {//对于每条文法

            vector<string>::iterator tem=sentence[i].production.begin();//第一个单词
            set<string> temfirst = first[(*tem)];
            while (tem!= sentence[i].production.end()) {//
                temfirst = first[*tem];
                for (set<string>::iterator ii = temfirst.begin(); ii != temfirst.end(); ii++) {
                    if ((*ii) != "$")
                        sentence[i].firstset.insert(*ii);
                }
                    if (temfirst.find("$") != temfirst.end()) {//含有空
                        tem++;
                    }
                    else break;
                }
            if(tem == sentence[i].production.end()&& temfirst.find("$")!=temfirst.end())//最后一个也可以为空
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

        if (sentence[i].firstset.find("$") == sentence[i].firstset.end()) {//first不含空集
            //sentence[i].predictset = sentence[i].firstset;不可以直接赋值吗
            for (set<string>::iterator it = sentence[i].firstset.begin(); it != sentence[i].firstset.end(); it++) {
                sentence[i].predictset.insert(*it);
            }
        }
        else {
            //含有空集合，predict集合为follow集合+不含空的first集合
            for (set<string>::iterator it = sentence[i].firstset.begin(); it != sentence[i].firstset.end(); it++) {
                if ((*it) != "$")
                    sentence[i].predictset.insert(*it);
            }//first除了空都复制进去
            for (set<string>::iterator it = follow[sentence[i].vn].begin(); it != follow[sentence[i].vn].end(); it++)
                sentence[i].predictset.insert(*it);
            //follow也都加进去
        }
    }

    for (int i = 0; i < 104; i++) {
        cout << sentence[i].num << " "<< sentence[i].vn << "        ";
        for (set<string>::iterator ii = sentence[i].predictset.begin(); ii != sentence[i].predictset.end(); ii++)
            cout << *ii << "  ";
        cout << endl;
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
