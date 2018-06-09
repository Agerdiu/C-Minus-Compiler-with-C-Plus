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

	Parser(Tree*);	//构造函数
	~Parser();	//析构函数

private:
	map<string, funcNode> funcPool;			//函数池
	vector<Record> recordStack;				//维护的栈
	InnerCode innerCode;					//中间代码生成工具
	//set<string> build_in_function;

	struct Tree* root;

	void Init();
	void parseTree(struct Tree* node);

	
	struct Tree* parser_declaration(struct Tree* node);		//分析parser_declaration的节点
	void parser_init_declarator_list(string, struct Tree*);
	void parser_init_declarator(string, struct Tree* );			//分析parser_init_declarator的节点

	struct Tree* parser_function_definition(struct Tree*);
	void parser_parameter_list(struct Tree*,string,bool);			//获取函数形参列表
	void parser_parameter_declaration(struct Tree*, string,bool);	//获取函数单个形参

	struct Tree* parser_statement(struct Tree*);

	void parser_expression_statement(struct Tree*);
	varNode parser_expression(struct Tree*);

	void parser_argument_expression_list(struct Tree*,string);

	void parser_jump_statement(struct Tree*);
	void parser_compound_statement(struct Tree*);
	void parser_selection_statement(struct Tree*);
	void parser_iteration_statement(struct Tree*);

	varNode parser_assignment_expression(struct Tree*);			//赋值表达式
	varNode parser_logical_or_expression(struct Tree*);			//逻辑或表达式
	varNode parser_logical_and_expression(struct Tree*);		//逻辑或表达式
	varNode parser_inclusive_or_expression(struct Tree*);
	varNode parser_exclusive_or_expression(struct Tree*);
	varNode parser_and_expression(struct Tree*);
	varNode parser_equality_expression(struct Tree*);
	varNode parser_relational_expression(struct Tree*);
	varNode parser_shift_expression(struct Tree*);
	varNode parser_additive_expression(struct Tree*);
	varNode parser_multiplicative_expression(struct Tree*);
	varNode parser_unary_expression(struct Tree*);
	varNode parser_postfix_expression(struct Tree*);
	varNode parser_primary_expression(struct Tree*);


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