#include"parser.h"
#include"record.h"
#include "tools.h"
#include<map>
using namespace std;

Parser::Parser(Tree* root) {
	this->root = root;
	Init();
}

Parser::~Parser() {
	print_code();
}

void Parser::Init() {
	Record wholeRecord;
	recordStack.push_back(wholeRecord);

	//事先内置函数print和read
	funcNode printFunc;
	printFunc.name = "print";
	printFunc.rtype = "void";
	varNode pnode;
	pnode.type = "int";
	printFunc.paralist.push_back(pnode);

	funcPool.insert({ "print", printFunc });

	funcNode readFunc;
	readFunc.name = "read";
	readFunc.rtype = "int";
	funcPool.insert({"read",readFunc });

	parseTree(root);		//开始分析语法树
}

void Parser::parseTree(struct Tree* node) {
	if (node == NULL || node->line == -1)
		return;

	if (node->name == "declaration") {
		node = parser_declaration(node);
	}
	else if (node->name == "function_definition") {
		node = parser_function_definition(node);
	}
	else if (node->name == "statement") {
		node = parser_statement(node);
	}

	//继续向下分析
	if (node != NULL) {
		parseTree(node->left);
		parseTree(node->right);
	}
}

struct Tree* Parser::parser_statement(struct Tree* node) {
	struct Tree* next = node->left;
	if (node->left->name == "labeled_statement") {

	}
	if (node->left->name == "compound_statement") {
		parser_compound_statement(node->left);
	}
	if (node->left->name == "expression_statement") {
		parser_expression_statement(node->left);
	}
	if (node->left->name == "selection_statement") {
		parser_selection_statement(node->left);
	}
	if (node->left->name == "iteration_statement") {
		parser_iteration_statement(node->left);
	}
	if (node->left->name == "jump_statement") {
		parser_jump_statement(node->left);
	}

	return node->right;
}

void Parser::parser_jump_statement(struct Tree* node) {
	if (node->left->name == "JUMP") {

	}
	else if (node->left->name == "CONTINUE") {

	}
	else if (node->left->name == "BREAK") {
		int num = getBreakRecordNumber();
		if (num < 0) {
			error(node->left->line, "This scope doesn't support break.");
		}
	
		innerCode.addCode("JUMP " + recordStack[num].breakLabelname);
	}
	else if (node->left->name == "RETURN") {
		string funcType = getFuncRType();
		if (node->left->right->name == "expression") {//return expression
			varNode rnode = parser_expression(node->left->right);
			innerCode.addCode(innerCode.createCodeforReturn(rnode));
			if (rnode.type != funcType) {
				error(node->left->right->line, "return type doesn't equal to function return type.");
			}
		}
		else if (node->left->right->name == ";"){//return ;
			innerCode.addCode("RETURN");
			if (funcType != "void") {
				error(node->left->right->line, "You should return " + recordStack.back().func.rtype);
			}
		}
	}
}

void Parser::parser_expression_statement(struct Tree *node) {
	if (node->left->name == "expression") {
		parser_expression(node->left);
	}
}

varNode Parser::parser_expression(struct Tree* node) {
	if (node->left->name == "expression") {
		return parser_expression(node->left);
	}
	else if (node->left->name == "assignment_expression") {
		return parser_assignment_expression(node->left);
	}
	if (node->right->name == ",") {
		return parser_assignment_expression(node->right->right);
	}
}

void Parser::parser_compound_statement(struct Tree* node) {
	//继续分析处理compound_statement
	parseTree(node);
}

//if else
void Parser::parser_selection_statement(struct Tree* node) {


	if (node->left->name == "IF") {
		if (node->left->right->right->right->right->right == NULL) {
			//添加一个新的block
			Record newrecord;
			recordStack.push_back(newrecord);

			Tree* expression = node->left->right->right;
			varNode exp_rnode = parser_expression(expression);
			Tree* statement = node->left->right->right->right->right;

			string label1 = innerCode.getLabelName();
			string label2 = innerCode.getLabelName();

			if (exp_rnode.type == "bool") {
				innerCode.addCode("IF " + exp_rnode.boolString + " JUMP " + label1);
			}
			else {
				string tempzeroname = "temp" + inttostr(innerCode.tempNum);
				++innerCode.tempNum;
				varNode newznode = createTempVar(tempzeroname, "int");
				innerCode.addCode(tempzeroname + " := #0");

				innerCode.addCode("IF " + innerCode.getNodeName(exp_rnode) + " != " + tempzeroname + " JUMP " + label1);
			}
			
			innerCode.addCode("JUMP " + label2);
			innerCode.addCode("LABEL " + label1 + " :");


			parser_statement(statement);
			
			innerCode.addCode("LABEL " + label2 + " :");

			//弹出添加的block
			recordStack.pop_back();

		}
		else if (node->left->right->right->right->right->right->name == "ELSE") {
			//添加一个新的block
			Record newrecord1;
			recordStack.push_back(newrecord1);

			Tree* expression = node->left->right->right;
			varNode exp_rnode = parser_expression(expression);
			Tree* statement1 = node->left->right->right->right->right;
			Tree* statement2 = node->left->right->right->right->right->right->right;

			string label1 = innerCode.getLabelName();
			string label2 = innerCode.getLabelName();
			string label3 = innerCode.getLabelName();

			if (exp_rnode.type == "bool") {
				innerCode.addCode("IF " + exp_rnode.boolString + " JUMP " + label1);
			}
			else {
				string tempzeroname = "temp" + inttostr(innerCode.tempNum);
				++innerCode.tempNum;
				varNode newznode = createTempVar(tempzeroname, "int");
				innerCode.addCode(tempzeroname + " := #0");

				innerCode.addCode("IF " + innerCode.getNodeName(exp_rnode) + " != " + tempzeroname + " JUMP " + label1);
			}

			innerCode.addCode("JUMP " + label2);
			innerCode.addCode("LABEL " + label1 + " :");

			parser_statement(statement1);
			
			innerCode.addCode("JUMP " + label3);
			//弹出添加的block
			recordStack.pop_back();

			//else
			innerCode.addCode("LABEL " + label2 + " :");

			Record newrecord2;
			recordStack.push_back(newrecord2);

			parser_statement(statement2);

			innerCode.addCode("LABEL " + label3 + " :");

			//弹出添加的block
			recordStack.pop_back();

		}
	}
	else if (node->left->name == "SWITCH") {

	}
	
}

//循环 while for do while
void Parser::parser_iteration_statement(struct Tree* node) {
	if (node->left->name == "WHILE") {

		//添加一个新的block
		Record newrecord;
		newrecord.canBreak = true;
		recordStack.push_back(newrecord);

		struct Tree* expression = node->left->right->right;
		struct Tree* statement = node->left->right->right->right->right;

		string label1 = innerCode.getLabelName();
		string label2 = innerCode.getLabelName();
		string label3 = innerCode.getLabelName();

		recordStack.back().breakLabelname = label3;

		innerCode.addCode("LABEL " + label1 + " :");

		varNode var = parser_expression(expression);

		if (var.type == "bool") {  
			innerCode.addCode("IF " + var.boolString + " JUMP " + label2);
		}
		else {
			string tempzeroname = "temp" + inttostr(innerCode.tempNum);
			++innerCode.tempNum;
			varNode newznode = createTempVar(tempzeroname, "int");
			innerCode.addCode(tempzeroname + " := #0");

			innerCode.addCode("IF " + innerCode.getNodeName(var) + " != " + tempzeroname + " JUMP " + label2);
		}
		innerCode.addCode("JUMP " + label3);
		innerCode.addCode("LABEL " + label2 + " :");

		parser_statement(statement);

		innerCode.addCode("JUMP " + label1);
		innerCode.addCode("LABEL " + label3 + " :");
		

		//弹出添加的block
		recordStack.pop_back();
	}
	else if (node->left->name == "DO") {
		//添加一个新的block
		Record newrecord;
		newrecord.canBreak = true;
		recordStack.push_back(newrecord);

		struct Tree* statement = node->left->right;
		struct Tree* expression = node->left->right->right->right->right;

		string label1 = innerCode.getLabelName();
		string label2 = innerCode.getLabelName();

		recordStack.back().breakLabelname = label2;

		innerCode.addCode("LABEL " + label1 + " :");

		parser_statement(statement);

		varNode var = parser_expression(expression);

		if (var.type == "bool") {
			innerCode.addCode("IF " + var.boolString + " JUMP " + label1);
		}
		else {
			string tempzeroname = "temp" + inttostr(innerCode.tempNum);
			++innerCode.tempNum;
			varNode newznode = createTempVar(tempzeroname, "int");
			innerCode.addCode(tempzeroname + " := #0");

			innerCode.addCode("IF " + innerCode.getNodeName(var) + " != " + tempzeroname + " JUMP " + label1);
		}

		/*innerCode.addCode("JUMP " + label1);*/
		innerCode.addCode("LABEL " + label2 + " :");

		//弹出添加的block
		recordStack.pop_back();

	}
	else if (node->left->name == "FOR") {
		if (node->left->right->right->name == "expression_statement") {
			//FOR '(' expression_statement expression_statement ')'statement
			if (node->left->right->right->right->right->name == ")") {
				//添加一个新的block
				Record newblock;
				newblock.canBreak = true;
				recordStack.push_back(newblock);

				Tree* exp_state1 = node->left->right->right;
				Tree* exp_state2 = exp_state1->right;
				Tree* statement = exp_state2->right->right;

				string label1 = innerCode.getLabelName();
				string label2 = innerCode.getLabelName();
				string label3 = innerCode.getLabelName();

				recordStack.back().breakLabelname = label3;

				if (exp_state1->left->name == "expression") {
					parser_expression(exp_state1->left);
				}
				innerCode.addCode("LABEL " + label1 + " :");

				varNode var;
				if (exp_state2->left->name == "expression") {
					var = parser_expression(exp_state2->left);
					if (var.type == "bool") {
						innerCode.addCode("IF " + var.boolString + " JUMP " + label2);
					}
					else {
						string tempzeroname = "temp" + inttostr(innerCode.tempNum);
						++innerCode.tempNum;
						varNode newznode = createTempVar(tempzeroname, "int");
						innerCode.addCode(tempzeroname + " := #0");

						innerCode.addCode("IF " + innerCode.getNodeName(var) + " != " + tempzeroname + " JUMP " + label2);
					}
				}
				else {
					innerCode.addCode("JUMP " + label2);
				}

				innerCode.addCode("JUMP " + label3);
				innerCode.addCode("LABEL " + label2 + " :");

				parser_statement(statement);

				innerCode.addCode("JUMP " + label1);
				innerCode.addCode("LABEL " + label3 + " :");

				////如果需要break
				//if (blockStack.back().breakLabelNum > 0) {
				//	innerCode.addCode("LABEL label" + inttostr(blockStack.back().breakLabelNum) + " :");
				//}

				//弹出添加的block
				recordStack.pop_back();
			}
			//FOR ( expression_statement expression_statement expression ) statement
			else if (node->left->right->right->right->right->name == "expression") {
				//添加一个新的block
				Record newblock;
				newblock.canBreak = true;
				recordStack.push_back(newblock);

				Tree* exp_state1 = node->left->right->right;
				Tree* exp_state2 = exp_state1->right;
				Tree* exp = exp_state2->right;
				Tree* statement = exp->right->right;

				string label1 = innerCode.getLabelName();
				string label2 = innerCode.getLabelName();
				string label3 = innerCode.getLabelName();

				recordStack.back().breakLabelname = label3;

				if (exp_state1->left->name == "expression") {
					parser_expression(exp_state1->left);
				}
				innerCode.addCode("LABEL " + label1 + " :");

				varNode var;
				if (exp_state2->left->name == "expression") {
					var = parser_expression(exp_state2->left);

					if (var.type == "bool") {
						innerCode.addCode("IF " + var.boolString + " JUMP " + label2);
					}
					else {
						string tempzeroname = "temp" + inttostr(innerCode.tempNum);
						++innerCode.tempNum;
						varNode newznode = createTempVar(tempzeroname, "int");
						innerCode.addCode(tempzeroname + " := #0");

						innerCode.addCode("IF " + innerCode.getNodeName(var) + " != " + tempzeroname + " JUMP " + label2);
					}
				}
				else {
					innerCode.addCode("JUMP " + label2);
				}

				innerCode.addCode("JUMP " + label3);
				innerCode.addCode("LABEL " + label2 + " :");

				parser_statement(statement);

				parser_expression(exp);

				innerCode.addCode("JUMP " + label1);
				innerCode.addCode("LABEL " + label3 + " :");

				////如果需要break
				//if (blockStack.back().breakLabelNum > 0) {
				//	innerCode.addCode("LABEL label" + inttostr(blockStack.back().breakLabelNum) + " :");
				//}

				//弹出添加的block
				recordStack.pop_back();
			}
		}
		if (node->left->right->right->name == "declaration") {
			//FOR '(' declaration expression_statement ')' statement
			if (node->left->right->right->right->right->name == ")") {
				//添加一个新的block
				Record newblock;
				newblock.canBreak = true;
				recordStack.push_back(newblock);

				Tree *declaration = node->left->right->right;
				Tree *expression_statement = declaration->right;
				Tree *statement = expression_statement->right->right;

				string label1 = innerCode.getLabelName();
				string label2 = innerCode.getLabelName();
				string label3 = innerCode.getLabelName();

				recordStack.back().breakLabelname = label3;

				parser_declaration(declaration);
				innerCode.addCode("LABEL " + label1 + " :");

				varNode var;
				if (expression_statement->left->name == "expression") {

					var = parser_expression(expression_statement->left);

					if (var.type == "bool") {
						innerCode.addCode("IF " + var.boolString + " JUMP " + label2);
					}
					else {
						string tempzeroname = "temp" + inttostr(innerCode.tempNum);
						++innerCode.tempNum;
						varNode newznode = createTempVar(tempzeroname, "int");
						innerCode.addCode(tempzeroname + " := #0");

						innerCode.addCode("IF " + innerCode.getNodeName(var) + " != " + tempzeroname + " JUMP " + label2);
					}
				}
				else {
					innerCode.addCode("JUMP " + label2);
				}
				innerCode.addCode("JUMP " + label3);
				innerCode.addCode("LABEL " + label2 + " :");

				parser_statement(statement);

				//cout << "here" << endl;
				innerCode.addCode("JUMP " + label1);
				innerCode.addCode("LABEL " + label3 + " :");

				////如果需要break
				//if (blockStack.back().breakLabelNum > 0) {
				//	innerCode.addCode("LABEL label" + inttostr(blockStack.back().breakLabelNum) + " :");
				//}

				//弹出添加的block
				recordStack.pop_back();

			}
			//FOR ( declaration expression_statement expression ) statement
			else if (node->left->right->right->right->right->name == "expression") {
				//添加一个新的block
				Record newblock;
				newblock.canBreak = true;
				recordStack.push_back(newblock);

				Tree *declaration = node->left->right->right;
				Tree *expression_statement = declaration->right;
				Tree *expression = expression_statement->right;
				Tree *statement = expression->right->right;

				string label1 = innerCode.getLabelName();
				string label2 = innerCode.getLabelName();
				string label3 = innerCode.getLabelName();

				recordStack.back().breakLabelname = label3;

				parser_declaration(declaration);
				innerCode.addCode("LABEL " + label1 + " :");

				varNode var;
				if (expression_statement->left->name == "expression") {
					var = parser_expression(expression_statement->left);

					if (var.type == "bool") {
						innerCode.addCode("IF " + var.boolString + " JUMP " + label2);
					}
					else {
						string tempzeroname = "temp" + inttostr(innerCode.tempNum);
						++innerCode.tempNum;
						varNode newznode = createTempVar(tempzeroname, "int");
						innerCode.addCode(tempzeroname + " := #0");

						innerCode.addCode("IF " + innerCode.getNodeName(var) + " != " + tempzeroname + " JUMP " + label2);
					}
				}
				else {
					innerCode.addCode("JUMP " + label2);
				}
				innerCode.addCode("JUMP " + label3);
				innerCode.addCode("LABEL " + label2 + " :");

				parser_statement(statement);

				parser_expression(expression);
				//cout << "here" << endl;
				innerCode.addCode("JUMP " + label1);
				innerCode.addCode("LABEL " + label3 + " :");

				////如果需要break
				//if (blockStack.back().breakLabelNum > 0) {
				//	innerCode.addCode("LABEL label" + inttostr(blockStack.back().breakLabelNum) + " :");
				//}

				//弹出添加的block
				recordStack.pop_back();
			}
		}
	}
}

//函数定义
struct Tree* Parser::parser_function_definition(struct Tree* node) {
	Tree* type_specifier = node->left;
	Tree* declarator = node->left->right;
	Tree* compound_statement = declarator->right;
	
	string funcType = type_specifier->left->content;
	string funcName = declarator->left->left->content;

	/*if (build_in_function.find(funcName) != build_in_function.end()) {
		error(declarator->left->left->line, "Function name can't be bulid in function.");
	}*/

	bool isdeclared = false;
	funcNode declarFunc;
	if (funcPool.find(funcName) != funcPool.end()) {
		//函数重复定义
		if (funcPool[funcName].isdefinied) {
			error(declarator->left->left->line, "Function " + funcName + " is duplicated definition.");
		}
		//函数事先声明过但是没有定义
		else {
			isdeclared = true;
			//先删除掉函数池中的函数的声明
			declarFunc = funcPool[funcName];
			funcPool.erase(funcPool.find(funcName));
		}
	}

	//进入新的block
	Record funBlock;
	funBlock.isfunc = true;
	funBlock.func.name = funcName;
	funBlock.func.rtype = funcType;
	funBlock.func.isdefinied = true;
	//将函数记录在块内并添加到函数池
	recordStack.push_back(funBlock);
	funcPool.insert({funcName,funBlock.func});

	innerCode.addCode("FUNCTION " + funcName + " :");

	//获取函数形参列表
	if(declarator->left->right->right->name == "parameter_list")
		parser_parameter_list(declarator->left->right->right, funcName,true);

	//此时函数池中的func已经添加了参数列表
	funcNode func = funcPool[funcName];
	//如果函数事先声明过，则比较函数的参数列表和返回类型
	if (isdeclared) {
		if (func.rtype != declarFunc.rtype) {
			error(type_specifier->left->line, "Function return type doesn't equal to the function declared before.");
		}
		cout << funBlock.func.paralist.size() << endl;
		if (func.paralist.size() != declarFunc.paralist.size()) {
			error(declarator->left->right->right->line, "The number of function parameters doesn't equal to the function declared before.");
		}
		for (int i = 0; i < funBlock.func.paralist.size(); i++) {
			if (func.paralist[i].type != declarFunc.paralist[i].type)
				error(declarator->left->right->right->line, "The parameter " + funBlock.func.paralist[i].name + "'s type doesn't equal to the function declared before." );
		}
	}
	//更新Block中func的参数列表
	funBlock.func = func;
	//分析函数的正文
	parser_compound_statement(compound_statement);

	//函数结束后，弹出相应的block
	recordStack.pop_back();

	return node->right;
}

//获取函数形参列表，函数定义需要获取形参，声明则不需要
void Parser::parser_parameter_list(struct Tree* node,string funcName,bool definite) {
	if (node->left->name == "parameter_list") {
		parser_parameter_list(node->left, funcName,definite);
	}
	else if (node->left->name == "parameter_declaration") {
		parser_parameter_declaration(node->left,funcName,definite);
	}

	if (node->right->name == ",") {
		parser_parameter_declaration(node->right->right, funcName,definite);
	}
}

//获取单个形参内容,函数定义需要获取形参，声明则不需要
void Parser::parser_parameter_declaration(struct Tree* node, string funcName,bool definite) {
	//cout << "parser_parameter_declaration" << endl;
	Tree* type_specifier = node->left;
	Tree* declarator = node->left->right;
	string typeName = type_specifier->left->content;
	if (typeName == "void") {
		error(type_specifier->line, "Void can't definite parameter.");
	}
	//================================================
	//暂时只考虑变量，不考虑数组作为形参
	string varName = declarator->left->content;
	varNode newnode;
	newnode.name = varName;
	newnode.type = typeName;
	if (definite) {
		newnode.num = innerCode.varNum++;
		recordStack.back().func.paralist.push_back(newnode);
	}

	funcPool[funcName].paralist.push_back(newnode);
	
	//将函数的形参添加到当前块的变量池中
	recordStack.back().varInsert(newnode);
	if(definite)
		innerCode.addCode(innerCode.createCodeforParameter(newnode));
}


struct Tree* Parser::parser_declaration(struct Tree *node) {
	//cout << "at " << node->name << endl;
	//node = declaration
	struct Tree* begin = node->left;	//begin:type_specifier
	if (begin->right->name == ";")
		return node->right;
	
	string vartype = begin->left->content;

	if (vartype == "void") {
		error(begin->left->line,"void type can't assign to variable");	//报错
 	}
	struct Tree* decl = begin->right;	//init_declarator_list


	/*while (decl->right) {
		parser_init_declarator(vartype, decl->right->right);
		decl = decl->left;
	}
	parser_init_declarator(vartype, decl);*/
	parser_init_declarator_list(vartype, decl);
	return node->right;

}

void Parser::parser_init_declarator_list(string vartype, struct Tree* node) {
	if (node->left->name == "init_declarator_list") {
		parser_init_declarator_list(vartype, node->left);
	}
	else if (node->left->name == "init_declarator") {
		parser_init_declarator(vartype, node->left);
	}

	if (node->right->name == ",") {
		parser_init_declarator(vartype, node->right->right);
	}
}


//分析变量初始化
void Parser::parser_init_declarator(string vartype, struct Tree* node) {
	//cout << "at " << node->name << endl;
	struct Tree* declarator = node->left;

	if (!declarator->right) {
		//获取变量的名字
		if (declarator->left->name == "IDENTIFIER") {
			struct Tree* id = declarator->left;
			string var = id->content;
			if (!lookupCurruntVar(var)) {
				varNode newvar;
				newvar.name = var;
				newvar.type = vartype;
				newvar.num = innerCode.varNum++;
				recordStack.back().varInsert(newvar);
			}
			else error(declarator->left->line, "Variable multiple declaration.");
		}
		else {
			//函数声明
			if (declarator->left->right->name == "(") {
				string funcName = declarator->left->left->content;
				string funcType = vartype;
				if (recordStack.size() > 1) {
					error(declarator->left->right->line, "Functinon declaration must at global environment.");
				}
				Tree* parameter_list = declarator->left->right->right;
				funcNode newFunc;
				newFunc.isdefinied = false;
				newFunc.name = funcName;
				newFunc.rtype = funcType;
				funcPool.insert({ funcName,newFunc });
				//分析函数形参列表
				parser_parameter_list(parameter_list,funcName,false);
			}
			//数组声明
			else if (declarator->left->right->name == "[") {
				string arrayName = declarator->left->left->content;
				string arrayType = vartype;
				Tree* assign_exp = declarator->left->right->right;
				varNode rnode = parser_assignment_expression(assign_exp);

				if (rnode.type != "int") {
					error(declarator->left->right->line,"Array size must be int.");
				}
				

				varNode tnode;
				if (arrayType == "int") {
					//创建一个新的临时变量来储存数组的大小
					string tempname = "temp" + inttostr(innerCode.tempNum);
					++innerCode.tempNum;
					tnode = createTempVar(tempname, "int");

					recordStack.back().varInsert(tnode);
					varNode tempVar3;
					string tempName3 = "temp" + inttostr(innerCode.tempNum);
					++innerCode.tempNum;
					tempVar3.name = tempName3;
					tempVar3.type = "int";
					recordStack.back().varInsert(tempVar3);
					innerCode.addCode(tempName3 + " := #4");

					innerCode.addCode(tnode.name + " := " + tempName3 +" * " + rnode.name);
				}
				else if (arrayType == "double") {
					//创建一个新的临时变量来储存数组的大小
					string tempname = "temp" + inttostr(innerCode.tempNum);
					++innerCode.tempNum;
					tnode = createTempVar(tempname, "int");

					recordStack.back().varInsert(tnode);
					varNode tempVar3;
					string tempName3 = "temp" + inttostr(innerCode.tempNum);
					++innerCode.tempNum;
					tempVar3.name = tempName3;
					tempVar3.type = "int";
					recordStack.back().varInsert(tempVar3);
					innerCode.addCode(tempName3 + " := #8");

					innerCode.addCode(tnode.name + " := " + tempName3 + " * " + rnode.name);
				}
				else if (arrayType == "bool") {
					tnode = rnode;
				}
				

				arrayNode anode;
				anode.name = arrayName;
				anode.type = arrayType;
				anode.num = innerCode.arrayNum++;
				innerCode.addCode("DEC " + innerCode.getarrayNodeName(anode) + " " + tnode.name);

				recordStack.back().arrayInsert(anode);
			}
		}
	}
	//有初始化
	else if (declarator->right->name == "=") {	
		//获取变量的名字
		varNode newvar;
		if (declarator->left->name == "IDENTIFIER") {
			struct Tree* id = declarator->left;
			string var = id->content;
			if (!lookupCurruntVar(var)) {
				newvar.name = var;
				newvar.type = vartype;
				newvar.num = innerCode.varNum++;
				recordStack.back().varInsert(newvar);
			}
			else error(declarator->left->line, "Variable multiple declaration.");
		}
		else error(declarator->left->line, "It's not a variable!");


		Tree* initializer = declarator->right->right;
		if (initializer == NULL) {
			error(declarator->line, "Lack the initializer for variable.");
		}
		else {
			if (initializer->left->name == "assignment_expression") {
				varNode rnode = parser_assignment_expression(initializer->left);
				innerCode.addCode(innerCode.createCodeforAssign(newvar,rnode));
				string rtype = rnode.type;
				if (rtype != vartype)
					error(initializer->left->line, "Wrong type to variable " + declarator->left->content + ": " + 
					rtype + " to " + vartype);
			}
		}
	}
	else error(declarator->right->line, "Wrong value to variable");
}

varNode Parser::parser_assignment_expression(struct Tree* assign_exp) {	//返回变量节点

	//cout << "parser_assignment_expression" << endl;

	if (assign_exp->left->name == "logical_or_expression") {
		struct Tree* logical_or_exp = assign_exp->left;

		return parser_logical_or_expression(logical_or_exp);
	}
	//赋值运算
	else if(assign_exp->left->name == "unary_expression"){
		struct Tree* unary_exp = assign_exp->left;
		string op = assign_exp->left->right->left->name;
		struct Tree* next_assign_exp = assign_exp->left->right->right;
		varNode node1 = parser_unary_expression(unary_exp);
		varNode node2 = parser_assignment_expression(next_assign_exp);
		varNode node3;
		if (op == "=") {
			node3 = node2;
		}
		else {
			string tempname = "temp" + inttostr(innerCode.tempNum);
			++innerCode.tempNum;
			node3 = createTempVar(tempname, node1.type);

			recordStack.back().varInsert(node3);
			if (op == "MUL_ASSIGN") { //*=
				if (node1.type != node2.type) {
					error(assign_exp->left->line, "Different type for two variables.");
				}
				innerCode.addCode(innerCode.createCodeforVar(tempname, "*", node1, node2));
			}
			else if (op == "DIV_ASSIGN") { //*=
				if (node1.type != node2.type) {
					error(assign_exp->left->line, "Different type for two variables.");
				}
				innerCode.addCode(innerCode.createCodeforVar(tempname, "/", node1, node2));
			}
			else if (op == "MOD_ASSIGN") { //*=
				if (node1.type != "int" || node2.type != "int") {
					error(assign_exp->left->line, "The two variables must be int.");
				}
				innerCode.addCode(innerCode.createCodeforVar(tempname, "%", node1, node2));
			}
			else if (op == "ADD_ASSIGN") { //*=
				if (node1.type != node2.type) {
					error(assign_exp->left->line, "Different type for two variables.");
				}
				innerCode.addCode(innerCode.createCodeforVar(tempname, "+", node1, node2));
			}
			else if (op == "SUB_ASSIGN") { //*=
				if (node1.type != node2.type) {
					error(assign_exp->left->line, "Different type for two variables.");
				}
				innerCode.addCode(innerCode.createCodeforVar(tempname, "-", node1, node2));
			}
			else if (op == "LEFT_ASSIGN") { //*=
				if (node1.type != "int" || node2.type != "int") {
					error(assign_exp->left->line, "The two variables must be int.");
				}
				innerCode.addCode(innerCode.createCodeforVar(tempname, "<<", node1, node2));
			}
			else if (op == "RIGHT_ASSIGN") { //*=
				if (node1.type != "int" || node2.type != "int") {
					error(assign_exp->left->line, "The two variables must be int.");
				}
				innerCode.addCode(innerCode.createCodeforVar(tempname, ">>", node1, node2));
			}
			else if (op == "AND_ASSIGN") { //*=
				if (node1.type != "int" || node2.type != "int") {
					error(assign_exp->left->line, "The two variables must be int.");
				}
				innerCode.addCode(innerCode.createCodeforVar(tempname, "&", node1, node2));
			}
			else if (op == "XOR_ASSIGN") { //*=
				if (node1.type != "int" || node2.type != "int") {
					error(assign_exp->left->line, "The two variables must be int.");
				}
				innerCode.addCode(innerCode.createCodeforVar(tempname, "^", node1, node2));
			}
			else if (op == "OR_ASSIGN") { //*=
				if (node1.type != "int" || node2.type != "int") {
					error(assign_exp->left->line, "The two variables must be int.");
				}
				innerCode.addCode(innerCode.createCodeforVar(tempname, "|", node1, node2));
			}
		}

		innerCode.addCode(innerCode.createCodeforAssign(node1, node3));
		return node1;
	}
}

varNode Parser::parser_logical_or_expression(struct Tree* logical_or_exp) {

	if(logical_or_exp->left->name == "logical_and_expression"){
		struct Tree* logical_and_exp = logical_or_exp->left;
		return parser_logical_and_expression(logical_and_exp);
	}
	else if (logical_or_exp->left->name == "logical_or_expression") {
		//logical_or_expression -> logical_or_expression OR_OP logical_and_expression
		varNode node1 = parser_logical_or_expression(logical_or_exp->left);
		varNode node2 = parser_logical_and_expression(logical_or_exp->left->right->right);

		if (node1.type != "bool" || node2.type != "bool") {
			error(logical_or_exp->left->right->line, "Logical Or operation should only used to bool. ");
		}

		string tempname = "temp" + inttostr(innerCode.tempNum);
		++innerCode.tempNum;
		varNode newnode = createTempVar(tempname, node1.type);

		recordStack.back().varInsert(newnode);
		innerCode.addCode(innerCode.createCodeforVar(tempname, "||", node1, node2));

		newnode.boolString = innerCode.getNodeName(node1) + " || " + innerCode.getNodeName(node2);

		return newnode;

	}

}

varNode Parser::parser_logical_and_expression(struct Tree* logical_and_exp) {
	
	if (logical_and_exp->left->name == "inclusive_or_expression") {
		Tree* inclusive_or_exp = logical_and_exp->left;
		return parser_inclusive_or_expression(inclusive_or_exp);
	}
	else if (logical_and_exp->left->name == "logical_and_expression") {
		varNode node1 = parser_logical_and_expression(logical_and_exp->left);
		varNode node2 = parser_inclusive_or_expression(logical_and_exp->left->right->right);

		if (node1.type != "bool" || node2.type != "bool") {
			error(logical_and_exp->left->right->line, "Logical And operation should only used to bool. ");
		}

		string tempname = "temp" + inttostr(innerCode.tempNum);
		++innerCode.tempNum;
		varNode newnode = createTempVar(tempname, node1.type);
		recordStack.back().varInsert(newnode);
		innerCode.addCode(innerCode.createCodeforVar(tempname, "&&", node1, node2));

		newnode.boolString = innerCode.getNodeName(node1) + " && " + innerCode.getNodeName(node2);

		return newnode;

	}
}

varNode Parser::parser_inclusive_or_expression(struct Tree* inclusive_or_exp) {
	
	if (inclusive_or_exp->left->name == "exclusive_or_expression") {
		Tree* exclusive_or_exp = inclusive_or_exp->left;
		return parser_exclusive_or_expression(exclusive_or_exp);
	}
	else if (inclusive_or_exp->left->name == "inclusive_or_expression") {
		varNode node1 = parser_inclusive_or_expression(inclusive_or_exp->left);
		varNode node2 = parser_exclusive_or_expression(inclusive_or_exp->left->right->right);

		if (node1.type != "int" || node2.type != "int") {
			error(inclusive_or_exp->left->right->line, "Inclusive Or operation should only used to int. ");
		}

		string tempname = "temp" + inttostr(innerCode.tempNum);
		++innerCode.tempNum;
		varNode newnode = createTempVar(tempname, node1.type);
		recordStack.back().varInsert(newnode);
		innerCode.addCode(innerCode.createCodeforVar(tempname, "|", node1, node2));
		return newnode;
	}
}

varNode Parser::parser_exclusive_or_expression(struct Tree *exclusive_or_exp) {
	
	if (exclusive_or_exp->left->name == "and_expression") {
		Tree* and_exp = exclusive_or_exp->left;
		return parser_and_expression(and_exp);
	}
	else if (exclusive_or_exp->left->name == "exclusive_or_expression") {
		varNode node1 = parser_exclusive_or_expression(exclusive_or_exp->left);
		varNode node2 = parser_and_expression(exclusive_or_exp->left->right->right);

		if (node1.type != "int" || node2.type != "int") {
			error(exclusive_or_exp->left->right->line, "Exclusive Or operation should only used to int. ");
		}

		string tempname = "temp" + inttostr(innerCode.tempNum);
		++innerCode.tempNum;
		varNode newnode = createTempVar(tempname, node1.type);
		recordStack.back().varInsert(newnode);
		innerCode.addCode(innerCode.createCodeforVar(tempname, "^", node1, node2));
		return newnode;
	}
}

varNode Parser::parser_and_expression(struct Tree* and_exp) {
	if (and_exp->left->name == "equality_expression") {
		Tree* equality_exp = and_exp->left;
		return parser_equality_expression(equality_exp);
	}
	else if (and_exp->left->name == "and_expression") {
		varNode node1 = parser_and_expression(and_exp->left);
		varNode node2 = parser_equality_expression(and_exp->left->right->right);

		if (node1.type != "int" || node2.type != "int") {
			error(and_exp->left->right->line, "And operation should only used to int. ");
		}

		string tempname = "temp" + inttostr(innerCode.tempNum);
		++innerCode.tempNum;

		varNode newnode = createTempVar(tempname, node1.type);

		recordStack.back().varInsert(newnode);
		innerCode.addCode(innerCode.createCodeforVar(tempname, "&", node1, node2));
		return newnode;
	}
}

varNode Parser::parser_equality_expression(struct Tree* equality_exp) {
	
	if (equality_exp->left->name == "relational_expression") {
		Tree* relational_exp = equality_exp->left;
		return parser_relational_expression(relational_exp);
	}
	else if (equality_exp->left->right->name == "EQ_OP" || equality_exp->left->right->name == "NE_OP") {
		string op;
		if (equality_exp->left->right->name == "EQ_OP")
			op = "==";
		else op = "!=";

		varNode node1 = parser_equality_expression(equality_exp->left);
		varNode node2 = parser_relational_expression(equality_exp->left->right->right);

		if (node1.type != node2.type) {
			error(equality_exp->left->right->line, "Different type for two variables.");
		}

		string tempname = "temp" + inttostr(innerCode.tempNum);
		++innerCode.tempNum;

		varNode newnode = createTempVar(tempname, "bool");
		recordStack.back().varInsert(newnode);
		innerCode.addCode(innerCode.createCodeforVar(tempname, op, node1, node2));

		newnode.boolString = innerCode.getNodeName(node1) + " " + op + " " + innerCode.getNodeName(node2);

		return newnode;
	}
}

varNode Parser::parser_relational_expression(struct Tree* relational_exp) {
	if (relational_exp->left->name == "shift_expression") {
		Tree* shift_exp = relational_exp->left;
		return parser_shift_expression(shift_exp);
	}
	else {
		string op = relational_exp->left->right->name;
		if (op == "LE_OP")
			op = "<=";
		else if (op == "GE_OP")
			op = ">=";
		if (op == ">" || op == "<" || op == ">=" || op == "<=") {
			varNode node1 = parser_relational_expression(relational_exp->left);
			varNode node2 = parser_shift_expression(relational_exp->left->right->right);

			if (node1.type != node2.type) {
				error(relational_exp->left->right->line, "Different type for two variables.");
			}

			string tempname = "temp" + inttostr(innerCode.tempNum);
			++innerCode.tempNum;

			varNode newnode = createTempVar(tempname, "bool");
			recordStack.back().varInsert(newnode);
			innerCode.addCode(innerCode.createCodeforVar(tempname, op, node1, node2));

			newnode.boolString = innerCode.getNodeName(node1) + " " + op + " " + innerCode.getNodeName(node2);

			return newnode;
		}
	}
}

varNode Parser::parser_shift_expression(struct Tree*shift_exp) {
	if (shift_exp->left->name == "additive_expression") {
		Tree* additive_exp = shift_exp->left;
		return parser_additive_expression(additive_exp);
	}
	else if (shift_exp->left->right->name == "LEFT_OP" || shift_exp->left->right->name == "RIGHT_OP") {
		string op;
		if (shift_exp->left->right->name == "LEFT_OP") {
			op = "<<";
		}
		else op = ">>";

		varNode node1 = parser_shift_expression(shift_exp->left);
		varNode node2 = parser_additive_expression(shift_exp->left->right->right);

		if (node1.type != "int" || node2.type != "int" ) {
			error(shift_exp->left->right->line, "Shift operation should only used to int. ");
		}

		string tempname = "temp" + inttostr(innerCode.tempNum);
		++innerCode.tempNum;

		varNode newnode = createTempVar(tempname, node1.type);

		recordStack.back().varInsert(newnode);
		innerCode.addCode(innerCode.createCodeforVar(tempname, op, node1, node2));
		return newnode;
	}
}

varNode Parser::parser_additive_expression(struct Tree* additive_exp) {
	if (additive_exp->left->name == "multiplicative_expression") {
		Tree* mult_exp = additive_exp->left;
		return parser_multiplicative_expression(mult_exp);
	}
	else if (additive_exp->left->right->name == "+" || additive_exp->left->right->name == "-") {
		varNode node1 = parser_additive_expression(additive_exp->left);
		varNode node2 = parser_multiplicative_expression(additive_exp->left->right->right);

		if (node1.type != node2.type) {
			error(additive_exp->left->right->line, "Different type for two variables.");
		}

		string tempname = "temp" + inttostr(innerCode.tempNum);
		++innerCode.tempNum;
		varNode newnode = createTempVar(tempname, node1.type);
		recordStack.back().varInsert(newnode);
		innerCode.addCode(innerCode.createCodeforVar(tempname, additive_exp->left->right->name, node1, node2));
		return newnode;
	}
}

varNode Parser::parser_multiplicative_expression(struct Tree* mult_exp) {

	if (mult_exp->left->name == "unary_expression") {
		Tree* unary_exp = mult_exp->left;
		return parser_unary_expression(unary_exp);
	}
	else if (mult_exp->left->right->name == "*" || mult_exp->left->right->name == "/" || 
		mult_exp->left->right->name == "%") {
		varNode node1 = parser_multiplicative_expression(mult_exp->left);
		varNode node2 = parser_unary_expression(mult_exp->left->right->right);

		if (node1.type != node2.type) {
			error(mult_exp->left->right->line, "Different type for two variables.");
		}

		string tempname = "temp" + inttostr(innerCode.tempNum);
		++innerCode.tempNum;
		varNode newNode = createTempVar(tempname, node1.type);
		recordStack.back().varInsert(newNode);
		innerCode.addCode(innerCode.createCodeforVar(tempname, mult_exp->left->right->name,node1,node2));
		return newNode;

	}
}

varNode Parser::parser_unary_expression(struct Tree*unary_exp) {
	if (unary_exp->left->name == "postfix_expression") {
		Tree* post_exp = unary_exp->left;
		return parser_postfix_expression(post_exp);
	}
	else if (unary_exp->left->name == "INC_OP") {
		varNode rnode = parser_unary_expression(unary_exp->left->right);
		if (rnode.type != "int")
			error(unary_exp->left->right->line, "++ operation can only use for int type.");

		string tempname = "temp" + inttostr(innerCode.tempNum);
		++innerCode.tempNum;
		varNode newNode = createTempVar(tempname, "int");
		recordStack.back().varInsert(newNode);
		innerCode.addCode(tempname + " := #1");

		//变量储存的是地址
		if (rnode.useAddress) {
			innerCode.addCode("*" + rnode.name + " := *" + rnode.name + " + " + tempname);
		}
		else {
			innerCode.addCode(innerCode.getNodeName(rnode) + " := " + innerCode.getNodeName(rnode) + " + "  + tempname);
		}

		return rnode;

	}
	else if (unary_exp->left->name == "DEC_OP") {

		varNode rnode = parser_unary_expression(unary_exp->left->right);
		if (rnode.type != "int")
			error(unary_exp->left->right->line, "-- operation can only use for int type.");

		string tempname = "temp" + inttostr(innerCode.tempNum);
		++innerCode.tempNum;
		varNode newNode = createTempVar(tempname, "int");
		recordStack.back().varInsert(newNode);
		innerCode.addCode(tempname + " := #1");

		//变量储存的是地址
		if (rnode.useAddress) {
			innerCode.addCode("*" + rnode.name + " := *" + rnode.name + " - " + tempname);
		}
		else {
			innerCode.addCode(innerCode.getNodeName(rnode) + " := " + innerCode.getNodeName(rnode) + " - " + tempname);
		}

		return rnode;
	}
	else if (unary_exp->left->name == "unary_operator") {
		string op = unary_exp->left->left->name;
		varNode rnode = parser_unary_expression(unary_exp->left->right);
		if (op == "+") {

			if (rnode.type != "int" && rnode.type != "double")
				error(unary_exp->left->left->line, "operator '+' can only used to int or double");
			return rnode;
		}
		else if (op == "-") {

			if (rnode.type != "int" && rnode.type != "double")
				error(unary_exp->left->left->line, "operator '-' can only used to int or double");

			string tempzeroname = "temp" + inttostr(innerCode.tempNum);
			++innerCode.tempNum;
			varNode newzeronode = createTempVar(tempzeroname, rnode.type);
			recordStack.back().varInsert(newzeronode);
			innerCode.addCode(tempzeroname + " := #0");

			string tempname = "temp" + inttostr(innerCode.tempNum);
			++innerCode.tempNum;
			varNode newnode = createTempVar(tempname, rnode.type);
			recordStack.back().varInsert(newnode);

			if (rnode.useAddress) {
				innerCode.addCode(tempname + " := " + tempzeroname + " - *" + rnode.name);
			}
			else {
				innerCode.addCode(tempname + " := " + tempzeroname + " - " + innerCode.getNodeName(rnode));
			}
			return newnode;
		}
		else if (op == "~") {

		}
		else if (op == "!") {

		}
	}
}

varNode Parser::parser_postfix_expression(struct Tree* post_exp) {
	//cout << "here" << endl;
	if (post_exp->left->name == "primary_expression") {
		Tree* primary_exp = post_exp->left;
		return parser_primary_expression(primary_exp);
	}
	else if (post_exp->left->right->name == "[") {
		//数组调用
		string arrayName = post_exp->left->left->left->content;
		Tree* expression = post_exp->left->right->right;
		varNode enode = parser_expression(expression);
		arrayNode anode = getArrayNode(arrayName);

		if (anode.num < 0)
			error(post_exp->left->right->line, "Undifined array " + arrayName);

		varNode tempVar;
		string tempName = "temp" + inttostr(innerCode.tempNum);
		++innerCode.tempNum;
		tempVar.name = tempName;
		tempVar.type = anode.type;
		tempVar.useAddress = true;
		recordStack.back().varInsert(tempVar);

		if (anode.type == "int" || anode.type == "double") {
			varNode tempVar2;
			string tempName2 = "temp" + inttostr(innerCode.tempNum);
			++innerCode.tempNum;
			tempVar2.name = tempName2;
			tempVar2.type = "int";
			recordStack.back().varInsert(tempVar2);
			if (anode.type == "int") {

				varNode tempVar3;
				string tempName3 = "temp" + inttostr(innerCode.tempNum);
				++innerCode.tempNum;
				tempVar3.name = tempName3;
				tempVar3.type = "int";
				recordStack.back().varInsert(tempVar3);
				innerCode.addCode(tempName3 + " := #4");

				innerCode.addCode(tempName2 + " := " + innerCode.getNodeName(enode) + " * " + tempName3);
			}
			else if (anode.type == "double") {
				varNode tempVar3;
				string tempName3 = "temp" + inttostr(innerCode.tempNum);
				++innerCode.tempNum;
				tempVar3.name = tempName3;
				tempVar3.type = "int";
				recordStack.back().varInsert(tempVar3);
				innerCode.addCode(tempName3 + " := #8");

				innerCode.addCode(tempName2 + " := " + innerCode.getNodeName(enode) + " * " + tempName3);
			}

			innerCode.addCode(tempName + " := &" + innerCode.getarrayNodeName(anode) + " + " + innerCode.getNodeName(tempVar2));
			return tempVar;
		}

		innerCode.addCode(tempName + " := &" + innerCode.getarrayNodeName(anode) + " + " + innerCode.getNodeName(enode));
		return tempVar;
	}
	else if (post_exp->left->right->name == "(") {
		//函数调用
		string funcName = post_exp->left->left->left->content;
		varNode newNode;
		
		if (funcPool.find(funcName) == funcPool.end()) {
			error(post_exp->left->left->left->line, "Undefined function " + funcName);
		}

		if (post_exp->left->right->right->name == "argument_expression_list") {
			Tree* argument_exp_list = post_exp->left->right->right;
			parser_argument_expression_list(argument_exp_list, funcName);
			//cout << "funcCall" << endl;

		}

		funcNode func = funcPool[funcName];
		
		if (func.rtype == "void") {
			innerCode.addCode("CALL " + funcName);
		}
		else {
			string tempname = "temp" + inttostr(innerCode.tempNum);
			++innerCode.tempNum;

			newNode = createTempVar(tempname, funcPool[funcName].rtype);
			innerCode.addCode(tempname + " := CALL " + funcName);

		}

		return newNode;
		
	}
	else if (post_exp->left->right->name == "INC_OP") {
		varNode rnode = parser_postfix_expression(post_exp->left);

		if (rnode.type != "int")
			error(post_exp->left->right->line, "++ operation can only use for int type.");

		string tempname = "temp" + inttostr(innerCode.tempNum);
		++innerCode.tempNum;
		varNode newnode = createTempVar(tempname, "int");
		recordStack.back().varInsert(newnode);
		string tempnameone = "temp" + inttostr(innerCode.tempNum);
		++innerCode.tempNum;
		varNode newNode = createTempVar(tempnameone, "int");
		recordStack.back().varInsert(newNode);
		innerCode.addCode(tempnameone + " := #1");

		//变量储存的是地址
		if (rnode.useAddress) {
			innerCode.addCode(tempname + " := *" + rnode.name);
			innerCode.addCode("*" + rnode.name + " := *" + rnode.name + " + " + tempnameone);
		}
		else {
			innerCode.addCode(tempname += " := " + innerCode.getNodeName(rnode));
			innerCode.addCode(innerCode.getNodeName(rnode) +  " := " + innerCode.getNodeName(rnode) + " + " + tempnameone);
		}

		return newnode;
	}
	else if (post_exp->left->right->name == "DEC_OP") {

		varNode rnode = parser_postfix_expression(post_exp->left);

		if (rnode.type != "int")
			error(post_exp->left->right->line, "-- operation can only use for int type.");

		string tempname = "temp" + inttostr(innerCode.tempNum);
		++innerCode.tempNum;
		varNode newnode = createTempVar(tempname, "int");
		recordStack.back().varInsert(newnode);
		string tempnameone = "temp" + inttostr(innerCode.tempNum);
		++innerCode.tempNum;
		varNode newNode = createTempVar(tempnameone, "int");
		recordStack.back().varInsert(newNode);
		innerCode.addCode(tempnameone + " := #1");

		//变量储存的是地址
		if (rnode.useAddress) {
			innerCode.addCode(tempname + " := *" + rnode.name);
			innerCode.addCode("*" + rnode.name + " := *" + rnode.name + " - " + tempnameone);
		}
		else {
			innerCode.addCode(tempname += " := " + innerCode.getNodeName(rnode));
			innerCode.addCode(innerCode.getNodeName(rnode) + " := " + innerCode.getNodeName(rnode) + " - " + tempnameone);
		}

		return newnode;
	}
}

void Parser::parser_argument_expression_list(struct Tree* node, string funcName) {
	Tree* argu_exp_list = node->left;
	funcNode func = funcPool[funcName];
	int i = 0;
	while (argu_exp_list->name == "argument_expression_list") {
		varNode rnode = parser_assignment_expression(argu_exp_list->right->right);

		innerCode.addCode(innerCode.createCodeforArgument(rnode));

		argu_exp_list = argu_exp_list->left;
		i++;
		if (func.paralist[func.paralist.size() - i].type != rnode.type) {
			error(argu_exp_list->line, "Wrong type arguments to function " + funcName);
		}
	}
	varNode rnode = parser_assignment_expression(argu_exp_list);
	innerCode.addCode(innerCode.createCodeforArgument(rnode));
	i++;
	if (func.paralist[func.paralist.size() - i].type != rnode.type) {
		error(argu_exp_list->line, "Wrong type arguments to function " + funcName);
	}
	if (i != func.paralist.size()) {
		error(argu_exp_list->line, "The number of arguments doesn't equal to the function parameters number.");
	}
}

varNode Parser::parser_primary_expression(struct Tree* primary_exp) {
	if (primary_exp->left->name == "IDENTIFIER") {
		string content = primary_exp->left->content;
		varNode rnode = lookupNode(content);
		if (rnode.num < 0) {
			error(primary_exp->left->line, "Undefined variable " + content);
		}
		return rnode;
	}
	else if (primary_exp->left->name == "TRUE" || primary_exp->left->name == "FALSE") {
		string content = primary_exp->left->content;
		string tempname = "temp" + inttostr(innerCode.tempNum);
		++innerCode.tempNum;
		varNode newNode = createTempVar(tempname, "bool");
		recordStack.back().varInsert(newNode);
		if(primary_exp->left->name == "TRUE") 
			innerCode.addCode(tempname + " := #1");
		else {
			innerCode.addCode(tempname + " := #0");
		}
		return newNode;
	}
	else if (primary_exp->left->name == "CONSTANT_INT") {
		string content = primary_exp->left->content;
		string tempname = "temp" + inttostr(innerCode.tempNum);
		++innerCode.tempNum;
		
		varNode newNode = createTempVar(tempname, "int");
		recordStack.back().varInsert(newNode);
		innerCode.addCode(tempname + " := #"  + content);
		return newNode;
	}
	else if (primary_exp->left->name == "CONSTANT_DOUBLE") {
		string content = primary_exp->left->content;
		string tempname = "temp" + inttostr(innerCode.tempNum);
		++innerCode.tempNum;

		varNode newNode = createTempVar(tempname, "double");

		recordStack.back().varInsert(newNode);
		innerCode.addCode(tempname + " := F" + content);
		return newNode;
	}
	else if (primary_exp->left->name == "(") {
		struct Tree* expression = primary_exp->left->right;
		return parser_expression(expression);
	}
}


//全局查找
string Parser::lookupVar(string name) {
	cout << "全局查找！" << endl;
	int N = recordStack.size();
	for (int i = N - 1; i >= 0; i--) {
		if (recordStack[i].varFind(name)==true)
			return recordStack[i].varGet(name).type;
	}
	return "";
}
//当前块查找
bool Parser::lookupCurruntVar(string name) {
	return recordStack.back().varFind(name);
}

struct varNode Parser::lookupNode(string name) {
	int N = recordStack.size();
	for (int i = N - 1; i >= 0; i--) {
		if (recordStack[i].varFind(name) == true)
			return recordStack[i].varGet(name);
	}
	varNode temp;
	temp.num = -1;
	return temp;
}

string Parser::getFuncRType() {
	int N = recordStack.size();
	for (int i = N - 1; i >= 0; i--) {
		if (recordStack[i].isfunc)
			return recordStack[i].func.rtype;
	}
	return "";
}

string Parser::getArrayType(string name) {
	cout << "GetArrayType!" << endl;
	int N = recordStack.size();
	for (int i = N - 1; i >= 0; i--) {
		if (recordStack[i].arrayFind(name) == true)
			return recordStack[i].arrayGet(name).type;
	}
	return "";
}

struct arrayNode Parser::getArrayNode(string name) {
	cout << "GetArrayNode!" << endl;
	int N = recordStack.size();
	for (int i = N - 1; i >= 0; i--) {
		if (recordStack[i].arrayFind(name) == true)
			return recordStack[i].arrayGet(name);
	}
	arrayNode temp;
	temp.num = -1;
	return temp;
}

int Parser::getBreakRecordNumber() {
	int N = recordStack.size();
	for (int i = N - 1; i >= 0; i--) {
		if (recordStack[i].canBreak)
			return i;
	}
	return -1;
}

void Parser::error(int line, string error) {

	print_code();

	cout << "Error! line " << line << ": ";
	cout << error << endl;
	exit(1);
}

struct varNode Parser::createTempVar(string name, string type) {
	varNode var;
	var.name = name;
	var.type = type;
	var.num = -1;
	return var;
}

void Parser::print_map() {
	int N = recordStack.size();
	for (int i = N - 1; i >= 0; i--) {
		cout << "Block " << i << endl;
	}
}

void Parser::print_code() {
	innerCode.printCode();
}

