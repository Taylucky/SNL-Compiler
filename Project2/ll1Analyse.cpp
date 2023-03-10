#include "ll1Analyse.h"
#include "lexerAnalyse.h"
#include "parseAnalyse.h"
#include <cstring>
#include <iostream>
#include "recursiveAnalyse.h"
using namespace std;
static vector<Token> tokens;  // token 序列
static size_t tkPtr = 0;  // Token指针

// ------------------------------------ 构建 LL(1) 符号表 --------------------------------------------------
#define MaxTerminalValue int(LAMBDA)  // 最后一个终极符的 编号
#define MaxSymbolValue int(MultOp)  // 最后一个 TokenType (含终极符和非终极符)的编号
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
static Token currentToken;  // 当前访问到的 token

static vector<Symbol> symbolStack;     // 符号栈 ,存放 语法符号 Vt 和 Vn
static vector<TreeNode **> syntaxStack;        // 语法树栈 , 存放指向儿子或兄弟节点的指针的地址 处理声明部分和语句部分
static vector<TreeNode *> operatorStack;     // 操作符栈  
static vector<TreeNode *> numStack;     // 操作数栈 

static TreeNode * rt = NULL;  // 语法树根指针
static TreeNode ** tmpPtrPtr;  // 语法树节点指针地址临时变量 , 用于承接 语法树栈 弹出的
static TreeNode * curTreePtr; // 当前节点指针
DecKinds* decTemp;
SArrayAttr* arrTemp;//可以不加struct吗？？？
TreeNode* saveP;
TreeNode* saveFuncP;
bool getExpResult=true;//??
int getExpResult2=false;
int expflag=0;
TokenType sTop;
TreeNode* t;
//--------------- LL(1) 驱动程序 -----------------

bool ReadOneToken(Token & tk);

bool match(Symbol s){
    if(s == currentToken.type) return true;
    return false;
}

int findLL1Table(Symbol s,struct Token& tk){
    // 用 符号栈栈顶 s 和 输入流头符 tk 去查LL(1)分析表, 返回产生式编号
    return LL1Table[s][tk.type];
}

void insertError(string error){  // 插入语法分析错误信息 
    errors.push_back(error);
}

// todo : getPredictStr()

static void (*funcs[ProductExpressionNum+5])() = {NULL};   // 对应于每条产生式的函数指针数组
void initParserLL1Funcs();

void predict(int num){  // 根据产生式编号选择一个要执行的函数
    funcs[num]();
}


bool LL1parse(){            // LL(1)驱动程序 , 返回真假值表示是否出现了语法错误   P107
    initParserLL1Funcs();
    createLL1Table();
    errors.clear();
    symbolStack.clear();
    operatorStack.clear();
    numStack.clear();
    syntaxStack.clear();
 
    symbolStack.push_back(Program);
    rt = new TreeNode(0,ProK);   // 根节点
    for(int i = 2; i >= 0; i--)
        syntaxStack.push_back(&(rt->son[i]));  // 三个儿子节点压入语法树栈

    ReadOneToken(currentToken);
    while(!symbolStack.empty()){
        Symbol  tmpSymbol = symbolStack.back(); 
        symbolStack.pop_back();
       if( tmpSymbol <= MaxTerminalValue){  // 如果是终极符   则匹配
            if(match(tmpSymbol))
                ReadOneToken(currentToken);  // 匹配成功,读入下一个 token
            else{   // 匹配不成功, 报错 
                sprintf(strError,"syntax error : unexpected tokentype= '%s' name= %s found at line %d,\n\
                        expected Token type is %s.\n",
                        getTokenStr(currentToken.type).c_str(),currentToken.name.c_str(),
                        currentToken.lineNum,getTokenStr(tmpSymbol).c_str());
                insertError(strError);
                return false;
            }
        }
        else {   // 是 非终极符 , 则用栈顶和输入流头符去查表
            int n = findLL1Table(tmpSymbol, currentToken);
            if( n == -1){  // 错误编号, 报错
                sprintf(strError,"syntax error : unexpected tokentype= '%s' name= %s found at line %d.\n",
                                        getTokenStr(currentToken.type).c_str(),currentToken.name.c_str(),
                                        currentToken.lineNum);
                insertError(strError);
                return false;
            }
            else{     // 得到正确产生式编号, 将其右部逆序压入栈
                if(n < 1 || n > ProductExpressionNum)   return false;
                for (int i = products[n].size() - 1; i > 0; i--) {       // 右部 逆序入栈
                    if (products[n][i] != LAMBDA) {           // lambda 不入符号栈
                        //cout << "in:" << n << " "<<products[n][i]<<endl;
                        symbolStack.push_back(products[n][i]);
                    }
                }
                predict(n); //选择执行那条产生式对应的函数
            }
        }
    }
    // 符号栈已空 , 输入流中还有 程序结束符 . 和 文件结束符 EOF
    // 匹配 结束符 
    if( !match(DOT)){
        sprintf(strError,"syntax error : unexpected token found at line %d,Expected Dot\n",currentToken.lineNum);
        insertError(strError);
        return false;
    }else if(!ReadOneToken(currentToken)){
        sprintf(strError,"syntax error : the token EOF is missed in tokenList\n");
        insertError(strError);
        return false;
    }

    if( match(ENDFILE) ) 
        return true;  // 文件终止符
    else {
        sprintf(strError,"syntax error : unexpected token '%s' found at line %d,Expected EOF\n"
                ,getTokenStr(currentToken.type).c_str(),currentToken.lineNum);
        insertError(strError);
        return false;
    }
    return errors.size()==0;
}





int priosity(TokenType op){
    if (op == END) return 0;
    if (op == LPAREN) return 1;
    if (op == LT || op == EQ) return 2;
    if (op == PLUS || op == MINUS) return 3;
    if (op == TIMES || op == OVER) return 4;
    else {
        
        sprintf(strError, "No such operator and no priosity for it.syntax error : unexpected token '%s' found at line %d,Expected Operator\n"
            , getTokenStr(op).c_str(), currentToken.lineNum);
        insertError(strError);
        return -1;
    }
}


// ---------------------------------- 各个产生式处理函数 见 P111 -----------------------------------------------------
// 注意 : P111 process1() 虽然说要 右部非终极符逆序入符号栈, 但其实在 LL1parse()中已经入过符号栈了
// 所以对于 process1() , 语法树无动作, 那就什么都不用做   ???
// 后面的processn()都是这样, 只需要处理语法树动作即可

void process1(){  // Program ::= ProgramHead DeclarePart ProgramBody
   // 无需动作
}

void process2(){  // ProgramHead ::= PROGRAM ProgramName
    curTreePtr = new TreeNode(currentToken.lineNum, PheadK);  // 生成程序头节点
    tmpPtrPtr = syntaxStack.back(); // 弹语法树栈
    syntaxStack.pop_back();  // 由于逆序弹入语法树栈, 此时栈里弹出的刚好是 指向 ProgramHead 节点的指针
    (*tmpPtrPtr) = curTreePtr; // 对弹出元素进行赋值, 使得语法树根节点的 儿子节点指向程序头节点
}

void process3(){   // ProgramName ::= ID
    curTreePtr->name.push_back(currentToken.name);   // 当前标识符的语义信息 (标识符名) 写入程序头结点, 即当前结点
    // 这里书上说 标识符个数+1 其实name 是vector<string> 结构, 直接 name.size()就可以知道标识符个数了, 所以不存标识符个数
}

void process4(){  // DeclarePart ::= TypeDec VarDec ProcDec
    // 语法树部分没有动作
}
void process5(){}   // TypeDec ::= LAMBDA
void process6(){}   // TypeDec ::= TypeDeclaration

void process7(){    // TypeDec ::= TYPE TypeDecList
    curTreePtr = new TreeNode(currentToken.lineNum,TypeK); // 生成类型声明标志节点
    tmpPtrPtr = syntaxStack.back(); // 弹语法树栈
    syntaxStack.pop_back();  
    (*tmpPtrPtr) = curTreePtr; // 令指针指向此声明节点, 使得此节点作为根节点的儿子节点
    syntaxStack.push_back(&(curTreePtr->sibling));  // 当前节点的兄弟节点入语法树栈
    syntaxStack.push_back(&(curTreePtr->son[0]));  // 当前节点的第一个儿子节点入语法树栈
}

void process8(){   // TypeDecList ::= TypeId EQ TypeName SEMI TypeDecMore
    curTreePtr = new TreeNode(currentToken.lineNum,DecK); // 生成类型声明节点
    tmpPtrPtr = syntaxStack.back(); // 弹语法树栈
    syntaxStack.pop_back();  
    (*tmpPtrPtr) = curTreePtr; // 令指针指向此声明类型节点
    syntaxStack.push_back(&(curTreePtr->sibling));  // 压入当前节点的兄弟指针
}

void process9(){   // TypeDecMore ::= LAMBDA
    syntaxStack.pop_back();
}
void process10(){}//TypeDecMore::=TypeDecList

void process11(){  // TypeId ::= ID
    curTreePtr->name.push_back(currentToken.name);   // 标识符名存入节点中
}
void process12() {// TypeDef:=BaseType
     decTemp = &(curTreePtr->kind.dec);
}
void process13() {}//TypeDef:=StructureType
void process14() {//TypeDef ::= ID
    curTreePtr->kind.dec = IdK;
    curTreePtr->attr.type_name = currentToken.name.c_str();
}
void process15() {//BaseType:=INTEGER
    //curTreePtr->kind.dec = IntegerK;
    (*decTemp) = IntegerK;
}
void process16() {//BaseType:=CHAR
    (*decTemp) = CharK;
    //curTreePtr->kind.dec = CharK;
}
void process17() {}//StructureType:=ArrayType
void process18() {}//StructureType:=RecType
void process19() {//ArrayType	::=	ARRAY [low..top ] OF BaseType
    curTreePtr->kind.dec = ArrayK;
    //(*decTemp) = ArrayK;
    arrTemp = &(curTreePtr->attr.ArrayAttr);
    decTemp = &curTreePtr->attr.ArrayAttr.memtype;
}
void process20() {//Low::=INTC
    arrTemp->low = atoi(currentToken.name.c_str());
}
void process21() {//Top::=INTC
    arrTemp->up = atoi(currentToken.name.c_str());
}

void process22() {//RecType::=RECORD FieldDecList END
   //(*decTemp) = RecordK;
    curTreePtr->kind.dec = RecordK;
    saveP=curTreePtr;//Right???
    syntaxStack.push_back(&(curTreePtr->son[0]));//第一个儿子节点吗？？？
}
void process23() {//FieldDecList::=BaseType IdList ; FieldDecMore
    curTreePtr = new TreeNode(currentToken.lineNum, DecK);
    tmpPtrPtr = syntaxStack.back(); 
    syntaxStack.pop_back();
    (*tmpPtrPtr) = curTreePtr;
    decTemp= &(curTreePtr->kind.dec);
    syntaxStack.push_back(&(curTreePtr->sibling));
}
void process24() {//FieldDecList::=ArrayType IdList ; FieldDecMore
    curTreePtr = new TreeNode(currentToken.lineNum, DecK);
    tmpPtrPtr = syntaxStack.back();
    syntaxStack.pop_back();
    (*tmpPtrPtr) = curTreePtr;
    syntaxStack.push_back(&(curTreePtr->sibling));
}
void process25(){//FieldDecMore::=LAMBDA
    syntaxStack.pop_back();
    curTreePtr = saveP;
}
void process26() {}//FieldDecMore::=FieldDecList
void process27() {//FieldDecMore::=ID IdMore
    curTreePtr->name.push_back(currentToken.name);   // 标识符名存入节点中
}
void process28() {}//IdMore::=LAMBDA
void process29() {}//IdMore::=， IdList
void process30() {}//VarDec::=LAMBDA
void process31() {}//VarDec::=VarDeclaration
void process32() {// VarDec ::= VAR VarDecList
    curTreePtr = new TreeNode(currentToken.lineNum, VarK); // 生成类型声明标志节点
    tmpPtrPtr = syntaxStack.back(); // 弹语法树栈
    syntaxStack.pop_back();
    (*tmpPtrPtr) = curTreePtr; // 令指针指向此声明节点, 使得此节点作为根节点的儿子节点
    syntaxStack.push_back(&(curTreePtr->sibling));  // 当前节点的兄弟节点入语法树栈
    syntaxStack.push_back(&(curTreePtr->son[0]));  // 当前节点的第一个儿子节点入语法树栈
}
void process33() {  // 	VarDecList ::= TypeName	VarIdList ;  VarDecMore
    curTreePtr = new TreeNode(currentToken.lineNum, DecK); // 生成类型声明标志节点
    tmpPtrPtr = syntaxStack.back(); // 弹语法树栈
    syntaxStack.pop_back();
    (*tmpPtrPtr) = curTreePtr;
    syntaxStack.push_back(&(curTreePtr->sibling));
}
void process34() {   // VarDecMore::= LAMBDA
    syntaxStack.pop_back();
}
void process35() {}//VarDecMore::=VarDecList
void process36() {//VarIdList::=id VarIdMore
    curTreePtr->name.push_back(currentToken.name);
}
void process37() {}//VarIdMore::=LAMBDA
void process38() {}///VarIdMore::=, VarIdList
void process39() {}//ProcDec::==LAMBDA
void process40() {}//ProcDec::==ProcDeclaration

void process41() {
    curTreePtr = new TreeNode(currentToken.lineNum, ProcDecK);//??  
    //saveFuncP=curTreePtr;//??
    tmpPtrPtr = syntaxStack.back(); 
    syntaxStack.pop_back();
    (*tmpPtrPtr) = curTreePtr; // 令指针指向此声明节点, 使得此节点作为根节点的儿子节点
    syntaxStack.push_back(&(curTreePtr->sibling));  
    for (int i = 2; i >= 0; i--)
        syntaxStack.push_back(&(curTreePtr->son[i]));  // 三个儿子节点压入语法树栈
}
void process42() {}//ProcDecMore::=LAMBDA
void process43() {}//ProcDecMore::=ProcDeclaration
void process44() {//ProcName	 ::=	ID
    curTreePtr->name.push_back(currentToken.name);   // 当前标识符的函数名 写入函数头结点, 即当前结点
}
void process45() {//ParamList::==LAMBDA
    //tmpPtrPtr = syntaxStack.back();
    syntaxStack.pop_back();
    //？？？？？？？（*tmpPtrPtr）=
}
void process46() {}//ParamList::==ParamDecList
void process47() {}//ParamDecList	::=	 Param  ParamMore

void process48() {//ParamMore	::=	 LAMBDA
    syntaxStack.pop_back();
    //curTreePtr = saveFuncP;
    //decTemp=函数返回类型信息的成员地址???
}
void process49() {}//ParamMore::=; ParamDecList	
void process50() {//Param::=TypeName  FormList
    curTreePtr = new TreeNode(currentToken.lineNum, DecK);
    curTreePtr->attr.ProcAttr.param = ValparamType;
    tmpPtrPtr = syntaxStack.back();
    syntaxStack.pop_back();
    (*tmpPtrPtr) = curTreePtr;
    syntaxStack.push_back(&(curTreePtr->sibling));
}
void process51() {//Param::=VAR TypeName  FormList
    curTreePtr = new TreeNode(currentToken.lineNum, DecK);
    curTreePtr->attr.ProcAttr.param = VarparamType;
    tmpPtrPtr = syntaxStack.back();
    syntaxStack.pop_back();
    (*tmpPtrPtr) = curTreePtr;
    syntaxStack.push_back(&(curTreePtr->sibling));
}
void process52() {  // FormList ::= ID FidMore
    curTreePtr->name.push_back(currentToken.name);   // 标识符名存入节点中
}
void process53() {}//FidMore	::=	 LAMBDA
void process54() {}//FidMore	::=, FormList
void process55() {}//ProcDecPart	::=	DeclarePart  
void process56() {}//ProcBody		::=	ProgramBody
void process57() {//ProgramBody	::=	BEGIN  StmList END
   
    cout << "!!!" << ((*syntaxStack.back()) == curTreePtr->son[1]);
    //while((*syntaxStack.back())!= curTreePtr->son[2])
        syntaxStack.pop_back();//??弹栈至是child[2]??
    curTreePtr = new TreeNode(currentToken.lineNum, StmLK);
    tmpPtrPtr = syntaxStack.back();
    syntaxStack.pop_back();
    (*tmpPtrPtr) = curTreePtr;
    syntaxStack.push_back(&(curTreePtr->son[0]));
}
void process58() {}//StmList::=	Stm StmMore	
void process59() {//StmMore::=LAMBDA
    syntaxStack.pop_back();
}
void process60() {}//StmMore::=; StmList
void process61() {//Stm::=	ConditionalStm
    curTreePtr = new TreeNode(currentToken.lineNum, StmtK);
    curTreePtr->kind.stmt = IfK;
    tmpPtrPtr = syntaxStack.back();
    syntaxStack.pop_back();
    (*tmpPtrPtr) = curTreePtr;
    syntaxStack.push_back(&(curTreePtr->sibling));
}
void process62() {//Stm::=LoopStm
    curTreePtr = new TreeNode(currentToken.lineNum, StmtK);
    curTreePtr->kind.stmt = WhileK;
    tmpPtrPtr = syntaxStack.back();
    syntaxStack.pop_back();
    (*tmpPtrPtr) = curTreePtr;
    syntaxStack.push_back(&(curTreePtr->sibling));
}
void process63() {//Stm::==InputStm
    curTreePtr = new TreeNode(currentToken.lineNum, StmtK);
    curTreePtr->kind.stmt = ReadK;
    tmpPtrPtr = syntaxStack.back();
    //cout <<"mdmdmd"<<(syntaxStack.size());
    syntaxStack.pop_back();
    (*tmpPtrPtr) = curTreePtr;
    syntaxStack.push_back(&(curTreePtr->sibling));
}
void process64() {//Stm::==OutputStm
    curTreePtr = new TreeNode(currentToken.lineNum, StmtK);
    curTreePtr->kind.stmt = WriteK;
    tmpPtrPtr = syntaxStack.back();
   // cout << "mdmdmd" << (syntaxStack.size());
    syntaxStack.pop_back();
    (*tmpPtrPtr) = curTreePtr;
    syntaxStack.push_back(&(curTreePtr->sibling));
}
void process65() {//Stm::==ReturnStm
    curTreePtr = new TreeNode(currentToken.lineNum, StmtK);
    curTreePtr->kind.stmt = ReturnK;
    tmpPtrPtr = syntaxStack.back();
    syntaxStack.pop_back();
    (*tmpPtrPtr) = curTreePtr;
    syntaxStack.push_back(&(curTreePtr->sibling));
}

void process66() {//Stm::==ID AssCall
    curTreePtr = new TreeNode(currentToken.lineNum, StmtK);
    t = new TreeNode(currentToken.lineNum, ExpK);
    t->kind.exp = VariK;
    t->name.push_back(currentToken.name);//??
    curTreePtr->son[0] = t;
    tmpPtrPtr = syntaxStack.back();
    syntaxStack.pop_back();
    (*tmpPtrPtr) = curTreePtr;
    syntaxStack.push_back(&(curTreePtr->sibling));
}
void process67() {//AssCall	::=  	AssignmentRest	
    curTreePtr->kind.stmt = AssignK;
}
void process68() {//AssCall	::=  	CallStmRest	
    curTreePtr->kind.stmt = CallK;
    curTreePtr->son[0]->attr.ExpAttr.varkind = IdV;
    //???
}
//特殊栈底标志
void process69() {//AssignmentRest	::= VariMore	:= Exp
    syntaxStack.push_back(&(curTreePtr->son[1]));//赋值右部在第二个儿子！！！
    curTreePtr = curTreePtr->son[0];
    //syntaxStack.push_back()???？？？
    t = new TreeNode(currentToken.lineNum, ExpK); // newExpNode(OpK);???
    t->kind.exp = OpK;
    t->attr.ExpAttr.op = END;//???
    ////压入特殊操作符，使其优先级最低
    operatorStack.push_back(t);
}
void process70() {//ConditionalStm::=IF RelExp THEN StmList ELSE StmList FI
    syntaxStack.push_back(&(curTreePtr->son[2]));
    syntaxStack.push_back(&(curTreePtr->son[1]));
    syntaxStack.push_back(&(curTreePtr->son[0]));
}
void process71() {//LoopStm::=WHILE RelExp DO StmList ENDWH
    syntaxStack.push_back(&(curTreePtr->son[1]));
    syntaxStack.push_back(&(curTreePtr->son[0]));
}
void process72() {}
void process73() {
    curTreePtr->name.push_back(currentToken.name);
}
void process74() {
    syntaxStack.push_back(&(curTreePtr->son[0]));
   // operatorStack.clear();
    t= new TreeNode(currentToken.lineNum,ExpK); // newExpNode(OpK);???
    t->kind.exp = OpK;
    t->attr.ExpAttr.op = END;//???
    ////压入特殊操作符，使其优先级最低
    operatorStack.push_back(t);
}
void process75() {

}
void process76() {
    syntaxStack.push_back(&(curTreePtr->son[1]));//
}
void process77() {
    syntaxStack.pop_back();
}
void process78() {
    
    //operatorStack.clear();
    t = new TreeNode(currentToken.lineNum, ExpK); // newExpNode(OpK);???
    t->kind.exp = OpK;
    t->attr.ExpAttr.op = END;//???
    ////压入特殊操作符，使其优先级最低
    operatorStack.push_back(t);
}
void process79() {}
void process80() {//???
    //syntaxStack.push_back(&(curTreePtr->son[1]));//产生式右边入栈？？？
    syntaxStack.push_back(&(curTreePtr->sibling));
}

void process81() {
    t = new TreeNode(currentToken.lineNum, ExpK); // newExpNode(OpK);???
    t->kind.exp = OpK;
    t->attr.ExpAttr.op = END;//???
    ////压入特殊操作符，使其优先级最低
    operatorStack.push_back(t);
    getExpResult = false;
}

void process82() {
    curTreePtr = new TreeNode(currentToken.lineNum, ExpK); // newExpNode(OpK);???
    curTreePtr->kind.exp = OpK;
    curTreePtr->attr.ExpAttr.op = currentToken.type;//???
    t = operatorStack.back();
    sTop= t->attr.ExpAttr.op;
    while (priosity(sTop) >= priosity(currentToken.type)) {//curTreePtr->attr.ExpAttr.op
        t = operatorStack.back();
        operatorStack.pop_back();
        TreeNode* Rnum = numStack.back();
        numStack.pop_back();
        TreeNode* Lnum = numStack.back();
        numStack.pop_back();
        t->son[0] = Lnum;
        t->son[1] = Rnum;
        numStack.push_back(t);
        t = operatorStack.back();
        TokenType sTop = t->attr.ExpAttr.op;
    }
    operatorStack.push_back(curTreePtr);
    getExpResult = true;
}
void process83() {}

void process84() {
        if ((currentToken.type == RPAREN) && (expflag != 0)) {
            //if(operatorStack.back()->attr.ExpAttr.op == END)
            while (operatorStack.back()->attr.ExpAttr.op != LPAREN) {
                //cout << "top:" << operatorStack.back()->attr.ExpAttr.op<<endl;
                t = operatorStack.back();
                operatorStack.pop_back();
                TreeNode* Rnum = numStack.back();
                numStack.pop_back();
                TreeNode* Lnum = numStack.back();
                numStack.pop_back();
                t->son[0] = Lnum;
                t->son[1] = Rnum;
                numStack.push_back(t);
            }
            operatorStack.pop_back();
            expflag--;
        }
    else {
        if ((getExpResult) || (getExpResult2)) {
            while (operatorStack.back()->attr.ExpAttr.op != END) {
                t = operatorStack.back();
                operatorStack.pop_back();
                TreeNode* Rnum = numStack.back();
                numStack.pop_back();
                TreeNode* Lnum = numStack.back();
                numStack.pop_back();
                t->son[0] = Lnum;
                t->son[1] = Rnum;
                numStack.push_back(t);
            }
            operatorStack.pop_back();
            curTreePtr = numStack.back();
            numStack.pop_back();
            tmpPtrPtr = syntaxStack.back();
            syntaxStack.pop_back();
            (*tmpPtrPtr) = curTreePtr;
            getExpResult2 = false;
        }
    }
}
void process85() {
    curTreePtr = new TreeNode(currentToken.lineNum, ExpK); // newExpNode(OpK);???
    curTreePtr->kind.exp = OpK;
    curTreePtr->attr.ExpAttr.op = currentToken.type;//???
    t = operatorStack.back();
    sTop = t->attr.ExpAttr.op;
    while (priosity(sTop) >= priosity(currentToken.type)) {//curTreePtr->attr.ExpAttr.op
        t = operatorStack.back();
        operatorStack.pop_back();
        TreeNode* Rnum = numStack.back();
        numStack.pop_back();
        TreeNode* Lnum = numStack.back();
        numStack.pop_back();
        t->son[0] = Lnum;
        t->son[1] = Rnum;
        numStack.push_back(t);
        t = operatorStack.back();
         sTop = t->attr.ExpAttr.op;
    }
    operatorStack.push_back(curTreePtr);
}
void process86() {}
void process87() {}
void process88() {
    curTreePtr = new TreeNode(currentToken.lineNum, ExpK); // newExpNode(OpK);???
    curTreePtr->kind.exp = OpK;
    curTreePtr->attr.ExpAttr.op = currentToken.type;//???
    t = operatorStack.back();
    sTop = t->attr.ExpAttr.op;
    while (priosity(sTop) >= priosity(currentToken.type)) {//curTreePtr->attr.ExpAttr.op
        t = operatorStack.back();
        operatorStack.pop_back();
        TreeNode* Rnum = numStack.back();
        numStack.pop_back();
        TreeNode* Lnum = numStack.back();
        numStack.pop_back();
        t->son[0] = Lnum;
        t->son[1] = Rnum;
        numStack.push_back(t);
        t = operatorStack.back();
        sTop = t->attr.ExpAttr.op;
       
    }
     operatorStack.push_back(curTreePtr);//
}

void process89() {
    curTreePtr = new TreeNode(currentToken.lineNum, ExpK); // newExpNode(OpK);???
    curTreePtr->kind.exp = OpK;
    cout <<"wuuwu"<< currentToken.type << endl;
    curTreePtr->attr.ExpAttr.op = currentToken.type;//LPAREN;//???
    operatorStack.push_back(curTreePtr);
    expflag++;
}
void process90() {
    curTreePtr = new TreeNode(currentToken.lineNum, ExpK); // newExpNode(OpK);???
    curTreePtr->kind.exp = ConstK;
    curTreePtr->attr.ExpAttr.val = atoi(currentToken.name.c_str());//???
    numStack.push_back(curTreePtr);
}
void process91() {}
void process92() {//???
    curTreePtr = new TreeNode(currentToken.lineNum, ExpK); // newExpNode(OpK);???
    curTreePtr->kind.exp = VariK;
    curTreePtr->name.push_back(currentToken.name);   // 标识符名存入节点中
    numStack.push_back(curTreePtr);
}
void process93() {
    curTreePtr->attr.ExpAttr.varkind = IdV;
    //curTreePtr->attr.ExpAttr.type = ;//???变量类型信息在哪？？
}

void process94() {
    curTreePtr->attr.ExpAttr.varkind = ArrayMembV;
    syntaxStack.push_back(&(curTreePtr->son[0]));//？？
    t = new TreeNode(currentToken.lineNum, ExpK); // newExpNode(OpK);???
    t->kind.exp = OpK;
    t->attr.ExpAttr.op = END;//???
    ////压入特殊操作符，使其优先级最低
    operatorStack.push_back(t);
    getExpResult2 = true;
}
void process95() {
    curTreePtr->attr.ExpAttr.varkind = FieldMembV;
    syntaxStack.push_back(&(curTreePtr->son[0]));//？？
}
void process96() {
    curTreePtr = new TreeNode(currentToken.lineNum, ExpK); // newExpNode(OpK);???
    curTreePtr->kind.exp = VariK;
    curTreePtr->name.push_back(currentToken.name);   // 标识符名存入节点中
    tmpPtrPtr = syntaxStack.back();
    syntaxStack.pop_back();
    (*tmpPtrPtr) = curTreePtr;
}
void process97() {
    curTreePtr->attr.ExpAttr.varkind = IdV;
    //curTreePtr->attr.ExpAttr.type = ;//???变量类型信息在哪？？
}
void process98() {
    //process94();
    curTreePtr->attr.ExpAttr.varkind = ArrayMembV;
    syntaxStack.push_back(&(curTreePtr->son[0]));//？？
    t = new TreeNode(currentToken.lineNum, ExpK); // newExpNode(OpK);???
    t->kind.exp = OpK;
    t->attr.ExpAttr.op = END;//???
    ////压入特殊操作符，使其优先级最低
    operatorStack.push_back(t);
    getExpResult2 = true;
}
void process99() {}
void process100() {}
void process101() {}
void process102() {}
void process103() {}
void process104(){

}

// ------------------------------------------ 产生式初始化函数 函数指针指向函数 ------------------------------------------------------------
// funcs[n] = processn  
#define SETFUNC(n) funcs[n]=process##n
#define SETFUNC10(n) \
    SETFUNC(n##0) ;\
    SETFUNC(n##1) ;\
    SETFUNC(n##2) ;\
    SETFUNC(n##3) ;\
    SETFUNC(n##4) ;\
    SETFUNC(n##5) ;\
    SETFUNC(n##6) ;\
    SETFUNC(n##7) ;\
    SETFUNC(n##8) ;\
    SETFUNC(n##9) ;
    
void initParserLL1Funcs(){
    SETFUNC(1);
    SETFUNC(2);
    SETFUNC(3);
    SETFUNC(4);
    SETFUNC(5);
    SETFUNC(6);
    SETFUNC(7);
    SETFUNC(8);
    SETFUNC(9);

    SETFUNC10(1);
    SETFUNC10(2);
    SETFUNC10(3);
    SETFUNC10(4);
    SETFUNC10(5);
    SETFUNC10(6);
    SETFUNC10(7);
    SETFUNC10(8);
    SETFUNC10(9);

    SETFUNC(100);
    SETFUNC(101);
    SETFUNC(102);
    SETFUNC(103);
    SETFUNC(104);
}


// ------------------------------------ 输出语法分析错误信息 ------------------------------------------------

// 输出语法错误信息
void outputParserLL1Errors(bool toScrean,const char errorFileName[])
{
    if(toScrean)
    {
        for(string tmp : errors)
            printf("%s\n",tmp.c_str());
    }
    else
    {
        FILE * fn = fopen(errorFileName,"w");
        for(string tmp : errors)           // 遍历Vector并输出
            fprintf(fn,tmp.c_str());
        fclose(fn);
    }
}


// -------------------------------------- LL(1) 语法分析测试 main ---------------------------------------------------


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


int main() {
    // token 序列读进来
	tokens = getTokenList();
	cout<< tokens.size()<< endl;
	
	initReadOneToken();
	initProducts();   // 千万不要忘了  init !!!!!
	initPredict();
   
    LL1parse();
	 const char fileName[] = "D:/txt/LL1syntaxTree.txt";
     outputParserTree(rt , fileName);
     const char errorFileName[] = "D:/txt/LL1syntaxError.txt";
     outputParserLL1Errors(false,errorFileName);
    return 0;
}