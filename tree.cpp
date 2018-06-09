#include"tree.h"
#include"tools.h"


//void print(Tree *root, int leavel) {
//	if (root != NULL) {
//		string Name = root->name;
//		if (root->line != -1) {
//			for (int i = 0; i<leavel; ++i) {
//				cout << ". ";
//			}
//			cout << root->name;
//
//			if (root->name == "IDENTIFIER" || root->name == "BOOL" || root->name == "INT" ||root->name == "CHAR" || root->name == "DOUBLE") {
//				cout << ":" << root->content;
//			}
//			else if (root->name == "CONSTANT_INT" || root->name == "TRUE" || root->name == "FALSE") {
//				cout << ":" << root->content << " ";
//			}
//			else if (root->name == "CONSTANT_DOUBLE") {
//				cout << ":" << root->content << " ";
//			}
//			else if (root->name == "STRING_LITERAL") {
//				cout << ":" << root->content;
//			}
//			else {
//				cout << " <" << root->line << ">";
//			}
//			cout << endl;
//		}
//		print(root->left, leavel + 1);
//		print(root->right, leavel);
//	}
//}


void freeTree(Tree* node) {
	if (node == NULL)
		return;
	freeTree(node->left);
	delete node;
	freeTree(node->right);
}



Tree* createTree(string name, int num,...) {
    va_list valist;
    Tree* root = new Tree();
    if(!root) {
        printf("Out of space \n");
        exit(0);
    }   
    root->left = NULL;
    root->right = NULL;
    root->content = "";
    Tree* temp = NULL;
    root->name = name;
    va_start(valist,num);
    if(num > 0) {
        temp = va_arg(valist,Tree*);
        root->left = temp;
        root->line = temp->line;
        if(num == 1) {
            //head->content = temp->content;
            if(temp->content.size() > 0) {
                root->content = temp->content;
            }
            else root->content = "";
        }
        else {
            for(int i = 1; i < num; ++i ) {
                temp->right = va_arg(valist,Tree*);
                temp = temp->right;
            }
        }
    }
    else {
        int line = va_arg(valist,int);
        root->line = line;
        if(root->name == "CONSTANT_INT") {
           int value;
           if(strlen(yytext) > 1 && yytext[0] == '0' && yytext[1] != 'x') {
               sscanf(yytext,"%o",&value); //8进制整数
           }
           else if(strlen(yytext) > 1 && yytext[1] == 'x'){
               sscanf(yytext,"%x",&value); //16进制整数
           }
           else value = atoi(yytext);      //10进制整数
           root->content = inttostr(value);
           //printf("%d",value);
        }
        else if(root->name == "CONSTANT_DOUBLE") {
           root->content = yytext;
        }
        else if(root->name == "TRUE") {
           root->content = inttostr(1);
        }
        else if(root->name == "FALSE") {
           root->content = inttostr(0);
        }
        else if(root->name == "STRING_LITERAL") {
           root->content = yytext;
        }
        else {
            root->content = yytext;
        }
    
    }
    return root;
}
