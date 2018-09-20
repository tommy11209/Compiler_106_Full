#include "symbolTable.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char* return_type_buffer(const Type* , int );
extern Type* buildtype(const char* );

Expr* relation_operation(Expr* LHS, char* operater, Expr* RHS)
{
    if (LHS == NULL || RHS == NULL)
        return NULL;
    Expr* expre = (Expr*)malloc(sizeof(Expr));
    strcpy(expre->kind, "var");
    expre->current_size = 0;
    expre->entry = NULL;
    expre->type = buildtype("boolean");
    if (strcmp(LHS->kind, "err") == 0 || strcmp(RHS->kind, "err") == 0)
    {
        strcpy(expre->kind, "err");
        return expre;
    }
    if (!((strcmp(return_type_buffer(LHS->type, LHS->current_size), "integer") != 0 && strcmp(return_type_buffer(RHS->type, RHS->current_size), "integer") != 0)
        ||(strcmp(return_type_buffer(LHS->type, LHS->current_size), "real") != 0 && strcmp(return_type_buffer(RHS->type, RHS->current_size), "real") != 0)))
    {
        printf("<Error> found in Line %d: between %s are not both integer/real\n", linenum, operater);
        strcpy(expre->kind, "err");
        expre->type = buildtype(LHS->type->name);
        return expre;
    }
    if (strcmp(LHS->type->name, "string") == 0 || strcmp(RHS->type->name, "string") == 0)
    {
        printf("<Error> found in Line %d: Side of '%s' is '%s' type\n", linenum, operater, return_type_buffer(LHS->type, LHS->current_size));
        strcpy(expre->kind, "err");
        return expre;
    }
    return expre;
}
Expr* add_operation(Expr* LHS, char* operater, Expr* RHS)
{
    if (LHS == NULL || RHS == NULL)
        return NULL;
    if (strcmp(LHS->kind, "err") == 0 || strcmp(RHS->kind, "err") == 0)
        return 0;
    Expr* expre = (Expr*)malloc(sizeof(Expr));
    expre->current_size = 0;
    expre->entry = NULL;
    strcpy(expre->kind, "var");

    if (strcmp(LHS->type->name, "string") == 0 && strcmp(RHS->type->name, "string") == 0)
    {
        expre->type = buildtype("string");
        strcpy(expre->kind, "var");
        return expre;
    }
    if (strcmp(return_type_buffer(LHS->type, LHS->current_size), "real") == 0 || strcmp(return_type_buffer(RHS->type, RHS->current_size), "real") == 0)
    {
        expre->type = buildtype("real");
        return expre;
    }
    if ((strcmp(return_type_buffer(LHS->type, LHS->current_size), "integer") != 0 && strcmp(return_type_buffer(LHS->type, LHS->current_size), "real") != 0)
      || (strcmp(return_type_buffer(RHS->type, RHS->current_size), "integer") != 0 && strcmp(return_type_buffer(RHS->type, RHS->current_size), "real") != 0))
    {
        printf("<Error> found in Line %d: between %s are not integer/real\n", linenum, operater);
        strcpy(expre->kind, "err");
        expre->type = buildtype(LHS->type->name);
        return expre;
    }

    free(expre->type);
    free(expre);
    return LHS;
}
Expr* bool_operation(Expr* LHS, char* operater, Expr* RHS)
{
    if (LHS == NULL || RHS == NULL)
        return NULL;
    if (strcmp(LHS->kind, "err") == 0 || strcmp(RHS->kind, "err") == 0)
        return NULL;
    Expr* expre = (Expr*)malloc(sizeof(Expr));
    expre->current_size = 0;
    expre->entry = NULL;
    strcpy(expre->kind, "var");

    if ((strcmp(return_type_buffer(LHS->type, LHS->current_size), "boolean") != 0) || (strcmp(return_type_buffer(RHS->type, RHS->current_size), "boolean") != 0))
    {
        printf("<Error> found in Line %d: operands between '%s' are not boolean\n", linenum, operater);
        strcpy(expre->kind, "err");
        expre->type = buildtype(LHS->type->name);
        return expre;
    }

    free(expre->type);
    free(expre);
    return LHS;
}

Expr* multi_operation(Expr* LHS, char* operater, Expr* RHS)
{
    if (LHS == NULL || RHS == NULL)
        return NULL;
    if (strcmp(LHS->kind, "err") == 0 || strcmp(RHS->kind, "err") == 0)
        return NULL;
    Expr* expre = (Expr*)malloc(sizeof(Expr));
    expre->current_size = 0;
    expre->entry = NULL;

    if (strcmp(operater, "mod") == 0)
    {
        if (strcmp(return_type_buffer(LHS->type, LHS->current_size), "integer") != 0 || strcmp(return_type_buffer(RHS->type, RHS->current_size), "integer") != 0)
        {
            printf("<Error> found in Line %d: between 'mod' are not integer\n", linenum);
            strcpy(expre->kind, "err");
            expre->type = buildtype(LHS->type->name);
            return expre;
        }
    }
    if ((strcmp(return_type_buffer(LHS->type, LHS->current_size), "integer") != 0 && strcmp(return_type_buffer(LHS->type, LHS->current_size), "real") != 0)
        || (strcmp(return_type_buffer(LHS->type, LHS->current_size), "integer") != 0 && strcmp(return_type_buffer(LHS->type, LHS->current_size), "real") != 0))
    {
        printf("<Error> found in Line %d: between %s are not integer/real\n", linenum, operater);
        strcpy(expre->kind, "err");
        expre->type = buildtype(LHS->type->name);
        return expre;
    }

    if (strcmp(return_type_buffer(LHS->type, LHS->current_size), "real") == 0 || strcmp(return_type_buffer(RHS->type, RHS->current_size), "real") == 0) {
        expre->type = buildtype("real");
        strcpy(expre->kind, "err");
        return expre;
    }

    free(expre->type);
    free(expre);
    return LHS;
}

Value* sub_operation(Value* v)
{
    if (v == NULL)
        return NULL;
    Type* temp = v->type;
    if (strcmp(temp->name, "real") == 0)
    {
        if (strstr(v->string_value, "E") || strstr(v->string_value, "e"))
        {
            char* tmp = v->string_value;
            v->string_value = (char*)malloc(strlen(v->string_value) + 2);
            v->string_value[0] = '-';
            strcat(v->string_value, tmp);
            free(tmp);
        }
        else
        {
            v->double_value *= -1.0;
        }
    }
    else if (strcmp(temp->name, "integer") == 0)
    {
        v->int_value *= -1;
    }
    return v;
}
