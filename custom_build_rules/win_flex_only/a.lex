%{

int wordCount = 0;

int numcount = 0;

%}

chars [A-Za-z\_\'\.\"]

numbers ([0-9])+

delim [" "\n\t]

whitespace {delim}+

words {chars}+

%%

while  {ECHO; printf("%s\n",yytext);}

{words} { wordCount++; 

  /* increase the word count by one*/ }

{whitespace} { /* do nothing*/ }

([0-9])+ { numcount++; /* one may want to add some processing           here*/ }

%%
void main()

{

printf("ok1\n");



yylex(); /* start the  analysis*/



printf("ok2\n");

printf(" No of words: %d\n  number: %d\n", wordCount, numcount);



return 0;



}



int yywrap()

{

return 1;

}
