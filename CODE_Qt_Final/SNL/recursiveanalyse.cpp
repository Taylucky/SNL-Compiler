#include"recursiveanalyse.h"
#include"predict.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>
using namespace std;

// ---------------------------------- 语法分析 -----------------------------------------
// 函数声明 , 互相调用
TreeNode *program();
TreeNode *programHead();
TreeNode *declarePart();
TreeNode *programBody();
TreeNode *typeDec();
TreeNode *varDec();
TreeNode *procDec();
TreeNode *typeDeclaration();
TreeNode *typeDecList();
TreeNode *typeDecMore();
TreeNode *varDeclaration();
TreeNode *varDecList();
TreeNode *varDecMore();
TreeNode *procDeclaration();
TreeNode *fieldDecList();
TreeNode *fieldDecMore();
void typeDef(TreeNode *t);
void baseType(TreeNode *t);
void strutureType(TreeNode *t);
void arrayType(TreeNode *t);
void idMore(TreeNode *t);
void recType(TreeNode *t);
void idList(TreeNode *t);
void varIdList(TreeNode *t);
void varIdMore(TreeNode *t);
void paramList(TreeNode *t);
TreeNode *paramDecList();
TreeNode *param();
TreeNode *paramMore();
void formList(TreeNode *t);
void fidMore(TreeNode *t);
TreeNode *procDecPart();
TreeNode *procBody();
TreeNode *stmList();
TreeNode *stmMore();
TreeNode *stm();
TreeNode *assCall();
TreeNode *assignmentRest();
TreeNode *exp();
TreeNode *loopStm();
TreeNode *inputStm();
TreeNode *outputStm();
TreeNode *returnStm();
TreeNode *callStmRest();
TreeNode *actParamList();
TreeNode *actParamMore();
TreeNode *term();
TreeNode *simple_exp();
TreeNode *factor();
void variMore(TreeNode *t);
TreeNode *variable();
TreeNode *fieldvar();
void fieldvarMore(TreeNode *t);
TreeNode *conditionalStm();

#define MAXERRORLEN 1024
static int linenum = 0;
static Token token;
static string temp_name;
static bool Error = false;  // 若 Error == true 则不继续进行下面的语法分析了
static char tmpStrError[MAXERRORLEN];  //暂存出错信息
static const char errorFileName[100] = "D:/CODE_Qt/txt/debug_info.txt";

void syntaxError(char* message)
{
    //printf(">>> Syntax error : %s, at line %d\n", message, token.lineNum);
    char tmp[MAXERRORLEN];
    sprintf(tmp,">>> Syntax error : %s, at line %d\n", message, token.lineNum);
    FILE * fn=fopen(errorFileName,"a"); // 错误信息输出到 debug_info 文件
    fputs(tmp,fn);
    fclose(fn);

    Error = true;   // 唯一一个会改变 Error 值的地方
}

void match(TokenType tkt)
{
    if(token.type == tkt)   // 类型匹配
        ReadOneToken(token);
    else              // token 类型不匹配
    {
        sprintf(tmpStrError,"token 类型不匹配,expected:%s ,but now is:%s ", getTokenStr(tkt).c_str(), getTokenStr(token.type).c_str());
        syntaxError(tmpStrError);
        ReadOneToken(token);
    }
}

bool belong(const int &n)  // 每一个产生式左边的 Vn 是唯一的,所以根据产生式是哪一条 n, 去找对应的 Predict[n]
{
    int tokenSize = count(Predict[n].begin(),Predict[n].end(),token.type);  // 查找目标值为 token.type 的个数
    return tokenSize;
}

// 为 snl 的产生式左边的  Vn 写函数, 遇到终极符则 match() ,遇到非终极符则 调用  这个 Vn 的函数
// ------------------------------  总程序  -----------------------------------------------
//   1 的 Vn
TreeNode *program()
{
    if (Error)
        return NULL;
    TreeNode *t = programHead();   // 程序头
    TreeNode *q = declarePart();   // 声明部分
    TreeNode *s = programBody();   // 程序体

    TreeNode *root = new TreeNode(linenum,ProK);  // ProK 语法树根节点
    if (root != NULL)
    {
        root->lineno = 0;  // lineno = 0 表示语法树节点类型不是具体节点
        if (t != NULL)
            root->son[0] = t;   // 总程序树  有三个分支
        else{
            sprintf(tmpStrError,"缺少程序头 !");
            syntaxError(tmpStrError);
        }
        if (q != NULL)
            root->son[1] = q;  // 声明部分可以没有
        if (s != NULL)
            root->son[2] = s;
        else{
            sprintf(tmpStrError,"缺少程序体 !");
            syntaxError(tmpStrError);
        }
    }
    match(DOT);  // 匹配 .  程序结束的标志
    return root;
}

//--------------- 程序头 -----------------
//   2/3 的 Vn
TreeNode * programHead()
{
    if (Error)
        return NULL;
    TreeNode *t = new TreeNode(linenum,PheadK);    // 在program 之前创建程序头类型 节点
    match(PROGRAM);
    if ((t != NULL) && (token.type == ID))      // programName::=ID 直接写一起了
    {
        t->lineno = 0;       // 仅仅表示 节点类型 则令lineno = 0
        t->name.push_back(token.name);  // match(ID) 之前要做的
    }
    match(ID);
    return t;
}


//   4 Vn
TreeNode *declarePart()
{
    if (Error)
        return NULL;
    TreeNode *tp;    // 临时节点

    tp = typeDec();           // 类型声明
    TreeNode * type = NULL;
    if (tp != NULL)
    {
        type = new TreeNode(linenum,TypeK);  // type 只表示 TypeK 这个类型节点
        type->son[0] = tp;  // tp 这个临时节点返回的是 具体内容的 类型声明节点, 作为 type 儿子节点
        type->lineno = 0; // 只表示类型, 没有具体内容 的节点
    }

    tp = varDec();            // 变量声明
    TreeNode *var = NULL;
    if (tp != NULL)
    {
        var = new TreeNode(linenum,VarK); // var 只表示 VarK 这个类型节点
        var->son[0] = tp;  // tp 才是具体的变量声明节点, 作为 var 的儿子节点
        var->lineno = 0;
    }

    TreeNode *proc = procDec();    // 过程声明

    if (var != NULL)
        var->sibling = proc;
    else
        var = proc;
    if (type != NULL)
        type->sibling = var;
    else
        type = var;
    return type;
}


// -----------------------------------  类型声明  -------------------------------------

//    5/6 的 Vn
TreeNode *typeDec()
{
    if (Error)
        return NULL;
    TreeNode *t = NULL;

    if (belong(6))
        t = typeDeclaration(); // 调用
    else if ( !(belong(5)) )  // 5是空语句
    {
        ReadOneToken(token);
        sprintf(tmpStrError,"不匹配的token !");
        syntaxError(tmpStrError);    // 类型匹配不正确的 token
    }
    return t;
}

//   7 的 Vn
TreeNode *typeDeclaration()
{
    if (Error)
        return NULL;
    match(TYPE);				 // 匹配 type
    TreeNode *t = typeDecList(); // 调用
    if (t == NULL){
        sprintf(tmpStrError,"缺少类型声明 !");   // 语法错误  type 后缺失类型声明
        syntaxError(tmpStrError);
    }
    return t;
}

//   8 的 Vn
TreeNode *typeDecList()
{
    if (Error)
        return NULL;
    TreeNode *t = new TreeNode(linenum,DecK); //类型声明节点, 具体节点, 有内容
    if (t != NULL)
    {
        t->lineno = token.lineNum;  // 具体节点, 行号
        if (token.type == ID)
            t->name.push_back(token.name); // match ID 之前把 name 放进去
        match(ID);						   // typedId
        match(EQ);						   // =
        typeDef(t);						   // 调用
        match(SEMI);					   // ;
        t->sibling = typeDecMore();		   // 兄弟节点  更多的类型声明
    }
    return t;
}

//    9/10 的 Vn
TreeNode *typeDecMore()
{
    if (Error)
        return NULL;
    TreeNode *t = NULL;
    if (belong(10))
        t = typeDecList(); // 调用
    else if (!belong(9))// 9 是空语句
    {
        ReadOneToken(token);
        sprintf(tmpStrError,"不匹配的token !");
        syntaxError(tmpStrError);
    }
    return t;
}

//    12/13/14 的 Vn
void typeDef(TreeNode *t)     //  PPT上的 typeDef 就是 书上的 typeName
{
    if (Error)
        return;
    if (belong(12))
        baseType(t); // 基类型
    else if (belong(13))
        strutureType(t); // 结构类型  包括数组和记录
    else if (belong(14))
    {
        t->kind.dec = IdK;				// 节点类型为 id
        t->attr.type_name = token.name; //  ID  自定义的 类型名
        match(ID);
    }
    else
    {
        ReadOneToken(token);
        sprintf(tmpStrError,"不匹配的token !");
        syntaxError(tmpStrError);
    }
}

// -------------  基类型  -------------------

//    15/16的 Vn
void baseType(TreeNode *t)
{
    if (Error)
        return;
    if (belong(15))
    {
        match(INTEGER);
        t->kind.dec = IntegerK;
    }
    else if (belong(16))
    {
        match(CHAR);
        t->kind.dec = CharK;
    }
    else
    {
        ReadOneToken(token);
        sprintf(tmpStrError,"不匹配的token !");
        syntaxError(tmpStrError);
    }
}

// -------------  结构类型  -------------------
//   17/18 的 Vn
void strutureType(TreeNode *t)   // array 或者 record
{
    if (Error)
        return;
    if (belong(17))
        arrayType(t); // 调用 数组类型
    else if (belong(18))
        recType(t); // 调用  记录类型
    else
    {
        ReadOneToken(token);
        sprintf(tmpStrError,"不匹配的token !");
        syntaxError(tmpStrError);
    }
}

// -------------  数组类型  -------------------
//   19 的 Vn
void arrayType(TreeNode *t)
{
    if (Error)
        return;
    match(ARRAY);	  // array
    match(LMIDPAREN); // [
    if (token.type == INTC)
        t->attr.ArrayAttr.low = atoi(token.name.c_str()); // atoi()字符转 int   low 的值
    match(INTC);
    match(UNDERANGE); // ..
    if (token.type == INTC)
        t->attr.ArrayAttr.up = atoi(token.name.c_str()); // top的值
    match(INTC);
    match(RMIDPAREN); // ]
    match(OF);		  // of
    baseType(t);	  // 调用 Vn
    t->attr.ArrayAttr.memtype = t->kind.dec;
    t->kind.dec = ArrayK; // 节点为 数组类型
}

// -----------  记录类型  ------------

//    22 的 Vn
void recType(TreeNode *t)
{
    if (Error)
        return;
    t->kind.dec = RecordK; // 记录类型的 节点
    TreeNode *p = NULL;
    match(RECORD);		// 匹配关键字 record
    p = fieldDecList(); // 调用
    if (p != NULL)
        t->son[0] = p; // 儿子节点  具体节点
    else{
        sprintf(tmpStrError,"记录体被你弄丢了 !");
        syntaxError(tmpStrError);
    }

    match(END); // 匹配关键字  end
}

// ----------- 记录类型里面的内容 -----------------
//   23/24的 Vn
TreeNode *fieldDecList()
{
    if (Error)
        return NULL;
    TreeNode *t = new TreeNode(linenum,DecK);   // 具体声明节点
    TreeNode *p = NULL;
    if (t != NULL)
    {
        t->lineno = token.lineNum;
        if (belong(23))
        {
            baseType(t);
            idList(t);
            match(SEMI);
            p = fieldDecMore();
        }
        else if (belong(24))
        {
            arrayType(t);
            idList(t);
            match(SEMI);
            p = fieldDecMore();
        }
        else
        {
            ReadOneToken(token);
            sprintf(tmpStrError,"不匹配的token !");
            syntaxError(tmpStrError);
        }
        t->sibling = p; //兄弟节点     位于同一层次, 记录的内部声明
    }
    return t;
}

//   25/26 的 Vn
TreeNode *fieldDecMore()
{
    if (Error)
        return NULL;
    TreeNode *t = NULL;
    if (belong(25))
        ; // 25 产生式是 空语句
    else if (belong(26))
        t = fieldDecList(); // 遇到非终极符调用
    else
    {
        ReadOneToken(token);
        sprintf(tmpStrError,"不匹配的token !");
        syntaxError(tmpStrError);
    }
    return t;
}

//    27 的 Vn
void idList(TreeNode *t)
{
    if (Error)
        return;
    if (token.type == ID)
    {
        t->name.push_back(token.name); // 遇到标识符  节点的 name 写进去
        match(ID);
    }
    idMore(t); // 调用
}

//   28/29 的 Vn
void idMore(TreeNode *t)
{
    if (Error)
        return;
    if (belong(28))
        ; // 空语句
    else if (belong(29))
    {
        match(COMMA); // 匹配 ,
        idList(t);	  // 调用 Vn
    }
    else
    {
        ReadOneToken(token);
        sprintf(tmpStrError,"不匹配的token !");
        syntaxError(tmpStrError);
    }
}

// --------------------------------  变量声明  ------------------------------------

//   30/31 的 Vn
TreeNode *varDec()
{
    if (Error)
        return NULL;
    TreeNode *t = NULL;
    if (belong(30))
        ; // 空语句 可能没有变量声明
    else if (belong(31))
        t = varDeclaration(); // 调用变量声明 Vn 的函数
    else
    {
        ReadOneToken(token);
        sprintf(tmpStrError,"不匹配的token !");
        syntaxError(tmpStrError);
    }
    return t;
}

//   32 的 Vn
TreeNode *varDeclaration()
{
    if (Error)
        return NULL;
    match(VAR);					// 匹配保留字 VAR  表示变量声明的开始
    TreeNode *t = varDecList(); // 调用
    if (t == NULL){
        sprintf(tmpStrError,"缺少变量声明 !");
        syntaxError(tmpStrError);
    }
    return t;
}

//   33 的 Vn
TreeNode *varDecList()
{
    if (Error)
        return NULL;
    TreeNode *t = new TreeNode(linenum,DecK); // 创建 Deck 类型的节点 ,具体节点
    TreeNode *p = NULL;
    if (t != NULL)
    {
        t->lineno = token.lineNum;
        typeDef(t);       //   typeDef   也是  TypeName()
        varIdList(t);
        match(SEMI);
        p = varDecMore();
        t->sibling = p; // 变量声明 可能有好几个   兄弟节点  一层的
    }
    return t;
}

//   34/35的 Vn
TreeNode *varDecMore()
{
    if (Error)
        return NULL;
    TreeNode *t = NULL;
    if (belong(34))
        ; // 空
    else if (belong(35))
        t = varDecList(); // 调用 Vn
    else
    {
        ReadOneToken(token);
        sprintf(tmpStrError,"不匹配的token !");
        syntaxError(tmpStrError);
    }
    return t;
}

//   36 的 Vn
void varIdList(TreeNode *t)
{
    if (Error)
        return;
    if (token.type == ID)
    {
        t->name.push_back(token.name); // 遇到 id 还要写进 这个节点的 name
        match(ID);
    }
    else
    {
        sprintf(tmpStrError,"缺少匹配的标识符 !"); //todo
        syntaxError(tmpStrError);  // 应该是 标识符 ,却没有匹配的标识符   语法错误  !
        ReadOneToken(token);
    }
    varIdMore(t); // 调用 Vn
}

//   37/38 的 Vn
void varIdMore(TreeNode *t)
{
    if (Error)
        return;
    if (belong(37))
        ;
    else if (belong(38))
    {
        match(COMMA); // , 匹配
        varIdList(t); // 遇到非终极符 调用
    }
    else
    {
        ReadOneToken(token);
        sprintf(tmpStrError,"不匹配的token !");
        syntaxError(tmpStrError);
    }
}




// ----------------------------------- 过程声明 -------------------------------------------
//    39/40  42/43  Vn
TreeNode *procDec()  // procDecMore()
{
    if (Error)
        return NULL;
    TreeNode *t = NULL;
    if (belong(39))
        ;            // 空语句
    else if (belong(40))
        t = procDeclaration();     // 调用
    else
    {
        ReadOneToken(token);
        sprintf(tmpStrError,"不匹配的token !");
        syntaxError(tmpStrError);
    }
    return t;
}

//   41 / 44 Vn
TreeNode *procDeclaration()
{
    if (Error)
        return NULL;
    TreeNode *t = new TreeNode(linenum,ProcDecK);        // 过程声明节点 ,具体节点

    match(PROCEDURE);    // procedure
    if (t != NULL)
    {
        t->lineno = token.lineNum;
        if (token.type == ID)            //  含 44 Vn  ProcNmae ::= ID
        {
            t->name.push_back(token.name);
            match(ID);
        }
        match(LPAREN);  // (
        paramList(t);  // 调用
        match(RPAREN);  // )
        match(SEMI);  // ;
        t->son[1] = procDecPart();       //  两个部分
        t->son[2] = procBody();
        t->sibling = procDec();   // 兄弟节点   可能多个 过程声明   //  同procDecMore()
    }
    return t;
}



//--------------- 参数 -----------------------
//   45/46 的 Vn
void paramList(TreeNode *t)
{
    if (Error)
        return;
    TreeNode *p = NULL;
    if (belong(45))
        ;
    else if (belong(46))
    {
        p = paramDecList();
        t->son[0] = p;          //   t->son[0]
    }
    else
    {
        ReadOneToken(token);
        sprintf(tmpStrError,"不匹配的token !");
        syntaxError(tmpStrError);
    }
}


//   47 的 Vn
TreeNode *paramDecList()
{
    if (Error)
        return NULL;
    TreeNode *t = param();    // 调用
    TreeNode *p = paramMore();
    t->sibling = p;  // 兄弟节点
    return t;
}


//   48/49 的 Vn
TreeNode *paramMore()
{
    if (Error)
        return NULL;
    TreeNode *t = NULL;
    if (belong(48))
        ;
    else if (belong(49))
    {
        match(SEMI);		// ;
        t = paramDecList(); // 调用
        if (t == NULL){
            sprintf(tmpStrError,"缺少参数声明 !");  //todo
            syntaxError(tmpStrError);
        }
    }
    else
    {
        ReadOneToken(token);
        sprintf(tmpStrError,"不匹配的token !");
        syntaxError(tmpStrError);
    }
    return t;
}



//   50/51 的 Vn
TreeNode *param()
{
    if (Error)
        return NULL;
    TreeNode *t = new TreeNode(linenum,DecK);  // 声明节点, 具体节点
    if (t != NULL)
    {
        t->lineno = token.lineNum;

        if (belong(50))
        {                           // eg. integer s1
            t->attr.ProcAttr.param = ValparamType;       //节点  参数类型    值参
            typeDef(t);
            formList(t);
        }
        else if (belong(51))
        {
            match(VAR);  // 带 var 的  eg. var integer s2
            t->attr.ProcAttr.param = VarparamType;    // 变参
            typeDef(t);
            formList(t);
        }
        else
        {
            ReadOneToken(token);
            sprintf(tmpStrError,"不匹配的token !");
            syntaxError(tmpStrError);
        }
    }
    return t;
}




//   52 的 Vn
void formList(TreeNode *t)
{
    if (Error)
        return;
    if (token.type == ID)
    {
        t->name.push_back(token.name); // ID   形参名
        match(ID);
    }
    fidMore(t); // 调用
}


//   53/54 的 Vn
void fidMore(TreeNode *t)
{
    if (Error)
        return;
    if (belong(53))
        ; //空语句
    else if (belong(54))
    {
        match(COMMA); // ,
        formList(t);  // 调用
    }
    else
    {
        ReadOneToken(token);
        sprintf(tmpStrError,"不匹配的token !");
        syntaxError(tmpStrError);
    }
}


// ------------ 过程声明 和 过程体 -------------------
//   55 的 Vn
TreeNode *procDecPart()
{
    if (Error)
        return NULL;
    return declarePart(); //  过程声明部分 同  程序声明部分
}

//   56 的 Vn
TreeNode *procBody()
{
    if (Error)
        return NULL;
    TreeNode *t = programBody();   // 过程体  同  程序体
    if (t == NULL){
        sprintf(tmpStrError,"缺少过程体 !");
        syntaxError(tmpStrError);
    }
    return t;
}

//--------------------------------  程序体  -------------------------------------------
//    57 的 Vn
TreeNode *programBody()
{
    if (Error)
        return NULL;
    TreeNode *t = new TreeNode(linenum,StmLK);   // 节点类型  语句序列
    match(BEGIN);   // begin
    if (t != NULL)
    {
        t->lineno = 0;
        t->son[0] = stmList();
    }
    match(END); // end
    return t;
}


//    58 Vn  语句序列
TreeNode *stmList()
{
    if (Error)
        return NULL;
    TreeNode *t = stm();
    // cout<<token.name<<endl;
    if (t != NULL)
        t->sibling = stmMore();
    return t;
}

//   59/60  Vn
TreeNode *stmMore()
{
    if (Error)
        return NULL;
    TreeNode *t = NULL;
    if (belong(59))
        ;
    else if (belong(60))
    {
        match(SEMI);  // ;
        t = stmList();   //调用
    }
    else
    {
        ReadOneToken(token);
        sprintf(tmpStrError,"不匹配的token !");
        syntaxError(tmpStrError);
    }
    return t;
}

// ------------- 各种语句 ---------------
//   61/62/63/64/65/66  Vn
TreeNode *stm()
{
    if (Error)
        return NULL;
    TreeNode *t = NULL;
    if (belong(61))
        t = conditionalStm();  // 调用 条件语句
    else if (belong(62))
        t = loopStm(); // 调用 循环语句
    else if (belong(63))
        t = inputStm();  // 调用 输入语句
    else if (belong(64))
        t = outputStm(); // 调用 输出语句
    else if (belong(65))
        t = returnStm();  // 调用 返回语句
    else if (belong(66))
    {
        temp_name = token.name;
        match(ID);
        t = assCall();   // 赋值语句  or 函数调用语句     二者写在一起,因为均以 ID 开头
    }
    else
    {
        ReadOneToken(token);
        sprintf(tmpStrError,"不匹配的token !");
        syntaxError(tmpStrError);
    }
    return t;
}

//   67/68 Vn
TreeNode *assCall()
{
    if (Error)
        return NULL;
    TreeNode *t = NULL;
    if (belong(67))
        t = assignmentRest();  // 赋值语句
    else if (belong(68))
        t = callStmRest();  // 函数调用语句
    else
    {
        ReadOneToken(token);
        sprintf(tmpStrError,"不匹配的token !");
        syntaxError(tmpStrError);
    }
    return t;
}



//   69 Vn  赋值语句
TreeNode *assignmentRest()
{
    if (Error)
        return NULL;
    TreeNode *t = new TreeNode(linenum,StmtK); // newStmtK(AssignK)
    t->kind.stmt = AssignK;
    // 赋值语句节点的第一个儿子节点记录赋值语句的左侧变量名
    // 第二个儿子结点记录赋值语句的右侧表达式
    if (t != NULL)
    {
        t->lineno = token.lineNum;
        //处理第一个儿子结点，为变量表达式类型节点
        TreeNode *son1 = new TreeNode(linenum,ExpK); // newExpNode(VariK);
        son1->kind.exp = VariK;
        if (son1 != NULL)
        {
            son1->lineno = token.lineNum;
            son1->name.push_back(temp_name);
            variMore(son1);
            t->son[0] = son1;
        }
        //赋值号匹配
        match(ASSIGN); // :=
        //处理第二个儿子节点
        t->son[1] = exp();
    }
    return t;
}



//   70 Vn  条件语句
TreeNode *conditionalStm()
{
    if (Error)
        return NULL;
    TreeNode *t = new TreeNode(linenum,StmtK); // newStmtNode(IfK);
    t->kind.stmt = IfK;
    match(IF);  // if
    if (t != NULL)
    {
        t->lineno = token.lineNum;
        t->son[0] = exp();                 //  IF RelExp THEN  StmList FI
    }
    match(THEN);    // then
    if (t != NULL)
        t->son[1] = stmList();
    if (token.type == ELSE)   // else
    {
        match(ELSE);
        if (t != NULL)
            t->son[2] = stmList();
    }
    match(FI);  // fi
    return t;
}

//    71 Vn  循环语句
TreeNode *loopStm()
{
    if (Error)
        return NULL;
    TreeNode *t = new TreeNode(linenum,StmtK); // newStmtNode(WhileK);
    t->kind.stmt = WhileK;
    match(WHILE);   // while
    if (t != NULL)
    {
        t->lineno = token.lineNum;        // while  RelExp do  StmList endwh
        t->son[0] = exp();
        match(DO);  // do
        t->son[1] = stmList();
        match(ENDWH);  // endwh
    }
    return t;
}


//   72/73 Vn   输入语句
TreeNode *inputStm()
{
    if (Error)
        return NULL;
    TreeNode *t = new TreeNode(linenum,StmtK); // newStmtNode(ReadK);
    t->kind.stmt = ReadK;
    match(READ);  // read
    match(LPAREN); // (
    if ((t != NULL) && (token.type == ID))   // InputStm ::= Read( Invar )  , Invar ::= ID
    {
        t->lineno = token.lineNum;
        t->name.push_back(token.name);
    }
    match(ID);
    match(RPAREN); // )
    return t;
}


//   74 Vn  输出语句
TreeNode *outputStm()
{
    if (Error)
        return NULL;
    TreeNode *t = new TreeNode(linenum,StmtK); // newStmtNode(WriteK);
    t->kind.stmt = WriteK;
    match(WRITE);
    match(LPAREN);
    if (t != NULL)
    {
        t->lineno = token.lineNum;
        t->son[0] = exp();            // OutputStm ::= WRITE ( Exp )
    }
    match(RPAREN);
    return t;
}


//    75 Vn  返回语句
TreeNode *returnStm()
{
    if (Error)
        return NULL;
    TreeNode *t = new TreeNode(linenum,StmtK); // newStmtNode(ReturnK);
    t->kind.stmt = ReturnK;
    match(RETURN);  // return
    if (t != NULL)
        t->lineno = token.lineNum;
    return t;
}


//   76 Vn   函数调用语句
TreeNode *callStmRest()
{
    if (Error)
        return NULL;
    TreeNode *t = new TreeNode(linenum,StmtK); // newStmtNode(CallK);
    t->kind.stmt = CallK;
    match(LPAREN);
    //函数调用时，其子节点指向实参
    if (t != NULL)
    {
        t->lineno = token.lineNum;
        //函数名的结点也用表达式类型结点
        TreeNode *son0 = new TreeNode(linenum,ExpK); // newExpNode(VariK);
        son0->kind.exp = VariK;
        if (son0 != NULL)
        {
            son0->lineno = token.lineNum;
            son0->name.push_back(temp_name);
            t->son[0] = son0;
        }
        t->son[1] = actParamList();
    }
    match(RPAREN);
    return t;
}


//   77/78 Vn
TreeNode *actParamList()
{
    if (Error)
        return NULL;
    TreeNode *t = NULL;

    if (belong(77))
        ;
    else if (belong(78))
    {
        t = exp();
        if (t != NULL)
            t->sibling = actParamMore();
    }
    else
    {
        ReadOneToken(token);
        sprintf(tmpStrError,"不匹配的token !");
        syntaxError(tmpStrError);
    }
    return t;
}

//   79/80  Vn
TreeNode *actParamMore()
{
    if (Error)
        return NULL;
    TreeNode *t = NULL;
    if (belong(79))
        ;
    else if (belong(80))
    {
        match(COMMA);  // ,
        t = actParamList();
    }
    else
    {
        ReadOneToken(token);
        sprintf(tmpStrError,"不匹配的token !");
        syntaxError(tmpStrError);
    }
    return t;
}



// ----------------- 表达式 ---------------------

//   81/82  99/100 条件表达式  < | =
TreeNode *exp()
{
    if (Error)
        return NULL;
    TreeNode *t = simple_exp();	// 调用简单表达式处理函数simple_exp()
    // 当前单词token为逻辑运算单词LT或者EQ
    if ((token.type == LT) || (token.type == EQ))
    {
        TreeNode *p = new TreeNode(linenum,ExpK); // newExpNode(OpK); 创建新的OpK类型语法树节点
        p->kind.exp = OpK;
        // 新语法树节点p创建成功,初始化p第一个子节点成员child[0]
         // 并将当前单词token(为EQ或者LT)赋给语法树节点p的运算符成员attr.op
        if (p != NULL)
        {
            p->lineno = token.lineNum;
            p->son[0] = t;
            p->attr.ExpAttr.op = token.type;
            t = p;	// 将新的表达式类型语法树节点p作为函数返回值t
        }
        match(token.type);	// 当前单词token与指定逻辑运算符单词(为EQ或者LT)匹配
        // 语法树节点t非空,调用简单表达式处理函数simple_exp()
        // 函数返回语法树节点指针给t的第二子节点成员child[1]
        t->son[1] = simple_exp();
    }
    // 函数返回表达式类型语法树节点t
    return t;
}


//   83/84/85  101/102  Vn  简单算术表达式
TreeNode *simple_exp()
{
    if (Error)
        return NULL;
    TreeNode *t = term();  //调用 term()

    // 当前单词token为加法运算符单词PLUS或MINUS
    while ((token.type == PLUS) || (token.type == MINUS))   //  AddOp  为 + | -
    {
        TreeNode *p = new TreeNode(linenum,ExpK); // newExpNode(OpK);  OpK表达式类型语法树节点
        p->kind.exp = OpK;
        if (p != NULL)
        {
            p->lineno = token.lineNum;
            p->son[0] = t;
            p->attr.ExpAttr.op = token.type;
            t = p; // 将函数返回值t赋成语法树节点p
            match(token.type);	// 当前单词token与指定加法运算单词(为PLUS或MINUS)匹配
            t->son[1] = term();// 调用 term(),返回语法树节点指针给t的第二子节点
        }
    }
    return t;
}



//   86/87/88  103/104 Vn  项
TreeNode *term()
{
    if (Error)
        return NULL;
    TreeNode *t = factor();	// 调用因子处理函数factor()

    while ((token.type == TIMES) || (token.type == OVER))  // 当前单词token为乘法运算符单词TIMES或OVER
    {
        TreeNode *p = new TreeNode(linenum,ExpK); 	// 创建新的OpK表达式类型语法树节点
        p->kind.exp = OpK;
        // 新语法树节点p创建成功,初始化第一个子节点成员child[0]为t
        // 将当前单词token赋值给语法树节点p的运算符成员attr.op
        if (p != NULL)
        {
            p->lineno = token.lineNum;
            p->son[0] = t;
            p->attr.ExpAttr.op = token.type;
            t = p;
        }

        match(token.type);		// 当前单词token与指定乘法运算符单词(为TIMES或OVER)匹配

        p->son[1] = factor();// 调用因子处理函数factor(),函数返回语法树节点指针赋给p第二个子节点成员son[1]
    }

    return t;
}



//    89/90/91  Vn  因子
TreeNode *factor()
{
    if (Error)
        return NULL;
    TreeNode *t = NULL;
    if (belong(90))
    {
        t = new TreeNode(linenum,ExpK); 	// 创建表达式类型语法树节点  具体节点
        t->kind.exp = ConstK;

        if ((t != NULL) && (token.type == INTC))
        {
            t->lineno = token.lineNum;
            t->attr.ExpAttr.val = atoi(token.name.c_str());
        }

        match(INTC);	// 当前单词token与数字单词NUM匹配
    }
    else if (belong(91))
        t = variable();
    else if (belong(89))
    {
        match(LPAREN);
        t = exp();
        match(RPAREN);
    }
    else
    {
        ReadOneToken(token);
        sprintf(tmpStrError,"不匹配的token !");
        syntaxError(tmpStrError);
    }
    return t;
}


//   92 Vn
TreeNode *variable()
{
    if (Error)
        return NULL;
    TreeNode *t = new TreeNode(linenum,ExpK);   // 表达式类型 具体节点
    t->kind.exp = VariK;  // 自定义 标识符
    if ((t != NULL) && (token.type == ID))
    {
        t->lineno = token.lineNum;
        t->name.push_back(token.name);
    }
    match(ID);   // ID
    variMore(t);  // 调用
    return t;
}



//   93/94/95 Vn
void variMore(TreeNode *t)
{
    if (Error)
        return;
    if (belong(93))  // 空语句
        t->attr.ExpAttr.varkind = IdV;  //guess here a bug
    else if (belong(94))
    {
        match(LMIDPAREN);  // [
        t->son[0] = exp();  // 调用 exp(), 求出其表达式的值，送入用于数组下标计算
        t->attr.ExpAttr.varkind = ArrayMembV;	// 此表达式为数组成员变量类型
        t->son[0]->attr.ExpAttr.varkind = IdV;
        match(RMIDPAREN);  // ]
    }
    else if (belong(95))
    {
        match(DOT);  // .
        t->son[0] = fieldvar();  	// 第一个儿子指向域成员变量结点
        t->attr.ExpAttr.varkind = FieldMembV;
        t->son[0]->attr.ExpAttr.varkind = IdV;
    }
    else
    {
        ReadOneToken(token);
        sprintf(tmpStrError,"不匹配的token !");
        syntaxError(tmpStrError);
    }
}

//   96 Vn
TreeNode *fieldvar()
{
    if (Error)
        return NULL;

    TreeNode *t = new TreeNode(linenum,ExpK);
    t->kind.exp = VariK;
    if ((t != NULL) && (token.type == ID))
    {
        t->lineno = token.lineNum;
        t->name.push_back(token.name);
    }
    match(ID);   // ID
    fieldvarMore(t);  // 调用
    return t;
}


//    97/98 Vn
void fieldvarMore(TreeNode *t)
{
    if (Error)
        return;
    if (belong(97))
        ;  // 空语句
    else if (belong(98))
    {
        match(LMIDPAREN);  // [
        // 用来以后求出其表达式的值，送入用于数组下标计算
        t->son[0] = exp();  // 调用 exp()
        t->son[0]->attr.ExpAttr.varkind = ArrayMembV;
        match(RMIDPAREN);  // ]
    }
    else
    {
        ReadOneToken(token);
        sprintf(tmpStrError,"不匹配的token !");
        syntaxError(tmpStrError);
    }
}

// ------------------------------- 递归下降 语法分析程序 ------------------------------------------------------

TreeNode * recursiveAnalyse()
{
    TreeNode *t = NULL;
    Error = false;
    ReadOneToken(token);
    t = program();
    if (token.type != ENDFILE){
        sprintf(tmpStrError,"程序结束符'.'后面有多余的字符 !");
        syntaxError(tmpStrError);
    }
    return t;
}


//---------------------------------- 对外接口 方便语义分析程序获取语法树 ----------------------------------------------
TreeNode * getParseTree(){
    // token 序列读进来
    tokens = getTokenList();
   // cout<< ">>> There are " << tokens.size() << " Tokens ."<< endl;

    initReadOneToken();
    initProducts();   // 千万不要忘了  init
    getAllPredict();

    if(tokens.size()==0)
        return NULL; // 如果词法分析错误, 那么不进行语法分析


    TreeNode *rt = recursiveAnalyse();
    if( !Error ){                   // 如果语法分析没有错误
        //printf(">>> RecursiveParserAnalyse is successful without errors!\n");
        FILE * fn=fopen(errorFileName,"a"); // 控制台信息输出到 debug_info 文件 , 除词法分析都是 a
        fputs(">>> 递归下降语法分析正确 !\n",fn);
        fclose(fn);      
    }
    else{
        rt = NULL;
    }
    const char fileName[] = "D:/CODE_Qt/txt/recursiveSyntaxTree.txt";
    outputParserTree(rt , fileName);  // 语法分析有错误的话, 树是空的

    return rt;
}
