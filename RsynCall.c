/************递归下降建立语法树************/
/*目的就是把token变成语法树形式,打印出来*/

	int Rcurline = 0;//当前行
	token* Rptr=NULL;//指针指向tokenlist
    typedef enum{
	ProK,PheadK,TypeK,VarK,ProcDecK,StmK,DecK,Stmk,ExpK,
    ArrayK,CharK,IntergerK,RecordK,IdK,
    Ifk,WhileK,AssignK,ReadK,WriteK,CallK,ReturnK,
    OpK,ConstK,IdK,
    IdV,ArrayMembV,FieldMembV,
    Void,Interger,BooLean,
    valparamtype,varparamtype
    }KindType;

    typedef struct{
        int low;
        int up;
        kindType childkind;
    } ArrayAttr;

    typedef struct{
        KindType paramt[10];
    } ProcAttr;

    typedef struct{
        LexType op;
        int val;
        int up;
        kindType childkind;
    } ExpAttr;

    typedef union{
            ArrayAttr aa;
            ProcAttr pa;
            ExpAttr ea;
    } Attr; 

	class Treenode//树节点
	{
	public:
		        
		Treenode* child[3];
        Treetode* sibling;
		int lineno;
        kindType nodekind;
        kindType kind;
		int idnum;
        string name[100];
        int* ptr[100];//每个元素是指针
        string typename[10];
        Attr arr;
		Treenode(KindType types)
		{   
            
			for (int i = 0; i < 3; i++)
			{
				child[i] = NULL;
			}
			sibling=NULL;
            lineno= 0;//初始化
			nodekind=types;
			idnum=0;
			name='\0';
		}
		// void add(Treenode* t) {
		// 	child[childIndex] = t;
		// 	if (t != NULL){//???
		// 		child[childIndex]->linenum = linenum + 1;//this->linenum
		// 		childIndex++;
		// 	}

		// }
	};
	//读入当前token
	void Readtoken() {
		Rptr = &tokenlist[Rcurline];
		Rcurline++;
	}
    void Nextoken() {
		Rcurline++;
		Rptr = &tokenlist[Rcurline];
	}
	void Match(LexType TOK) {
        if(Rptr->content.tok==TOK) Nextoken();
        else{
            cout<<"exit";//???如何退出程序???
        } 
	}
	
	Treenode* Parse(FILE* );
	Treenode* Program();
	Treenode* ProgramHead();
	Treenode* DeclarePart();
	Treenode* ProcDec();
	Treenode* ProcDeclaration();
	Treenode* ProcDecMore();
	Treenode* ProcDecPart();
	Treenode* ProcBody();
	Treenode* ParamList();
	Treenode* ParamDecList();
	Treenode* ParamMore();
	Treenode* Param();
	Treenode* FormList();
	Treenode* FidMore();
	Treenode* VarDec();
	Treenode* VarDeclaration();
	Treenode* VarDecList();
	Treenode* VarIdMore();
	Treenode* VarIdList();
	Treenode* TypeDecMore();
	Treenode* StmList();
	Treenode* StmMore();
	Treenode* Stm();
	Treenode* AssCall();
	Treenode* AssignmentRest();
	Treenode* ConditionalStm();
	Treenode* LoopStm();
	Treenode* InputStm();
	Treenode* OutputStm();
	Treenode* ReturnStm();
	Treenode* CallStmRest();
	Treenode* ActParamList();
	Treenode* ActParamMore();
	Treenode* RelExp();
	Treenode* OtherRelE();
	Treenode* Exp();
	Treenode* OtherTerm();
	Treenode* Term();
	Treenode* Factor();
	Treenode* OtherFactor();
	Treenode* Variable();
	Treenode* VariMore();
	Treenode* FieldVar();
	Treenode* FieldVarMore();
	Treenode* CmpOp();
	Treenode* AddOp();
	Treenode* MultOp();
	Treenode* ProgramBody();
	Treenode* ProgramName();
	Treenode* VarDecMore();
	Treenode* TypeDec();
	Treenode* TypeDeclaration();
	Treenode* TypeDecList();
	Treenode* TypeId();
	Treenode* TypeDef();
	Treenode* BaseType();
	Treenode* StructureType();
	Treenode* ArrayType();
	Treenode* RecType();
	Treenode* FieldDecList();
	Treenode* IdList();
	Treenode* IdMore();
	Treenode* FieldDecMore();
/**************各函数具体实现*************/
	
	Treenode* Parse() {
		Readtoken();
		Treenode* t = Program();
		if (tokenlist[Rcurline].content.tok != ENDFILE) {
			cout << "PARSE ERROR!!!";//错误处理
		}
		else return t;
	}

	Treenode* Program() {
		Treenode* t1 = ProgramHead();
		Treenode* q1 = DeclarePart();
		Treenode* s1 = ProgramBody();
		Treenode* t = new Treenode(ProK);//生成节点root
		if (t == NULL)
			cout << "Creating Tree falls near line";//<< Rcurline;////
		else {
			t->child[0]=t1;//调用程序头部分分析函数
			t->child[1]=q1);//调用程序声明部分分析函数
			t->child[2]=s1;//调用程序体部分分析函数

			if (t1 == NULL)
				cout << "MissingProgramHead near line"<< Rcurline;////
			if (q1 == NULL)
				cout << "MissingDeclarePart near line"<< Rcurline;////
			if (s1 == NULL)
				cout << "MissingProgramBody near line"<< Rcurline;////

		}
		//匹配最后的match???
		if (Rptr->content.tok != DOT) cout << "MissingEndDot";
		else Match(DOT);
		return t;
	}

	Treenode* ProgramHead() {	
		Treenode* t = new Treenode(PheadK);
		Match(PROGRAM);
		if (Rptr->content.tok == ID) {
            t->name=Rptr->content.str;
        }
        Match(ID);//???若不是ID呢?
		return t;
		
	}

	Treenode* DeclarePart() {
		Treenode* t1 = new Treenode(TypeK);
		tp1=TypeDec();
        t1->child[0]=tp1;

        Treenode* t2 = new Treenode(VarK);
        t2=t1->sibling;
        tp2=VarDec();
        t2->child[0]=tp2;
        
        Treenode* t3 = new Treenode(ProK);
        t3=t2->sibling;
		tp3=ProcDec();//???
        t3->child[0]=tp3;
		return t;
	}
	
	Treenode* ProcDec(){
        Treenode* t=NULL;
        if(Rptr->content.tok!=BEGIN){
            if(Rptr->content.tok==PROCEDURE)
                t=ProcDeclaration(t);
            else Nextoken();
        }
        return t;
    }
	Treenode* ProcDeclaration(){
        Treenode* t=new Treenode(ProK);
        Match(PROCEDURE);
        if(t!=NULL){
            if(Rptr->content.tok==ID){
                t->name[t->idnum]=Rptr->content.str;
                t->idnum++;
                Match(ID);
            }else{
                Match(LPAREN);
                ParamList();
                Match(RPAREN);
                match(COLON);
                t->child[1]=ProcDecPart();
                t->child[2]=ProcBody();
            }
        }
        return t;
    }
	Treenode* ProcDecMore();
	Treenode* ProcDecPart(){
        Treenode* t=DeclarePart();
        return t;//不成功直接就是NULL么???
    }
	Treenode* ProcBody(){
        Treenode* t=ProgramBody();
        if(t==NULL) cout<<"ProcBody fails";
        return t;
    }
	void ParamList(Treenode* t){
        Treenode* p=NULL;
        if(Rptr->content.tok!=RPAREN){
            if(Rptr->content.tok==INTERGER||Rptr->content.tok==CHAR||Rptr->content.tok==RECORD||Rptr->content.tok==ARRAY||Rptr->content.tok==ID||Rptr->content.tok==VAR){
                p=ParamDecList();
                t->child[0]=p;
            }else Nextoken();
        }
    }
	Treenode* ParamDecList(){
        Treenode* t=Param();
        Treenode* p=ParamMore();
        if(p!=NULL){
            t->sibling=p;
        }
        return t;
    }
	Treenode* ParamMore(){
        Treenode* t=NULL;
        if(Rptr->content.tok!=RPAREN){
            if(Rptr->content.tok==SEMI){
                Match(SEMI);
                t=ParamDecList();
                if(t==NULL) cout<<"ParamMore fails";
            }else Nextoken();
        }
        return t;
    }
	Treenode* Param(){
        Treenode* t=new Treenode(DecK);
        if(t!=NULL){//若创建不成功呢?
            ifRptr->content.tok==INTERGER||Rptr->content.tok==CHAR||Rptr->content.tok==RECORD||Rptr->content.tok==ARRAY||Rptr->content.tok==ID){
                t->arr.param=valparamtype;
                TypeDef(t);
                FormList(t);
            }else if(Rptr->content.tok==VAR){
                Match(VAR);
                t->arr.param=varparamtype;
                TypeDef(t);
                FormList(t);
            }else Nextoken();
        }
        return t;
    }
	void FormList(Treenode* t){
        if(Rptr->content.tok==ID){
            t->name[t->idnum]=Rptr->content.str;
            t->idnum++；
            Match(ID);
        }   else FidMore();
    }
	void FidMore(Treenode* t){
        if(Rptr->content.tok!=SEMI&&Rptr->content.tok!=RPAREN){
            if(Rptr->content.tok==COMMA){
                Match(COMMA);
                FormList(t);
            }else Nextoken();
        }
    }
	Treenode* VarDec(){
        Tree*node t=NULL;
        if(Rptr->content.tok!=PROCEDURE&&Rptr->content.tok!=BEGIN){
            if(Rptr->content.tok==VAR) VarDeclaration();
            else Nextoken();
        }
        return t;
    }
	Treenode* VarDeclaration(){
        Match(VAR);
        Treenode* t=VarDecList();
        if(t==NULL) cout<<"VarDeclaration fails";
        return t;
    }
	Treenode* VarDecList(){
        Treenode* t=new Treenode(Deck);
        Treenode* p=NULL;
        if(t!=NULL){
            TypeDec(t);
            VarIdList(t);
            Match(SEMI);
            p=VarDecMore();
            t->sibling=p;
        }
        return t;
    }
	Treenode* VarIdMore(){
        Treenode* t=NULL;
        if(Rptr->content.tok!=PROCEDURE&&Rptr->content.tok!=BEGIN){
            if(Rptr->content.tok==INTERGER||Rptr->content.tok==CHAR||Rptr->content.tok==ARRAY||Rptr->content.tok==RECORD||Rptr->content.tok==ID)
                t=VarDeclaration();
            else Nextoken();
        }
        return t;
    }
	void VarIdList(Treenode* t){
        if(Rptr->content.tok==ID){
            t->name[t->idnum]=Rptr->content.str;
            t->idnum++;
        }else cout<<"VarIdList fails";
        VarIdMore(t);
    }
	void TypeDecMore(Treenode* t){
        if(Rptr->content.tok!=SEMI)
            if(Rptr->content.tok==COMMA){
                Match(COMMA);
                VarIdList(t);
            }else Nextoken();
    }
	Treenode* StmList(){
        Treenode* t,p;
        t=Stm();
        p=StmMore();
        if(p!=NULL) t->sibling=p;
        return t;
    }
	Treenode* StmMore(){
        Treenode* t=NULL;
        if(Rptr->content.tok!=END&&Rptr->content.tok!=ENDWH){
            if(Rptr->content.tok==SEMI){
                Match(SEMI);
                t=StmList();//???
            }else Nextoken();
        }
        return t;
    }
	Treenode* Stm(){
        Treenode* t=NULL;
        if(Rptr->content.tok==IF)
            t=ConditionalStm();
        else if(Rptr->content.tok==WHILE)
            t=LoopStm();
        else if(Rptr->content.tok==RETURN)
            t=ReturnStm();
        else if(Rptr->content.tok==READ)
            t=InputStm();
        else if(Rptr->content.tok==WRITE)
            t=OutputStm();
        else if(Rptr->content.tok==ID){
            string temp_name=Rptr->content.str;
            t=AssCall();
        }else Nextoken();

        return t;
    }
	Treenode* AssCall(){
        Treenode* t=NULL;
        if(Rptr->content.tok==ASSIGN)
            t=AssignmentRest();
        else if(Rptr->content.tok==LPAREN)
            t=CallStmRest();
        else Nextoken();
        return t;
    }
	Treenode* AssignmentRest(){
        Treenode* t=new Treenode(AssignK);
        Match(EQ);
        if(t!=NULL){
            t->child[0]=Exp();
            t->name=temp_name;
        }
        return t;
    }
	Treenode* ConditionalStm(){
        Treenode* t=new Treenode(IfK);
        Match(IF);
        if(t!=NULL)
            t->child[0]=Exp();//if真的语句
        Match(THEN);
            t->child[1]=StmList();//if真的语句
        //Match(THEN);
        if(Rptr->content.tok==ELSE){
            Match(ELSE);
            if(t!=NULL) t->child[2]=StmList();
        }
        Match(FI);
        return t;
    }
	Treenode* LoopStm(){
        Treenode* t=new Treenode(WhileK);
        Match(WHILE);
        if(t!=NULL)
            t->child[0]=Exp();//if真的语句
        Match(DO);
            t->child[1]=StmList();//if真的语句
        
        return t;
    }
	Treenode* InputStm(){
        Treenode* t=new Treenode(ReadK);
        Match(READ);
        Match(RPAREN);
        if(t!=NULL&&Rptr->content.tok==ID)
            t->name[t->idnum]=Rptr->content.str;
        Match(ID);
        Match(LPAREN);
            
        return t;
    }
	Treenode* OutputStm(){
        Treenode* t=new Treenode(WriteK);
        Match(WRITE);
        Match(LPAREN);
        if(t!=NULL)
            t->child[0]=exp();
        Match(RPAREN);
        return t;
    }
        
	Treenode* ReturnStm(){
         Treenode* t=new Treenode(ReturnK);
         Match(RETURN);
         return t;
    }
	Treenode* CallStmRest(){
        Treenode* t=new Treenode(CallK);
        Match(LPAREN);
        if(t!=NULL){
            t->child[0]=ActParamList();
            t->name[idnum++]=Rptr->content.str;
        }
        Match(RPAREN);
        return t;
    }
	Treenode* ActParamList(){
        Treenode* t=NULL
        if(Rptr->content.tok!=RPAREN){
            if(Rptr->content.tok==ID||Rptr->content.tok==INTC){
                t=Exp();
                if(t==NULL)
                    t->sibling=ActParamMore();
            }else Nextoken();
        }
        return t;
    }
	Treenode* ActParamMore(){
         Treenode* t=NULL;
        if(Rptr->content.tok!=RPAREN){
            if(Rptr->content.tok==COMMA){
                Match(COMMA);
                t=ActParamList();
            }else Nextoken();
        }
        return t;
    }
	Treenode* SimExp(){
        Treenode* t=Term();
        while(Rptr->content.tok==PLUS||Rptr->content.tok==MINUS){
         if(Rptr->content.tok==PLUS){
             Treenode* p=new Treenode(OpK);
             if(p!=NULL){
                 P->child[0]=t;
                 p->arr.op=Rptr->content.tok;
                 t=p;
             }
              Match(PLUS);//???LT或EQ???
         }
         if(Rptr->content.tok==MINUS){
             Treenode* p=new Treenode(OpK);
             if(p!=NULL){
                 P->child[0]=t;
                 p->arr.op=Rptr->content.tok;
                 t=p;
             }
              Match(MINUS);//???LT或EQ???
         }
          t->child[1]=SimExp();
        }
           
        return t;
    }
	Treenode* OtherRelE();
	Treenode* Exp(){
         Treenode* t=SimExp();
         if(Rptr->content.tok==LT){
             Treenode* p=new Treenode(OpK);
             if(p!=NULL){
                 P->child[0]=t;
                 p->arr.op=Rptr->content.tok;
                 t=p;
             }Match(LT);//???LT或EQ???
         }
         
         if(Rptr->content.tok==EQ){
             Treenode* p=new Treenode(OpK);
             if(p!=NULL){
                 P->child[0]=t;
                 p->arr.op=Rptr->content.tok;
                 t=p;
             } Match(EQ);
         }
         if(t!=NULL)
            t->child[1]=SimExp();
        return t;
    }
	//Treenode* OtherTerm();
	Treenode* Term(){
        Treenode* t=Factor();
        while(Rptr->content.tok==TIMES||Rptr->content.tok==OVER){
         if(Rptr->content.tok==TIMES){
             Treenode* p=new Treenode(OpK);
             if(p!=NULL){
                 P->child[0]=t;
                 p->arr.op=Rptr->content.tok;
                 t=p;
             }
              Match(TIMES);//???LT或EQ???
         }
         if(Rptr->content.tok==OVER){
             Treenode* p=new Treenode(OpK);
             if(p!=NULL){
                 P->child[0]=t;
                 p->arr.op=Rptr->content.tok;
                 t=p;
             }
              Match(OVER);//???LT或EQ???
         }
          t->child[1]=Factor();
        }
           
        return t;
    }
	Treenode* Factor(){
        Treenode* t=NULL;//???
        if(Rptr->content.tok==INTC) //???

        else if(Rptr->content.tok==ID) t=Variable();
        else if(Rptr->content.tok==LPAREN){
            Match(LPAREN);
            t=Exp();
            Match(RPAREN)；
        }else{
            cout<<"wrong token:"<<Rptr->content.tok;
            Nextoken();
        }
        return t;//当前吗???
    }
	//Treenode* OtherFactor();
	Treenode* Variable(){
        Treenode* t=new Treenode(ExpK);
        if(t!=NULL&&Rptr->content.tok==ID){
        t->lineno=Rcurline;//???
        t->name[t->idnum++]=Rptr->content.str;//???
        Match(ID);
        VariMore(t);
        }
        return t;
        
    }
	void VariMore(Treenode* t){
        if(Rptr->content.tok==LMIDPAREN){
            Match(LMIDPAREN);
            t->child[0]=Exp();
            t->arr.var
        }
    }
	Treenode* FieldVar(){
        Treenode* t=new Treenode(ExpK);
        if(t!=NULL&&Rptr->content.tok==ID){
        t->lineno=Rcurline;//???
        t->name[t->idnum++]=Rptr->content.str;//???
        Match(ID);
        FieldVarMore(t);
        }
        return t;
    }
	Treenode* FieldVarMore();
	Treenode* CmpOp();
	Treenode* AddOp();
	Treenode* MultOp();
	Treenode* ProgramBody(){
        Treenode* t=new Treenode(StmK);
        Match(BEGIN);
        if(t!=NULL) t->child[0]=StmList();
        Match(END);
        return t;
    }
	Treenode* ProgramName();
	Treenode* VarDecMore();
	Treenode* TypeDec(){
		Treenode* t = NULL;
		LexType temp = Rptr->content.tok;
		if (temp== TYPE) {
			t = TypeDeclaration();
			return t;
		}else {
			if (temp == VAR || temp == PROCEDURE || temp == BEGIN) {  }//??
				
			else Nextoken(); 
		}
		return t;
	}

	Treenode* TypeDeclaration() {
		Match(TYPE);
		Treenode* t = TypeDecList();
		if (t == NULL) cout << "typedeclaration fails";
		else return t;
	}

	Treenode* TypeDecList() {
		Treenode* t = new Treenode(DecK);
		if (t != NULL) {
			TypeId();
			Match(EQ);
			TypeDef();
			Match(SEMI);
			t3=TypeDecMore();
            if(t3!=NULL) t->sibling=t3;
		}
		return t;
	}
    Treenode* TypedecMore(){
        Treenode 
    }
	void TypeId(Treenode*  t) {
        if(Rptr->content.tok==ID&&t!=NULL){
            t->name[idnum]=Rptr->content.str
            t->idnum++;
        }
        Match(ID);
	}

	void TypeDef(Treenode*  t){
        if(t!=NULL){
            if(Rptr->content.tok==INTERGER||Rptr->content.tok==CHAR){
                BaseType();
            }else if(Rptr->content.tok==ARRAY||Rptr->content.tok==RECORD){
                StructureType();
            }else if(Rptr->content.tok==ID){
                t->kind=IdK;
                typename[0]=Rptr->content.str;
                Match(ID);
            }else Nextoken();
        }
    }
	void BaseType(Treenode* t){
        if(Rptr->content.tok==INTERGER){
            Match(INTERGER);
            t->kind=IntergerK;
        }else if(Rptr->content.tok==CHAR){
            Match(CHAR);
            t->kind=CharK;
        }else Nextoken();
    }
	void StructureType(Treenode* t){
        if(Rptr->content.tok==ARRAY){
            ArrayType();
        }else if(Rptr->content.tok==RECORD){
            t->kind=RecordK;
            RecType();
        }else Nextoken();
    }
	void ArrayType(Treenode* t){
        Match(ARRAY);
        Match(LMIDPAREN);
        if(Rptr->content.tok==INTC){
            int temp=0;
            size=sizeof(Rptr->content.str);
            for(int i=0;i<size;i++){
               temp=temp*10+Rptr->content.str[i](Rptr->content.str-48);
            }
            t->arr.under=temp;
        }
        Match(INTC);
        Match(UNDERANGE);
        if(Rptr->content.tok==INTC){
            int temp=0;
            size=sizeof(Rptr->content.str);
            for(int i=0;i<size;i++){
               temp=temp*10+Rptr->content.str[i](Rptr->content.str-48);
            }
            t->arr.up=temp;
        }
        Match(INTC);
        Match(RMIDPAREN);
        Match(OF);
        BaseType(t);
        t->arr.childKind=t->kind;
        t->kind=ArrayK;
    }
	void RecType(Treenode* t){
        Match(RECORD);
        Treenode* p=FieldDecList();
        if(p!=NULL){
            p=Tt->child[0];///???
        }else{
            cout<<"RecType fails!";
        }
    }
	Treenode* FieldDecList(){//理解???
        Treenode* t=new Treenode(DecK);
        Treenode* p=NULL;
        if(t!=NULL){
            if(Rptr->content.tok==INTERGER||Rptr->content.tok==CHAR){
                BaseType(t);
                IdList(t);
                Match(SEMI);
                p=FieldDecMore();
            }else if(Rptr->content.tok==ARRAY){
                ArrayType(t);
                IdList(t);
                Match(SEMI);
                p=FieldDecMore();
            }else Nextoken();
            t->sibling=p;
        }
    }
	 void IdList(Treenode* t){
        if(Rptr->content.tok==ID){
            t->name[t->idnum]=Rptr->content.str;
            t->idnum++;
        }else IdMore();
    }

	Treenode* IdMore(){
        if(Rptr->content.tok!=SEMI){
            if(Rptr->content.tok==COMMA){
                Match(COMMA);
                IdList();
            }else Nextoken();
        }
    }
	Treenode* FieldDecMore(){
        if(Rptr->content.tok!=END){
            if(Rptr->content.tok==INTERGER||Rptr->content.tok==CHAR||Rptr->content.tok==ARRAY)
                Treenode* t=FieldDecList();
            else Nextoken();
        }
        return t;
    }