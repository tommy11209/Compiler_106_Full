/**
 * Introduction to Compiler Design by Prof. Yi Ping You
 * Prjoect 2 main function
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbolTable.h"

extern int yyparse();	/* declared by yacc */
extern FILE* yyin;	/* declared by lex */
extern Id_List* idlist_buffer;
extern SymbolTable* symbol_table;

char* fn;
char* temp_fn;

int  main( int argc, char **argv )
{
    symbol_table=buildtable();
    idlist_buffer=buildidlist();
	if( argc != 2 ) {
		fprintf(  stdout,  "Usage:  ./parser  [filename]\n"  );
		exit(0);
	}

	FILE *fp = fopen( argv[1], "r" );
    fn=strdup(argv[1]);
    temp_fn =strdup(argv[1]);

	if( fp == NULL )  {
		fprintf( stdout, "Open  file  error\n" );
		exit(-1);
	}

	yyin = fp;
	yyparse();	/* primary procedure of parser */

	/*fprintf( stdout, "\n|--------------------------------|\n" );
	fprintf( stdout, "|  There is no syntactic error!  |\n" );
	fprintf( stdout, "|--------------------------------|\n" );*/
	exit(0);
}
