#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include "boolean.h"

/* Defalts */
typedef enum {ETERNAL_TABLE_ATTR_CODE, DATA_TABLE_ATTR_CODE, CODE_TABLE_ATTR_CODE} attribute;
#define EMPTY_ENTRY -1

/* Structures */
typedef struct TableLine{
    char *label; /* symbol label name */
    int value;  /* symbol value */
    attribute attr;  /*  symbol attribute */
    bool isEntry; /* is an entry label*/
    bool isExtern; /* is an extern label*/
    struct TableLine *next_line; /* linked list of table lines*/
    int line_index; /*line index*/
} table_line;


typedef struct Table{
    table_line *line; 
    int length;
} table;

/* Protorypes*/
table *new_table ();
bool push_to_table(table *t, char *label, int value, attribute attr, int code_line, bool is_entry, bool is_extern);
bool write_line(table **t,char *label, int value, attribute attr, int code_line);
table_line *search(table *t,char *label);
void set_entry(table **t, char *label);
void set_extern(table **t, char *label, int line_number);
void update_value(table *t, char *label, int value);
void free_table(table *t);

#endif