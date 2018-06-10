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

	
	TreePtr parser_declaration(TreePtr node);		//分析parser_declaration的节点
	void parser_init_declarator_list(string, TreePtr);
	void parser_init_declarator(string, TreePtr );			//分析parser_init_declarator的节点

	TreePtr parser_function_definition(TreePtr);
	void parser_parameter_list(TreePtr,string,bool);			//获取函数形参列表
	void parser_parameter_declaration(TreePtr, string,bool);	//获取函数单个形参

	TreePtr parser_statement(TreePtr);

	void parser_expression_statement(TreePtr);
	varNode parser_expression(TreePtr);

	void parser_argument_expression_list(TreePtr,string);

	void parser_jump_statement(TreePtr);
	void parser_compound_statement(TreePtr);
	void parser_selection_statement(TreePtr);
	void parser_iteration_statement(TreePtr);

	varNode parser_assignment_expression(TreePtr);			//赋值表达式
	varNode parser_logical_or_expression(TreePtr);			//逻辑或表达式
	varNode parser_logical_and_expression(TreePtr);		//逻辑或表达式
	varNode parser_inclusive_or_expression(TreePtr);
	varNode parser_exclusive_or_expression(TreePtr);
	varNode parser_and_expression(TreePtr);
	varNode parser_equality_expression(TreePtr);
	varNode parser_relational_expression(TreePtr);
	varNode parser_shift_expression(TreePtr);
	varNode parser_additive_expression(TreePtr);
	varNode parser_multiplicative_expression(TreePtr);
	varNode parser_unary_expression(TreePtr);
	varNode parser_postfix_expression(TreePtr);
	varNode parser_primary_expression(TreePtr);


	string lookupVar(string name);			//返回变量类型，找不到返回""
	bool lookupCurruntVar(string name);		//查找当前块的var
	struct varNode lookupNode(string name);	//返回变量节点
	string getFuncRType();
	string getArrayType(string);
	struct arrayNode getArrayNode(string);

	int getBreakRecordNumber();

	struct varNode createTempVar(string name, string type);

	void error(int line, string error);

	void print_map();
	void print_code();
};




#endif // !_parser_H_