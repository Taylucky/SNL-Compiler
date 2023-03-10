#ifndef SEMANTICANALYSE_H
#define SEMANTICANALYSE_H


#include "parseanalyse.h"
#include "recursiveanalyse.h"
#include<string>
#include<vector>
using namespace std;


// --------------------------------------- 类型的内部表示 ------------------------------------------------------
typedef enum{                      // P157
    arrayTy,charTy,intTy,recordTy,boolTy
} TypeKinds;

struct typeIR       // 类型的内部表示
{
    int size;       // 类型所占空间大小
    TypeKinds kind; // 类型
};

struct intIR:typeIR // int类型内部表示
{
    intIR() {size = 1; kind = intTy;}
};

struct boolIR:typeIR // bool类型内部表示
{
    boolIR() {size = 1; kind = boolTy;}
};

struct charIR:typeIR // char类型内部表示
{
    charIR() {size = 1; kind = charTy;}
};

struct arrayIR:typeIR // 数组类型内部表示
{
    int low;
    typeIR * elemTyPtr; ///指向数组元素的类型内部表示
    arrayIR(const int & s,typeIR * eptr,int L) {size = s; kind = arrayTy; elemTyPtr = eptr; low = L;}
};

// ------------ 记录类型 -------------------
struct rdnode   // 域节点
{
    string idname;        // 记录域中的标识符名
    typeIR * unitTypePtr; // 指向域类型的内部表示
    int offset;           // 表示当前标识符在记录中的内部偏移
};
typedef vector<rdnode> RecordDomain;   // 记录域列表, 用vector实现而不用链表
struct recordIR:typeIR          // 记录类型的内部表示
{
    int size;       // 类型所占空间大小
    TypeKinds kind; // 类型
    RecordDomain rdlist;
    recordIR(const int & s) {size = s;kind = recordTy;}
};

// ------------------------------------- 标识符的内部表示 ------------------------------------------------------
typedef enum {          // 标识符类型
    typeKind,varKind,procKind
} IdKinds;

typedef enum {         // 直接还是间接变量
    dir,indir
} AccessKinds;

typedef vector<int> ParamTable ; // 形参信息表，用vector实现

struct idIR    // 标识符的内部表示
{
    typeIR * typePtr;   // 指向标识符的类型内部表示
    IdKinds kind;       // 标识符的类型

        // varIR
        AccessKinds access;   // 直接还是间接变量
        int level; // 标识符层数
        int off;   // 当前偏移量

        // procIR
        ParamTable param; // 形参信息表，用vector实现
        // int level;
};




// -------------------------------------------------- 符号表的结构 ---------------------------------------------------
struct symbNode          // 符号节点
{
    string idname;   // 标识符名
    idIR attrIR;     // 标识符属性
};
typedef vector<symbNode> symbTable;



// ----------
extern void semanticAnalyse();  // 对外接口
void semanticsError(const string & s,const int & linenumber);
symbNode * findSymb(const int & lev,const string & snamen);
symbNode * findSymbAll(const string & sname);
void analyzeProK(TreeNode * rt); ///分析根节点
void analyzePheadK(TreeNode *  rt);///分析头节点
void analyzeDeclareK(TreeNode * rt); ///分析声明标志节点
void analyzeStmLK(TreeNode * rt); ///分析语句节点
void analyzeTypedecK(TreeNode * rt); ///分析类型声明具体节点
void analyzeVardecK(TreeNode * rt,symbNode * stp = NULL,bool isparam = 0);///分析变量声明具体节点
void analyzeProcdecK(TreeNode * rt);///分析过程声明具体节点
void analyzeParam(TreeNode * rt,symbNode * stp);

void createTable();///往scope栈添加一行符号表
void addSymble(const symbNode & st);///给当前层添加符号(改动)
void printSymbTable(const int & lev); ///输出某层符号表
void printSymb(const symbNode & st);///输出单个符号
void printSymbType();///输出scope栈所有符号表
void destroyTable();

void StmLKWhile(TreeNode * rt);
void StmLKIf(TreeNode * rt);
void StmLKWrite(TreeNode * rt);
void StmLKRead(TreeNode * rt);
void StmLKReturn(TreeNode * rt);
void StmLKCall(TreeNode * rt);
void StmLKAssign(TreeNode * rt);

int getTypeSize(TreeNode * rt);
typeIR * getTypePtr(TreeNode * rt);///根据树声明节点创建类型内部表示并返回内部表示指针
typeIR * getExpType(TreeNode * rt,AccessKinds * ac = NULL,typeIR * recmemptr = NULL);

#endif // SEMANTICANALYSE_H
