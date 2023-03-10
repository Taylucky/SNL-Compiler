#ifndef RECURSIVEANALYSE_H_INCLUDED
#define RECURSIVEANALYSE_H_INCLUDED

#include"lexerAnalyse.h"
#include"parseAnalyse.h"
#include <vector>
#include <string>
#include <cstring>
using namespace std;

void outputParserTree(TreeNode* rt);
TreeNode * recursiveAnalyse();

#endif