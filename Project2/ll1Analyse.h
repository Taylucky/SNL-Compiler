#ifndef LL1ANALYSE_H_INCLUDED
#define LL1ANALYSE_H_INCLUDED

#include"lexerAnalyse.h"
#include"parseAnalyse.h"
#include"parseAnalyse.h"
#include <vector>
#include <string>
#include <cstring>
using namespace std;

void outputLL1Tree(TreeNode* rt,const char fileName[]);
TreeNode* ll1Analyse();
//string tokenToStr();//token±äÎªstring
#endif