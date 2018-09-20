#include "symbolTable.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int const_assign(Expr* ex)
{
    if (ex == NULL || ex->entry == NULL)
        return 0;
    if (strcmp(ex->kind, "err") == 0)
        return 0;
    if (strcmp(ex->entry->kind, "constant") == 0)
    {
        printf("<Error> found in Line %d: constant %s cannot be assigned\n", linenum, ex->entry->name);
        return 1;
    }
    else if (strcmp(ex->entry->kind, "loop varible") == 0)
    {
        printf("<Error> found in Line %d: loop varible '%s' cannot be assigned\n", linenum, ex->entry->name);
        return 1;
    }
    return 0;
}

int function_parameter(Expr* e)
{
    if (e == NULL)
        return 0;
    if (strcmp(e->kind, "err") == 0)
        return 0;
    if (strcmp(e->kind, "function") != 0)
        return 0;
    if (strcmp(e->type->name, "void") == 0 && e->para == NULL)
    {
        return 0;
    }
    else if (strcmp(e->type->name, "void") == 0 && e->para != NULL)
    {
        printf("<Error> found in Line %d: too many arguments to function '%s'\n", linenum, e->name);
        return 1;
    }
    else if (strcmp(e->type->name, "void") != 0 && e->para == NULL)
    {
        printf("<Error> found in Line %d: too few arguments to function '%s'\n", linenum, e->name);
        return 1;
    }
    else if (e->para->current_size > e->entry->attri->type_list->current_size)
    {
        printf("<Error> found in Line %d: too many arguments to function '%s'\n", linenum, e->name);
        return 1;
    }
    else if (e->para->current_size < e->entry->attri->type_list->current_size)
    {
        printf("<Error> found in Line %d: too few arguments to function '%s'\n", linenum, e->name);
        return 1;
    }
    return 0;
}
