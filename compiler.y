%{
#include <cstdlib>
#include <cstdio>
#include <string>
#include "tree.h"
#include "record.h"
#include "Parser.h"
using namespace std;

extern char *yytext;
extern int column;
extern FILE * yyin;
extern FILE * yyout;
Tree *root;
extern int yylineno;

int yylex(void);
void yyerror(const char*); 
%}

%union{
	struct Tree* gt;
}

%token <gt> IDENTIFIER CONSTANT STRING_LITERAL SIZEOF CONSTANT_INT CONSTANT_DOUBLE
%token <gt> PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token <gt> AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token <gt> SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token <gt> XOR_ASSIGN OR_ASSIGN TYPE_NAME

%token <gt> CHAR INT DOUBLE VOID BOOL 

%token <gt> CASE IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%token <gt> TRUE FALSE

%token <gt> ';' ',' ':' '=' '[' ']' '.' '&' '!' '~' '-' '+' '*' '/' '%' '<' '>' '^' '|' '?' '{' '}' '(' ')'

%type <gt> primary_expression postfix_expression argument_expression_list unary_expression unary_operator
%type <gt> multiplicative_expression additive_expression shift_expression relational_expression equality_expression
%type <gt> and_expression exclusive_or_expression inclusive_or_expression logical_and_expression logical_or_expression
%type <gt> assignment_expression assignment_operator expression

%type <gt> declaration init_declarator_list init_declarator type_specifier

%type <gt> declarator 

%type <gt> parameter_list parameter_declaration identifier_list
%type <gt> abstract_declarator initializer initializer_list designation designator_list
%type <gt> designator statement labeled_statement compound_statement block_item_list block_item expression_statement
%type <gt> selection_statement iteration_statement jump_statement translation_unit external_declaration function_definition
%type <gt> declaration_list



%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%%

Program: 
	translation_unit {
		root = createTree("Program",1,$1);
	}
	;

primary_expression: 
	IDENTIFIER {
		$$ = createTree("primary_expression",1,$1);
	}
	|
	TRUE {
		$$ = createTree("primary_expression",1,$1);
		// $$->type = "bool";
		// $$->int_value = $1->int_value;
	}
	|
	FALSE {
		$$ = createTree("primary_expression",1,$1);
		// $$->type = "bool";
		// $$->int_value = $1->int_value;
	}
	| CONSTANT_INT {
		//printf("%d",$1->int_value);
		$$ = createTree("primary_expression",1,$1);
		// $$->type = "int";
		// $$->int_value = $1->int_value;
		
	}
	| CONSTANT_DOUBLE {
		$$ = createTree("primary_expression",1,$1);
		// $$->type = "double";
		// $$->double_value = $1->double_value;
	}
	| '(' expression ')'{
		$$ = createTree("primary_expression",3,$1,$2,$3);
	}
	;

postfix_expression:
	primary_expression{
		$$ = createTree("postfix_expression",1,$1);
	}
	| 	postfix_expression '[' expression ']'{
		$$ = createTree("postfix_expression",4,$1,$2,$3,$4);
		//数组调用
	}
	| 	postfix_expression '(' ')'{
		$$ = createTree("postfix_expression",3,$1,$2,$3);
		//函数调用
	}
	| 	postfix_expression '(' argument_expression_list ')'{
		$$ = createTree("postfix_expression",4,$1,$2,$3,$4);
		//函数调用
	}
	| 	postfix_expression INC_OP{
		//++
		$$ = createTree("postfix_expression",2,$1,$2);
	}
	| 	postfix_expression DEC_OP{
		//--
		$$ = createTree("postfix_expression",2,$1,$2);
	}
	;

argument_expression_list:
	assignment_expression{
		$$ = createTree("argument_expression_list",1,$1);
	}
	| 	argument_expression_list ',' assignment_expression {
		$$ = createTree("argument_expression_list",3,$1,$2,$3);
	}
	;

/*一元表达式*/
unary_expression:
	postfix_expression{
		//printf("postfix");
		$$ = createTree("unary_expression",1,$1);
	}
	| 	INC_OP unary_expression{
		//++
		$$ = createTree("unary_expression",2,$1,$2);
	}
	| 	DEC_OP unary_expression{
		//--
		$$ = createTree("unary_expression",2,$1,$2);
	}
	| 	unary_operator unary_expression{
		$$ = createTree("unary_expression",2,$1,$2);
	}
	;
unary_operator:
	'+' {
		$$ = createTree("unary_operator",1,$1);
	}
	| '-' {
		$$ = createTree("unary_operator",1,$1);
	}
	| '~' {
		$$ = createTree("unary_operator",1,$1);
	}
	| '!' {
		$$ = createTree("unary_operator",1,$1);
	}
	;

multiplicative_expression:
	unary_expression {
		$$ = createTree("multiplicative_expression",1,$1);
	}
	| multiplicative_expression '*' unary_expression {
		$$ = createTree("multiplicative_expression",3,$1,$2,$3);
	}
	| multiplicative_expression '/' unary_expression {
		$$ = createTree("multiplicative_expression",3,$1,$2,$3);
	}
	| multiplicative_expression '%' unary_expression {
		$$ = createTree("multiplicative_expression",3,$1,$2,$3);
	}
	;

additive_expression:
	multiplicative_expression  {
		$$ = createTree("additive_expression",1,$1);
	}
	| additive_expression '+' multiplicative_expression {
		$$ = createTree("additive_expression",3,$1,$2,$3);
	}
	| additive_expression '-' multiplicative_expression {
		$$ = createTree("additive_expression",3,$1,$2,$3);
	}
	;

/*左移右移*/
shift_expression:
	additive_expression {
		$$ = createTree("shift_expression",1,$1);
	}
	| shift_expression LEFT_OP additive_expression {
		//<<
		$$ = createTree("shift_expression",3,$1,$2,$3);
	}
	| shift_expression RIGHT_OP additive_expression {
		//<<
		$$ = createTree("shift_expression",3,$1,$2,$3);
	}
	;

relational_expression:
	shift_expression {
		$$ = createTree("relational_expression",1,$1);
	}
	| relational_expression '<' shift_expression {
		$$ = createTree("relational_expression",3,$1,$2,$3);
	}
	| relational_expression '>' shift_expression {
		$$ = createTree("relational_expression",3,$1,$2,$3);
	}
	| relational_expression LE_OP shift_expression {
		// <= 
		$$ = createTree("relational_expression",3,$1,$2,$3);
	}
	| relational_expression GE_OP shift_expression {
		// >=
		$$ = createTree("relational_expression",3,$1,$2,$3);
	}
	;

equality_expression:
	relational_expression {
		$$ = createTree("equality_expression",1,$1);
	}
	| equality_expression EQ_OP relational_expression {
		// ==
		$$ = createTree("equality_expression",3,$1,$2,$3);
	}
	| equality_expression NE_OP relational_expression {
		// !=
		$$ = createTree("equality_expression",3,$1,$2,$3);
	}
	;

and_expression:
	equality_expression {
		$$ = createTree("and_expression",1,$1);
	}
	| and_expression '&' equality_expression {
		$$ = createTree("and_expression",3,$1,$2,$3);
	}
	;

/*异或*/
exclusive_or_expression:
	and_expression {
		$$ = createTree("exclusive_or_expression",1,$1);
	}
	| exclusive_or_expression '^' and_expression {
		$$ = createTree("exclusive_or_expression",3,$1,$2,$3);
	}
	;

inclusive_or_expression:
	exclusive_or_expression {
		$$ = createTree("inclusive_or_expression",1,$1);
	}
	| inclusive_or_expression '|' exclusive_or_expression {
		$$ = createTree("inclusive_or_expression",3,$1,$2,$3);
	}
	;


logical_and_expression:
	inclusive_or_expression {
		$$ = createTree("logical_and_expression",1,$1);
	}
	| logical_and_expression AND_OP inclusive_or_expression {
		//&&
		$$ = createTree("logical_and_expression",3,$1,$2,$3);
	}
	;

logical_or_expression:
	logical_and_expression {
		$$ = createTree("logical_or_expression",1,$1);
	}
	| logical_or_expression OR_OP logical_and_expression {
		//||
		$$ = createTree("logical_or_expression",3,$1,$2,$3);
	}
	;

/*赋值表达式*/
assignment_expression:
	logical_or_expression {
		//条件表达式
		$$ = createTree("assignment_expression",1,$1);
	}
	| unary_expression assignment_operator assignment_expression {
		$$ = createTree("assignment_expression",3,$1,$2,$3);
	}
	;

/*赋值运算符*/
assignment_operator:
	'=' {
		$$ = createTree("assignment_operator",1,$1);
	}
	| MUL_ASSIGN {
		//*=
		$$ = createTree("assignment_operator",1,$1);
	}
	| DIV_ASSIGN {
		// /=
		$$ = createTree("assignment_operator",1,$1);
	}
	| MOD_ASSIGN {
		// %=
		$$ = createTree("assignment_operator",1,$1);
	}
	| ADD_ASSIGN {
		// += 
		$$ = createTree("assignment_operator",1,$1);
	}
	| SUB_ASSIGN {
		// -=
		$$ = createTree("assignment_operator",1,$1);
	}
	| LEFT_ASSIGN {
		// <<=
		$$ = createTree("assignment_operator",1,$1);
	}
	| RIGHT_ASSIGN {
		// >>=
		$$ = createTree("assignment_operator",1,$1);
	}
	| AND_ASSIGN {
		// &=
		$$ = createTree("assignment_operator",1,$1);
	}
	| XOR_ASSIGN {
		// ^=
		$$ = createTree("assignment_operator",1,$1);
	}
	| OR_ASSIGN {
		// |=
		$$ = createTree("assignment_operator",1,$1);
	}
	;


expression:
	assignment_expression {
		//赋值表达式
		$$ = createTree("expression",1,$1);
	}
	| expression ',' assignment_expression {
		//逗号表达式
		$$ = createTree("expression",3,$1,$2,$3);
	}
	;


declaration:
	type_specifier ';' {
		$$ = createTree("declaration",2,$1,$2); //?
	}
	| type_specifier init_declarator_list ';' {
		$$ = createTree("declaration",3,$1,$2,$3);
	}
	;


init_declarator_list:
	init_declarator {
		$$ = createTree("init_declarator_list",1,$1);
	}
	| init_declarator_list ',' init_declarator {
		$$ = createTree("init_declarator_list",3,$1,$2,$3);
	}
	;

init_declarator:
	declarator {
		$$ = createTree("init_declarator",1,$1);
	}
	| declarator '=' initializer {
		$$ = createTree("init_declarator",3,$1,$2,$3);
	}
	;



type_specifier:
	VOID {
		$$ = createTree("type_specifier",1,$1);
	}
	| CHAR {
		$$ = createTree("type_specifier",1,$1);
	}
	| INT {
		$$ = createTree("type_specifier",1,$1);
	}
	| DOUBLE {
		$$ = createTree("type_specifier",1,$1);	
	}
	| BOOL {
		$$ = createTree("type_specifier",1,$1);
	}
	;



declarator:
	IDENTIFIER {
		//变量
		$$ = createTree("declarator",1,$1);
	}
	| '(' declarator ')' {
		//.....
		$$ = createTree("declarator",3,$1,$2,$3);
	}
	| declarator '[' assignment_expression ']' {
		//数组
		//printf("assignment_expression");
		$$ = createTree("declarator",4,$1,$2,$3,$4);
	}
	| declarator '[' '*' ']' {
		//....
		$$ = createTree("declarator",4,$1,$2,$3,$4);
	}
	| declarator '[' ']' {
		//数组
		$$ = createTree("declarator",3,$1,$2,$3);
	}
	| declarator '(' parameter_list ')' {
		//函数
		$$ = createTree("declarator",4,$1,$2,$3,$4);
	}
	| declarator '(' identifier_list ')' {
		//函数
		$$ = createTree("declarator",4,$1,$2,$3,$4);
	}
	| declarator '(' ')' {
		//函数
		$$ = createTree("declarator",3,$1,$2,$3);
	}
	;


//参数列表
parameter_list:
	parameter_declaration {
		$$ = createTree("parameter_list",1,$1);
	}
	| parameter_list ',' parameter_declaration {
		$$ = createTree("parameter_list",3,$1,$2,$3);
	}
	;

parameter_declaration:
	type_specifier declarator {
		$$ = createTree("parameter_declaration",2,$1,$2);
	}
	| type_specifier abstract_declarator {
		$$ = createTree("parameter_declaration",2,$1,$2);
	}
	| type_specifier {
		$$ = createTree("parameter_declaration",1,$1);
	}
	;

identifier_list:
	IDENTIFIER {
		$$ = createTree("identifier_list",1,$1);
	}
	| identifier_list ',' IDENTIFIER {
		$$ = createTree("identifier_list",3,$1,$2,$3);
	}
	;

abstract_declarator:
	'(' abstract_declarator ')' {
		$$ = createTree("abstract_declarator",3,$1,$2,$3);
	}
	| '[' ']' {
		$$ = createTree("abstract_declarator",2,$1,$2);
	}
	| '[' assignment_expression ']' {
		$$ = createTree("abstract_declarator",3,$1,$2,$3);
	}
	| abstract_declarator '[' ']' {
		$$ = createTree("abstract_declarator",3,$1,$2,$3);
	}
	| abstract_declarator '[' assignment_expression ']' {
		$$ = createTree("abstract_declarator",4,$1,$2,$3,$4);
	}
	| '[' '*' ']' {
		$$ = createTree("abstract_declarator",3,$1,$2,$3);
	}
	| abstract_declarator '[' '*' ']' {
		$$ = createTree("abstract_declarator",4,$1,$2,$3,$4);
	}
	| '(' ')' {
		$$ = createTree("abstract_declarator",2,$1,$2);
	}
	| '(' parameter_list ')' {
		$$ = createTree("abstract_declarator",3,$1,$2,$3);
	}
	| abstract_declarator '(' ')' {
		$$ = createTree("abstract_declarator",3,$1,$2,$3);
	}
	| abstract_declarator '(' parameter_list ')' {
		$$ = createTree("abstract_declarator",4,$1,$2,$3,$4);
	}
	;

//初始化
initializer:
	assignment_expression {
		$$ = createTree("initializer",1,$1);
	}
	| '{' initializer_list '}' {
		//列表初始化 {1,1,1}
		$$ = createTree("initializer",3,$1,$2,$3);
	}
	| '{' initializer_list ',' '}' {
		//列表初始化 {1,1,1,}
		$$ = createTree("initializer",4,$1,$2,$3,$4);
	}
	;

initializer_list:
	initializer {
		$$ = createTree("initializer_list",1,$1);
	}
	| designation initializer {
		$$ = createTree("initializer_list",2,$1,$2);
	}
	| initializer_list ',' initializer {
		$$ = createTree("initializer_list",3,$1,$2,$3);
	}
	| initializer_list ',' designation initializer {
		$$ = createTree("initializer_list",3,$1,$2,$3);
	}
	;

designation:
	designator_list '=' {
		$$ = createTree("designation",2,$1,$2);
	}
	;

designator_list:
	designator {
		$$ = createTree("designator_list",1,$1);
	}
	| designator_list designator {
		$$ = createTree("designator_list",2,$1,$2);
	}
	;

designator: 
	'[' logical_or_expression ']' {
		$$ = createTree("designator",3,$1,$2,$3);
	}
	| '.' IDENTIFIER {
		$$ = createTree("designator",2,$1,$2);
	}
	;

//声明
statement:
	labeled_statement {
		$$ = createTree("statement",1,$1);
	}
	| compound_statement {
		$$ = createTree("statement",1,$1);
	}
	| expression_statement{
		$$ = createTree("statement",1,$1);
	}
	| selection_statement {
		$$ = createTree("statement",1,$1);
	}
	| iteration_statement {
		$$ = createTree("statement",1,$1);
	}
	| jump_statement {
		$$ = createTree("statement",1,$1);
	}
	;

//标签声明
labeled_statement:
	IDENTIFIER ':' statement {
		$$ = createTree("labeled_statement",3,$1,$2,$3);
	}
	| CASE logical_or_expression ':' statement {
		$$ = createTree("labeled_statement",4,$1,$2,$3,$4);
	}
	;

//复合语句
compound_statement:
	'{' '}' {
		$$ = createTree("compound_statement",2,$1,$2);
	}
	| '{' block_item_list '}' {
		$$ = createTree("compound_statement",3,$1,$2,$3);
	}
	;

block_item_list:
	block_item {
		$$ = createTree("block_item_list",1,$1);
	}
	| block_item_list block_item {
		$$ = createTree("block_item_list",2,$1,$2);
	}
	;

block_item:
	declaration {
		$$ = createTree("block_item",1,$1);
	}
	| statement {
		$$ = createTree("block_item",1,$1);
	}
	;

expression_statement:
	';' {
		$$ = createTree("expression_statement",1,$1);
	}
	| expression ';' {
		$$ = createTree("expression_statement",2,$1,$2);
	}
	;

//条件语句
selection_statement:
	IF '(' expression ')' statement %prec LOWER_THAN_ELSE {
		$$ = createTree("selection_statement",5,$1,$2,$3,$4,$5);
	}
    | IF '(' expression ')' statement ELSE statement {
		$$ = createTree("selection_statement",7,$1,$2,$3,$4,$5,$6,$7);
	}
    | SWITCH '(' expression ')' statement {
		$$ = createTree("selection_statement",5,$1,$2,$3,$4,$5);
	}
    ;

//循环语句
iteration_statement:
	WHILE '(' expression ')' statement {
		$$ = createTree("iteration_statement",5,$1,$2,$3,$4,$5);
	}
	| DO statement WHILE '(' expression ')' ';' {
		$$ = createTree("iteration_statement",7,$1,$2,$3,$4,$5,$6,$7);
	}
	| FOR '(' expression_statement expression_statement ')' statement {
		$$ = createTree("iteration_statement",6,$1,$2,$3,$4,$5,$6);
	}
	| FOR '(' expression_statement expression_statement expression ')' statement {
		$$ = createTree("iteration_statement",7,$1,$2,$3,$4,$5,$6,$7);
	}
	| FOR '(' declaration expression_statement ')' statement {
		$$ = createTree("iteration_statement",6,$1,$2,$3,$4,$5,$6);
	}
	| FOR '(' declaration expression_statement expression ')' statement {
		$$ = createTree("iteration_statement",7,$1,$2,$3,$4,$5,$6,$7);
	}
	;

//跳转指令
jump_statement:
	GOTO IDENTIFIER ';' {
		$$ = createTree("jump_statement",2,$1,$2);
	}
	| CONTINUE ';' {
		$$ = createTree("jump_statement",2,$1,$2);
	}
	| BREAK ';' {
		$$ = createTree("jump_statement",2,$1,$2);
	}
	| RETURN ';' {
		$$ = createTree("jump_statement",2,$1,$2);
	}
	| RETURN expression ';' {
		$$ = createTree("jump_statement",3,$1,$2,$3);
	}
	;

translation_unit:
	external_declaration {
		$$ = createTree("translation_unit",1,$1);
	}
	| translation_unit external_declaration {
		$$ = createTree("translation_unit",2,$1,$2);
	}
	;

external_declaration:
	function_definition {
		$$ = createTree("external_declaration",1,$1);
		//函数定义
		//printf("function_definition");
	}
	| declaration {
		$$ = createTree("external_declaration",1,$1);
		//变量声明
		//printf("declaration");
	}
	;

function_definition:
	type_specifier declarator declaration_list compound_statement {
		$$ = createTree("function_definition",4,$1,$2,$3,$4);
	}
	| type_specifier declarator compound_statement {
		$$ = createTree("function_definition",3,$1,$2,$3);
	}
	;

declaration_list:
	declaration {
		$$ = createTree("declaration_list",1,$1);
	}
	| declaration_list declaration {
		$$ = createTree("declaration_list",2,$1,$2);
	}
	;

%%


void yyerror(char const *s)
{
	fflush(stdout);
	printf("\n%*s\n%*s\n", column, "^", column, s);
}


int main(int argc,char* argv[]) {

	yyin = fopen(argv[1],"r");
	
	//freopen("output/output.txt","w", stdout);
	yyparse();
	printf("\n");
	//print(root,0);	//输出语法分析树

	Parser parser(root);

	freeTree(root);

	fclose(yyin);
	return 0;
}