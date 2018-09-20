#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "header.h"
#include "symtab.h"

extern FILE* Outfp;


int check_main = 0;
int check_static_var = 0;
int simple = 0;
int check_assign = 0;
int check_const = 0;
int check_print = 0;
int check_read = 0;
int check_param = 0;
int check_condition = 0;
int check_return = 0;
int check_for = 0;
int check_while = 0;

int IS_READ=0;
int IS_CALL=0;

int     labelLoopNum= 0;        // count Loop's label
int     localnumber = 0;
int     label_num = 0;

//char* myname;

extern struct SymTable *symbolTable;
extern int scope;

void GenProgramStart(char* pname)
{
  fprintf(Outfp, "; %s.j\n", pname);
  fprintf(Outfp, ".class public %s\n", pname);
  fprintf(Outfp, ".super java/lang/Object\n");
  fprintf(Outfp, ".field public static _sc Ljava/util/Scanner;\n");
}

void GenProgramEnd(int check)
{
  if(check)
  {
    fprintf(Outfp, "return\n");
    fprintf(Outfp, ".end method\n");
    check = 0;
  }
}

void GenMethod(char* pname , int check)
{
  if(check == 1)
  {
      fprintf(Outfp, "\n.method public static main([Ljava/lang/String;)V\n");
      fprintf(Outfp, "\t.limit stack 100\n");
      fprintf(Outfp, "\t.limit locals 100\n");
      fprintf(Outfp, "\tnew java/util/Scanner\n");
      fprintf(Outfp, "\tdup\n");
      fprintf(Outfp, "\tgetstatic java/lang/System/in Ljava/io/InputStream;\n");
      fprintf(Outfp, "\tinvokespecial java/util/Scanner/<init>(Ljava/io/InputStream;)V\n");
      fprintf(Outfp, "\tputstatic %s/_sc Ljava/util/Scanner;\n", pname);
  }
  else
  {

      fprintf(Outfp, ".limit stack 100\n");
      //fprintf(Outfp, ";Line#%d: %s\n",linenum,buf);
      fprintf(Outfp, ".limit locals 100\n");
  }
}

void GenGlobalVar(char* pname,struct PType* type)
{
  if(type->type == INTEGER_t)
  {
		fprintf(Outfp, ".field public static %s %s\n",pname,"I");
	}
  else if(type->type == STRING_t)
  {
    fprintf(Outfp, ".field public static %s %s\n",pname,"C");
  }
	else if(type->type == BOOLEAN_t)
  {
		fprintf(Outfp, ".field public static %s %s\n",pname,"Z");
	}
	else if(type->type == REAL_t)
  {
		fprintf(Outfp, ".field public static %s %s\n",pname,"F");
	}
}

void printlcd_int(int t)
{
  if(check_assign || check_print || check_return || check_condition || check_for || check_while) {
      fprintf(Outfp, "\tldc %d\n", t);
  }
}
void printlcd_f(float t)
{
  if(check_assign || check_print || check_return || check_condition || check_for || check_while) {
      fprintf(Outfp, "\tldc %f\n", t);
  }
}
void printlcd_str(char *s)
{
  if(check_assign)
  {
      //fprintf(Outfp, "\tldc %s\n", s);
  }
  if(check_print)
  {
      fprintf(Outfp, "\tldc \"%s\"\n", s);
      //fprintf(Outfp, "\tinvokevirtual java/io/PrintStream/print(Ljava/lang/String;)V\n");
  }
}
void printlcd_true()
{
  if(check_assign || check_print || check_return || check_condition || check_for || check_while) {
      fprintf(Outfp, "\ticonst_1\n");
  }
}
void printlcd_false()
{
  if(check_assign || check_print || check_return || check_condition || check_for || check_while) {
      fprintf(Outfp, "\ticonst_0\n");
  }
}

void print_load(char *id)
{
    struct SymNode *node = 0;
    if(check_for)
    {
        node = lookupLoopVar( symbolTable, id);
    }
    if(node == 0)
    {
        node = lookupSymbol( symbolTable, id, scope, __FALSE);
    }
    if(check_for || check_return || check_while)
    {
      if(node != 0)
      {
          switch(node->type->type){
          case INTEGER_t:
              fprintf(Outfp, "\tiload %d\n", node->symLocalNum );
              //fprintf(Outfp, "\tinvokevirtual java/io/PrintStream/print(I)V\n");
              break;
          case BOOLEAN_t:
              fprintf(Outfp, "\tiload %d\n", node->symLocalNum ) ;
              //fprintf(Outfp, "\tinvokevirtual java/io/PrintStream/print(Z)V\n");
              break;
          case STRING_t:
              fprintf(Outfp, "\tldc \"%s\" \n", node->attribute->constVal->value.stringVal);
              //fprintf(Outfp, "\tinvokevirtual java/io/PrintStream/print(Ljava/lang/String;)V\n");
              break;
          case REAL_t:
              fprintf(Outfp, "\tfload %d\n", node->symLocalNum );
              //fprintf(Outfp, "\tinvokevirtual java/io/PrintStream/print(F)V\n");
              break;
          }
      }
    }
    else if(check_assign || check_print || check_condition)
    {

        if(node != 0)
        {
            switch(node->type->type){
            case INTEGER_t:
                fprintf(Outfp, "\tiload %d\n", node->symLocalNum + 1);
                //fprintf(Outfp, "\tinvokevirtual java/io/PrintStream/print(I)V\n");
                break;
            case BOOLEAN_t:
                fprintf(Outfp, "\tiload %d\n", node->symLocalNum + 1) ;
                //fprintf(Outfp, "\tinvokevirtual java/io/PrintStream/print(Z)V\n");
                break;
            case STRING_t:
                fprintf(Outfp, "\tldc \"%s\" \n", node->attribute->constVal->value.stringVal);
                //fprintf(Outfp, "\tinvokevirtual java/io/PrintStream/print(Ljava/lang/String;)V\n");
                break;
            case REAL_t:
                fprintf(Outfp, "\tfload %d\n", node->symLocalNum + 1);
                //fprintf(Outfp, "\tinvokevirtual java/io/PrintStream/print(F)V\n");
                break;
            }
        }
    }
}
void Control_read_stmt(char *id)
{
    struct SymNode *node = 0;

    if(check_for)
    {
        node = lookupLoopVar( symbolTable, id);
    }
    if(node == 0)
    {
        node = lookupSymbol( symbolTable, id, scope, __FALSE);
    }

    if(check_read)
    {
        if(node != 0)
        {

            switch(node->type->type) {
            case INTEGER_t:
                fprintf(Outfp, "\tinvokevirtual java/util/Scanner/nextInt()I\n");
                fprintf(Outfp, "\tistore %d\n", node->symLocalNum + 1);
                break;
            case BOOLEAN_t:
                fprintf(Outfp, "\tinvokevirtual java/util/Scanner/nextBoolean()Z\n");
                fprintf(Outfp, "\tistore %d\n", node->symLocalNum + 1);
                break;
            case STRING_t:
                fprintf(Outfp, "\tinvokevirtual java/util/Scanner/nextString()S\n");
                fprintf(Outfp, "\tiload %d\n", node->symLocalNum + 1);
                break;
            case REAL_t:
                fprintf(Outfp, "\tinvokevirtual java/util/Scanner/nextFloat()F\n");
                fprintf(Outfp, "\tfstore %d\n", node->symLocalNum + 1);
                break;
              }
        }
    }
}
void GenFunctionStart(struct PType* ret)
{
  switch(ret->type){
		case INTEGER_t:
			fprintf(Outfp, ")I\n");
			break;
		case REAL_t:
			fprintf(Outfp, ")F\n");
			break;
		case BOOLEAN_t:
			fprintf(Outfp, ")Z\n");
			break;
		default:
			fprintf(Outfp, ")V\n");
			break;
	}
}

void GenFunctionEnd(struct PType* ret)
{
  switch(ret->type){
    case INTEGER_t:
        fprintf(Outfp, "\tireturn\n");
        break;
    case BOOLEAN_t:
        fprintf(Outfp, "\tbreturn\n");
        break;
    case REAL_t:
        fprintf(Outfp, "\tfreturn\n");
        break;
    default:
        fprintf(Outfp, "\treturn\n");
        break;
    }
    fprintf(Outfp, ".end method\n");
}
void GenFuncRutern(struct expr_sem* ret)
{
  switch(ret->pType->type){
    case INTEGER_t:
        fprintf(Outfp, "\tireturn\n");
        break;
    case BOOLEAN_t:
        fprintf(Outfp, "\tbreturn\n");
        break;
    case REAL_t:
        fprintf(Outfp, "\tfreturn\n");
        break;
    }
}
void GenFunctionStart_param(struct param_sem *parPtr)
{
  struct idNode_sem *idPtr;
  for(idPtr = parPtr->idlist; idPtr != 0; idPtr = idPtr->next)
  {
    switch(parPtr->pType->type)
    {
      case INTEGER_t:
        fprintf(Outfp, "I");
        break;
      case REAL_t:
        fprintf(Outfp, "F");
        break;
      case BOOLEAN_t:
        fprintf(Outfp, "Z");
        break;
    }
  }
}

void GenSaveExpr(struct expr_sem* expr,struct expr_sem* RHS)
{
  struct SymNode* lookup ;
  lookup = lookupLoopVar(symbolTable, expr->varRef->id);
  if(lookup == 0)
  {
      lookup = lookupSymbol(symbolTable, expr->varRef->id, scope, __FALSE);
  }

  if(expr->pType->type == REAL_t && RHS->pType->type == INTEGER_t)
  {
      fprintf(Outfp, "\ti2f\n");
  }

  if (simple && lookup->category == CONSTANT_t)
  {
      switch(lookup->type->type) {
          case INTEGER_t:
              fprintf(Outfp, "sipush %d\n", lookup->attribute->constVal->value.integerVal);
              break;
          case BOOLEAN_t:
              fprintf(Outfp, "iconst_%d\n", lookup->attribute->constVal->value.booleanVal);
              break;
          case STRING_t:
              //if(lookup->attribute->constVal->value.stringVal != "\n")
              fprintf(Outfp, "ldc %s\n", lookup->attribute->constVal->value.stringVal);
              break;
          case REAL_t:
              fprintf(Outfp, "ldc %s\n", lookup->attribute->constVal->value.realVal);
              break;
          }
  }

  else if(simple && lookup->category != CONSTANT_t)
  {
      if (lookup->scope > 0)
      {
          switch(lookup->type->type) {
          case INTEGER_t:
              fprintf(Outfp, "\tistore %d\n", lookup->symLocalNum + 1);
              break;
          case BOOLEAN_t:
              fprintf(Outfp, "\tistore %d\n", lookup->symLocalNum + 1);
              break;
          case REAL_t:
              fprintf(Outfp, "\tfstore %d\n", lookup->symLocalNum + 1);
              break;
          }
      }
  }
}

void GenPrint(struct expr_sem* expr)
{
	switch(expr->pType->type){
		case STRING_t:
			fprintf(Outfp, "\tinvokevirtual java/io/PrintStream/print(Ljava/lang/String;)V\n");
			break;
		case INTEGER_t:
			fprintf(Outfp, "\tinvokevirtual java/io/PrintStream/print(I)V\n");
			break;
		case REAL_t:
			fprintf(Outfp, "\tinvokevirtual java/io/PrintStream/print(F)V\n");
			break;
		case BOOLEAN_t:
			fprintf(Outfp, "\tinvokevirtual java/io/PrintStream/print(Z)V\n");
			break;
	}
}
void GenFunctionCall(char* id)
{
  struct SymNode *node = 0;
	node = lookupSymbol( symbolTable, id, 0, __FALSE );
  switch(node->type->type){
    case INTEGER_t:
        fprintf(Outfp, "\t") ;
        break;
    case BOOLEAN_t:
        fprintf(Outfp, "\tb") ;
        break;
    case STRING_t:
        fprintf(Outfp, "\tc") ;
        break;
    case REAL_t:
        fprintf(Outfp, "\t") ;
        break;
  }
}
void GenForLoop(char* id,int start,int end)
{
  struct SymNode *node = 0;
  node = lookupLoopVar( symbolTable,id );
  if(node != 0)
      node->symLocalNum = ++localnumber;

  fprintf(Outfp, "\tldc %d\n", start);
  fprintf(Outfp, "\tistore %d\n", node->symLocalNum);

  fprintf(Outfp, "Lbegin_%d:\n", labelLoopNum++);
  fprintf(Outfp, "\tiload %d\n", node->symLocalNum);
  //fprintf(Outfp, "\tsipush %d\n", end);
  fprintf(Outfp, "\tldc %d\n", end);
  fprintf(Outfp, "\tisub\n");

  fprintf(Outfp, "\tifle Ltrue_%d\n", labelLoopNum);
  fprintf(Outfp, "\ticonst_0\n");
  fprintf(Outfp, "\tgoto Lfalse_%d\n", labelLoopNum);

  fprintf(Outfp, "Ltrue_%d:\n", labelLoopNum);
  fprintf(Outfp, "\ticonst_1\n");
  fprintf(Outfp, "Lfalse_%d:\n", labelLoopNum++);
  fprintf(Outfp, "\tifeq Lexit_%d\n", labelLoopNum-2);
}

void GenForLoopEnd(char* id)
{
	struct SymNode* ptr;
	ptr=lookupLoopVar(symbolTable,id);
  fprintf(Outfp, "\tiload %d\n", ptr->symLocalNum);
  fprintf(Outfp, "\tsipush 1\n");
  fprintf(Outfp, "\tiadd\n");
  fprintf(Outfp, "\tistore %d\n", ptr->symLocalNum);
  labelLoopNum = labelLoopNum - 2;
  fprintf(Outfp, "\tgoto Lbegin_%d\n", labelLoopNum);
  fprintf(Outfp, "Lexit_%d:\n", labelLoopNum);

}
void GenToList()
{
  fprintf(Outfp, "\ticonst_0\n");
  fprintf(Outfp, "\tgoto Lfalse_%d\n", label_num);
  fprintf(Outfp, "Ltrue_%d:\n", label_num);
  fprintf(Outfp, "\ticonst_1\n");
  fprintf(Outfp, "Lfalse_%d:\n", label_num++);
  fprintf(Outfp, "\tifeq Lelse_%d\n", label_num);
}
void GenRelational( struct expr_sem *op1, OPERATOR operator, struct expr_sem *op2)
{
  if(op2->pType->type == INTEGER_t)
      fprintf(Outfp, "\tisub\n\t");
  else if(op2->pType->type == REAL_t)
      fprintf(Outfp, "\tfcmpl\n\t");

  switch(operator){
    case LT_t:
        fprintf(Outfp, "iflt Ltrue_%d\n", label_num);
        GenToList();
        break;
    case LE_t:
  			fprintf(Outfp, "ifle Ltrue_%d\n", label_num);
        GenToList();
  			break;
		case NE_t:
  			fprintf(Outfp, "ifne\n");
        GenToList();
  			break;
		case GE_t:
  			fprintf(Outfp, "ifge Ltrue_%d\n", label_num);
        GenToList();
  			break;
		case GT_t:
  			fprintf(Outfp, "ifgt Ltrue_%d\n", label_num);
        GenToList();
  			break;
		case EQ_t:
  			fprintf(Outfp, "ifeq Ltrue_%d\n", label_num);
        GenToList();
  			break;
  }
}

void GenArithmetic( struct expr_sem *op1, OPERATOR operator, struct expr_sem *op2)
{
  switch(operator){
		case ADD_t:
			if(op1->pType->type == INTEGER_t){
				fprintf(Outfp, "\tiadd\n");
			}
			else if(op1->pType->type == REAL_t){
				fprintf(Outfp, "\tfadd\n");
			}
			break;
		case SUB_t:
			if(op1->pType->type == INTEGER_t){
				fprintf(Outfp, "\tisub\n");
			}
			else if(op1->pType->type == REAL_t){
				fprintf(Outfp, "\tfsub\n");
			}
			break;
		case MUL_t:
			if(op1->pType->type == INTEGER_t){
				fprintf(Outfp, "\timul\n");
			}
			else if(op1->pType->type == REAL_t){
				fprintf(Outfp, "\tfmul\n");
			}
			break;
		case DIV_t:
			if(op1->pType->type == INTEGER_t){
				fprintf(Outfp, "\tidiv\n");
			}
			else if(op1->pType->type == REAL_t){
				fprintf(Outfp, "\tfdiv\n");
			}
			break;
		case MOD_t:
			  fprintf(Outfp, "\tirem\n");
			  break;
	}
}

void GenNegative(char *id)
{
    struct SymNode *node = 0;
    node = lookupSymbol( symbolTable, id, 0, __FALSE );


    switch(node->type->type) {
    case INTEGER_t:
        fprintf(Outfp, "\ti") ;
        break;
    case BOOLEAN_t:
        fprintf(Outfp, "\tb") ;
        break;
    case STRING_t:
        fprintf(Outfp, "\tc") ;
        break;
    case REAL_t:
        fprintf(Outfp, "\tf") ;
        break;
      }
    fprintf(Outfp, "neg\n");
}
