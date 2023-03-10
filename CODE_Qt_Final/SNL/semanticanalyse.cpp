#include "semanticanalyse.h"

static vector<string> errorlist;
static vector<symbTable> scope; // 分程序表 ,用来记录 各层 分程序符号表的起始位置,当前 level 即 scope.size() -1
static vector<int> offsets; // 记录各层的偏移 , 与 scope 同步
static vector<symbNode> allparam;  // 储存参数

static FILE * listing;  // 输出符号表到文件
static const char errorFileName[100] = "D:/CODE_Qt/txt/debug_info.txt";

// --------------------------------------- 符号表的实现 ---------------------------------------------------------------------
int getLevel(){
    return scope.size()-1;   // level 即 scope.size() -1
}

void createTable(){
    symbTable table;
    scope.push_back(table);  // 创建新一层符号表
    offsets.push_back(0);  // 初始化偏移为 0
}

void destroyTable(){
    printSymbTable(getLevel());   // 删除符号表之前打印出来
    scope.pop_back();  // 撤销一个符号表
    offsets.pop_back();
}

void addSymble(const symbNode & st)
{
    scope[getLevel()].push_back(st);      // 添加一个 symb 在本层, 这个symb在加进去之前会填写好信息
}


// ---------------------- 查表 -------------------------------
symbNode * findSymb(const int & lev,const string & sname)
{
    for(int i = 0; i < scope[lev].size(); i++)      // 声明性出现:  在 lev 这一层找 symb ; 声明的时候遇到标识符, 是在本层找 symb
        if(scope[lev][i].idname == sname)
            return &scope[lev][i];
    return NULL;
}

symbNode * findSymbAll(const string & sname)       // 使用性出现: 语句中遇到标识符, 从本层开始,在所有层找 idname为 sname 的symbl
{
    symbNode * sn = NULL;
    for(int lev = scope.size()-1;lev>=0;--lev)    //从 scope[lev].size()-1 开始往前到 0 逆序找
        if(sn = findSymb(lev,sname))
            return sn;
    return NULL;
}


// -------------------- 符号表输出 -------------------------

void printTy(const TypeKinds & tk)
{
    switch(tk)
    {
        case intTy:     fprintf(listing,"intTy     ");     break;
        case charTy:    fprintf(listing,"charTy    ");    break;
        case arrayTy:   fprintf(listing,"arrayTy   ");   break;
        case recordTy:  fprintf(listing,"recordTy  ");  break;
        case boolTy:    fprintf(listing,"boolTy    ");    break;
    }
}

void  printSymb(const symbNode & st)
{
    fprintf(listing,"  %-10s  ",st.idname.c_str());

    switch(st.attrIR.kind)
    {
    case typeKind:
        fprintf(listing,"typekind  ");
        if(st.attrIR.typePtr) printTy(st.attrIR.typePtr->kind);
        fprintf(listing,"\n");
        break;
    case varKind:
        fprintf(listing,"varkind   ");
        if(st.attrIR.typePtr) printTy(st.attrIR.typePtr->kind);
        fprintf(listing,"level=%d  offset=%d  ",st.attrIR.level,st.attrIR.off);
        fprintf(listing,"%s\n",st.attrIR.access==dir ? "dir  " : "indir");
        break;
    case procKind:
        fprintf(listing,"prockind  ");
        fprintf(listing,"          level=%d  noff\n",st.attrIR.level);   // 实在函数没有偏移
        break;
    }
}

void  printSymbTable(const int & lev)  // 将生成的符号表输出到列表文件 listing
{
    if(scope[lev].size())
    {
        fprintf(listing,"\n\n - * - * - * - * - * - Level: %d - * - * - * - * - * - * - \n",lev);
        for(int i = 0;i < scope[lev].size();i++)
            printSymb(scope[lev][i]);
        fprintf(listing," - * - * - * - * - * - * - * - * - * - * - * - * - * - * - \n");
    }
}


// -------------------------------------- 语义分析错误处理 -----------------------------------------
string intToStr(int n)
{
    char str[5];
    sprintf(str, "%d", n);  // 把 n 以 %d 格式读到 str 中
    return (string)str;
}

void  semanticsError(const string & s,const int & linenumber)
{
    errorlist.push_back(s + ", at line " + ": " + intToStr(linenumber) + " \n");
}

// 输出语义错误信息
void outputSemanticError()
{
//    for(auto tmp : errorlist)
//        printf(">>> Semantic error : %s\n",tmp.c_str());

    FILE * fn=fopen(errorFileName,"a");
    for(size_t i = 0; i < errorlist.size(); i++)
    {
        errorlist[i] = ">>> Semantic error : " + errorlist[i] + "\n";
        fputs(errorlist[i].c_str(),fn);
    }
    fclose(fn);
}



// ---------------------------------- 各种语义分析函数 先序遍历语法树 --------------------------------------------
static intIR intir;
static charIR charir;
static boolIR boolir;
static int size = 0; // for bug , 暂存 记录的size

void  analyzeProK(TreeNode * rt)
{
    if(rt == NULL) {
        return;
    }
    createTable();
    analyzePheadK(rt->son[0]);
    analyzeDeclareK(rt->son[1]);  // 声明部分
    analyzeStmLK(rt->son[2]->son[0]);  // 过程体 语句序列 StmLK 的 第一个语句 StmtK
    destroyTable();
}

void  analyzePheadK(TreeNode * rt)
{
    if(rt == NULL) {
        return;
    }
    // 无需加入符号表
}

void  analyzeDeclareK(TreeNode * rt)
{
    if(rt == NULL) return;
    switch(rt->nodekind)
    {
        case TypeK:     analyzeTypedecK(rt->son[0]);break;  // 可能有很多类型声明, 所以取 rt->son[0]
        case VarK:      analyzeVardecK(rt->son[0]);break;  // 第一个变量声明 具体节点 DecK
        case ProcDecK:  analyzeProcdecK(rt);break;  // 不是 rt->son[0] , 虽然过程声明也可以有好几个,因为后面还有rt->sibling
    }
    analyzeDeclareK(rt->sibling);
}

void  analyzeTypedecK(TreeNode * rt)
{
    if(rt == NULL) return;

    symbNode * sn = findSymb(getLevel(),rt->name[0]);
    if(sn == NULL)        // 第一次声明, 没找到 这个标识符, 则把这个标识符加入 本层的符号表中
    {
        symbNode st;           // 新建节点, 填入 类型声明 的信息 , 再加入符号表
        st.idname = rt->name[0];
        st.attrIR.kind = typeKind;
        typeIR * tmp = getTypePtr(rt);  // see here, recordTy  为什么一返回就 size, kind 都不对了
        if(size != 0){
            tmp->kind = recordTy;
            tmp->size = size;
            size = 0;
        }
        st.attrIR.typePtr = tmp;   // here for bug
        addSymble(st);
    }
    else
        semanticsError("类型重复声明 !",rt->lineno);   // 又找到了说明 类型重复声明
    analyzeTypedecK(rt->sibling);
}

void  analyzeVardecK(TreeNode * rt,symbNode * stp,bool isparam)
{
    if(rt == NULL) return;
    for(int i = 0;i < rt->name.size(); ++i)      //变量声明 有 name.size() 个变量名标识符
    {
        symbNode * sn = findSymb(getLevel(),rt->name[i]);  // 先根据标识符名查表
        if(sn == NULL)
        {
            symbNode st;            // 新建 变量声明节点, 填完信息后再把这个节点加入符号表中
            st.idname = rt->name[i];
            st.attrIR.kind = varKind;
            st.attrIR.level = getLevel();
            st.attrIR.access = dir;
            st.attrIR.off = offsets[getLevel()];
            st.attrIR.typePtr = getTypePtr(rt);  //todo: 加一个 record 类型变量试试
            offsets[getLevel()] += st.attrIR.typePtr->size;

            if(isparam)
                st.attrIR.access = (AccessKinds)rt->attr.ProcAttr.param;  //根据语法树节点信息确定参数类型: 值参/变参
            addSymble(st);
            if(isparam)
            {
                stp->attrIR.param.push_back(allparam.size());   // 参数存这个参数在allparam 里的下标
                allparam.push_back(st);  // 所有的参数都在这个参数表里
            }
        }
        else semanticsError("变量重复声明 !",rt->lineno);   // 变量 重复声明
    }
    analyzeVardecK(rt->sibling,stp,isparam);
}

void analyzeProcdecK(TreeNode * rt)///过程语义分析
{
    if(rt == NULL) return;

    symbNode * sn = findSymb(getLevel(),rt->name[0]);
    if(sn == NULL)
    {
        symbNode st;      // 新建 过程声明节点st, 填入信息,
        st.idname = rt->name[0];
        st.attrIR.kind = procKind;
        st.attrIR.typePtr = NULL;
        st.attrIR.level = getLevel();
        st.attrIR.off = offsets[getLevel()];

        createTable();  // 过程其他信息要新建一层符号表
        analyzeParam(rt->son[0],&st);    // 参数个数信息也要存进st

        scope[getLevel()-1].push_back(st);  //这里没有调用 addSymble() 是因为,过程内部新建表, 然而这个过程声明节点应该填在上一个表

        analyzeDeclareK(rt->son[1]);  // 过程声明
        analyzeStmLK(rt->son[2]->son[0]);  // 过程体  语句序列
        destroyTable();   //出去一个过程体则删除本层符号表
    }
    else
        semanticsError("过程重复声明 !",rt->lineno);
}


void  analyzeParam(TreeNode * rt,symbNode * stp)
{
    if(rt==NULL) return;
    analyzeVardecK(rt,stp,1);
}


// ------------ 语句 分析部分 -------------------
void  StmLKIf(TreeNode * rt)
{
    if(getExpType(rt->son[0]) == &boolir)       // 获取表达式的类型
    {
        analyzeStmLK(rt->son[1]);  // 条件为真的语句序列
        analyzeStmLK(rt->son[2]);   // 条件为假的语句序列
    }  // 条件语句的表达式不是  bool类型则 error
    else semanticsError("If语句的条件表达式必须是bool类型 !",rt->lineno);
}

void  StmLKWhile(TreeNode * rt)
{
    if(getExpType(rt->son[0]) == &boolir)
    {
        analyzeStmLK(rt->son[1]);
    }
    else semanticsError("While语句的条件表达式不是bool类型 !",rt->lineno);
}

void  StmLKAssign(TreeNode * rt)
{
    AccessKinds ac0,ac1;
    typeIR * gext1 = getExpType(rt->son[0],&ac0);  // 同时ac0,ac1获取值
    typeIR * gext2 = getExpType(rt->son[1],&ac1);

    if(gext1 == &charir && ac0 == dir && gext2 == &intir) return ;
    if(gext1 != gext2 || ac0 == indir) {semanticsError("等号左右类型 !",rt->lineno);return;}
    if(gext1 != &charir && gext1 != &intir) semanticsError("无法给数组或记录类型赋值 !",rt->lineno);
}

void  StmLKRead(TreeNode * rt)
{
    symbNode * sn = findSymbAll(rt->name[0]);
    if(sn == NULL || sn->attrIR.kind != varKind)
        semanticsError("未声明的变量 !",rt->lineno);
    else
    {
        arrayIR * air = (arrayIR *)(sn->attrIR.typePtr);
        if((sn->attrIR.typePtr->kind == arrayTy && air->elemTyPtr==&intir) || sn->attrIR.typePtr->kind == recordTy)
            semanticsError("无法读数组或记录类型 !",rt->lineno);
    }
}

void  StmLKWrite(TreeNode * rt)
{
    typeIR * gext = getExpType(rt->son[0]);

    if(gext == &intir || gext == &charir ||(gext->kind == arrayTy &&((arrayIR*)gext)->elemTyPtr == &charir))
        ;  //todo: 写语句 只要条件表达式不是 bool就行
    else
        semanticsError("无法写数组或记录类型 !",rt->lineno);
}

void  StmLKCall(TreeNode * rt)
{
    symbNode * sn = findSymbAll(rt->son[0]->name[0]);

    if(sn == NULL || sn->attrIR.kind != procKind)
    {
        semanticsError("过程未声明 !",rt->lineno);
        return;
    }

    int i;  TreeNode * p;
    for(i=0,p = rt->son[1];p != NULL && i<sn->attrIR.param.size();p = p->sibling,++i)
    {
        symbNode entry = allparam[sn->attrIR.param[i]];  //sn->attrIR.param[i]中存的是参数在 allparam 中的下标
        if(entry.attrIR.access == dir)
        {
            if(getExpType(p) != entry.attrIR.typePtr) break;
        }
        else
        {
            AccessKinds tac;
            if(getExpType(p,&tac) != entry.attrIR.typePtr || tac == indir)
                break;
        }
    }
    if(i != sn->attrIR.param.size() || p != NULL)
        semanticsError("参数错误 !",rt->lineno);
}

void  StmLKReturn(TreeNode * rt)
{
    if(getLevel() == 0)
        semanticsError("在这里返回是不正确的 !",rt->lineno);
}

void  analyzeStmLK(TreeNode * rt)
{
    if(rt == NULL) return;
    switch(rt->kind.stmt)   // 转去相应的分支
    {
        case IfK:       StmLKIf(rt);        break;
        case WhileK:    StmLKWhile(rt);     break;
        case AssignK:   StmLKAssign(rt);    break;
        case ReadK:     StmLKRead(rt);      break;
        case WriteK:    StmLKWrite(rt);     break;
        case CallK:     StmLKCall(rt);      break;
        case ReturnK:   StmLKReturn(rt);    break;
    }
    analyzeStmLK(rt->sibling);
}



// -----------------------------------------------------------------------------------------------

int  getTypeSize(TreeNode * rt)  // 获取类型的 size , charK, intK ,boolK 均为 1个单位
{
    if(rt->kind.dec == ArrayK)
        return rt->attr.ArrayAttr.up - rt->attr.ArrayAttr.low + 1;
    else
        return 1;
}

typeIR * getTypePtr(TreeNode * rt)         // 指向内部类型 的指针
{
    switch(rt->kind.dec)
    {
    case IntegerK:     return &intir;
    case CharK:      return &charir;

    case ArrayK:
        {
            const int s = rt->attr.ArrayAttr.up - rt->attr.ArrayAttr.low + 1;
            if(s<=0) semanticsError("数组下标越界 !",rt->lineno);
            if(rt->attr.ArrayAttr.memtype == CharK)
                return new arrayIR(s,&charir,rt->attr.ArrayAttr.low);
            else
                return new arrayIR(s,&intir,rt->attr.ArrayAttr.low);
        }

    case IdK:
        {
            symbNode * ss = findSymbAll(rt->attr.type_name);
            if(ss != NULL)
                return ss->attrIR.typePtr;
            else {
                semanticsError("类型未声明 !",rt->lineno);
                return &intir;
            }
        }

    case RecordK:
        {
            recordIR * rst = new recordIR(0);      // size:0 , recordTy
            for(TreeNode * p = rt->son[0]; p != NULL; p = p->sibling)  // 类型声明的节点 具体节点
                for(int j = 0; j < p->name.size();++j)   // 每个类型可能有好几个变量
                {
                    for(int i = 0;i < rst->rdlist.size();++i)   // 遍历域成员 看有无跟这个新节点的名字重复
                        if(rst->rdlist[i].idname == p->name[j])
                            semanticsError("记录域成员重复声明 !",rt->lineno);

                    rdnode rn;     // 域节点
                    rn.idname = p->name[j];
                    rn.offset = rst->size;  // 从 0 开始
                    rn.unitTypePtr = getTypePtr(p);
                    rst->size += getTypeSize(p);
                    rst->rdlist.push_back(rn);      // rdlist 挂了一串 域节点
                }
            size = rst->size;  // for bug
            // typeIR * recordBase = rst;
            // recordBase->kind = rst->kind;
            // recordBase->size = rst->size;
            // return recordBase;   // 肯定不能返回父类, 后面语义分析还要查表, 用到子类的其他信息
            return rst;   // 到这里都是对的, 为什么返回就都不对了
        }
    }
}


typeIR * getExpType(TreeNode * rt,AccessKinds * ac,typeIR * recmemptr)  // 获取表达式内部类型
{
    switch(rt->kind.exp)
    {
        case ConstK:
            if(ac != NULL) (*ac) = indir;
            return &intir;

        case VariK:
            {
                if(ac != NULL) (*ac) = dir;
                symbNode * sn = findSymbAll(rt->name[0]);  // 使用性出现
                symbNode snode;
                snode.idname = rt->name[0];
                snode.attrIR.typePtr = recmemptr;
                if(recmemptr)
                    sn = &snode;
                if(sn != NULL)    // 找到标识符信息 , 拿出来使用
                {
                    switch(rt->attr.ExpAttr.varkind)  // here varkind is unknown !!! bug is here
                    {
                        case ArrayMembV:
                            {
                                if(getExpType(rt->son[0]) != &intir)
                                    semanticsError("数组下标类型错误 ！",rt->lineno);
                                arrayIR * tmp = (arrayIR *)sn->attrIR.typePtr;  // 又强转为子类
                                return tmp->elemTyPtr;
                            }

                        case IdV:
                            return sn->attrIR.typePtr;

                        case FieldMembV:
                            {
                                int pos = -1;
                                recordIR * rptr =(recordIR *)sn->attrIR.typePtr;  // 又强转为子类

                                for(int i = 0;i < rptr->rdlist.size();++i)
                                        if(rt->son[0]->name[0] == rptr->rdlist[i].idname)
                                            pos = i;

                                if(pos != -1)
                                {
                                    return getExpType(rt->son[0],NULL,rptr->rdlist[pos].unitTypePtr);
                                }
                                else
                                    {semanticsError("记录域成员未声明 !",rt->lineno);return &intir;}
                            }
                            break;
                    }
                }
                else {semanticsError("变量未声明 !",rt->lineno);return &intir;}
            }

        case OpK:
            if(ac != NULL) (*ac) = indir;
            typeIR * gext0 = getExpType(rt->son[0]);
            typeIR * gext1 = getExpType(rt->son[1]);
            if(gext0 != &intir && gext0 != &charir)
                {semanticsError("无法操作数组或记录 !",rt->lineno);return gext0;}
            if(gext0 == gext1)
            {
                if(rt->attr.ExpAttr.op == LT || rt->attr.ExpAttr.op == EQ)
                    return &boolir;
                else
                    return gext0;
            }
            else {semanticsError("表达式左右两边类型不匹配 !",rt->lineno);return &intir;}
    }
}



// ---------------------------------------------- 语义分析 接口 --------------------------------------------------------------
bool _semanticAnalyze(TreeNode * rt)
{
    errorlist.clear();
    listing = fopen("D:/CODE_Qt/txt/SemanticSymbleTable.txt","w");
    analyzeProK(rt);
    fclose(listing);
    return errorlist.size() == 0;
}

// -------------------------------------- 对外接口 语义分析 -----------------------------------------------

void semanticAnalyse(){
    TreeNode* rt = getParseTree();
    if(rt == NULL) return ;

    if ( _semanticAnalyze(rt) ){
       // printf(">>> SemanticAnalyse is sucessful without errors!\n");
        FILE * fn=fopen(errorFileName,"a"); // 信息输出到 debug_info 文件
        fputs(">>> 语义分析正确 !\n",fn);
        fclose(fn);
    }
    else{
        outputSemanticError();
        listing = fopen("D:/CODE_Qt/txt/SemanticSymbleTable.txt","w");
        fclose(listing);  // 再次以 w 方式打开语义表文件, 清空内容
    }
}

