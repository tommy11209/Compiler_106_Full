%{
#define LIST               strcat(buf, yytext)
#define token(t)           {LIST; if (Opt_T) printf("<%s>\n", #t);}
#define MAX_LINE_LENG      256

void print_real();
void print_sienc();
void print_id();
void print_oct();
void print_int();
void print_op1();

int Opt_S = 1;
int Opt_T = 1;
int linenum = 1;
char buf[MAX_LINE_LENG];
%}


digit [0-9]
letter [a-zA-Z_]
float [0-9]+\.[0-9]*
scienc  (([0-9]+\.[0-9]*)|[0-9]*)?[eE][+-]?[0-9] 
id   [a-zA-Z_]([a-zA-Z_]|[0-9])*  
oct   0[0-7]+   
integer  0|([1-9][0-9]*) 
operator1 "["|"]"|";"|":"|","|"("|")" 
operator2 "+"|"-"|"*"|"/"|"mod"|"<"|"<="|"<>"|">"|">="|"="|"and"|"or"|"not"|":="  
                  

%Start COMMENT1
%%

"/*".* {LIST;BEGIN COMMENT1;}
<COMMENT1>.  {LIST;}
<COMMENT1>.*"*/" {LIST;BEGIN INITIAL;}


"//&S+".* {Opt_S=1;LIST;}
"//&S-".* {Opt_S=0;LIST;}
"//&T+".* {Opt_T=1;LIST;}
"//&T-".* {Opt_T=0;LIST;}

"//".* {LIST;}

\n      {
    LIST;
    if (Opt_S)
        printf("%d: %s", linenum, buf);
    linenum++;
    buf[0] = '\0';
}

[\t ]   {LIST;}



\".*\" {
	LIST;
	char *output;
	size_t length = strlen(yytext);
	char temp[length];
	int i = 0;
	for(int i =0 ; i < length ; i++)
	{
	    temp[i] = yytext[i];
	}
	temp[length - 1] = '\0';
    output = &temp[1];
    printf("<string: %s>\n", output);
}






"array" {token(KWarray)}
"begin" {token(KWbegin)}
"boolean" {token(KWboolean)}
"def" {token(KWdef)}
"do" {token(KWdo)}
"else" {token(KWelse)}
"end" {token(KWend)}
"false" {token(KWfalse)}
"for" {token(KWfor)}
"integer" {token(KWinteger)}
"if" {token(KWif)}
"of" {token(KWof)}
"print" {token(KWprint)}
"read" {token(KWread)}
"real" {token(KWreal)}
"string" {token(KWstring)}
"then" {token(KWthen)}
"to" {token(KWto)}
"true" {token(KWtrue)}
"return" {token(KWreturn)}
"var" {token(KWvar)}
"while" {token(KWwhile)}

{float} print_real("");
{scienc} print_sienc("");
{id}  print_id("");
{oct} print_oct("");
{integer} print_int("");
{operator1} print_op1("");
{operator2} print_op1("");
. printf("error at line %d: bad character \"%s\"\n",linenum,yytext);


%%
void print_real(char *format)
{
    LIST;
    if (Opt_T)
        printf("<float: %s%s>\n" , format , yytext);
}
void print_sienc(char *format)
{   
    LIST;
    if (Opt_T)
        printf("<scientific: %s%s>\n" , format , yytext);   
}
void print_id(char *format)
{
    LIST;
    if (Opt_T)
        printf("<id: %s%s>\n" , format , yytext); 

}
void print_oct(char *format)
{
    LIST;
    if (Opt_T)
        printf("<oct_integer: %s%s>\n" , format , yytext); 

}
void print_int(char *format)
{
    LIST;
    if (Opt_T)
        printf("<integer: %s%s>\n" , format , yytext);
}
void print_op1(char *format)
{
    LIST;
    if (Opt_T)
        printf("<%s%s>\n" , format , yytext);
}

int main( int argc, char **argv )
{
  if( argc != 2 ) {
    fprintf( stderr, "Usage: ./scanner [filename]\n" );
    exit(0);
  }

  FILE *fp = fopen( argv[1], "r" );
  if( fp == NULL ) {
    fprintf( stderr, "Open file error\n" );
    exit(-1);
  }

  yyin = fp;

  yylex();

  exit(0);
}
