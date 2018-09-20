#include "Symbol.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TableEntry* build_entry(char* name, const char* kind, int level, Type* type, Attribute* attri)
{
    TableEntry* new = (TableEntry*)malloc(sizeof(TableEntry));
    strcpy(new->name, name);
    strcpy(new->kind, kind);
    new->level = level;
    new->type = type;
    new->attri = attri;
    return new;
}

void insert_entry(SymbolTable* t, TableEntry* e)
{

    if (find_in_scope(t, e->name) != NULL || find_in_loop(t, e->name) != NULL)
    {
        int i;
        char temp[32];
        for(i = 0 ; i < 32 ; i++)
        {
          temp[i] = e -> name[i];
        }
        printf("<Error> found in Line %d: symbol ",linenum );
        for(i = 0 ; i < 32 ; i++)
        {
          if(temp[i] == 0)
              continue;
          printf("%c",temp[i]);
        }
        printf(" is redeclared\n");
        return;
    }
    if (t->position == t->size_)
    {
        t->size_ *= 2;
        TableEntry** tmp_entries = t->Entries;
        t->Entries = (TableEntry**)malloc(sizeof(TableEntry*) * (t->size_));
        int i;
        for (i = 0; i < t->position; i++) {
            (t->Entries)[i] = tmp_entries[i];
        }
        free(tmp_entries);
    }

    t->Entries[t->position++] = e;
}

void insert_entry_list(SymbolTable* t, IdList* l, const char* kind, Type* type, Attribute* attri)
{
    int i;
    for (i = 0; i < l->position; i++)
    {
        TableEntry* new_entry = build_entry(l->Ids[i], kind,
            t->current_level, type, attri);
        insert_entry(t, new_entry);
    }
}

TypeList* add_type_list(TypeList* l, Type* t, int count)
{
    int i;
    if (l == NULL)
    {
        l = (TypeList*)malloc(sizeof(TypeList));
        l->types = (Type**)malloc(sizeof(Type**) * 4);
        l->size_ = 4;
        l->current_size = 0;
    }
    if (l->current_size <= l->size_ + count)
    {
        l->size_ *= 2;
        Type** tmp = l->types;
        l->types = (Type**)malloc(sizeof(Type**) * l->size_);
        for (i = 0; i < l->current_size; i++) {
            (l->types)[i] = tmp[i];
        }
        free(tmp);
    }
    for (i = 0; i < count; i++) {
        l->types[l->current_size++] = t;
    }
    return l;
}

TableEntry* find_in_scope(SymbolTable* table_, char* name)
{
    int i;
    for (i = 0; i < table_->position; i++)
    {
        TableEntry* it = table_->Entries[i];
        if (strcmp(name, it->name) == 0 && it->level == table_->current_level) {
            return it;
        }
    }
    return NULL;
}

TableEntry* find_in_global(SymbolTable* table_, char* name)
{
    int i;
    for (i = 0; i < table_->position; i++)
    {
        TableEntry* it = table_->Entries[i];
        if (strcmp(name, it->name) == 0 && it->level == 0) {
            return it;
        }
    }
    return NULL;
}
TableEntry* find_in_loop(SymbolTable* table_, char* name)
{
    int i;
    for (i = 0; i < table_->position; i++)
    {
        TableEntry* it = table_->Entries[i];
        if (strcmp(name, it->name) == 0 && strcmp(it->kind, "loop var") == 0) {
            return it;
        }
    }
    return NULL;
}

Expr* find_varref(SymbolTable* table_, char* name)
{
    Expr* e = (Expr*)malloc(sizeof(Expr));
    TableEntry* finding = find_in_scope(table_, name);
    if (finding == NULL)
        finding = find_in_global(table_, name);
    if (finding == NULL)
        finding = find_in_loop(table_, name);
    if (finding == NULL) {
        strcpy(e->name, name);
        e->current_dimension = 0;
        e->entry = NULL;
        e->para = NULL;
        return e;
    }
    strcpy(e->name, name);
    e->current_dimension = 0;
    e->entry = finding;
    e->type = e->entry->type;
    return e;
}
Expr* call_func(char* name, ExprList* l)
{
    int i;
    Expr* e = (Expr*)malloc(sizeof(Expr));
    strcpy(e->kind, "function");
    strcpy(e->name, name);
    e->current_dimension = 0;
    e->entry = find_in_global(symbol_table, name);
    if (e->entry != NULL)
    {
        e->type = e->entry->type;
    }
    if (l == NULL)
    {
        e->para = NULL;
    }
    else
    {
        TypeList* para = add_type_list(NULL, l->exprs[0]->type, 1);
        for (i = 1; i < l->current_size; i++)
        {
            add_type_list(para, l->exprs[i]->type, 1);
        }
        e->para = para;
    }
    return e;
}
