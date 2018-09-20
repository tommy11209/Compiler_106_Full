#include "symbolTable.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SymbolTable* buildtable()
{
    SymbolTable* table = (SymbolTable*)malloc(sizeof(SymbolTable));
    table->current_level = 0;
    table->position = 0;
    table->capacity = 4;
    table->Entries = (TableEntry**)malloc(sizeof(TableEntry*) * 4);
    return table;
}

Type* buildtype(const char* typename)
{
    Type* type_ = (Type*)malloc(sizeof(Type));
    strcpy(type_->name, typename);
    type_->Array_Signlature = NULL;
    return type_;
}

Value* buildscientific(const char* val)
{
  Value* v = (Value*)malloc(sizeof(Value));
  v->type = buildtype("real");
  v->string_value = NULL;
  v->int_value = 0;
  v->double_value = atof(val);
  v->string_value = strdup(val);
  return v;
}
Value* buildoctal(const char* val)
{
  Value* v = (Value*)malloc(sizeof(Value));
  v->type = buildtype("octal");
  v->string_value = NULL;
  v->int_value = 0;
  v->int_value = strtol(val, NULL, 8);
  return v;
}
Value* buildboolean(const char* val)
{
  Value* v = (Value*)malloc(sizeof(Value));
  v->type = buildtype("boolean");
  v->string_value = NULL;
  v->int_value = 0;
  v->string_value = strdup(val);
  return v;
}
Value* buildstring(const char* val)
{
  Value* v = (Value*)malloc(sizeof(Value));
  v->type = buildtype("string");
  v->string_value = NULL;
  v->int_value = 0;
  v->string_value = strdup(val);
  return v;
}
Value* buildreal(const char* val)
{
  Value* v = (Value*)malloc(sizeof(Value));
  v->type = buildtype("real");
  v->string_value = NULL;
  v->int_value = 0;
  v->double_value = atof(val);
  v->string_value = strdup(val);
  return v;
}
Value* buildint(const char* val)
{
  Value* v = (Value*)malloc(sizeof(Value));
  v->type = buildtype("integer");
  v->string_value = NULL;
  v->int_value = 0;
  v->int_value = atoi(val);
  return v;
}
Id_List* buildidlist()
{
    Id_List* list = (Id_List*)malloc(sizeof(Id_List));
    list->position = 0;
    list->capacity = 4;
    list->Ids = (char**)malloc(sizeof(char*) * 4);
    return list;
}

Expr_List* buildexprlist(Expr_List* list, Expr* expre)
{
    int i;
    if (list == NULL)
    {
        list = (Expr_List*)malloc(sizeof(Expr_List));
        list->exprs = (Expr**)malloc(sizeof(Expr**) * 4);
        list->capacity = 4;
        list->current_size = 0;
    }
    if (list->current_size == list->capacity)
    {
        list->capacity *= 2;
        Expr** tmp = list->exprs;
        list->exprs = (Expr**)malloc(sizeof(Expr**) * list->capacity);
        for (i = 0; i < list->current_size; i++)
        {
            (list->exprs)[i] = tmp[i];
        }
        free(tmp);
    }
    list->exprs[list->current_size++] = expre;
    return list;
}

TableEntry* buildentry(char* name, const char* kind, int level, Type* type, Attribute* attri)
{
    TableEntry* entrys = (TableEntry*)malloc(sizeof(TableEntry));
    strcpy(entrys->name, name);
    strcpy(entrys->kind, kind);
    entrys->level = level;
    entrys->type = type;
    entrys->attri = attri;
    return entrys;
}
