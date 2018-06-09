#ifndef _PARSER_H_
#define _PARSER_H_
#include "record.h"
#include "tree.h"
#include "innerCode.h"
#include <vector>
#include <set>

#include"record.h"
using namespace std;

class Parser {
public:

	Parser(TreePtr);	//构造函数
	~Parser();	//析构函数

private:
	map<string, funcNode> funcPool;			//函数池
	vector<Record> recordStack;				//维护的栈
	InnerCode innerCode;					//中间代码生成工具
	//set<string> build_in_function;

	TreePtr root;

	void Init();
	void parseTree(TreePtr node);

	
	TreePtr parserDeclaration(TreePtr node);		//分析parserDeclaration的节点
	void parserInitDeclaratorList(string, TreePtr);
	void parserInitDeclarator(string, TreePtr );			//分析parserInitDeclarator的节点

	TreePtr parserFunctionDefinition(TreePtr);
	void parserParameterList(TreePtr,string,bool);			//获取函数形参列表
	void parserParameterDeclaration(TreePtr, string,bool);	//获取函数单个形参

	TreePtr parserStatement(TreePtr);

	void parserExpressionStatement(TreePtr);
	varNode parserExpression(TreePtr);

	void parserArgumentExpressionList(TreePtr,string);

	void parserJumpStatement(TreePtr);
	void parserCompoundStatement(TreePtr);
	void parserSelectionStatement(TreePtr);
	void parserIterationStatement(TreePtr);

	varNode parserAssignmentExpression(TreePtr);			//赋值表达式
	varNode parserLogicalOrExpression(TreePtr);			//逻辑或表达式
	varNode parserLogicalAndExpression(TreePtr);		//逻辑或表达式
	varNode parserInclusiveOrExpression(TreePtr);
	varNode parserExclusiveOrExpression(TreePtr);
	varNode parserAndExpression(TreePtr);
	varNode parserEqualityExpression(TreePtr);
	varNode parserRelationalExpression(TreePtr);
	varNode parserShiftExpression(TreePtr);
	varNode parserAdditiveExpression(TreePtr);
	varNode parserMultiplicativeExpression(TreePtr);
	varNode parserUnaryExpression(TreePtr);
	varNode parserPostfixExpression(TreePtr);
	varNode parserPrimaryExpression(TreePtr);


	string lookupVar(string name);			//返回变量类型，找不到返回""
	bool lookupCurruntVar(string name);		//查找当前块的var
	struct varNode lookupNode(string name);	//返回变量节点
	string getFuncRType();
	string getArrayType(string name);
	struct arrayNode getArrayNode(string name);

	int getBreakRecordNumber();

	struct varNode createVar(string name, string type);

	void printError(int line, string error);

	void print_map();
};




#endif // !_parser_H_