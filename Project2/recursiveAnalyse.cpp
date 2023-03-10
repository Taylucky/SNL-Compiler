#include "recursiveAnalyse.h"
#include "lexerAnalyse.h"
#include "parseAnalyse.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>
using namespace std;

// ------------------------------- 语法树 ------------------------------------
// 采用松散形式输出语法树
#define OutTreeInsertBlank

#define MaxNodeKindsValue   int(NodeKinds::ExpK)
#define MaxDecKindsValue    int(DecKinds::IdK)
#define MaxStmtKindsValue   int(StmtKinds::ReturnK)
#define MaxExpKindsValue    int(ExpKinds::VariK)
#define MaxParamKindsValue  int(ParamKinds::VarparamType)
#define MaxVarKindsValue    int(VarKinds::FieldMembV)
#define MaxExpTypeValue     int(ExpType::Boolean)

string strNodeKinds[]= {"ProK","PheadK","TypeK","VarK","ProcDecK","StmLK","DecK","StmtK","ExpK"};
string strDecKinds[]= {"ArrayK","CharK","IntegerK","RecordK","IdK"} ;
string strStmtKinds[]= {"IfK","WhileK","AssignK","ReadK","WriteK","CallK","ReturnK"};
string strExpKinds[]= {"OpK","ConstK","VariK"} ;
string strParamKinds[]= {"ValparamType","VarparamType"} ;
string strVarKinds[]= {"IdV","ArrayMembV","FieldMembV"} ;
string strExpType[]= {"Void","Integer","Boolean"} ;

string getKindStr(NodeKinds nks,int onk=-1)    // 取得对应的 Kind 的字符串表示
{
    if(int(nks) < 0 || int(nks) > MaxNodeKindsValue) return string("");  // 不在 Tokentype 里
    if(onk<0)
        return strNodeKinds[nks];  // 默认是 NodeKind , 在 strNodeKinds[]里
    switch (nks)   // nks 已经确定是哪一种 NodeKind 了,要去对应类型 str 数组找 string 
    {
    case DecK:
        if(onk <= MaxDecKindsValue)
            return strDecKinds[onk];  // onk >= 0 是数组下标
        break;
    case StmtK:
        if(onk <= MaxStmtKindsValue)
            return strStmtKinds[onk];
        break;
    case ExpK:
        if(onk<=MaxExpKindsValue)
            return strExpKinds[onk];
        break;
    case VarK:
        if(onk <= MaxVarKindsValue )
            return strVarKinds[onk];
        break;
    case PheadK :
        if(onk <=MaxParamKindsValue)
            return strParamKinds[onk];
        break;
    case TypeK:
        if(onk <= MaxExpTypeValue)
            return strExpType[onk];
        break;
    default:
        return string("  ---Error");
    }
    return string(" ");
}


static  vector<string> outTree;   // 保存要输出的语法树字符串信息，一个字符串为一行

#define IndentSeg   4   // 缩进的空格个数
static int indent = 0; // 记录语法树输出时的缩进

string getBlankStr(int n)   // 得到对应长度的 空白字符串 和 |___ 来表示缩进
{
    string t = "";
    for(int i = 0; i < n-IndentSeg; i++)  // 输出缩进 空白字符串
        t = t + " ";

    if(n)
        t += "|___";  // 除了 ProK 节点, 每一个节点在输出之前 都要先输出 |___
    return t;
}


void inserNodeInfo(const string str)   // 插入一个要输出的语法树节点
{
    outTree.push_back(str);
	//outTree.push_back(string("\n"));
	// "|                                            " , | 是由 beautifyOutTree() 添加
	// 前面要空这么多空格,(最起码保证超过了最长的那一行输出) , 是因为 调用 beautifyOutTree() 美化时需要 把空格 换成 | 
    outTree.push_back(string("                                                                            \n"));
}

void beautifyOutTree()
{
    for(int i = outTree.size()-1; i >= 0 ; i--)     // 添加一个 | 在 "              \n" 之前                
    {
        int tag = -1;
        for(int j = 0; j < outTree[i].size(); j += IndentSeg)
            if(outTree[i][j]=='|' && outTree[i-1][j]==' ')   
            {               // 找到第 i 行第 j 列的 | , 其上面空的一行 对应位置 如果是空格则标记
                tag = j;
                break;
            }
        if(tag != -1)
        {
            for(int j = i-1; j; j--)     
                if(outTree[j][tag] == ' ')
                    outTree[j][tag] = '|'; // 把 | 那一行上面的所有 空格都换成 | , 这样更美观
                else break;
        }
    }
}

string getIntStr(int n)  // 将整数转为字符串 
{
    char t[20];
    sprintf(t,"%d",n);    // 123 -> "123"
    return string(t);
}

static bool _outParam = false;  // 过程声明时输出参数

void _outputOneNode(TreeNode * rt)       // 输出语法树的一个节点  
{
    if(rt == NULL)
		return ;
    string tmpStr = getBlankStr(indent);  // 获取前面的缩进 
    switch(rt->nodekind)
    {
	case PheadK:
    case ProK :
    case TypeK:
    case StmLK:
        tmpStr += getKindStr(rt->nodekind);    // 前面三个 case 没有 break, 会自动执行这里
        break;
    case VarK :
        tmpStr += getKindStr(rt->nodekind)+" ";
        break;
    case ProcDecK:
        tmpStr += getKindStr(rt->nodekind)+" ";
        for(string t : rt->name)
            tmpStr = tmpStr + t+" ";
        break;
    case StmtK:
        tmpStr = tmpStr + "StmtK " + getKindStr(StmtK,rt->kind.stmt);
        break;
    case ExpK:
        tmpStr = tmpStr + "ExpK ";//+getKindStr(TypeK,rt->attr.ExpAttr.type)+" ";
        for(string t : rt->name)
            tmpStr = tmpStr + t + " ";
        if(rt->kind.exp == ExpKinds::OpK)
        {
            tmpStr += getTokenStr(rt->attr.ExpAttr.op) + " ";  
        }
        else if(rt->kind.exp == ExpKinds::ConstK)
        {
            tmpStr += getIntStr(rt->attr.ExpAttr.val) + " ";
        }
        else  if(rt->kind.exp == ExpKinds::VariK)
        {
           ;               // tmpStr+=getKindStr(NodeKinds::VarK,rt->attr.ExpAttr.varkind)+" ";//IdV
        }
        tmpStr+=getKindStr(NodeKinds::ExpK,rt->kind.exp)+" ";
        break;
    case DecK:
        tmpStr = tmpStr+getKindStr(rt->nodekind)+" ";
        tmpStr = tmpStr + getKindStr(DecK,rt->kind.dec) + " " + rt->attr.type_name + ": ";
        for(string t:rt->name)
            tmpStr = tmpStr + t + " ";
        
        if(_outParam)
            tmpStr += getKindStr(PheadK,rt->attr.ProcAttr.param) + " ";
        break;
    default:
        break;
    }
    inserNodeInfo(tmpStr + "\n");
}


void _outputParserTree(TreeNode * rt)       // 递归输出整个语法树
{
    if(rt == NULL) return;
    _outputOneNode(rt);  // 每输出一个节点,
    indent += IndentSeg; //  对于儿子节点, 要 indent + 4
    for(int i = 0; i < MAXSON; i++)
        if(rt->son[i])
        {
            _outParam = rt->nodekind == NodeKinds::ProcDecK;  
            _outputParserTree(rt->son[i]);   // 先输出儿子节点
        }
    indent -= IndentSeg;  // 输出完儿子节点, 再 indent - 4
    _outputParserTree(rt->sibling);  // 再输出兄弟节点
}

void outputParserTree(TreeNode * rt)    // 输出语法树到屏幕
{
    indent = 0;
    _outParam = false;
    outTree.clear();

    _outputParserTree(rt);
    beautifyOutTree();

    for(string t : outTree)
        printf("%s",t.c_str());  // 直接输出
}

void outputParserTree(TreeNode *rt,const char fileName[])   // 输出语法树到文件
{
    indent = 0;
    _outParam = false;
    outTree.clear();

    _outputParserTree(rt);
    beautifyOutTree();

    FILE *fn = fopen(fileName,"w");
    for(string t : outTree)                       //将 outTree 中的字符串输出到文件  即语法树结构
        fprintf(fn,"%s",t.c_str());  
    fclose(fn);
}


// ---------------------------------- 语法分析 -----------------------------------------
// 函数声明 , 互相调用
TreeNode *program();
TreeNode *varDec();
void match(TokenType tkt);
void syntaxError(char *message);
TreeNode *programHead();
TreeNode *declarePart();
TreeNode *programBody();
TreeNode *procDec();
TreeNode *typeDec();
TreeNode *typeDeclaration();
TreeNode *typeDecList();
TreeNode *typeDecMore();
void typeDef(TreeNode *t);
TreeNode *fieldDecList();
bool belong(const int &n);
void baseType(TreeNode *t);
void strutureType(TreeNode *t);
void arrayType(TreeNode *t);
void idMore(TreeNode *t);
void recType(TreeNode *t);
void idList(TreeNode *t);
TreeNode *fieldDecMore();
TreeNode *varDeclaration();
TreeNode *varDecList();
void varIdList(TreeNode *t);
void varIdMore(TreeNode *t);
TreeNode *procDeclaration();
TreeNode *varDecMore();
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

#define TreeNode(nkk) TreeNode(linenum, nkk)
#define defaulttoken                              \
	{                                             \
		ReadOneToken(token);                      \
		syntaxError("unexpected token is here!"); \
	}
// defaulttoken 是类型匹配不正确的 token

static int linenum = 0;
static Token token;
static FILE *listing;
static string temp_name;
static bool Error = false;

void printnowtoken()
{
	cout << token.name << endl
		 << token.lineNum << endl;
}

void match(TokenType tkt)
{
	if(token.type == tkt)   // 类型匹配 
		ReadOneToken(token);
	else   // token 类型不匹配
	{
		syntaxError("not match error ");
		fprintf(listing, "'%s'\n", token.name.c_str());

		printf("\nSyntax Error At Line:%d ,You Can Get The Reason In the Output File.\n", token.lineNum);
		fprintf(listing, "\nexpected: %s\nbut now is:%s\n", getTokenStr(tkt).c_str(), getTokenStr(token.type).c_str()); 
		ReadOneToken(token);          
	}
}

void syntaxError(char* message)
{
	fprintf(listing, "\n>>> error :   ");
	fprintf(listing, "Syntax error at line %d: %s\n", token.lineNum, message);
	Error = true;   // 唯一一个会改变 Error 值的地方
}

bool belong(const int &n)  // 每一个产生式左边的 Vn 是唯一的,所以根据产生式是哪一条 n, 去找对应的 Predict[n]
{
	int tokenSize = count(Predict[n].begin(),Predict[n].end(),token.type);  // 查找目标值为 token.type 的个数
	// int tokenSize = Predict[n].count(token.type); // token 是全局变量,指向 当前访问的 token  
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

	TreeNode *root = new TreeNode(ProK);  // ProK 语法树根节点   // #define TreeNode(nkk) TreeNode(linenum, nkk)
	if (root != NULL)
	{
		root->lineno = 0;
		if (t != NULL)
			root->son[0] = t;   // 总程序树  有三个分支    
		else
			syntaxError("A program head is expected!");
		if (q != NULL)
			root->son[1] = q;  // 声明部分可以没有
		if (s != NULL)
			root->son[2] = s;
		else
			syntaxError("A program body is expected!");
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
	TreeNode *t = new TreeNode(PheadK);             // 节点类型  程序头
	match(PROGRAM);
	if ((t != NULL) && (token.type == ID))      // programName::=ID 直接写一起了
	{
		t->lineno = 0;
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
	TreeNode *tp;

	tp = typeDec();              // 类型声明
	TreeNode * type = NULL;        
	if (tp != NULL)                  
	{                                 
		type = new TreeNode(TypeK);
		type->son[0] = tp;        
		type->lineno = 0;           
	}

	tp = varDec();            // 变量声明      
	TreeNode *var = NULL;         
	if (tp != NULL)                   
	{                                  
		var = new TreeNode(VarK); 
		var->son[0] = tp;          
		var->lineno = 0;          
	}

	TreeNode *proc = procDec();    // 过程声明

	if (var != NULL)   
		var->sibling = proc;
	else
		var = proc;     // var = null 了 , 执行了这儿  !!!!!!!!!!!!!!!!!!!!!!
	if (type != NULL)
		type->sibling = var;
	else
		type = var;   // type = null 了, 执行了这儿  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	return type;
}


// -----------------------------------  类型声明  -------------------------------------

//    5/6 的 Vn
TreeNode *typeDec()
{
	if (Error)
		return NULL;
	TreeNode *t = NULL;
//	int tmp = belong(5);                      // I'll see here  !!!!! ok

	if (belong(6))
		t = typeDeclaration(); // 调用
	else if ( !(belong(5)) )  // 5是空语句  
		defaulttoken;     // 进入了这里, 然后就改变了 Error !!!!!  ok
	return t;
}

//   7 的 Vn
TreeNode *typeDeclaration()
{
	if (Error)
		return NULL;
	match(TYPE);				 // 匹配 type
	TreeNode *t = typeDecList(); // 调用
	if (t == NULL)
		syntaxError("A type declaration is expected!"); // 语法错误  type 后缺失类型声明
	return t;
}

//   8 的 Vn
TreeNode *typeDecList()
{
	if (Error)
		return NULL;
	TreeNode *t = new TreeNode(DecK); // 节点类型 类型声明
	if (t != NULL)
	{
		t->lineno = token.lineNum;
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
	else if (!belong(9))
		defaulttoken; // 9 是空语句
	return t;
}

//    12/13/14 的 Vn
void typeDef(TreeNode *t)                  //   typeDef 就是  typeName
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
		t->attr.type_name = token.name; //  ID  自定义类型
		match(ID);
	}
	else
		defaulttoken;
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
		defaulttoken;
}

// -------------  结构类型  -------------------
//   17/18 的 Vn
void strutureType(TreeNode *t)
{
	if (Error)
		return;
	if (belong(17))
		arrayType(t); // 调用 数组类型
	else if (belong(18))
		recType(t); // 调用  记录类型
	else
		defaulttoken;
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
		t->son[0] = p; // 儿子节点   记录 体        
	else
		syntaxError("a record body is requested!");
	match(END); // 匹配关键字  end
}

// ----------- 记录类型里面的内容 -----------------
//   23/24的 Vn
TreeNode *fieldDecList()
{
	if (Error)
		return NULL;
	TreeNode *t = new TreeNode(DecK); 
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
			defaulttoken;
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
		defaulttoken;
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
		defaulttoken;
}

// --------------------------------  变量声明  ------------------------------------

//   30/31 的 Vn
TreeNode *varDec()
{
	if (Error)             // bug is here !!!!!!!!!!!!!  when the Error changed
		return NULL;
	TreeNode *t = NULL;
	if (belong(30))
		; // 空语句 可能没有变量声明
	else if (belong(31))             
		t = varDeclaration(); // 调用变量声明 Vn 的函数
	else
		defaulttoken;
	return t;
}

//   32 的 Vn
TreeNode *varDeclaration()
{
	if (Error)
		return NULL;
	match(VAR);					// 匹配保留字 VAR  表示变量声明的开始
	TreeNode *t = varDecList(); // 调用
	if (t == NULL)
		syntaxError("a var declaration is expected!"); // 语法错误
	return t;
}

//   33 的 Vn
TreeNode *varDecList()
{
	if (Error)
		return NULL;
	TreeNode *t = new TreeNode(DecK); // 创建了一个  Deck 类型的节点
	TreeNode *p = NULL;
	if (t != NULL)
	{
		t->lineno = token.lineNum;
		typeDef(t);                    //   typeDef   也是  TypeName()   
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
		defaulttoken;
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
		syntaxError("a varid is expected here!"); // 应该是 标识符 ,却没有匹配的标识符   语法错误  !
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
		defaulttoken;
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
		defaulttoken;
	return t;
}

//   41 / 44 Vn
TreeNode *procDeclaration()
{
	if (Error)
		return NULL;
	TreeNode *t = new TreeNode(ProcDecK);        // 节点类型   过程声明节点

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
		defaulttoken;
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
		if (t == NULL)
			syntaxError("A param declaration is required!");
	}
	else
		defaulttoken;
	return t;
}



//   50/51 的 Vn
TreeNode *param()
{
	if (Error)
		return NULL;
	TreeNode *t = new TreeNode(DecK);
	if (t != NULL)
	{
		t->lineno = token.lineNum;

		if (belong(50))
		{
			t->attr.ProcAttr.param = ValparamType;       //节点  参数类型    值参
			typeDef(t);   
			formList(t);
		}
		else if (belong(51))
		{
			match(VAR);
			t->attr.ProcAttr.param = VarparamType;    // 形参 
			typeDef(t);
			formList(t);
		}
		else
			defaulttoken;
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
		t->name.push_back(token.name); // ID
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
		defaulttoken;
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
	if (t == NULL)
		syntaxError("A program body is requested!");
	return t;
}

//--------------------------------  程序体  -------------------------------------------
//    57 的 Vn 
TreeNode *programBody()
{
	if (Error)
		return NULL;
	TreeNode *t = new TreeNode(StmLK); // newStmlNode();    // 节点类型
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
		defaulttoken;
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
		defaulttoken;
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
		defaulttoken;
	return t;
}



//   69 Vn  赋值语句
TreeNode *assignmentRest() 
{
	if (Error)
		return NULL;
	TreeNode *t = new TreeNode(StmtK);
	t->kind.stmt = AssignK;
	// 赋值语句节点的第一个儿子节点记录赋值语句的左侧变量名
	// 第二个儿子结点记录赋值语句的右侧表达式
	if (t != NULL)
	{
		t->lineno = token.lineNum;
		//处理第一个儿子结点，为变量表达式类型节点
		TreeNode *son1 = new TreeNode(ExpK); // newExpNode(VariK);
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
	TreeNode *t = new TreeNode(StmtK); // newStmtNode(IfK);
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
	if (token.type == ELSE)    // else     PPT上少了 else 语句
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
	TreeNode *t = new TreeNode(StmtK); // newStmtNode(WhileK);
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
	TreeNode *t = new TreeNode(StmtK); // newStmtNode(ReadK);
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
	TreeNode *t = new TreeNode(StmtK); // newStmtNode(WriteK);
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
	TreeNode *t = new TreeNode(StmtK); // newStmtNode(ReturnK);           
	t->kind.stmt = ReturnK;
	match(RETURN);  // return 
	if (t != NULL)
		t->lineno = token.lineNum;                               // ?????????   ReturnStm ::= RETURN ( Exp )  
	return t;
}


//   76 Vn   函数调用语句
TreeNode *callStmRest()
{
	if (Error)
		return NULL;
	TreeNode *t = new TreeNode(StmtK); // newStmtNode(CallK);
	t->kind.stmt = CallK;
	match(LPAREN);
	//函数调用时，其子节点指向实参
	if (t != NULL)
	{
		t->lineno = token.lineNum;
		//函数名的结点也用表达式类型结点
		TreeNode *son0 = new TreeNode(ExpK); // newExpNode(VariK);
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
		defaulttoken;
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
		defaulttoken;
	return t;
}



// ----------------- 表达式 ---------------------

//   81/82  99/100 条件表达式  < | =
TreeNode *exp()
{
	if (Error)
		return NULL;
	// 调用简单表达式处理函数simple_exp(),返回语法树节点指针给t 
	TreeNode *t = simple_exp();
	// 当前单词token为逻辑运算单词LT或者EQ 
	if ((token.type == LT) || (token.type == EQ))
	{
		// 创建新的OpK类型语法树节点，新语法树节点指针赋给p 
		TreeNode *p = new TreeNode(ExpK); // newExpNode(OpK);
		p->kind.exp = OpK;
		// 新语法树节点p创建成功,初始化p第一个子节点成员child[0]
		 // 并将当前单词token(为EQ或者LT)赋给语法树节点p的运算符成员attr.op
		if (p != NULL)
		{
			p->lineno = token.lineNum;
			p->son[0] = t;
			p->attr.ExpAttr.op = token.type;
			// 将新的表达式类型语法树节点p作为函数返回值t 
			t = p;
		}
		// 当前单词token与指定逻辑运算符单词(为EQ或者LT)匹配 
		match(token.type);
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
	// 调用元处理函数term(),函数返回语法树节点指针给t 
	TreeNode *t = term();  //调用 term()
	// 当前单词token为加法运算符单词PLUS或MINUS 
	while ((token.type == PLUS) || (token.type == MINUS))   //  AddOp  为 + | -
	{
		// 创建新OpK表达式类型语法树节点，新语法树节点指针赋给p 
		TreeNode *p = new TreeNode(ExpK); // newExpNode(OpK);
		p->kind.exp = OpK;
		// 语法树节点p创建成功,初始化p第一子节点成员child[0]	
		// 返回语法树节点指针给p的运算符成员attr.op				
		if (p != NULL)
		{
			p->lineno = token.lineNum;
			p->son[0] = t;
			p->attr.ExpAttr.op = token.type;
			// 将函数返回值t赋成语法树节点p 
			t = p;
			// 当前单词token与指定加法运算单词(为PLUS或MINUS)匹配 
			match(token.type);
			// 调用元处理函数term(),函数返回语法树节点指针给t的第二子节点成员child[1] 
			t->son[1] = term();
		}
	}
	// 函数返回表达式类型语法树节点t 
	return t;
}



//   86/87/88  103/104 Vn  项
TreeNode *term()
{
	if (Error)
		return NULL;
	// 调用因子处理函数factor(),函数返回语法树节点指针给t 
	TreeNode *t = factor();
	// 当前单词token为乘法运算符单词TIMES或OVER 
	while ((token.type == TIMES) || (token.type == OVER))
	{
		// 创建新的OpK表达式类型语法树节点,新节点指针赋给p 
		TreeNode *p = new TreeNode(ExpK); // newExpNode(OpK);
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
		// 当前单词token与指定乘法运算符单词(为TIMES或OVER)匹配 
		match(token.type);
		// 调用因子处理函数factor(),函数返回语法树节点指针赋给p第二个子节点成员child[1] 
		p->son[1] = factor();
	}
	// 函数返回表达式类型语法树节点t 
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
		// 创建新的ConstK表达式类型语法树节点,赋值给t
		t = new TreeNode(ExpK); // newExpNode(ConstK);
		t->kind.exp = ConstK;

		if ((t != NULL) && (token.type == INTC))
		{
			t->lineno = token.lineNum;
			t->attr.ExpAttr.val = atoi(token.name.c_str());
		}
		// 当前单词token与数字单词NUM匹配 
		match(INTC);
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
		defaulttoken;
	return t;
}


//   92 Vn 
TreeNode *variable()
{
	if (Error)
		return NULL;
	TreeNode *t = new TreeNode(ExpK); // newExpNode(VariK);
	t->kind.exp = VariK;
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
		;
	else if (belong(94))
	{
		match(LMIDPAREN);  // [ 
		// 用来以后求出其表达式的值，送入用于数组下标计算
		t->son[0] = exp();  // 调用 exp()
		t->attr.ExpAttr.varkind = ArrayMembV;
		// 此表达式为数组成员变量类型 
		t->son[0]->attr.ExpAttr.varkind = IdV;
		match(RMIDPAREN);  // ] 
	}
	else if (belong(95))
	{
		match(DOT);  // .
		// 第一个儿子指向域成员变量结点
		t->son[0] = fieldvar();   // 调用 
		t->attr.ExpAttr.varkind = FieldMembV;
		t->son[0]->attr.ExpAttr.varkind = IdV;
	}
	else
		defaulttoken;
}

//   96 Vn
TreeNode *fieldvar()
{
	if (Error)
		return NULL;
	// 注意，可否将此处的IdEK改为一个新的标识，用来记录记录类型的域
	TreeNode *t = new TreeNode(ExpK); // newExpNode(VariK);
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
	else if (belong(98)) // case LMIDPAREN:
	{
		match(LMIDPAREN);  // [ 
		// 用来以后求出其表达式的值，送入用于数组下标计算 
		t->son[0] = exp();  // 调用 exp()
		t->son[0]->attr.ExpAttr.varkind = ArrayMembV;
		match(RMIDPAREN);  // ]
	}
	else
		defaulttoken;
}

// ------------------------------- 递归下降 语法分析程序 ------------------------------------------------------

TreeNode * recursiveAnalyse()
{
	listing = fopen("D:/txt/recursiveErrorOut.txt", "w");
	TreeNode *t = NULL;
	Error = false;
	ReadOneToken(token);
	t = program();
	if (token.type != ENDFILE)
		syntaxError("Code ends before file\n");
	if (Error)
	{
		t = NULL;
	}
	fclose(listing);
	return t;
}



// ------------------------------- 递归下降 语法分析测试 main ---------------------------------------------------
 /*static vector<Token> tokens;  // token 序列
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
 }*/


 /*int main() {
     // token 序列读进来
 	tokens = getTokenList();
 	 cout<< tokens.size()<< endl;
	
 	initReadOneToken();
 	initProducts();   // 千万不要忘了  init !!!!!
 	initPredict();

  TreeNode *rt = recursiveAnalyse();
 	const char fileName[] = "D:/txt/recursiveSyntaxTree.txt";
 	outputParserTree(rt , fileName);
     return 0;
 }*/



// 虽然包含了头文件,但是 却不能用头文件里的函数  undefined reference to    -->   配置 vscode 多个cpp同时编译
