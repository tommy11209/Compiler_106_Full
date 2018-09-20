%{
/**
 * Introduction to Compiler Design by Prof. Yi Ping You
 * Project 3 YACC sample
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"
#include "symtab.h"
#include "semcheck.h"

//#include "test.h"

int yydebug;

extern int linenum;		/* declared in lex.l */
extern FILE *yyin;		/* declared by lex */
extern char *yytext;		/* declared by lex */
extern char buf[256];		/* declared in lex.l */
extern int yylex(void);
int yyerror(char* );
int scope = 0;

int Opt_D = 1;			/* symbol table dump option */
char fileName[256];


struct SymTable *symbolTable;	// main symbol table

__BOOLEAN paramError;			// indicate is parameter have any error?

struct PType *funcReturn;		// record function's return type, used at 'return statement' production rule
//project4 variables
FILE*   Outfp;                  // output file
char*   program_name;               // Program name
char*   function_name;               // function name
extern int     localnumber;       // local variable's number
char    tab[4];                 // count number of tabs
char*   VariType;               //  Type of variable
int     parametter_num = 0;          //count function parametter
extern int     label_num;          //  count label
extern int     labelLoopNum ;       // count Loop's label
//extern char* myname;

extern int check_const ;
extern int check_print ;               // where are  in Print stmt
extern int check_read ;                // where are  in Read stmt
extern int check_param ;               // where are in decl function parameters
extern int check_condition ;
extern int check_return ;
extern int check_for ;                 // where are in for stmt
extern int check_while ;
extern int IS_READ;
extern int IS_CALL;
extern int check_main ;
extern int check_static_var ;
extern int simple ;
extern int check_assign ;
%}

%union {
	int intVal;
	float realVal;
	//__BOOLEAN booleanVal;
	char *lexeme;
	struct idNode_sem *id;
	//SEMTYPE type;
	struct ConstAttr *constVal;
	struct PType *ptype;
	struct param_sem *par;
	struct expr_sem *exprs;
	/*struct var_ref_sem *varRef; */
	struct expr_sem_node *exprNode;
};

/* tokens */
%token ARRAY BEG BOOLEAN DEF DO ELSE END FALSE FOR INTEGER IF OF PRINT READ REAL RETURN STRING THEN TO TRUE VAR WHILE
%token OP_ADD OP_SUB OP_MUL OP_DIV OP_MOD OP_ASSIGN OP_EQ OP_NE OP_GT OP_LT OP_GE OP_LE OP_AND OP_OR OP_NOT
%token MK_COMMA MK_COLON MK_SEMICOLON MK_LPAREN MK_RPAREN MK_LB MK_RB

%token <lexeme>ID
%token <intVal>INT_CONST
%token <realVal>FLOAT_CONST
%token <realVal>SCIENTIFIC
%token <lexeme>STR_CONST

%type<id> id_list
%type<constVal> literal_const
%type<ptype> type scalar_type array_type opt_type
%type<par> param param_list opt_param_list
%type<exprs> var_ref boolean_expr boolean_term boolean_factor relop_expr expr term factor boolean_expr_list opt_boolean_expr_list
%type<intVal> dim mul_op add_op rel_op array_index loop_param

/* start symbol */
%start program
%%

program     : ID
						{
              program_name = (char*) malloc(sizeof($1)+1);
              strcpy(program_name, $1);

              char* tmp;
              tmp = (char*) malloc(sizeof($1)+3);
              strcpy(tmp, $1);
              strcat(tmp, ".j");

              Outfp = fopen(tmp, "w");

							GenProgramStart(program_name);
              VariType = (char*) malloc(20);

              struct PType *pType = createPType( VOID_t );
			  			struct SymNode *newNode = createProgramNode( $1, scope, pType );
			  			insertTab( symbolTable, newNode );

			  			if( strcmp(fileName,$1) )
							{
									fprintf( stdout, "########## Error at Line#%d: program beginning ID inconsist with file name ########## \n", linenum );
			  			}

						}
			  MK_SEMICOLON
			  program_body
			  END ID
				{
					  if( strcmp($1, $6) )
						{
								fprintf( stdout, "########## Error at Line #%d: %s", linenum,"Program end ID inconsist with the beginning ID ########## \n");
						}
					  if( strcmp(fileName,$6) )
						{
						 fprintf( stdout, "########## Error at Line#%d: program end ID inconsist with file name ########## \n", linenum );
					  }
					  // dump symbol table
					  if( Opt_D == 1 )
						printSymTable( symbolTable, scope );

		        free(VariType);
		        fclose(Outfp);
			}
			;

program_body: {
									check_static_var = 1;
							}
							opt_decl_list
							{
									check_static_var = 0;
							}
							opt_func_decl_list
							{
									check_main = 1;
							}
							compound_stmt
							;

opt_decl_list   : decl_list
			    | /* epsilon */
    			;

decl_list   : decl_list decl
			| decl
			;

decl		: VAR id_list MK_COLON scalar_type MK_SEMICOLON       /* scalar type declaration */
			{
			  // insert into symbol table
			  struct idNode_sem *ptr;
			  struct SymNode *newNode;
			  for( ptr=$2 ; ptr!=0 ; ptr=(ptr->next) )
				{
			  	if( verifyRedeclaration( symbolTable, ptr->value, scope ) ==__FALSE ) { }
					else
					{
							newNode = createVarNode( ptr->value, scope, $4 );
              if(scope == 0)
							{
                  newNode->symLocalNum = 0;
									GenGlobalVar(ptr->value,$4);

              }
							else
							{
                  newNode->symLocalNum = localnumber++;
              }
              insertTab( symbolTable, newNode );
					}

			  }
			  deleteIdList( $2 );
			}
			| VAR id_list MK_COLON array_type MK_SEMICOLON        /* array type declaration */
			{
			  verifyArrayType( $2, $4 );
			  // insert into symbol table
			  struct idNode_sem *ptr;
			  struct SymNode *newNode;
			  for( ptr=$2 ; ptr!=0 ; ptr=(ptr->next) )
				{
			  	if( $4->isError == __TRUE ) { }
					else if( verifyRedeclaration( symbolTable, ptr->value, scope ) ==__FALSE ) { }
					else
					{
							newNode = createVarNode( ptr->value, scope, $4 );
							insertTab( symbolTable, newNode );
					}
			  }
			  deleteIdList( $2 );
			}
			| VAR id_list MK_COLON literal_const MK_SEMICOLON     /* const declaration */
			{
			  struct PType *pType = createPType( $4->category );
			  // insert constants into symbol table
			  struct idNode_sem *ptr;
			  struct SymNode *newNode;
			  for( ptr=$2 ; ptr!=0 ; ptr=(ptr->next) )
				{
			  	if( verifyRedeclaration( symbolTable, ptr->value, scope ) ==__FALSE ) { }
					else
					{
							newNode = createConstNode( ptr->value, scope, pType, $4 );
							insertTab( symbolTable, newNode );
					}
			  }
			  deleteIdList( $2 );
			}
			;

literal_const   : INT_CONST
			{
			  int tmp = $1;
			  $$ = createConstAttr( INTEGER_t, &tmp );
				printlcd_int($1);
			}
			| OP_SUB INT_CONST
			{
			  int tmp = -$2;
			  $$ = createConstAttr( INTEGER_t, &tmp );
        printlcd_int(-$2);
			}
			| FLOAT_CONST
			{
			  float tmp = $1;
			  $$ = createConstAttr( REAL_t, &tmp );
       	if(check_assign || check_print || check_return || check_condition || check_for || check_while)
				{
            fprintf(Outfp, "\tldc %f\n", tmp);
        }
							//printlcd_f(tmp);
			}
			| OP_SUB FLOAT_CONST
			{
			  float tmp = -$2;
			  $$ = createConstAttr( REAL_t, &tmp );
        if(check_assign || check_print || check_return || check_condition || check_for || check_while)
				{
            fprintf(Outfp, "\tldc %f\n", tmp);
        }
							//printlcd_f(tmp);
			}
			| SCIENTIFIC
			{
			  float tmp = $1;
			  $$ = createConstAttr( REAL_t, &tmp );
       	if(check_assign || check_print || check_return || check_condition || check_for || check_while)
				{
          	fprintf(Outfp, "\tldc %f\n", tmp);
        }
			}
			| OP_SUB SCIENTIFIC
			{
			  float tmp = -$2;
			  $$ = createConstAttr( REAL_t, &tmp );
       	if(check_assign || check_print || check_return || check_condition || check_for || check_while)
				{
          fprintf(Outfp, "\tldc %f\n", tmp);
        }
			}
			| STR_CONST
			{
			  $$ = createConstAttr( STRING_t, $1 );
				printlcd_str($1);
			}
			| TRUE
			{
			  __BOOLEAN tmp = __TRUE;
			  $$ = createConstAttr( BOOLEAN_t, &tmp );
				printlcd_true();
			}
			| FALSE
			{
			  __BOOLEAN tmp = __FALSE;
			  $$ = createConstAttr( BOOLEAN_t, &tmp );
				printlcd_false();
			}
			;

opt_func_decl_list	: func_decl_list
			| /* epsilon */
			;

func_decl_list		: func_decl_list func_decl
			| func_decl
			;

func_decl   : ID MK_LPAREN
            {
                fprintf(Outfp, "\n.method public static %s(", $1);
                check_param = 1;
            }
              opt_param_list
						{
                check_param = 0;
						    // check and insert parameters into symbol table
						    paramError = insertParamIntoSymTable( symbolTable, $4, scope+1 );
						}
						  MK_RPAREN opt_type
						{
								GenFunctionStart($7);
			                // check and insert function into symbol table
						    if( paramError == __TRUE )
								{
						  	    printf("--- param(s) with several fault!! ---\n");
						    }
								else
								{
							    insertFuncIntoSymTable( symbolTable, $1, $4, $7, scope );
						    }
						    funcReturn = $7;
						}
						  MK_SEMICOLON
						  compound_stmt{}
						  END ID
						{
						    if( strcmp($1,$13) )
								{
							    fprintf( stdout, "######### Error at Line #%d: the end of the functionName mismatch ######### \n", linenum );
						    }
								GenFunctionEnd($7);
                localnumber = 0;
						    funcReturn = 0;
						}
						;

opt_param_list		: param_list
            {
                $$ = $1;
            }
			| /* epsilon */ { $$ = 0; }
			;

param_list  : param_list MK_SEMICOLON param
			{
			  param_sem_addParam( $1, $3 );
			  $$ = $1;
			}
			| param { $$ = $1; }
			;

param       :	{
								parametter_num = 0;
						  }
							id_list MK_COLON type
            	{
                $$ = createParam( $2, $4 );

								GenFunctionStart_param($$);
                parametter_num = 0;
            	}
							;

id_list	    : id_list MK_COMMA ID
							{
							  idlist_addNode( $1, $3 );
							  $$ = $1;
				        parametter_num++;
							}
							| ID
	            {
	              parametter_num++;
	              $$ = createIdList($1);

	            }
							;

opt_type    : MK_COLON type { $$ = $2; }
			| /* epsilon */ { $$ = createPType( VOID_t ); }
			;

type        : scalar_type { $$ = $1; }
			| array_type { $$ = $1; }
			;

scalar_type : INTEGER { $$ = createPType( INTEGER_t ); strcpy(VariType, "I"); }
			| REAL { $$ = createPType( REAL_t ); strcpy(VariType, "F"); }
			| BOOLEAN { $$ = createPType( BOOLEAN_t ); strcpy(VariType, "Z"); }
			| STRING { $$ = createPType( STRING_t ); strcpy(VariType, "string"); }
			;

array_type		: ARRAY array_index TO array_index OF type
			{
				verifyArrayDim( $6, $2, $4 );
				increaseArrayDim( $6, $2, $4 );
				$$ = $6;
			}
			;

array_index : INT_CONST { $$ = $1; }
			| OP_SUB INT_CONST { $$ = -$2; }
			;

stmt        : compound_stmt
			| simple_stmt
			| cond_stmt
			| while_stmt
			| for_stmt
			| return_stmt
			| proc_call_stmt
			;

compound_stmt:
			{
			  scope++;
			}
			  BEG

			{
				GenMethod(program_name , check_main);
			}
			  opt_decl_list
			  opt_stmt_list
			  END
			{
			  // print contents of current scope
			  if( Opt_D == 1 )
			  	printSymTable( symbolTable, scope );
			  deleteScope( symbolTable, scope );	// leave this scope, delete...
        GenProgramEnd(check_main);
        scope--;
			}
			;

opt_stmt_list   : stmt_list
    			| /* epsilon */
			    ;

stmt_list   : stmt_list stmt
			| stmt
			;

simple_stmt : var_ref
            {
                simple = 1;
                check_assign = 1;

            }
            OP_ASSIGN boolean_expr MK_SEMICOLON
						{
			          // check if LHS exists
						    __BOOLEAN flagLHS = verifyExistence( symbolTable, $1, scope, __TRUE );
						    // id RHS is not dereferenced, check and deference
			                __BOOLEAN flagRHS = __TRUE;
						    if( $4->isDeref == __FALSE )
								{
							    flagRHS = verifyExistence( symbolTable, $4, scope, __FALSE );
						    }
						    // if both LHS and RHS are exists, verify their type
						    if( flagLHS==__TRUE && flagRHS==__TRUE )
							    verifyAssignmentTypeMatch( $1, $4 );
								GenSaveExpr($1,$4);
                check_assign = 0;
                simple = 0;
						}
						| PRINT
            {
                check_print = 1;
                fprintf(Outfp, "\tgetstatic java/lang/System/out Ljava/io/PrintStream;\n");
            }
            boolean_expr MK_SEMICOLON
            {
								GenPrint($3);
                verifyScalarExpr( $3, "print" ); check_print = 0;
            }
			 			| READ
            {
                fprintf(Outfp, "\t; invoke java.util.Scanner.nextXXX()\n");
                fprintf(Outfp, "\tgetstatic %s/_sc Ljava/util/Scanner;\n", program_name);
                check_read = 1;
            }
            boolean_expr MK_SEMICOLON
            {
                check_read = 0;
             		IS_READ=1;
           			IS_CALL=1;
                verifyScalarExpr( $3, "read" );
            }
						;

proc_call_stmt  : ID MK_LPAREN
								{
									IS_CALL=1;
								}
								opt_boolean_expr_list MK_RPAREN MK_SEMICOLON
								{
								    fprintf(Outfp, "");
					    			verifyFuncInvoke( $1, $4, symbolTable, scope );
								}
								;

cond_stmt   : IF condition THEN
						  opt_stmt_list
	            {
	                fprintf(Outfp, "\tgoto Lexit_%d\n", label_num);
	            }
						  ELSE
	            {
	                fprintf(Outfp, "Lelse_%d:\n", label_num++);
	            }
	              opt_stmt_list
						  END IF
	            {
	                fprintf(Outfp, "Lexit_%d:\n", --label_num);
	                label_num--;
	            }
							| IF condition THEN opt_stmt_list
	            {
	                fprintf(Outfp, "\tgoto Lexit_%d\n", label_num);
	            }
	              END IF
	            {
	                fprintf(Outfp, "Lelse_%d:\n", label_num++);
	                fprintf(Outfp, "Lexit_%d:\n", --label_num);
	                label_num--;
	            }
						;

condition   :
            {
                check_condition = 1;
            }
            boolean_expr
            {
                verifyBooleanExpr( $2, "if" );
                check_condition = 0;
            }
			;

while_stmt  : WHILE
            {
                check_while = 1;
                fprintf(Outfp, "Lbegin_%d:\n", labelLoopNum++);
            }
              condition_while{check_while=0;}
              DO
			  		opt_stmt_list
            {
                check_while = 0;
                labelLoopNum -= 1;
                fprintf(Outfp, "\tgoto Lbegin_%d\n", labelLoopNum);
                labelLoopNum++;
                fprintf(Outfp, "Lelse_%d:\n", labelLoopNum);
                fprintf(Outfp, "Lexit_%d:\n", labelLoopNum);
            }
			  		END DO
			;

condition_while		: boolean_expr { verifyBooleanExpr( $1, "while" ); }
			;

for_stmt    : FOR
            {
                check_for = 1;
            }
              ID
						{
						    insertLoopVarIntoTable( symbolTable, $3 );
						}
						  OP_ASSIGN loop_param TO loop_param
						{
              verifyLoopParam( $6, $8 );

							GenForLoop($3,$6,$8);
						}
						  DO
						  opt_stmt_list
						  END DO
						{
							GenForLoopEnd($3);
              check_for = 0;
              popLoopVar( symbolTable );
						}
			;

loop_param  : INT_CONST { $$ = $1; }
			| OP_SUB INT_CONST { $$ = -$2; }
			;

return_stmt : RETURN
            {
                check_return = 1;

            }
              boolean_expr MK_SEMICOLON
						{
                check_return = 0;
						    verifyReturnStatement( $3, funcReturn );
								GenFuncRutern($3);
						}
			;

opt_boolean_expr_list	: boolean_expr_list { $$ = $1; }
			| /* epsilon */ { $$ = 0; }	// null
			;

boolean_expr_list	: boolean_expr_list MK_COMMA boolean_expr
			{
			  struct expr_sem *exprPtr;
			  for( exprPtr=$1 ; (exprPtr->next)!=0 ; exprPtr=(exprPtr->next) );
			  exprPtr->next = $3;
			  $$ = $1;
			}
			| boolean_expr
			{
			  $$ = $1;
			}
			;

boolean_expr: boolean_expr OP_OR boolean_term
			{
			  verifyAndOrOp( $1, OR_t, $3 );
			  $$ = $1;
			}
			| boolean_term { $$ = $1;}
			;

boolean_term: boolean_term OP_AND boolean_factor
			{
			  verifyAndOrOp( $1, AND_t, $3 );
			  $$ = $1;
			}
			| boolean_factor { $$ = $1; }
			;

boolean_factor		: OP_NOT boolean_factor
			{
			  $$ = $2;
        printf("[%d]\n", $2->pType->type);
        fprintf(Outfp, "neg\n");
			}
			| relop_expr { $$ = $1; }
			;

relop_expr  : expr rel_op expr
							{
							    verifyRelOp( $1, $2, $3 );
							    $$ = $1;
									//be careful of change of LE_t!!!!
									GenRelational($1, $2, $3);
							}
							| expr { $$ = $1; }
							;

rel_op      : OP_LT { $$ = LT_t; }
			| OP_LE { $$ = LE_t; }
			| OP_EQ { $$ = EQ_t; }
			| OP_GE { $$ = GE_t; }
			| OP_GT { $$ = GT_t; }
			| OP_NE { $$ = NE_t; }
			;

expr        : expr add_op{ check_assign=1;} term
						{
                verifyArithmeticOp( $1, $2, $4 );
						    $$ = $1;
								GenArithmetic($1,$2,$4);
						}
						| term { $$ = $1; }
						;

add_op      : OP_ADD
            {
                $$ = ADD_t;
            }
						| OP_SUB
            {
                $$ = SUB_t;
            }
						;

term        : term mul_op factor
						{

				    if( $2 == MOD_t )
						{
					    verifyModOp( $1, $3 );
				    }
				    else
						{
			            verifyArithmeticOp( $1, $2, $3 );
				    }
						GenArithmetic($1,$2,$3);
	          $$ = $1;
						}
						| factor { $$ = $1; }
						;

mul_op      : OP_MUL { $$ = MUL_t; }
			| OP_DIV { $$ = DIV_t; }
			| OP_MOD { $$ = MOD_t; }
			;

factor      : var_ref
						{
						  verifyExistence( symbolTable, $1, scope, __FALSE );
							if(IS_CALL)
							{
						  	verifyExistence( symbolTable, $1, scope, __TRUE );
						  	IS_CALL=0;}
						  	$$ = $1;
						  	$$->beginningOp = NONE_t;
							}
						| OP_SUB var_ref
						{
							//add GenNegative($2);!!!
						  if( verifyExistence( symbolTable, $2, scope, __FALSE ) == __TRUE )
							verifyUnaryMinus( $2 );
						  $$ = $2;
						  $$->beginningOp = SUB_t;
							//GenNegative($2);
						}
						| MK_LPAREN boolean_expr MK_RPAREN
						{
						  $2->beginningOp = NONE_t;
						  $$ = $2;
						}
						| OP_SUB MK_LPAREN boolean_expr MK_RPAREN
						{
						  verifyUnaryMinus( $3 );
						  $$ = $3;
						  $$->beginningOp = SUB_t;
							//GenNegative($3);
						}
						| ID MK_LPAREN opt_boolean_expr_list MK_RPAREN
						{
						    $$ = verifyFuncInvoke( $1, $3, symbolTable, scope );
	        	    $$->beginningOp = NONE_t;
						}
						| OP_SUB ID MK_LPAREN opt_boolean_expr_list MK_RPAREN
						{
                $$ = verifyFuncInvoke( $2, $4, symbolTable, scope );
						    $$->beginningOp = SUB_t;
								////////////////////////
								GenFunctionCall($2);

                //fprintf(Outfp, "neg\n");
								// ADD neg!!!!
								GenNegative($2);
						}
            | literal_const
						{
							  $$ = (struct expr_sem *)malloc(sizeof(struct expr_sem));
							  $$->isDeref = __TRUE;
							  $$->varRef = 0;
							  $$->pType = createPType( $1->category );
							  $$->next = 0;
							  if( $1->hasMinus == __TRUE )
								{
							  	$$->beginningOp = SUB_t;
							  }
							  else
								{
									$$->beginningOp = NONE_t;
							  }
						}
						;

var_ref     : ID
						{
                $$ = createExprSem( $1 );
                if(IS_READ)
								{
										check_assign=0;
										IS_READ=0;
								}

								print_load($1);

								Control_read_stmt($1);
								//myname=$1;

						}
						| var_ref dim
						{
						  increaseDim( $1, $2 );
						  $$ = $1;
						}
						;

dim			: MK_LB boolean_expr MK_RB
			{
			  $$ = verifyArrayIndex( $2 );
			}
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
