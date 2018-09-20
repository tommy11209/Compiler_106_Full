#include "Symbol.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char* print_type(const Type* t, int current_dim)
{

    Array_signal* ptr = t->array_signature;
    char* output_buf = (char*)malloc(sizeof(char) * 18);
    char tmp_buf[18];

    int name_len = strlen(t->name) + 1;
    memset(output_buf, 0, 18);
    snprintf(output_buf, name_len, "%s", t->name);

    while (ptr != NULL) {
        if (current_dim)
        {
            current_dim--;
        }
        else
        {
            snprintf(tmp_buf, 10, "[%d]", ptr->size_);
        }
        strcat(output_buf, tmp_buf);
        ptr = ptr->next_dimension;
    }
    return output_buf;
}
void PrintSymbolTable(SymbolTable* t)
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
    for (i = 0; i < t->position; i++)
    {
        ptr = t->Entries[i];
        if (ptr->level == t->current_level)
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
            printf("%-17s", print_type(ptr->type, 0));

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
                TypeList* l = ptr->attri->type_list;
                int i;
                printf("%s ", print_type(l->types[0], 0));
                for (i = 1 ; i < l->current_size; i++)
                {
                    printf(", %s", print_type(l->types[i], 0));
                }
            }
            printf("\n");
        }
    }
    for (i = 0; i < 110; i++)
        printf("-");
    printf("\n");
}
