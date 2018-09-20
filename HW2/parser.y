%{
#include <stdio.h>
#include <stdlib.h>

extern int linenum;             /* declared in lex.l */
extern FILE *yyin;              /* declared by lex */
extern char *yytext;            /* declared by lex */
extern char buf[256];           /* declared in lex.l */
%}

/*symbols*/
%token SEMICOLON  /*';'*/
%token END VAR ARRAY OF TO BEGIN_ ASSIGN PRINT READ
%token WHILE DO FOR RETURN
%token ID TYPE MOD 
%token LEQ GEQ NEQ NOT AND OR 
%token IF THEN ELSE TRUE FALSE 
%token INT FLOAT_LITERAL SCIENTIFIC_LITERAL OCTAL STRING_LITERAL

%left PARENTHESES
%left NEG
%left '*' '/'
%left '+' '-'
%left '>' '<'
%left NOT
%left AND
%left OR


%%
	
program			: ID SEMICOLON programbody END ID
			;

programbody		:opt_var_decl_list opt_function_decl_list compound
			;

////////////////////////

idList		        :
			|nonEmptyidList
			;

nonEmptyidList		:nonEmptyidList ',' ID
			|ID
			;
literal_constant	:number
			|STRING_LITERAL
			|TRUE
                        |FALSE
			;
number		        :INT
		 	|OCTAL
			|SCIENTIFIC_LITERAL
			|FLOAT_LITERAL
			;
///////////////////////////////


opt_function_decl_list 	: 
			|function_decl_list
			;

function_decl_list 	: function_decl function_decl_list
			|function_decl
			;
function_decl		: ID '(' all_param_list ')' ':' TYPE SEMICOLON compound END ID 
		 	| ID '(' all_param_list ')' SEMICOLON compound END ID 
			;

all_param_list		: 
		  	|param_list
			;
param_list              :param_list SEMICOLON param
                        |param
                        ;
param                   :idList ':' all_type
                        ;


///////////////////////////////

///////////////////////

opt_var_decl_list	: 
			|var_decl_list
			;

var_decl_list		:var_decl_list varible
	     		|varible
			;

all_type:     		TYPE
              		|array_type
              		;

varible:                VAR idList ':' TYPE SEMICOLON
			|VAR idList ':' array_type SEMICOLON
			|VAR idList ':' literal_constant SEMICOLON
			;

array_type:  		ARRAY INT TO INT OF all_type;

///////////////////////////////////////////////////

////////////////////////////////////////

stmt :	  	  	compound
		  	|simple
		  	|all_expressions
		  	|conditional
		  	|while
		  	|for
		  	|return
		  	|function_invocation SEMICOLON
		   	;

stmts: 			stmt_list
		  	|
		  	;

stmt_list		: stmt_list stmt
		 	| stmt
			;

compound		: BEGIN_  opt_var_decl_list stmts END               
		 	;


///////////////////////////////////////////////////

simple			:varible_reference ASSIGN all_expressions SEMICOLON
			| PRINT varible_reference SEMICOLON
			| PRINT all_expressions SEMICOLON
			| READ varible_reference SEMICOLON
			;
array_reference		:ID array_reference_list
			;

array_reference_list 	:  
			|'[' integer_expression ']' array_reference_list
			;
varible_reference 	:array_reference
			;
////////////////////////////////////////////
///////////////////////////////////////////

all_expressions :	expression
			|integer_expression
			|boolean_expression
			;

expression		: '-' expression %prec NEG
		   	| expression '+' expression
		   	| expression '-' expression
		   	| expression '*' expression
		   	| expression '/' expression
		   	| expression MOD expression %prec '*'
		   	| '(' expression ')' %prec PARENTHESES
		   	| number
		   	| ID
		   	| STRING_LITERAL
		   	| function_invocation
		  	;

integer_expression : 	INT
                     	|ID
                     	;

boolean_expression:	expression '>' expression
			|expression '<' expression
			|expression LEQ expression %prec '>'
			|expression GEQ expression %prec '>'
			|expression '=' expression
			|expression NEQ expression %prec '>'
			|expression AND expression %prec AND
			|expression OR  expression %prec OR
			|NOT expression %prec NOT
			|TRUE
                        |FALSE
			|ID
			|function_invocation
			;

////////////////////////////////////////////
///////////////////////////////////////////

function_invocation 	:ID'('Nonempytyexpression_list')'
                     	;

Nonempytyexpression_list:expression_list
                        |
                        ;

expression_list		: expression
			|expression_list ',' expression
			;

///////////////////////////////////////////


conditional		:IF boolean_expression THEN stmts ELSE stmts END IF
                        |IF boolean_expression THEN stmts END IF
		        ;


while                   :WHILE boolean_expression DO stmts END DO
		 	;

for			:FOR ID ASSIGN INT TO INT DO stmts END DO
			;

return 			: RETURN all_expressions SEMICOLON
		  	;


%%

int yyerror( char *msg )
{
		fprintf( stderr, "\n|--------------------------------------------------------------------------\n" );
	fprintf( stderr, "| Error found in Line #%d: %s\n", linenum, buf );
	fprintf( stderr, "|\n" );
	fprintf( stderr, "| Unmatched token: %s\n", yytext );
		fprintf( stderr, "|--------------------------------------------------------------------------\n" );
		exit(-1);
}

int  main( int argc, char **argv )
{
	if( argc != 2 ) {
		fprintf(  stdout,  "Usage:  ./parser  [filename]\n"  );
		exit(0);
	}

FILE *fp = fopen( argv[1], "r" );

if( fp == NULL )  {
		fprintf( stdout, "Open  file  error\n" );
		exit(-1);
	}

yyin = fp;
	yyparse();

fprintf( stdout, "\n" );
	fprintf( stdout, "|--------------------------------|\n" );
	fprintf( stdout, "|  There is no syntactic error!  |\n" );
	fprintf( stdout, "|--------------------------------|\n" );
	exit(0);
}




