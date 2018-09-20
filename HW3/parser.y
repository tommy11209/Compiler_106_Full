%{
/**
 * Introduction to Compiler Design by Prof. Yi Ping You
 * Project 2 YACC sample
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Symbol.h"

extern int linenum;		/* declared in lex.l */

extern FILE *yyin;		/* declared by lex */
extern char *yytext;		/* declared by lex */
extern char buf[256];		/* declared in lex.l */
extern int yylex(void);
int yyerror(char* );


SymbolTable* symbol_table;
TableEntry* entry_buffer;
IdList* idlist_buffer;
Type* return_buffer;

int counter =0;

%}
/* types */
%union	{
	int num;
	double double_num;
	char* str;
	int nodetype;
	Value* value;
	Type* type;
	TableEntry* tableentry;
	TypeList* typelist;
	Expr* expression;
	ExprList* exprlist;
		}
/* tokens */
%token <str> ARRAY
%token <str> BEG
%token <str> BOOLEAN
%token <str> DEF
%token <str> DO
%token <str> ELSE
%token <str> END
%token <str> FOR
%token <str> INTEGER
%token <str> IF
%token <str> OF
%token <str> PRINT
%token <str> READ
%token <str> REAL
%token <str> RETURN
%token <str> STRING
%token <str> THEN
%token <str> TO
%token <str> TRUE
%token <str> VAR
%token <str> WHILE
%token 		 FALSE

%token <str> ID
%token OCTAL_CONST
%token <num> INT_CONST
%token <double_num>FLOAT_CONST
%token <str> SCIENTIFIC
%token <str> STR_CONST

%token <str> OP_ADD
%token <str> OP_SUB
%token <str> OP_MUL
%token <str> OP_DIV
%token <str> OP_MOD
%token <str> OP_ASSIGN
%token <str> OP_EQ
%token <str> OP_NE
%token <str> OP_GT
%token <str> OP_LT
%token <str> OP_GE
%token <str> OP_LE
%token <str> OP_AND
%token <str> OP_OR
%token <str> OP_NOT

%token <str> MK_COMMA
%token <str> MK_COLON
%token <str> MK_SEMICOLON
%token <str> MK_LPAREN
%token <str> MK_RPAREN
%token <str> MK_LB
%token <str> MK_RB
/* non-terminal */
%type <str> rel_op mul_op add_op
%type <type> scalar_type type opt_type array_type
%type <typelist> param_list opt_param_list param
%type <value> literal_const int_const
%type <tableentry> func_decl
%type <expression> var_ref factor boolean_expr term boolean_term boolean_factor relop_expr expr return_stmt
%type <exprlist> boolean_expr_list opt_boolean_expr_list


/* start symbol */
%start program
%%

program			: ID MK_SEMICOLON
							program_body
							END ID
							{
								TableEntry* tmp=build_entry($1,"program",symbol_table->current_level,BuildType("void"),NULL);
								insert_entry(symbol_table,tmp);
								PrintSymbolTable(symbol_table);
							}
							;

program_body		: opt_decl_list opt_func_decl_list compound_stmt
								;

opt_decl_list		: decl_list
								| /* epsilon */
								;

decl_list		: decl_list decl
						| decl
						;

decl			: VAR id_list MK_COLON scalar_type MK_SEMICOLON  /* scalar type declaration */
					{
							insert_entry_list(symbol_table,idlist_buffer,"variable",$4,NULL);
							int i;
		    			for (i = (idlist_buffer->position) - 1; i >= 0; i--) {
		        		free(idlist_buffer->Ids[i]);
		    	}
		    	idlist_buffer->position = 0;
		    	idlist_buffer->size_ = 4;
		    	idlist_buffer->Ids = (char**)malloc(sizeof(char*) * 4);;
					}

					| VAR id_list MK_COLON array_type MK_SEMICOLON       /* array type declaration */
					{

					insert_entry_list(symbol_table,idlist_buffer,"variable",$4,NULL);
					int i;
			    for (i = (idlist_buffer->position) - 1; i >= 0; i--) {
			        free(idlist_buffer->Ids[i]);
			    }
			    idlist_buffer->position = 0;
			    idlist_buffer->size_ = 4;
			    idlist_buffer->Ids = (char**)malloc(sizeof(char*) * 4);

					}
					| VAR id_list MK_COLON literal_const MK_SEMICOLON     /* const declaration */
					{
						Attribute* tmp_attri;
						tmp_attri = (Attribute*)malloc(sizeof(Attribute));
		    		tmp_attri->val = $4;
		    		tmp_attri->type_list = NULL;
						insert_entry_list(symbol_table,idlist_buffer,"constant",$4->type,tmp_attri);
						int i;
		    		for (i = (idlist_buffer->position) - 1; i >= 0; i--) {
		        	free(idlist_buffer->Ids[i]);
		    		}
		    		idlist_buffer->position = 0;
		    		idlist_buffer->size_ = 4;
		    		idlist_buffer->Ids = (char**)malloc(sizeof(char*) * 4);
					}
					;
int_const	:	INT_CONST
						{
							$$ = buildint(yytext);
						}
						|	OCTAL_CONST
						{
							$$=buildoctal(yytext);
						}
						;
/*FIXME*/
literal_const		: int_const {$$=$1;}
			| OP_SUB int_const
			| FLOAT_CONST
			{
				$$ = buildreal(yytext);
			}
			| OP_SUB FLOAT_CONST
			| SCIENTIFIC
			{
				$$=buildscientific(yytext);
			}
			| OP_SUB SCIENTIFIC
			| STR_CONST
			{
				$$ = buildstring(yytext);
			}
			| TRUE
			{
				$$ = buildboolean(yytext);
			}
			| FALSE
			{
				$$ = buildboolean(yytext);
			}
			;

opt_func_decl_list	: func_decl_list
			| /* epsilon */
			;

func_decl_list		: func_decl_list func_decl {insert_entry(symbol_table,$2);}
									| func_decl					{insert_entry(symbol_table,$1);}

									;

func_decl		: ID
				MK_LPAREN { symbol_table->current_level++;}
				opt_param_list
				MK_RPAREN { symbol_table->current_level--; }
				opt_type  {return_buffer=$7;}
				MK_SEMICOLON
				compound_stmt
				END ID
				{
					Attribute* func_attr;
					func_attr = (Attribute*)malloc(sizeof(Attribute));
			    func_attr->type_list = $4;
			    func_attr->val = NULL;
					$$=build_entry($1,"function",symbol_table->current_level,$7,func_attr);
					return_buffer=NULL;
				}
			  ;

opt_param_list		: param_list
									| {$$=NULL;}
									;

param_list		: param_list MK_SEMICOLON param
							{

								int i;
						    if ($1->size_ - $1->current_size < $3->current_size)
								{
						        while (($1->size_ - $1->current_size) < $3->current_size)
										{
						            $1->size_ *= 2;
						        }
						        Type** temp = $1->types;
						        $1->types = (Type**)malloc(sizeof(Type**) * $1->size_);

						        for (i = 0; i < $1->current_size; i++)
										{
						            ($1->types)[i] = temp[i];
						        }
						        free(temp);
						    }
						    for (i = 0; i < $3->current_size; i++) {
						        $1->types[$1->current_size++] = $3->types[i];
						    }
						    free($3);
						    $$ = $1;
							}
							| param 							{$$=$1;}
							;

param			: id_list MK_COLON type
					{
						$$=add_type_list(NULL,$3,idlist_buffer->position);
						insert_entry_list(symbol_table,idlist_buffer,"parameter",$3,NULL);
						int i;
			    	for (i = (idlist_buffer->position) - 1; i >= 0; i--) {
			        free(idlist_buffer->Ids[i]);
			    	}
			    	idlist_buffer->position = 0;
			    	idlist_buffer->size_ = 4;
			    	idlist_buffer->Ids = (char**)malloc(sizeof(char*) * 4);
					}
					;

id_list			: id_list MK_COMMA ID
						{
							char* id_tmp = strdup(yytext);

							if (idlist_buffer->position == idlist_buffer->size_)
							{
							idlist_buffer->size_ *= 2;
							char** tmp_Ids = idlist_buffer->Ids;
							idlist_buffer->Ids = (char**)malloc(sizeof(char*) * idlist_buffer->size_);
							int i;
							for (i = 0; i < idlist_buffer->position; i++) {
								(idlist_buffer->Ids)[i] = tmp_Ids[i];
							}
							free(tmp_Ids);
							}
						idlist_buffer->Ids[idlist_buffer->position++] = id_tmp;
						}
				| ID
				{
					char* id_tmp = strdup(yytext);

					if (idlist_buffer->position == idlist_buffer->size_)
					{
						idlist_buffer->size_ *= 2;
						char** tmp_Ids = idlist_buffer->Ids;
						idlist_buffer->Ids = (char**)malloc(sizeof(char*) * idlist_buffer->size_);
						int i;
						for (i = 0; i < idlist_buffer->position; i++) {
							(idlist_buffer->Ids)[i] = tmp_Ids[i];
						}
						free(tmp_Ids);
					}
					idlist_buffer->Ids[idlist_buffer->position++] = id_tmp;
				}
				;

opt_type		: MK_COLON type {$$=$2;}
						| {$$=BuildType("void");}
						;

type			: scalar_type 	{$$=$1;}
					| array_type 		{$$=$1;}
					;

scalar_type		: INTEGER 	{$$=BuildType("integer");}
							| REAL 			{$$=BuildType("real");}
							| BOOLEAN		{$$=BuildType("boolean");}
							| STRING		{$$=BuildType("string");}
							;

array_type		: ARRAY int_const TO int_const OF type
							{
									int size=($4->int_value)-($2->int_value)+1;

									Array_signal* it;
		    					if ($6->array_signature == NULL)
									{
		        				$6->array_signature = (Array_signal*)malloc(sizeof(Array_signal));
		        				$6->array_signature->size_ = size;
		        				$6->array_signature->next_dimension = NULL;
		    					}
									else
									{
		        				it = $6->array_signature;
		        				while (it->next_dimension != NULL)
		            			it = it->next_dimension;
		        				it->next_dimension = (Array_signal*)malloc(sizeof(Array_signal));
		        				it->next_dimension->size_ = size;
		        				it->next_dimension->next_dimension = NULL;
		    					}
									$$ = $6;
							}
							;

stmt			: compound_stmt
			| simple_stmt
			| cond_stmt
			| while_stmt
			| for_stmt
			| return_stmt
			| proc_call_stmt
			;

compound_stmt		: BEG 		{symbol_table->current_level++;}
			  opt_decl_list
			  opt_stmt_list
			  END
				{
					PrintSymbolTable(symbol_table);
					pop_entry(symbol_table);
					symbol_table->current_level--;
				}
				;

opt_stmt_list		: stmt_list
								| /* epsilon */
								;

stmt_list		: stmt_list stmt
			| stmt
			;

simple_stmt		: var_ref OP_ASSIGN boolean_expr MK_SEMICOLON
			| PRINT boolean_expr MK_SEMICOLON
			| READ boolean_expr MK_SEMICOLON

			;

proc_call_stmt		: ID MK_LPAREN opt_boolean_expr_list MK_RPAREN MK_SEMICOLON
			;

cond_stmt		: IF boolean_expr THEN
			  opt_stmt_list
			  ELSE
			  opt_stmt_list
			  END IF
			| IF boolean_expr THEN opt_stmt_list END IF
			;

while_stmt		: WHILE boolean_expr DO
			  			opt_stmt_list
			  			END DO
							;

for_stmt		: FOR ID OP_ASSIGN int_const TO int_const DO
						{
							TableEntry* tmp=build_entry($2,"loop var",symbol_table->current_level,BuildType("integer"),NULL);
							counter++;
							insert_entry(symbol_table,tmp);
						}
			  		opt_stmt_list
			  		END DO
			  		{
							pop_entry_name(symbol_table,$2);
			  		}
						;

return_stmt		: RETURN boolean_expr MK_SEMICOLON{$$=$2;}
			;

opt_boolean_expr_list	: boolean_expr_list
			| {$$=NULL;}
			;

boolean_expr_list	: boolean_expr_list MK_COMMA boolean_expr {$$=BuildExprList($1,$3);}
			| boolean_expr {$$=BuildExprList(NULL,$1);}
			;

boolean_expr		: boolean_expr OP_OR boolean_term
			| boolean_term
			;

boolean_term		: boolean_term OP_AND boolean_factor
			| boolean_factor
			;

boolean_factor		: OP_NOT boolean_factor
			| relop_expr
			;

relop_expr		: expr rel_op expr
			| expr
			;

rel_op			: OP_LT
			| OP_LE
			| OP_EQ
			| OP_GE
			| OP_GT
			| OP_NE
			;

expr			: expr add_op term
			| term
			;

add_op			: OP_ADD
			| OP_SUB
			;

term			: term mul_op factor
			| factor
			;

mul_op			: OP_MUL
			| OP_DIV
			| OP_MOD
			;

factor			: var_ref
			| OP_SUB var_ref {$$=$2;}
			| MK_LPAREN boolean_expr MK_RPAREN {$$=$2;}
			| OP_SUB MK_LPAREN boolean_expr MK_RPAREN {$$=$3;}
			| ID MK_LPAREN opt_boolean_expr_list MK_RPAREN {$$=call_func($1,$3);}
			| OP_SUB ID MK_LPAREN opt_boolean_expr_list MK_RPAREN
			{
				$$=call_func($2,$4);
			}
			| literal_const
			{
				$$ = (Expr*)malloc(sizeof(Expr));
				$$->current_dimension = 0;
		    $$->entry = NULL;
		    $$->type = $1->type;
			}
			;

var_ref			: ID
						{
							$$=find_varref(symbol_table,$1);
						}
						| var_ref dim
						{
							$1->current_dimension++;
							$$=$1;
						}
						;

dim			: MK_LB boolean_expr MK_RB
			;

%%

int yyerror( char *msg )
{
	(void) msg;
	fprintf( stderr, "\n|--------------------------------------------------------------------------\n" );
	fprintf( stderr, "| Error found in Line #%d: %s\n", linenum, buf );
	fprintf( stderr, "|\n" );
	fprintf( stderr, "| Unmatched token: %s\n", yytext );
	fprintf( stderr, "|--------------------------------------------------------------------------\n" );
	exit(-1);
}
