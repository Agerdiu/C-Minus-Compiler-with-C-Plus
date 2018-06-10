#ifndef _TREE_H_
#define _TREE_H_

#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cstdarg>
#include<iostream>
#include<string>
extern char *yytext;
extern int yylineno;//提供当前行数信息
using namespace std;


struct Tree {
    string content;
    string name;
    int line;       //所在代码行数
    struct Tree* left;
    struct Tree* right;
};
typedef struct Tree* TreePtr;

extern TreePtr root;

struct Tree* createTree(string name, int num,...);
//void print(TreePtr head,int leavel);
void freeTree(TreePtr node);

#endif