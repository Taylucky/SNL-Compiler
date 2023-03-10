#ifndef PARSEANALYSE_H
#define PARSEANALYSE_H

#include"lexeranalyse.h"
#include<vector>
#include<set>
#include<cstring>
using namespace std;


// 这在两个语法分析程序中都会用到
extern size_t tkPtr;  // Token指针
extern vector<Token> tokens;  // tokens 序列
extern void initReadOneToken();
extern bool ReadOneToken(Token & tk);

// --------------------------- 产生式  Predict集 ------------------------------------------

string getTokenStr(TokenType tkty); // 返回 TokenType 的字符串表示
typedef TokenType Symbol;   // Symbol 用于 下半部分 Vn  , TokenType 用于上半部分 Vt

#define ProductExpressionNum  105   // 共 104 条产生式, 下标从 1开始
extern vector<Symbol> products[ProductExpressionNum + 5];  // extern 之后在 两个语法分析 cpp 中都能使用
// extern set<TokenType> Predict[ProductExpressionNum + 5];

extern void initProducts();
extern void initPredict();

// ------------------------------------ 语法树结构  详见 59,60 ---------------------------------------------
const int MAXSON = 3;

typedef enum {ProK,PheadK,TypeK,VarK,ProcDecK,StmLK,DecK,StmtK,ExpK} NodeKinds;
typedef enum {ArrayK,CharK,IntegerK,RecordK,IdK} DecKinds;
typedef enum {IfK,WhileK,AssignK,ReadK,WriteK,CallK,ReturnK} StmtKinds;
typedef enum {OpK,ConstK,VariK} ExpKinds;
typedef enum {ValparamType,VarparamType} ParamKinds;
typedef enum {IdV,ArrayMembV,FieldMembV} VarKinds;
typedef enum {Void,Integer,Boolean} ExpType;
struct symbnode;
struct symbtable;

struct SArrayAttr
{
    int low; // 数组下界
    int up;  // 数组上界
    DecKinds memtype; // 数组成员类型
};

struct SProcAttr
{
    ParamKinds param;  // 函数参类型
};

struct SExpAttr  //记录表达式的属性
{
    TokenType op;  // 记录 语法树节点的运算符单词, 关系表达式取 LT,EQ; 算术表达式取 PLUS,MINUS,  乘法 TIMES,OVER
    int val;
    VarKinds varkind;
    ExpType type;
};

struct SAttr
{
    SArrayAttr ArrayAttr; // 记录数组类型的属性
    SProcAttr ProcAttr;  // 记录过程类型的属性
    SExpAttr ExpAttr;  // 记录表达式类型的属性
    string type_name; //记录类型名
};

struct TreeNode
{
    TreeNode * son[MAXSON];  // 儿子结点
    TreeNode * sibling;  // 兄弟结点
    int lineno; // 源程序行号
    NodeKinds nodekind; // 结点类型，取值有：ProK,PeadK,TypeK,VarK,ProcDecK,StmLK,DecK,StmtK,ExpK
    union
    {
        DecKinds dec;
        StmtKinds stmt;
        ExpKinds exp;
    } kind;
    vector<string> name; // 结点中标志符的名字
    SAttr attr; // 记录语法树结点的其他属性

    TreeNode(const int & linenum,const NodeKinds nks,int cks = -1)   // 成员函数  new TreeNode( ProK );
    {
        // 初始化新语法树节点t各子节点son[i]为NULL
        memset(son,NULL,sizeof(son));
        for(int i=0; i<MAXSON; i++)
            son[i]=NULL;

        sibling = NULL;  // 初始化新语法树节点t兄弟节点sibling为NULL
        nodekind = nks;  // 指定新语法树节点t成员:结点类型nodekind
        lineno = linenum;  // 指定新语法树节点t成员:源代码行号lineno为全局变量linenum
        if(cks != -1)         //  cks 默认 -1 是 kind 为 dec 的时候, 不是 -1 ,则是 kind 为 stmt 或 exp 时候
        {
            if(nks == StmtK)
                kind.stmt = (StmtKinds)cks;
            else if(nks == ExpK)
            {
                kind.exp = (ExpKinds)cks;
                attr.ExpAttr.varkind = IdV;
                attr.ExpAttr.type = Void;   // 指定新语法树节点t成员: 类型检查类型type为Void
            }
        }
    }
};

extern void outputParserTree(TreeNode *rt,const char fileName[]);   // 输出语法树到文件
#endif // PARSEANALYSE_H
