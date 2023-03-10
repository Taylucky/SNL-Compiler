#include"lexeranalyse.h"
#include"parseanalyse.h"

size_t tkPtr;   // token 指针
vector<Token> tokens;   // tokens 序列
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


static map<TokenType,string> tokenStrMap;
static bool setTokenStrMap = false;
string getTokenStr(TokenType tkty)
{
    if(!setTokenStrMap)
    {
        tokenStrMap[ENDFILE]=string("ENDFILE");
        tokenStrMap[ERROR]=string("ERROR");
        tokenStrMap[PROGRAM]=string("PROGRAM");
        tokenStrMap[PROCEDURE]=string("PROCEDURE");
        tokenStrMap[TYPE]=string("TYPE");
        tokenStrMap[VAR]=string("VAR");
        tokenStrMap[IF]=string("IF");
        tokenStrMap[THEN]=string("THEN");
        tokenStrMap[ELSE]=string("ELSE");
        tokenStrMap[FI]=string("FI");
        tokenStrMap[WHILE]=string("WHILE");
        tokenStrMap[DO]=string("DO");
        tokenStrMap[ENDWH]=string("ENDWH");
        tokenStrMap[BEGIN]=string("BEGIN");
        tokenStrMap[END]=string("END");
        tokenStrMap[READ]=string("READ");
        tokenStrMap[WRITE]=string("WRITE");
        tokenStrMap[ARRAY]=string("ARRAY");
        tokenStrMap[OF]=string("OF");
        tokenStrMap[RECORD]=string("RECORD");
        tokenStrMap[RETURN]=string("RETURN");
        tokenStrMap[INTEGER]=string("INTEGER");
        tokenStrMap[CHAR]=string("CHAR");
        tokenStrMap[ID]=string("ID");
        tokenStrMap[INTC]=string("INTC");
        tokenStrMap[CHARC]=string("CHARC");
        tokenStrMap[ASSIGN]=string("ASSIGN");
        tokenStrMap[EQ]=string("EQ");
        tokenStrMap[LT]=string("LT");
        tokenStrMap[PLUS]=string("PLUS");
        tokenStrMap[MINUS]=string("MINUS");
        tokenStrMap[TIMES]=string("TIMES");
        tokenStrMap[OVER]=string("OVER");
        tokenStrMap[LPAREN]=string("LPAREN");
        tokenStrMap[RPAREN]=string("RPAREN");
        tokenStrMap[DOT]=string("DOT");
        tokenStrMap[COLON]=string("COLON");
        tokenStrMap[SEMI]=string("SEMI");
        tokenStrMap[COMMA]=string("COMMA");
        tokenStrMap[LMIDPAREN]=string("LMIDPAREN");
        tokenStrMap[RMIDPAREN]=string("RMIDPAREN");
        tokenStrMap[UNDERANGE]=string("UNDERANGE");

        tokenStrMap[LAMBDA]=string("LAMBDA");
        tokenStrMap[Program]=string("Program");
        tokenStrMap[ProgramHead]=string("ProgramHead");
        tokenStrMap[ProgramName]=string("ProgramName");
        tokenStrMap[DeclarePart]=string("DeclarePart");
        tokenStrMap[TypeDec]=string("TypeDec");
        tokenStrMap[TypeDeclaration]=string("TypeDeclaration");
        tokenStrMap[TypeDecList]=string("TypeDecList");
        tokenStrMap[TypeDecMore]=string("TypeDecMore");
        tokenStrMap[TypeId]=string("TypeId");
        tokenStrMap[TypeName]=string("TypeName");
        tokenStrMap[BaseType]=string("BaseType");
        tokenStrMap[StructureType]=string("StructureType");
        tokenStrMap[ArrayType]=string("ArrayType");
        tokenStrMap[Low]=string("Low");
        tokenStrMap[Top]=string("Top");
        tokenStrMap[RecType]=string("RecType");
        tokenStrMap[FieldDecList]=string("FieldDecList");
        tokenStrMap[FieldDecMore]=string("FieldDecMore");
        tokenStrMap[IdList]=string("IdList");
        tokenStrMap[IdMore]=string("IdMore");
        tokenStrMap[VarDec]=string("VarDec");
        tokenStrMap[VarDeclaration]=string("VarDeclaration");
        tokenStrMap[VarDecList]=string("VarDecList");
        tokenStrMap[VarDecMore]=string("VarDecMore");
        tokenStrMap[VarIdList]=string("VarIdList");
        tokenStrMap[VarIdMore]=string("VarIdMore");
        tokenStrMap[ProcDec]=string("ProcDec");
        tokenStrMap[ProcDeclaration]=string("ProcDeclaration");
        tokenStrMap[ProcDecMore]=string("ProcDecMore");
        tokenStrMap[ProcName]=string("ProcName");
        tokenStrMap[ParamList]=string("ParamList");
        tokenStrMap[ParamDecList]=string("ParamDecList");
        tokenStrMap[ParamMore]=string("ParamMore");
        tokenStrMap[Param]=string("Param");
        tokenStrMap[FormList]=string("FormList");
        tokenStrMap[FidMore]=string("FidMore");
        tokenStrMap[ProcDecPart]=string("ProcDecPart");
        tokenStrMap[ProcBody]=string("ProcBody");
        tokenStrMap[ProgramBody]=string("ProgramBody");
        tokenStrMap[StmList]=string("StmList");
        tokenStrMap[StmMore]=string("StmMore");
        tokenStrMap[Stm]=string("Stm");
        tokenStrMap[AssCall]=string("AssCall");
        tokenStrMap[AssignmentRest]=string("AssignmentRest");
        tokenStrMap[ConditionalStm]=string("ConditionalStm");
        tokenStrMap[StmL]=string("StmL");
        tokenStrMap[LoopStm]=string("LoopStm");
        tokenStrMap[InputStm]=string("InputStm");
        tokenStrMap[InVar]=string("InVar");
        tokenStrMap[OutputStm]=string("OutputStm");
        tokenStrMap[ReturnStm]=string("ReturnStm");
        tokenStrMap[CallStmRest]=string("CallStmRest");
        tokenStrMap[ActParamList]=string("ActParamList");
        tokenStrMap[ActParamMore]=string("ActParamMore");
        tokenStrMap[RelExp]=string("RelExp");
        tokenStrMap[OtherRelE]=string("OtherRelE");
        tokenStrMap[Exp]=string("Exp");
        tokenStrMap[OtherTerm]=string("OtherTerm");
        tokenStrMap[Term]=string("Term");
        tokenStrMap[OtherFactor]=string("OtherFactor");
        tokenStrMap[Factor]=string("Factor");
        tokenStrMap[Variable]=string("Variable");
        tokenStrMap[VariMore]=string("VariMore");
        tokenStrMap[FieldVar]=string("FieldVar");
        tokenStrMap[FieldVarMore]=string("FieldVarMore");
        tokenStrMap[CmpOp]=string("CmpOp");
        tokenStrMap[AddOp]=string("AddOp");
        tokenStrMap[MultOp]=string("MultOp");
        setTokenStrMap=true;
    }

    if(tokenStrMap[tkty].size())
        return tokenStrMap[tkty];
    return string("unknown TokenType");
}



//------------------------------- 产生式 ---------------------------------------
vector<Symbol> products[ProductExpressionNum+5];
static bool isInitProducts=false;

void initProducts()     // 共104条产生式
{
    if(isInitProducts) return ;
    isInitProducts=true;
    products[1].push_back(Symbol::Program);
    products[1].push_back(Symbol::ProgramHead);
    products[1].push_back(Symbol::DeclarePart);
    products[1].push_back(Symbol::ProgramBody);
    products[2].push_back(Symbol::ProgramHead);
    products[2].push_back(Symbol::PROGRAM);
    products[2].push_back(Symbol::ProgramName);
    products[3].push_back(Symbol::ProgramName);
    products[3].push_back(Symbol::ID);
    products[4].push_back(Symbol::DeclarePart);
    products[4].push_back(Symbol::TypeDec);
    products[4].push_back(Symbol::VarDec);
    products[4].push_back(Symbol::ProcDec);
    products[5].push_back(Symbol::TypeDec);
    products[5].push_back(Symbol::LAMBDA);
    products[6].push_back(Symbol::TypeDec);
    products[6].push_back(Symbol::TypeDeclaration);
    products[7].push_back(Symbol::TypeDeclaration);
    products[7].push_back(Symbol::TYPE);
    products[7].push_back(Symbol::TypeDecList);
    products[8].push_back(Symbol::TypeDecList);
    products[8].push_back(Symbol::TypeId);
    products[8].push_back(Symbol::EQ);
    products[8].push_back(Symbol::TypeName);
    products[8].push_back(Symbol::SEMI);
    products[8].push_back(Symbol::TypeDecMore);
    products[9].push_back(Symbol::TypeDecMore);
    products[9].push_back(Symbol::LAMBDA);
    products[10].push_back(Symbol::TypeDecMore);
    products[10].push_back(Symbol::TypeDecList);
    products[11].push_back(Symbol::TypeId);
    products[11].push_back(Symbol::ID);
    products[12].push_back(Symbol::TypeName);
    products[12].push_back(Symbol::BaseType);
    products[13].push_back(Symbol::TypeName);
    products[13].push_back(Symbol::StructureType);
    products[14].push_back(Symbol::TypeName);
    products[14].push_back(Symbol::ID);
    products[15].push_back(Symbol::BaseType);
    products[15].push_back(Symbol::INTEGER);
    products[16].push_back(Symbol::BaseType);
    products[16].push_back(Symbol::CHAR);
    products[17].push_back(Symbol::StructureType);
    products[17].push_back(Symbol::ArrayType);
    products[18].push_back(Symbol::StructureType);
    products[18].push_back(Symbol::RecType);
    products[19].push_back(Symbol::ArrayType);
    products[19].push_back(Symbol::ARRAY);
    products[19].push_back(Symbol::LMIDPAREN);
    products[19].push_back(Symbol::Low);
    products[19].push_back(Symbol::UNDERANGE);
    products[19].push_back(Symbol::Top);
    products[19].push_back(Symbol::RMIDPAREN);
    products[19].push_back(Symbol::OF);
    products[19].push_back(Symbol::BaseType);
    products[20].push_back(Symbol::Low);
    products[20].push_back(Symbol::INTC);
    products[21].push_back(Symbol::Top);
    products[21].push_back(Symbol::INTC);
    products[22].push_back(Symbol::RecType);
    products[22].push_back(Symbol::RECORD);
    products[22].push_back(Symbol::FieldDecList);
    products[22].push_back(Symbol::END);
    products[23].push_back(Symbol::FieldDecList);
    products[23].push_back(Symbol::BaseType);
    products[23].push_back(Symbol::IdList);
    products[23].push_back(Symbol::SEMI);
    products[23].push_back(Symbol::FieldDecMore);
    products[24].push_back(Symbol::FieldDecList);
    products[24].push_back(Symbol::ArrayType);
    products[24].push_back(Symbol::IdList);
    products[24].push_back(Symbol::SEMI);
    products[24].push_back(Symbol::FieldDecMore);
    products[25].push_back(Symbol::FieldDecMore);
    products[25].push_back(Symbol::LAMBDA);
    products[26].push_back(Symbol::FieldDecMore);
    products[26].push_back(Symbol::FieldDecList);
    products[27].push_back(Symbol::IdList);
    products[27].push_back(Symbol::ID);
    products[27].push_back(Symbol::IdMore);
    products[28].push_back(Symbol::IdMore);
    products[28].push_back(Symbol::LAMBDA);
    products[29].push_back(Symbol::IdMore);
    products[29].push_back(Symbol::COMMA);
    products[29].push_back(Symbol::IdList);
    products[30].push_back(Symbol::VarDec);
    products[30].push_back(Symbol::LAMBDA);
    products[31].push_back(Symbol::VarDec);
    products[31].push_back(Symbol::VarDeclaration);
    products[32].push_back(Symbol::VarDeclaration);
    products[32].push_back(Symbol::VAR);
    products[32].push_back(Symbol::VarDecList);
    products[33].push_back(Symbol::VarDecList);
    products[33].push_back(Symbol::TypeName);
    products[33].push_back(Symbol::VarIdList);
    products[33].push_back(Symbol::SEMI);
    products[33].push_back(Symbol::VarDecMore);
    products[34].push_back(Symbol::VarDecMore);
    products[34].push_back(Symbol::LAMBDA);
    products[35].push_back(Symbol::VarDecMore);
    products[35].push_back(Symbol::VarDecList);
    products[36].push_back(Symbol::VarIdList);
    products[36].push_back(Symbol::ID);
    products[36].push_back(Symbol::VarIdMore);
    products[37].push_back(Symbol::VarIdMore);
    products[37].push_back(Symbol::LAMBDA);
    products[38].push_back(Symbol::VarIdMore);
    products[38].push_back(Symbol::COMMA);
    products[38].push_back(Symbol::VarIdList);
    products[39].push_back(Symbol::ProcDec);
    products[39].push_back(Symbol::LAMBDA);
    products[40].push_back(Symbol::ProcDec);
    products[40].push_back(Symbol::ProcDeclaration);
    products[41].push_back(Symbol::ProcDeclaration);
    products[41].push_back(Symbol::PROCEDURE);
    products[41].push_back(Symbol::ProcName);
    products[41].push_back(Symbol::LPAREN);
    products[41].push_back(Symbol::ParamList);
    products[41].push_back(Symbol::RPAREN);
    products[41].push_back(Symbol::SEMI);
    products[41].push_back(Symbol::ProcDecPart);
    products[41].push_back(Symbol::ProcBody);
    products[41].push_back(Symbol::ProcDecMore);
    products[42].push_back(Symbol::ProcDecMore);
    products[42].push_back(Symbol::LAMBDA);
    products[43].push_back(Symbol::ProcDecMore);
    products[43].push_back(Symbol::ProcDeclaration);
    products[44].push_back(Symbol::ProcName);
    products[44].push_back(Symbol::ID);
    products[45].push_back(Symbol::ParamList);
    products[45].push_back(Symbol::LAMBDA);
    products[46].push_back(Symbol::ParamList);
    products[46].push_back(Symbol::ParamDecList);
    products[47].push_back(Symbol::ParamDecList);
    products[47].push_back(Symbol::Param);
    products[47].push_back(Symbol::ParamMore);
    products[48].push_back(Symbol::ParamMore);
    products[48].push_back(Symbol::LAMBDA);
    products[49].push_back(Symbol::ParamMore);
    products[49].push_back(Symbol::SEMI);
    products[49].push_back(Symbol::ParamDecList);
    products[50].push_back(Symbol::Param);
    products[50].push_back(Symbol::TypeName);
    products[50].push_back(Symbol::FormList);
    products[51].push_back(Symbol::Param);
    products[51].push_back(Symbol::VAR);
    products[51].push_back(Symbol::TypeName);
    products[51].push_back(Symbol::FormList);
    products[52].push_back(Symbol::FormList);
    products[52].push_back(Symbol::ID);
    products[52].push_back(Symbol::FidMore);
    products[53].push_back(Symbol::FidMore);
    products[53].push_back(Symbol::LAMBDA);
    products[54].push_back(Symbol::FidMore);
    products[54].push_back(Symbol::COMMA);
    products[54].push_back(Symbol::FormList);
    products[55].push_back(Symbol::ProcDecPart);
    products[55].push_back(Symbol::DeclarePart);
    products[56].push_back(Symbol::ProcBody);
    products[56].push_back(Symbol::ProgramBody);
    products[57].push_back(Symbol::ProgramBody);
    products[57].push_back(Symbol::BEGIN);
    products[57].push_back(Symbol::StmList);
    products[57].push_back(Symbol::END);
    products[58].push_back(Symbol::StmList);
    products[58].push_back(Symbol::Stm);
    products[58].push_back(Symbol::StmMore);
    products[59].push_back(Symbol::StmMore);
    products[59].push_back(Symbol::LAMBDA);
    products[60].push_back(Symbol::StmMore);
    products[60].push_back(Symbol::SEMI);
    products[60].push_back(Symbol::StmList);
    products[61].push_back(Symbol::Stm);
    products[61].push_back(Symbol::ConditionalStm);
    products[62].push_back(Symbol::Stm);
    products[62].push_back(Symbol::LoopStm);
    products[63].push_back(Symbol::Stm);
    products[63].push_back(Symbol::InputStm);
    products[64].push_back(Symbol::Stm);
    products[64].push_back(Symbol::OutputStm);
    products[65].push_back(Symbol::Stm);
    products[65].push_back(Symbol::ReturnStm);
    products[66].push_back(Symbol::Stm);
    products[66].push_back(Symbol::ID);
    products[66].push_back(Symbol::AssCall);
    products[67].push_back(Symbol::AssCall);
    products[67].push_back(Symbol::AssignmentRest);
    products[68].push_back(Symbol::AssCall);
    products[68].push_back(Symbol::CallStmRest);
    products[69].push_back(Symbol::AssignmentRest);
    products[69].push_back(Symbol::VariMore);
    products[69].push_back(Symbol::ASSIGN);
    products[69].push_back(Symbol::Exp);
    products[70].push_back(Symbol::ConditionalStm);
    products[70].push_back(Symbol::IF);
    products[70].push_back(Symbol::RelExp);
    products[70].push_back(Symbol::THEN);
    products[70].push_back(Symbol::StmList);
    products[70].push_back(Symbol::ELSE);
    products[70].push_back(Symbol::StmList);
    products[70].push_back(Symbol::FI);
    products[71].push_back(Symbol::LoopStm);
    products[71].push_back(Symbol::WHILE);
    products[71].push_back(Symbol::RelExp);
    products[71].push_back(Symbol::DO);
    products[71].push_back(Symbol::StmList);
    products[71].push_back(Symbol::ENDWH);
    products[72].push_back(Symbol::InputStm);
    products[72].push_back(Symbol::READ);
    products[72].push_back(Symbol::LPAREN);
    products[72].push_back(Symbol::InVar);
    products[72].push_back(Symbol::RPAREN);
    products[73].push_back(Symbol::InVar);
    products[73].push_back(Symbol::ID);
    products[74].push_back(Symbol::OutputStm);
    products[74].push_back(Symbol::WRITE);
    products[74].push_back(Symbol::LPAREN);
    products[74].push_back(Symbol::Exp);
    products[74].push_back(Symbol::RPAREN);
    products[75].push_back(Symbol::ReturnStm);
    products[75].push_back(Symbol::RETURN);   //  RETURN  定义自己的规范
    products[76].push_back(Symbol::CallStmRest);
    products[76].push_back(Symbol::LPAREN);
    products[76].push_back(Symbol::ActParamList);
    products[76].push_back(Symbol::RPAREN);
    products[77].push_back(Symbol::ActParamList);
    products[77].push_back(Symbol::LAMBDA);
    products[78].push_back(Symbol::ActParamList);
    products[78].push_back(Symbol::Exp);
    products[78].push_back(Symbol::ActParamMore);
    products[79].push_back(Symbol::ActParamMore);
    products[79].push_back(Symbol::LAMBDA);
    products[80].push_back(Symbol::ActParamMore);
    products[80].push_back(Symbol::COMMA);
    products[80].push_back(Symbol::ActParamList);
    products[81].push_back(Symbol::RelExp);
    products[81].push_back(Symbol::Exp);
    products[81].push_back(Symbol::OtherRelE);
    products[82].push_back(Symbol::OtherRelE);
    products[82].push_back(Symbol::CmpOp);
    products[82].push_back(Symbol::Exp);
    products[83].push_back(Symbol::Exp);
    products[83].push_back(Symbol::Term);
    products[83].push_back(Symbol::OtherTerm);
    products[84].push_back(Symbol::OtherTerm);
    products[84].push_back(Symbol::LAMBDA);
    products[85].push_back(Symbol::OtherTerm);
    products[85].push_back(Symbol::AddOp);
    products[85].push_back(Symbol::Exp);
    products[86].push_back(Symbol::Term);
    products[86].push_back(Symbol::Factor);
    products[86].push_back(Symbol::OtherFactor);
    products[87].push_back(Symbol::OtherFactor);
    products[87].push_back(Symbol::LAMBDA);
    products[88].push_back(Symbol::OtherFactor);
    products[88].push_back(Symbol::MultOp);
    products[88].push_back(Symbol::Term);
    products[89].push_back(Symbol::Factor);
    products[89].push_back(Symbol::LPAREN);
    products[89].push_back(Symbol::Exp);
    products[89].push_back(Symbol::RPAREN);
    products[90].push_back(Symbol::Factor);
    products[90].push_back(Symbol::INTC);
    products[91].push_back(Symbol::Factor);
    products[91].push_back(Symbol::Variable);
    products[92].push_back(Symbol::Variable);
    products[92].push_back(Symbol::ID);
    products[92].push_back(Symbol::VariMore);
    products[93].push_back(Symbol::VariMore);
    products[93].push_back(Symbol::LAMBDA);
    products[94].push_back(Symbol::VariMore);
    products[94].push_back(Symbol::LMIDPAREN);
    products[94].push_back(Symbol::Exp);
    products[94].push_back(Symbol::RMIDPAREN);
    products[95].push_back(Symbol::VariMore);
    products[95].push_back(Symbol::DOT);
    products[95].push_back(Symbol::FieldVar);
    products[96].push_back(Symbol::FieldVar);
    products[96].push_back(Symbol::ID);
    products[96].push_back(Symbol::FieldVarMore);
    products[97].push_back(Symbol::FieldVarMore);
    products[97].push_back(Symbol::LAMBDA);
    products[98].push_back(Symbol::FieldVarMore);
    products[98].push_back(Symbol::LMIDPAREN);
    products[98].push_back(Symbol::Exp);
    products[98].push_back(Symbol::RMIDPAREN);
    products[99].push_back(Symbol::CmpOp);
    products[99].push_back(Symbol::LT);
    products[100].push_back(Symbol::CmpOp);
    products[100].push_back(Symbol::EQ);
    products[101].push_back(Symbol::AddOp);
    products[101].push_back(Symbol::PLUS);
    products[102].push_back(Symbol::AddOp);
    products[102].push_back(Symbol::MINUS);
    products[103].push_back(Symbol::MultOp);
    products[103].push_back(Symbol::TIMES);
    products[104].push_back(Symbol::MultOp);
    products[104].push_back(Symbol::OVER);               // 共 104 条产生式
}


/*
//------------------------------- Predict集 ---------------------------------------
set<TokenType> Predict[ProductExpressionNum+5];
static bool isInitPredict=false;

void initPredict()
{
    if(isInitPredict) return ;
    isInitPredict=true;
    Predict[1].insert(PROGRAM);
    Predict[2].insert(PROGRAM);
    Predict[3].insert(ID);
    Predict[4].insert(TYPE);
    Predict[4].insert(VAR);
    Predict[4].insert(PROCEDURE);
    Predict[4].insert(BEGIN);
    Predict[5].insert(VAR);
    Predict[5].insert(PROCEDURE);
    Predict[5].insert(BEGIN);
    Predict[6].insert(TYPE);
    Predict[7].insert(TYPE);
    Predict[8].insert(ID);
    Predict[9].insert(VAR);
    Predict[9].insert(PROCEDURE);
    Predict[9].insert(BEGIN);
    Predict[10].insert(ID);
    Predict[11].insert(ID);
    Predict[12].insert(INTEGER);
    Predict[12].insert(CHAR);
    Predict[13].insert(ARRAY);
    Predict[13].insert(RECORD);
    Predict[14].insert(ID);
    Predict[15].insert(INTEGER);
    Predict[16].insert(CHAR);
    Predict[17].insert(ARRAY);
    Predict[18].insert(RECORD);
    Predict[19].insert(ARRAY);
    Predict[20].insert(INTC);
    Predict[21].insert(INTC);
    Predict[22].insert(RECORD);
    Predict[23].insert(INTEGER);
    Predict[23].insert(CHAR);
    Predict[24].insert(ARRAY);
    Predict[25].insert(END);
    Predict[26].insert(INTEGER);
    Predict[26].insert(CHAR);
    Predict[26].insert(ARRAY);
    Predict[27].insert(ID);
    Predict[28].insert(SEMI);
    Predict[29].insert(COMMA);
    Predict[30].insert(PROCEDURE);
    Predict[30].insert(BEGIN);
    Predict[31].insert(VAR);
    Predict[32].insert(VAR);
    Predict[33].insert(INTEGER);
    Predict[33].insert(CHAR);
    Predict[33].insert(ARRAY);
    Predict[33].insert(RECORD);
    Predict[33].insert(ID);
    Predict[34].insert(PROCEDURE);
    Predict[34].insert(BEGIN);
    Predict[35].insert(INTEGER);
    Predict[35].insert(CHAR);
    Predict[35].insert(ARRAY);
    Predict[35].insert(RECORD);
    Predict[35].insert(ID);
    Predict[36].insert(ID);
    Predict[37].insert(SEMI);
    Predict[38].insert(COMMA);
    Predict[39].insert(BEGIN);
    Predict[40].insert(PROCEDURE);
    Predict[41].insert(PROCEDURE);
    Predict[42].insert(BEGIN);
    Predict[43].insert(PROCEDURE);
    Predict[44].insert(ID);
    Predict[45].insert(RPAREN);
    Predict[46].insert(INTEGER);
    Predict[46].insert(CHAR);
    Predict[46].insert(ARRAY);
    Predict[46].insert(RECORD);
    Predict[46].insert(ID);
    Predict[46].insert(VAR);
    Predict[47].insert(INTEGER);
    Predict[47].insert(CHAR);
    Predict[47].insert(ARRAY);
    Predict[47].insert(RECORD);
    Predict[47].insert(ID);
    Predict[47].insert(VAR);
    Predict[48].insert(RPAREN);
    Predict[49].insert(SEMI);
    Predict[50].insert(INTEGER);
    Predict[50].insert(CHAR);
    Predict[50].insert(ARRAY);
    Predict[50].insert(RECORD);
    Predict[50].insert(ID);
    Predict[51].insert(VAR);
    Predict[52].insert(ID);
    Predict[53].insert(SEMI);
    Predict[53].insert(RPAREN);
    Predict[54].insert(COMMA);
    Predict[55].insert(TYPE);
    Predict[55].insert(VAR);
    Predict[55].insert(PROCEDURE);
    Predict[55].insert(BEGIN);
    Predict[56].insert(BEGIN);
    Predict[57].insert(BEGIN);
    Predict[58].insert(ID);
    Predict[58].insert(IF);
    Predict[58].insert(WHILE);
    Predict[58].insert(RETURN);
    Predict[58].insert(READ);
    Predict[58].insert(WRITE);
    Predict[59].insert(END);
    Predict[59].insert(ENDWH);
    Predict[59].insert(ELSE);
    Predict[59].insert(FI);
    Predict[60].insert(SEMI);
    Predict[61].insert(IF);
    Predict[62].insert(WHILE);
    Predict[63].insert(READ);
    Predict[64].insert(WRITE);
    Predict[65].insert(RETURN);
    Predict[66].insert(ID);
    Predict[67].insert(ASSIGN);
    Predict[67].insert(LMIDPAREN);
    Predict[67].insert(DOT);
    Predict[68].insert(LPAREN);
    Predict[69].insert(ASSIGN);
    Predict[69].insert(LMIDPAREN);
    Predict[69].insert(DOT);
    Predict[70].insert(IF);
    Predict[71].insert(WHILE);
    Predict[72].insert(READ);
    Predict[73].insert(ID);
    Predict[74].insert(WRITE);
    Predict[75].insert(RETURN);
    Predict[76].insert(LPAREN);
    Predict[77].insert(RPAREN);
    Predict[78].insert(ID);
    Predict[78].insert(INTC);
    Predict[78].insert(LPAREN);
    Predict[79].insert(RPAREN);
    Predict[80].insert(COMMA);
    Predict[81].insert(LPAREN);
    Predict[81].insert(INTC);
    Predict[81].insert(ID);
    Predict[82].insert(LT);
    Predict[82].insert(EQ);
    Predict[83].insert(LPAREN);
    Predict[83].insert(INTC);
    Predict[83].insert(ID);
    Predict[84].insert(LT);
    Predict[84].insert(EQ);
    Predict[84].insert(THEN);
    Predict[84].insert(DO);
    Predict[84].insert(RPAREN);
    Predict[84].insert(END);
    Predict[84].insert(SEMI);
    Predict[84].insert(COMMA);
    Predict[84].insert(ENDWH);
    Predict[84].insert(ELSE);
    Predict[84].insert(FI);
    Predict[84].insert(RMIDPAREN);
    Predict[85].insert(PLUS);
    Predict[85].insert(MINUS);
    Predict[86].insert(LPAREN);
    Predict[86].insert(INTC);
    Predict[86].insert(ID);
    Predict[87].insert(PLUS);
    Predict[87].insert(MINUS);
    Predict[87].insert(LT);
    Predict[87].insert(EQ);
    Predict[87].insert(THEN);
    Predict[87].insert(ELSE);
    Predict[87].insert(FI);
    Predict[87].insert(DO);
    Predict[87].insert(ENDWH);
    Predict[87].insert(RPAREN);
    Predict[87].insert(END);
    Predict[87].insert(SEMI);
    Predict[87].insert(COMMA);
    Predict[87].insert(RMIDPAREN);
    Predict[88].insert(TIMES);
    Predict[88].insert(OVER);
    Predict[89].insert(LPAREN);
    Predict[90].insert(INTC);
    Predict[91].insert(ID);
    Predict[92].insert(ID);
    Predict[93].insert(ASSIGN);
    Predict[93].insert(TIMES);
    Predict[93].insert(OVER);
    Predict[93].insert(PLUS);
    Predict[93].insert(MINUS);
    Predict[93].insert(LT);
    Predict[93].insert(EQ);
    Predict[93].insert(THEN);
    Predict[93].insert(ELSE);
    Predict[93].insert(FI);
    Predict[93].insert(DO);
    Predict[93].insert(ENDWH);
    Predict[93].insert(RPAREN);
    Predict[93].insert(END);
    Predict[93].insert(SEMI);
    Predict[93].insert(COMMA);
    Predict[93].insert(RMIDPAREN);
    Predict[94].insert(LMIDPAREN);
    Predict[95].insert(DOT);
    Predict[96].insert(ID);
    Predict[97].insert(ASSIGN);
    Predict[97].insert(TIMES);
    Predict[97].insert(OVER);
    Predict[97].insert(PLUS);
    Predict[97].insert(MINUS);
    Predict[97].insert(LT);
    Predict[97].insert(EQ);
    Predict[97].insert(THEN);
    Predict[97].insert(ELSE);
    Predict[97].insert(FI);
    Predict[97].insert(DO);
    Predict[97].insert(ENDWH);
    Predict[97].insert(RPAREN);
    Predict[97].insert(END);
    Predict[97].insert(SEMI);
    Predict[97].insert(COMMA);
    Predict[98].insert(LMIDPAREN);
    Predict[99].insert(LT);
    Predict[100].insert(EQ);
    Predict[101].insert(PLUS);
    Predict[102].insert(MINUS);
    Predict[103].insert(TIMES);
    Predict[104].insert(OVER);
}

*/

// ------------------------------- 语法树 输出 ------------------------------------
// 采用松散形式输出语法树
#define OutTreeInsertBlank

#define MaxNodeKindsValue   int(NodeKinds::ExpK)
#define MaxDecKindsValue    int(DecKinds::IdK)
#define MaxStmtKindsValue   int(StmtKinds::ReturnK)
#define MaxExpKindsValue    int(ExpKinds::VariK)
#define MaxParamKindsValue  int(ParamKinds::VarparamType)
#define MaxVarKindsValue    int(VarKinds::FieldMembV)
#define MaxExpTypeValue     int(ExpType::Boolean)



string strNodeKinds[] = {"ProK","PheadK","TypeK","VarK","ProcDecK","StmLK","DecK","StmtK","ExpK"};
string strDecKinds[] = {"ArrayK","CharK","IntegerK","RecordK","IdK"} ;
string strStmtKinds[] = {"IfK","WhileK","AssignK","ReadK","WriteK","CallK","ReturnK"};
string strExpKinds[] = {"OpK","ConstK","VariK"} ;
string strParamKinds[] = {"ValparamType","VarparamType"} ;
string strVarKinds[] = {"IdV","ArrayMembV","FieldMembV"} ;
string strExpType[] = {"Void","Integer","Boolean"} ;


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
        tmpStr = tmpStr + "ExpK ";
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
