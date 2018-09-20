#include "symbolTable.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int function_parameter(Expr* );
extern TableEntry* buildentry(char* , const char* , int , Type* , Attribute*);

void insert_entry(SymbolTable* table, TableEntry* e)
{
    if (strcmp(return_type_buffer(e->type, 0), "null") == 0)
        return;
    if (find_in_scope(table, e->name) != NULL || find_in_loop(table, e->name) != NULL)
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
    //grow the capacity
    if (table->position == table->capacity)
    {
        table->capacity *= 2;
        TableEntry** tmp_entries = table->Entries;
        table->Entries = (TableEntry**)malloc(sizeof(TableEntry*) * (table->capacity));
        int i;
        for (i = 0; i < table->position; i++)
        {
            (table->Entries)[i] = tmp_entries[i];
        }
        free(tmp_entries);
    }
    table->Entries[table->position++] = e;
}

void insert_entry_list(SymbolTable* table, Id_List* list, const char* kind, Type* type, Attribute* attri)
{
    int i;
    for (i = 0; i < list->position; i++)
    {
        TableEntry* new_entry = buildentry(list->Ids[i], kind, table->current_level, type, attri);
        insert_entry(table, new_entry);
    }
}
void print_table(SymbolTable* table)
{
    if (!Opt_D)
        return;
    int i;
    TableEntry* ptr;

    for (i = 0; i < 110; i++)
        printf("=");
    printf("\n");
    printf("%-33s%-11s%-11s%-17s%-11s\n", "Name", "Kind", "Level", "Type", "Attribute");
    for (i = 0; i < 110; i++)
        printf("-");
    printf("\n");
    for (i = 0; i < table->position; i++)
    {
        ptr = table->Entries[i];
        if (ptr->level == table->current_level)
        {
            printf("%-33s",ptr->name);

            printf("%-11s", ptr->kind);
            if (ptr->level == 0)
            {
                printf("%d%-10s", ptr->level, "(global)");
            }
            else
            {
                printf("%d%-10s", ptr->level, "(local)");
            }
            printf("%-17s", return_type_buffer(ptr->type, 0));

            if (ptr->attri == NULL)
            {
            }
            else if (ptr->attri->val != NULL)
            {
                if (strcmp(ptr->attri->val->type->name, "string") == 0)
                    printf("%-11s", ptr->attri->val->string_value);
                else if (strcmp(ptr->attri->val->type->name, "integer") == 0
                || strcmp(ptr->attri->val->type->name, "octal") == 0)
                    printf("%-11d", ptr->attri->val->int_value);
                else if (strcmp(ptr->attri->val->type->name, "real") == 0)
                        printf("%-11f", ptr->attri->val->double_value);
                else if (strcmp(ptr->attri->val->type->name, "boolean") == 0)
                    printf("%-11s", ptr->attri->val->string_value);
            }
            else if (ptr->attri->type_list != NULL)
            {
                Type_List* l = ptr->attri->type_list;
                int i;
                printf("%s ", return_type_buffer(l->types[0], 0));
                for (i = 1 ; i < l->current_size; i++)
                {
                    printf(", %s", return_type_buffer(l->types[i], 0));
                }
            }
            printf("\n");
        }
    }
    for (i = 0; i < 110; i++)
        printf("-");
    printf("\n");
}

char* return_type_buffer(const Type* t, int current_dim)
{
    if (t == NULL)
      return "error_type";
    Array_Signl* ptr = t->Array_Signlature;
    char* output_buf = (char*)malloc(sizeof(char) * 18);
    char tmp_buf[18];

    int name_len = strlen(t->name) + 1;
    memset(output_buf, 0, 18);
    snprintf(output_buf, name_len, "%s", t->name);

    while (ptr != NULL)
    {
        if (current_dim)
        {
            current_dim--;
        }
        else
        {
            snprintf(tmp_buf, 4, "[%d]", ptr->capacity);
            strcat(output_buf, tmp_buf);
        }
        ptr = ptr->next_dimension;
    }
    return output_buf;
}

Type_List* add_typelist(Type_List* list, Type* t, int count)
{
    int i;
    if (list == NULL)
    {
        list = (Type_List*)malloc(sizeof(Type_List));
        list->types = (Type**)malloc(sizeof(Type**) * 4);
        list->capacity = 4;
        list->current_size = 0;
    }
    if (list->current_size <= list->capacity + count)
    {
        list->capacity *= 2;
        Type** tmp = list->types;
        list->types = (Type**)malloc(sizeof(Type**) * list->capacity);
        for (i = 0; i < list->current_size; i++)
        {
            (list->types)[i] = tmp[i];
        }
        free(tmp);
    }
    for (i = 0; i < count; i++)
    {
        list->types[list->current_size++] = t;
    }
    return list;
}

TableEntry* find_in_scope(SymbolTable* table, char* name)
{
    int i;
    for (i = 0; i < table->position; i++)
    {
        TableEntry* it = table->Entries[i];
        if (strcmp(name, it->name) == 0 && it->level == table->current_level)
        {
            return it;
        }
    }
    return NULL;
}

TableEntry* find_in_global(SymbolTable* table, char* name)
{
    int i;
    for (i = 0; i < table->position; i++) {
        TableEntry* it = table->Entries[i];
        if (strcmp(name, it->name) == 0 && it->level == 0)
        {
            return it;
        }
    }
    return NULL;
}
TableEntry* find_in_loop(SymbolTable* table, char* name)
{
    int i;
    for (i = 0; i < table->position; i++) {
        TableEntry* it = table->Entries[i];
        if (strcmp(name, it->name) == 0 && strcmp(it->kind, "loop varible") == 0)
        {
            return it;
        }
    }
    return NULL;
}

Expr* find_var_ref(SymbolTable* table, char* name)
{
    Expr* e = (Expr*)malloc(sizeof(Expr));
    TableEntry* tmp = find_in_scope(table, name);
    if (tmp == NULL)
        tmp = find_in_global(table, name);
    if (tmp == NULL)
        tmp = find_in_loop(table, name);
    if (tmp == NULL)
    {
        strcpy(e->kind, "err");
        strcpy(e->name, name);
        e->current_size = 0;
        e->entry = NULL;
        e->para = NULL;
        //printf("<Error> found in Line %d: symbol '%s' is not declared\n", linenum, name);
        return e;
    }
    strcpy(e->kind, "var");
    strcpy(e->name, name);
    e->current_size = 0;
    e->entry = tmp;
    e->type = e->entry->type;
    return e;
}

Expr* call_function(char* name, Expr_List* list)
{
    int i;
    Expr* expre = (Expr*)malloc(sizeof(Expr));
    strcpy(expre->kind, "function");
    strcpy(expre->name, name);
    expre->current_size = 0;
    expre->entry = find_in_global(symbol_table, name);
    if (expre->entry == NULL)
    {
        printf("<Error> found in Line %d: function '%s' is not declared\n", linenum, name);
        strcpy(expre->kind, "err");
        expre->para = NULL;
        return expre;
    }
    else
    {
        expre->type = expre->entry->type;
    }
    if (list == NULL)
    {
        expre->para = NULL;
    }
    else
    {
        Type_List* para = add_typelist(NULL, list->exprs[0]->type, 1);
        for (i = 1; i < list->current_size; i++)
        {
            add_typelist(para, list->exprs[i]->type, 1);
        }
        expre->para = para;
    }
    if (!function_parameter(expre))
    {
        int i;
        if (expre->para == NULL)
            return expre;
        for (i = 0; i < expre->para->current_size; i++)
        {
            if (strcmp(return_type_buffer(list->exprs[i]->type, list->exprs[i]->current_size),return_type_buffer(expre->entry->attri->type_list->types[i], 0))!= 0)
            {
                if (strcmp(return_type_buffer(list->exprs[i]->type, list->exprs[i]->current_size), "integer") == 0 && strcmp(return_type_buffer(expre->entry->attri->type_list->types[i], 0), "real") == 0)
                      continue;
                printf("<Error> found in Line %d: parameter type mismatch\n", linenum);
                return expre;
            }
        }
    }
    return expre;
}
