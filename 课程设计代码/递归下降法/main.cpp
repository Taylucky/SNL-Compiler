#include "cifa.h"
#include"parse.h"
#include <cstdio>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include<fstream>
#include<sstream>
#include<queue>
#include<utility>
using namespace std;
int main()
{
	Initial();
	TreeNode* root = Program();
	if (root)
	{
		cout << "Oh!!!Success" << endl;
		out_tree(root);
	}
	else {
		cout << "So sad,it's failed!" << endl;
	}
	return 0;
}