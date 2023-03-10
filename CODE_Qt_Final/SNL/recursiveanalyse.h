#ifndef RECURSIVEANALYSE_H
#define RECURSIVEANALYSE_H

#include"lexeranalyse.h"
#include"parseanalyse.h"
#include <vector>
#include <string>
#include <cstring>
using namespace std;

extern TreeNode * getParseTree();  // 方便语义分析程序获取语法树

#endif // RECURSIVEANALYSE_H
