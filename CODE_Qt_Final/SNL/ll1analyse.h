#ifndef LL1ANALYSE_H
#define LL1ANALYSE_H

#include "lexeranalyse.h"
#include "parseanalyse.h"
#include <vector>
#include <string>
#include <cstring>
using namespace std;

void outputLL1Tree(TreeNode* rt,const char fileName[]);
extern TreeNode* getLL1Tree();

#endif // LL1ANALYSE_H

