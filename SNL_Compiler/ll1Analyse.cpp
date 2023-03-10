#include "ll1Analyse.h"
#include "lexerAnalyse.h"
#include "parseAnalyse.h"
#include <cstring>
using namespace std;

// ------------------------------------ 构建 LL(1) 符号表 --------------------------------------------------

#define MaxSymbolValue int(MultOp)  // 最后一个 TokenType 的编号
int LL1Table[MaxSymbolValue+5][MaxSymbolValue+5];  // 定义二维矩阵  LL(1)符号表 

void createLL1Table()
{
    memset(LL1Table,-1,sizeof(LL1Table));   // 产生式 和 Predict集 都从下标 1 开始,  -1 作为错误编号
    for(int i = 1; i <= ProductExpressionNum; i++)    // 对每一条产生式 , 遍历其 Predict[i]
    {
        for(set<TokenType>::iterator it = Predict[i].begin(); it != Predict[i].end(); ++it)
        {
            LL1Table[int(products[i][0])][int(*it)] = i;  // 再遍历填入正确产生式编号
                    //一维是 Vn 编号   , 二维是 Predict集中的 Vt 编号
        }
    }
}

// -------------------------------------- LL(1) 语法分析 --------------------------------------------------------
#define MAXPARSERERRORLEN 1024 
static char strError[MAXPARSERERRORLEN];     // 临时存储错误信息
static vector<string> errors;     // 保存所有的语法错误信息 

static vector<Symbol> symbolStack;     // 符号栈 ,存放 语法符号 Vt 和 Vn
static vector<TreeNode **> syntaxStack;        // 语法树栈 , 存放指向儿子或兄弟节点的指针的地址 处理声明部分和语句部分
static vector<TreeNode *> operatorStack;     // 操作符栈  
static vector<TreeNode *> numStack;     // 操作数栈 

static TreeNode * rt = NULL;  // 语法树根指针

bool ReadOneToken(Token & tk);
static Token currentToken;  // 当前访问到的 token

// LL(1)驱动程序
TreeNode* LL1parse(){
    createLL1Table();
    errors.clear();
    symbolStack.clear();
    operatorStack.clear();
    numStack.clear();
    syntaxStack.clear();
 
    symbolStack.push_back(Program);
    rt = new TreeNode(0,ProK);   // 根节点
    for(int i = 2; i >= 0; i--)
        syntaxStack.push_back(&(rt->son[i]));

    ReadOneToken(currentToken);
    while(!symbolStack.empty()){
 



 
    }


    return rt;
}


void predict(int num){

}

int Priosity(TokenType op){

}


// ---------------------------------- 各个产生式处理函数 见 P111 -----------------------------------------------------

void process1(){

}


void process2(){

}
















// -------------------------------------- LL(1) 语法分析测试 main ---------------------------------------------------
static vector<Token> tokens;  // token 序列
static size_t tkPtr = 0;  // Token指针

void initReadOneToken()
{
    tkPtr = 0;
}

bool ReadOneToken(Token & tk) {    // 顺序读取一个token 
    if(tkPtr < tokens.size())
    {
        tk = tokens[tkPtr++];
        return true;
    }
    tkPtr = 0; 
    return false;
}


int main(){
    // token 序列读进来
	tokens = getTokenList();
	// cout<< tokens.size()<< endl;
	
	initReadOneToken();
	initProducts();   // 千万不要忘了  init !!!!!
	initPredict();
   
    // LL1parse();

	// const char fileName[] = "./txt/syntaxTree.txt";
	// outputParserTree(rt , fileName);
    return 0;
}
