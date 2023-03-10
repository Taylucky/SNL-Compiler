// 输入 : 源码文件
// 输出 : Token序列  & 词法错误
#include"lexerAnalyse.h"
#include <stdlib.h>
#include <vector>
#include <map>
#include <string>
using namespace std;

#define MAXIDLEN  32  // 标识符的最大长度
#define MAXERRORLEN 1024  // 错误信息的最大长度

// 自动机状态
typedef enum State   // 识别 SNL 单词的 DFA 状态
{
    Start,InID,InNum,Done,InAssign,     // 入口, 标识符状态, 数字状态, 完成状态, 赋值状态, 
    InComment,InRange,InChar,SError,  // 注释状态, 数组下标界限状态, 字符标志状态, 出错状态
    SFileEnd,Finished,   // 程序结束标志 , 出口
} State;

static int lineNum;  // 对应源文件中的行号
static int tmpLn; // 记录当前字符的行号,凡是考虑这个符号后面发生的错误,都要记录这个符号的行号 ,如 { 
static FILE* sourceFile = NULL;  // 指向源代码文件的文件指针

static vector<Token> tokens; // 存储所有的 token
static vector<string> errors; // 存储所有的词法错误
static char tmpStrError[MAXERRORLEN];  //暂存出错信息
static char preChar;  // 预读的下一个字符 (当需要预读时如用, .. 和 :=)
static char nowChar;  // 当前处理的字符
static State status;  // 自动机状态 

// 从源文件中读一个字符
char readOneCharFromSource(){
    char c = getc(sourceFile);
    if(c == '\n')   lineNum ++;  // 这里是唯一会改变 lineNum 的地方,避免因为 lineNum 而引发的错误
    return c;
}


// ------------------------------------- 用于识别的函数 -------------------------------------------------
Token isINTC(){
    char ch[MAXIDLEN];  //todo:  string 
    int n = 0;
    Token tk(lineNum);
    while(isdigit(nowChar))
    {
        ch[n++] = nowChar;
        if(n == MAXIDLEN)
        {
            sprintf(tmpStrError,"INTC is too long  , at line: %d\n",lineNum);
            status = SError;
            return Token(lineNum,"INTC_IS_TOO_LONG",ERROR);
        }
        nowChar = readOneCharFromSource();
    }
    ch[n] = '\0';
    tk.name = string(ch);
    tk.type = INTC;
    status = Done;
    if(isalpha(nowChar))  //todo: 数字后面可能还有其他的不能在数字后面出现的字符  如 [] () 等 ,先只考虑字母
    {
        status = SError;
        sprintf(tmpStrError,"Expected punctuation or blank after '%c',at line %d\n",ch[n-1],lineNum);
    }
    return tk;
}


// ------------ 保留字查找表 ------------
static map<string,TokenType> tokenMap; // 保留字查找表
static bool setTokenMap=false;
TokenType judgeID(string str){   
    if(!setTokenMap)
    {
        tokenMap["program"]=PROGRAM;
        tokenMap["type"]=TYPE;
        tokenMap["var"]=VAR;
        tokenMap["procedure"]=PROCEDURE;
        tokenMap["begin"]=BEGIN;
        tokenMap["end"]=END;
        tokenMap["array"]=ARRAY; 
        tokenMap["of"]=OF;
        tokenMap["record"]=RECORD;
        tokenMap["if"]=IF;
        tokenMap["else"]=ELSE;
        tokenMap["then"]=THEN;
        tokenMap["fi"]=FI;
        tokenMap["while"]=WHILE;
        tokenMap["do"]=DO;
        tokenMap["endwh"]=ENDWH;
        tokenMap["read"]=READ;
        tokenMap["write"]=WRITE;
        tokenMap["return"]=RETURN;
        tokenMap["integer"]=INTEGER;
        tokenMap["char"]=CHAR;
        setTokenMap=true;
    }
    if(tokenMap.find(str) != tokenMap.end()) return tokenMap[str];   // 能找到说明是保留字
    return ID;   // 没找到说明是标识符
}

Token isIDorReserved(){
    char ch[MAXIDLEN];  //todo:  string
    int n = 0;
    Token tk(lineNum);
    while(isalnum(nowChar)) // 字母和数字
    {
        ch[n++] = nowChar;
        if(n == MAXIDLEN)
        {
            sprintf(tmpStrError,"ID is too long  , at line: %d\n",lineNum); 
            status = SError;
            return Token(lineNum,"ID_IS_TOO_LONG",ERROR);
        }
        nowChar = readOneCharFromSource();
    }
    ch[n] = '\0';
    tk.name = string(ch);
    tk.type = judgeID(tk.name); //判断是保留字还是标识符
    status = State::Done;
    return tk;
}

void isComment(){
    tmpLn = lineNum;  // 记录 { 所在的行号
    while(nowChar != '}')
    {
        nowChar = readOneCharFromSource();
        if(nowChar == EOF)
        {
            status = SError;
            sprintf(tmpStrError,"{ excepted } ,at line :%d",tmpLn);
            return;
        }
    }
    nowChar = readOneCharFromSource();
    status = Done;
}


// -------------- 单个字符查找表 -------------------
static bool singleChar[256]= {0};
void initSingleChar(){
        singleChar['+'] = singleChar['-'] = singleChar['*'] = singleChar['/'] = true;
        singleChar['('] = singleChar[')'] = singleChar[';'] = singleChar['['] = true;
        singleChar[']'] = singleChar['='] = singleChar['<'] = singleChar['\n'] = true;
        singleChar['\t'] = singleChar[' '] = singleChar['\r'] = true;
        singleChar['\f'] = singleChar['\v'] = true;
    return;
}

bool isSingleChar(char c){
    return singleChar[c];
}

// -------------------------------- 词法分析 DFA直接转向法 ------------------------------------------
bool lexerAnalyse(const char sourceFilePath[]){
    initSingleChar();
    sourceFile = fopen(sourceFilePath, "r");
    if(!sourceFile)   //文件打开失败
    {
        errors.push_back(string("File open failed. Sure you've given a right file name.\n"));
        return false;
    }
    // 初始化
    lineNum = 1;
    tmpStrError[0]='\0';
    tokens.clear(); 
    errors.clear();
    nowChar = readOneCharFromSource();
    status = Start;

    while(status != SFileEnd){   
        status = Start;
        if(isdigit(nowChar))
            tokens.push_back(isINTC());  // 识别 INTC 
        else if(isalpha(nowChar))
            tokens.push_back(isIDorReserved());  // 识别标识符 含 ID or 保留字
        else if(nowChar == '{')
            isComment();  // 注释不用生成 tokens 
        else if(isspace(nowChar))  
        {   
            nowChar = readOneCharFromSource();
            status = Done;
        }
        else if(nowChar == EOF){
            tokens.push_back(Token(lineNum,"EOF",ENDFILE));
            status = SFileEnd;
        }
        else if(isSingleChar(nowChar)){    
            switch (nowChar)
            {
            case '+':
                tokens.push_back(Token(lineNum,"+",PLUS));
                break;
            case '-':
                tokens.push_back(Token(lineNum,"-",MINUS));
                break;
            case '*':
                tokens.push_back(Token(lineNum,"*",TIMES));
                break;
            case '/':
                tokens.push_back(Token(lineNum,"/",OVER));
                break;
            case '(':
                tokens.push_back(Token(lineNum,"(",LPAREN));
                break;
            case ')':
                tokens.push_back(Token(lineNum,")",RPAREN));
                break;
            case ';':
                tokens.push_back(Token(lineNum,";",SEMI));
                break;
            case '[':
                tokens.push_back(Token(lineNum,"[",LMIDPAREN));
                break;
            case ']':
                tokens.push_back(Token(lineNum,"]",RMIDPAREN));
                break;
            case '=':
                tokens.push_back(Token(lineNum,"=",EQ));
                break;
            case '<':
                tokens.push_back(Token(lineNum,"<",LT));
                break;
            default :   // 其他
                break;
            }
            nowChar = readOneCharFromSource();
            status = Done;
        }
        else if(nowChar == '.')
        {
            preChar = readOneCharFromSource();
           
            if(preChar != '.')  // 判断 .. 需要预读下一个字符
            {
                tokens.push_back(Token(lineNum,".",DOT));
                status = Done;
            }
            else
            {
                tokens.push_back(Token(lineNum,"..",UNDERANGE));
                preChar = readOneCharFromSource();
                status = Done;
            }
            nowChar = preChar;
        }
        else if (nowChar == ':')   
        {
            tmpLn = lineNum;
            preChar = readOneCharFromSource();
            if(preChar == '=')
            {
                tokens.push_back(Token(lineNum,":=",ASSIGN));
                nowChar = readOneCharFromSource();
                status = Done;
            }
            else
            {
                status = SError;
                sprintf(tmpStrError,"Unexpected char after ':',should be = ,line: %d\n",tmpLn); 
                nowChar = preChar;
            }
        }
        else if(nowChar == ',')
        {
            tmpLn = lineNum;
            preChar = readOneCharFromSource();
            if(isalnum(preChar) || isspace(preChar))
            {
                status = Done;
                tokens.push_back(Token(tmpLn,",",COMMA));
                nowChar = preChar;
                status = Done;
            }
            else
            {
                status = SError;
                sprintf(tmpStrError,"Unexpected char after ',' at line: %d\n",tmpLn);
                nowChar = preChar;
            }
        }
        else if(nowChar == '\'')  // charc   //todo: CHARC
        {
            char tch = readOneCharFromSource();
            nowChar = readOneCharFromSource();
            if(nowChar != '\'')
            {
                status = SError;
                sprintf(tmpStrError,"Unexpected char '%c',at line %d\n",nowChar,lineNum);
                nowChar = readOneCharFromSource();
            }
            else
            {
                char tstr[] = {tch,'\0'};
                tokens.push_back(Token(lineNum,tstr,CHARC));
                nowChar = readOneCharFromSource();
                status = Done;
            }
        }
        else
        {
            status = SError;
            sprintf(tmpStrError,"Unexpected char '%c',at line %d\n",nowChar,lineNum);
            nowChar = readOneCharFromSource();
        }

        if(status == SError){
            errors.push_back(string(tmpStrError));
        }
    }
    fclose(sourceFile); //关闭源文件
    return errors.size() == 0;
}


// -------------------------------------- token 转 string ----------------------------------------------------------
static map<TokenType,string> stringMap;
static bool setStringMap = false;
string tokenToString(TokenType type){
    if(!setStringMap)
    {
        // 保留字
        stringMap[PROGRAM]="PROGRAM";
        stringMap[TYPE]="TYPE";
        stringMap[VAR]="VAR";
        stringMap[PROCEDURE]="PROCEDURE";
        stringMap[BEGIN]="BEGIN";
        stringMap[END]="END";
        stringMap[ARRAY]="ARRAY"; 
        stringMap[OF]="OF";
        stringMap[RECORD]="RECORD";
        stringMap[IF]="IF";
        stringMap[ELSE]="ELSE";
        stringMap[THEN]="THEN";
        stringMap[FI]="FI";
        stringMap[WHILE]="WHILE";
        stringMap[DO]="DO";
        stringMap[ENDWH]="ENDWH";
        stringMap[READ]="READ";
        stringMap[WRITE]="WRITE";
        stringMap[RETURN]="RETURN";

        stringMap[INTEGER]="INTEGER";
        stringMap[CHAR]="CHAR";

        stringMap[CHARC]="CHARC";
        stringMap[INTC]="INTC";
        
        // 特殊符号
        stringMap[PLUS]="PLUS";
        stringMap[MINUS]="MINUS";
        stringMap[TIMES]="TIMES";
        stringMap[OVER]="OVER";
        stringMap[LPAREN]="LPAREN";
        stringMap[RPAREN]="RPAREN";
        stringMap[SEMI]="SEMI";
        stringMap[LMIDPAREN]="LMIDPAREN";
        stringMap[RMIDPAREN]="RMIDPAREN";
        stringMap[EQ]="EQ";
        stringMap[LT]="LT";
        stringMap[ENDFILE]="ENDFILE";
        stringMap[DOT]="DOT";
        stringMap[UNDERANGE]="UNDERANGE";
        stringMap[ASSIGN]="ASSIGN";
        stringMap[COMMA]="COMMA";

        stringMap[ID] = string("ID");

        setStringMap=true;
    }
    if(stringMap.find(type) != stringMap.end()) return stringMap[type];
    return "unknown TokenType";   // 没找到
}


// ----------------------------------------------- 打印输出 ---------------------------------------------
// 打印所有 Token 到文件或者屏幕,默认输出到文件
void outputTokens(bool toScreen ,const char tokenFileName[])
{
    if(toScreen) // 输出到屏幕
    {
        for(size_t i = 0; i < tokens.size(); i++)
            printf("%-2d:    TokenType:  %-20s  name=%s \n",tokens[i].lineNum,tokenToString(tokens[i].type).c_str(),tokens[i].name.c_str());
    }
    else // 输出到文件
    {
        FILE * fn = fopen(tokenFileName,"w");
        for(size_t i = 0; i < tokens.size(); i++)
           fprintf(fn,"%-2d:       TokenType: %-20s  name= %s \n",tokens[i].lineNum,tokenToString(tokens[i].type).c_str(),tokens[i].name.c_str());
        fclose(fn);
    }
}

// 输出错误信息到文件或屏幕, 默认输出到屏幕
void outputLexerErrors(bool toScreen ,const char errorFileName[])
{
    if(toScreen) // 输出到屏幕
    {
        for(size_t i = 0; i < errors.size(); i++)
            puts(errors[i].c_str());
    }
    else // 输出到文件
    {
        FILE * fn=fopen(errorFileName,"w");
        for(size_t i=0; i<errors.size(); i++)
            fputs(errors[i].c_str(),fn);
        fclose(fn);
    }
}

// ------------------------------------ 对外接口 ------------------------------------------------- 
vector<Token>& getTokenList()    // 对外的接口
{
    const char tokenFilePath[100]  =  "./txt/tokens.txt";  // tokens文件路径
    const char sourceFilePath[100]  =  "./txt/test1.txt";  // 源文件路径

    lexerAnalyse(sourceFilePath);
    outputTokens(false, tokenFilePath);
    outputLexerErrors(true, NULL);

    return tokens;  // 传递 tokens 
}


// ----------------------------------- mian 用于单独词法分析测试 ----------------------------------------------
// int main(){ 
//     const char tokenFilePath[100]  =  "./txt/tokens.txt";  // tokens文件路径
//     const char sourceFilePath[100]  =  "./txt/test1.txt";  // 源文件路径

//     lexerAnalyse(sourceFilePath);
//     outputTokens(false, tokenFilePath);
//     outputLexerErrors(true, NULL);

//     return 0;
// }

