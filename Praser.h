#ifndef _PRASER_H_
#define _PRASER_H_
#include "record.h"
#include "tree.h"
#include "innerCode.h"
#include <vector>
#include <set>

#include"record.h"
using namespace std;

class Praser {
public:

	Praser(Tree*);	//构造函数
	~Praser();	//析构函数

private:
	map<string, funcNode> funcPool;			//函数池
	vector<Record> recordStack;				//维护的栈
	InnerCode innerCode;					//中间代码生成工具
	//set<string> build_in_function;

	struct Tree* root;

	void praserInit();
	void praserTree(struct Tree* node);

	
	struct Tree* praser_declaration(struct Tree* node);		//分析praser_declaration的节点
	void praser_init_declarator_list(string, struct Tree*);
	void praser_init_declarator(string, struct Tree* );			//分析praser_init_declarator的节点

	struct Tree* praser_function_definition(struct Tree*);
	void praser_parameter_list(struct Tree*,string,bool);			//获取函数形参列表
	void praser_parameter_declaration(struct Tree*, string,bool);	//获取函数单个形参

	struct Tree* praser_statement(struct Tree*);

	void praser_expression_statement(struct Tree*);
	varNode praser_expression(struct Tree*);

	void praser_argument_expression_list(struct Tree*,string);

	void praser_jump_statement(struct Tree*);
	void praser_compound_statement(struct Tree*);
	void praser_selection_statement(struct Tree*);
	void praser_iteration_statement(struct Tree*);

	varNode praser_assignment_expression(struct Tree*);			//赋值表达式
	varNode praser_logical_or_expression(struct Tree*);			//逻辑或表达式
	varNode praser_logical_and_expression(struct Tree*);		//逻辑或表达式
	varNode praser_inclusive_or_expression(struct Tree*);
	varNode praser_exclusive_or_expression(struct Tree*);
	varNode praser_and_expression(struct Tree*);
	varNode praser_equality_expression(struct Tree*);
	varNode praser_relational_expression(struct Tree*);
	varNode praser_shift_expression(struct Tree*);
	varNode praser_additive_expression(struct Tree*);
	varNode praser_multiplicative_expression(struct Tree*);
	varNode praser_unary_expression(struct Tree*);
	varNode praser_postfix_expression(struct Tree*);
	varNode praser_primary_expression(struct Tree*);


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




#endif // !_PRASER_H_