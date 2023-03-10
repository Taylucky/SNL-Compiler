//文件路径
char OUT1[100] = "D:\\月月\\大三下\\必修课\\编译原理课设\\Project1\\token.txt";
char out2[100]= "D:\\月月\\大三下\\必修课\\编译原理课设\\Project1\\tree2.txt";
//非终极符
typedef enum
{
	Program, ProgramHead, ProgramName, DeclarePart,
	TypeDec, TypeDeclaration, TypeDecList, TypeDecMore,
	TypeId, TypeName, BaseType, StructureType,
	ArrayType, Low, Top, RecType,
	FieldDecList, FieldDecMore, IdList, IdMore,
	VarDec, VarDeclaration, VarDecList, VarDecMore,
	VarIdList, VarIdMore, ProcDec, ProcDeclaration,
	ProcDecMore, ProcName, ParamList, ParamDecList,
	ParamMore, Param, FormList, FidMore,
	ProcDecPart, ProcBody, ProgramBody, StmList,
	StmMore, Stm, AssCall, AssignmentRest,
	ConditionalStm, StmL, LoopStm, InputStm,
	InVar, OutputStm, ReturnStm, CallStmRest,
	ActParamList, ActParamMore, RelExp, OtherRelE,
	Exp, OtherTerm, Term, OtherFactor,
	Factor, Variable, VariMore, FieldVar,
	FieldVarMore, CmpOp, AddOp, MultOp
}NonTerminal; 
//终极符
typedef enum
{
	ENDFILE, ERROR,
	PROGRAM, PROCEDURE, TYPE, VAR, IF,
	THEN, ELSE, FI, WHILE, DO, ENDWH,
	BEGIN, END1, READ, WRITE, ARRAY, OF,
	RECORD, RETURN1,
	INTEGER, CHAR1, 
	ID, INTC, CHARC,
	ASSIGN, EQ, LT, PLUS, MINUS,
	TIMES, OVER, LPAREN, RPAREN, DOT,
	COLON, SEMI, COMMA, LMIDPAREN, RMIDPAREN,
	UNDERANGE
}Terminal;
//符号栈
typedef struct stackNode{
    LexType token1;//内容
    struct stackNode* next;//指向下一个节点
    bool isTerminal;//1:终极符;0:非终极符
}stacknode

 class stack{
     stacknode* top;//指向栈顶元素
     void push(){
         stacknode* p=new stacknode();
     }
     void pop(){
         
     }
 }  
//输入流

//LL1分析表
    int LL1table[68][42];
//LL1语法分析
TreeNode* programLL1()
{
	INDEX = 0;
	// readToken 再次使用 重新赋值为0

	NonTerminal stacktopN;

	Terminal stacktopT;

	InitLL1Table(); // 初始化LL1分析表

	TreeNode* root = NULL; // 语法树的根

	anlsstack->push(0, Program); // 分析栈 压入初始结点Program
	anlsstack->top->p = new TreeNode("Program"); // 新建树节点
	root = anlsstack->top->p; // 指向根 便于最后返回整棵树

	readToken(); // 读出INDEX=0对应token

	while (!anlsstack->isEmpty) // 循环结束条件
	{
		if (anlsstack->top->ntflag == 1) // 终极符
		{
			stacktopT = anlsstack->top->t; // 记录一下终极符是什么
			if (stacktopT == currentToken->wd.tok) // 比较一下 如果正确
			{
				//cout << currentToken->wd.str << " ";
				anlsstack->top->p->name = currentToken->wd.str; // 把节点的名字用实际代码命名
				anlsstack->top->p->tk = currentToken; // 因为是终极符 所以已经可以把token给树节点了
				// 上面这步很重要

				anlsstack->pop(); // 弹出分析栈
				//cout << currentToken->wd.str << endl;
				readToken(); // 再往后读一个token
			}
			else
			{
				//终极符不匹配
				string temp = "Line ";
				temp += to_string(currentToken->line);
				temp += " ";
				temp += "\"";
				temp += currentToken->wd.str;
				temp += "\" ";
				temp += "& T:";
				temp += to_string(currentToken->wd.tok);
				temp += " NotMatch Error!\n";
				printErrorMsg(temp);
			}
		}
		else // 非终极符
		{
			stacktopN = anlsstack->top->n; // 存一下非终极符

			int num = LL1Table[stacktopN][currentToken->wd.tok]; // LL1

			currentTree = anlsstack->top->p; // 记录一下将要被弹出的树节点
			// 后面将要扩展它的子树


			anlsstack->pop(); // 弹出分析栈

			process(num);

			//readToken(); // 再往后读一个token
		}
	}
	if (currentToken->wd.tok != ENDFILE)
	{
		string temp;
		temp += "NotEndFile Error!\n";
		printErrorMsg(temp);
	}

	return  root;
}
//求predict集

//打印语法树