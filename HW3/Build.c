#include "Symbol.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SymbolTable* BuildSymbolTable()
{
    SymbolTable* new = (SymbolTable*)malloc(sizeof(SymbolTable));
    new->current_level = 0;
    new->position = 0;
    new->size_ = 4;
    new->Entries = (TableEntry**)malloc(sizeof(TableEntry*) * 4);
    return new;
}

IdList* BuildIdList()
{
    IdList* new = (IdList*)malloc(sizeof(IdList));
    new->position = 0;
    new->size_ = 4;
    new->Ids = (char**)malloc(sizeof(char*) * 4);
    return new;
}

Type* BuildType(const char* typename)
{
    Type* new = (Type*)malloc(sizeof(Type));
    strcpy(new->name, typename);
    new->array_signature = NULL;
    return new;
}

ExprList* BuildExprList(ExprList* l, Expr* e)
{
    int i;
    if (l == NULL) {
        l = (ExprList*)malloc(sizeof(ExprList));
        l->exprs = (Expr**)malloc(sizeof(Expr**) * 4);
        l->size_ = 4;
        l->current_size = 0;
    }
    if (l->current_size == l->size_)
    {
        l->size_ *= 2;
        Expr** tmp = l->exprs;
        l->exprs = (Expr**)malloc(sizeof(Expr**) * l->size_);
        for (i = 0; i < l->current_size; i++)
        {
            (l->exprs)[i] = tmp[i];
        }
        free(tmp);
    }
    l->exprs[l->current_size++] = e;
    return l;
}

Value* buildscientific(const char* val)
{
  Value* v = (Value*)malloc(sizeof(Value));
  v->type = BuildType("real");
  v->string_value = NULL;
  v->int_value = 0;
  v->double_value = atof(val);
  v->string_value = strdup(val);
  return v;
}
Value* buildoctal(const char* val)
{
  Value* v = (Value*)malloc(sizeof(Value));
  v->type = BuildType("octal");
  v->string_value = NULL;
  v->int_value = 0;
  v->int_value = strtol(val, NULL, 8);
  return v;
}
Value* buildboolean(const char* val)
{
  Value* v = (Value*)malloc(sizeof(Value));
  v->type = BuildType("boolean");
  v->string_value = NULL;
  v->int_value = 0;
  v->string_value = strdup(val);
  return v;
}
Value* buildstring(const char* val)
{
  Value* v = (Value*)malloc(sizeof(Value));
  v->type = BuildType("string");
  v->string_value = NULL;
  v->int_value = 0;
  v->string_value = strdup(val);
  return v;
}
Value* buildreal(const char* val)
{
  Value* v = (Value*)malloc(sizeof(Value));
  v->type = BuildType("real");
  v->string_value = NULL;
  v->int_value = 0;
  v->double_value = atof(val);
  v->string_value = strdup(val);
  return v;
}
Value* buildint(const char* val)
{
  Value* v = (Value*)malloc(sizeof(Value));
  v->type = BuildType("integer");
  v->string_value = NULL;
  v->int_value = 0;
  v->int_value = atoi(val);
  return v;
}
