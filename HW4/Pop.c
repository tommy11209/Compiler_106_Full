#include "symbolTable.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void pop_entry(SymbolTable* table)
{
    int i;
    TableEntry* ptr;
    for (i = 0; i < table->position; i++)
    {
        ptr = table->Entries[i];
        if (ptr->level == table->current_level)
        {
            free(ptr);
            if (i < (table->position) - 1)
            {
                table->Entries[i] = table->Entries[--(table->position)];
                i--;
                continue;
            }
            else
            {
                table->position--;
            }
        }
    }
}

void pop_entry_name(SymbolTable* table, char* name)
{
    int i;
    TableEntry* ptr;
    for (i = 0; i < table->position; i++)
    {
        ptr = table->Entries[i];
        if (ptr->level == table->current_level && strcmp(ptr->name, name) == 0)
        {
            free(ptr);
            if (i < (table->position) - 1)
            {
                table->Entries[i] = table->Entries[--(table->position)];
                i--;
                continue;
            }
            else
            {
                table->position--;
            }
        }
    }
}
