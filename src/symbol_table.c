#include "../include/symbol_table.h"
#include <stdlib.h>
#include <string.h>
#include "../include/boolean.h"
#include <stdio.h>
#include "../include/error_log.h"
#include "../include/words.h"

/*
    this file is in charge of the symbol table
    - creating new table struct
    - writing new line
    - searching lines
    - updating lines
*/

/* create a new table struct and return it as a pointer */
table *new_table (){
    table *t;

    t = (table*) malloc(sizeof(table));
    if(t == NULL){
        allocation_fault("t",__FILE__,__LINE__);

        return NULL;
    }

    t->length = 0;
    t->line = NULL;

    return t;
}

/* add a new symbol to the symbole table */
bool write_line(table **t,char *label, int value, attribute attr, int code_line){
    table_line *line;
    char *buffer;

    /* first check if table exist  */
    if(*t == NULL){
        return false;
    }

    /* now check if this is a new line  */
    line = search((*t), label);

    if(line){
        /*might be an entry, if this is the case just update what you found*/
        if(line->isEntry){
            strcpy(line->label, label);
            line->attr = attr;
            line->value = value;
            line->line_index = code_line;

            return true;
        }

        buffer = (char *) malloc(sizeof(char) * MAX_WORD_LENGTH);
        if(buffer == NULL){
            allocation_fault("buffer",__FILE__,__LINE__);

            return false;
        }

        sprintf(buffer, "label %s is already used in %d",label, line->line_index);

        log_error(buffer, code_line);

        free(buffer);
        
        return false;
    } else {
      /* new line */
      if(push_to_table((*t),label,value, attr, code_line, false, false)){
          (*t)->length++;

          return true;
      }
    }

    return false;
}

/* search for by label name in the symbole table*/
table_line *search(table *t,char *label){
    table_line *ptr;

    /* table is empty - skip search */
    if(t->line == NULL){
        return NULL;
    }

    ptr = t->line;
    while (ptr != NULL){
        if(strcmp(ptr->label, label) == 0){
            return ptr;
        }
        ptr = ptr->next_line;
    }

    return NULL;
}

/* add a new line to table, return true if it worked and false otherwise */
bool push_to_table(table *t, char *label, int value, attribute attr, int code_line, bool is_entry, bool is_extern){
    table_line *new_line;
    table_line *last = t->line;

    new_line = (table_line *) malloc(sizeof(table_line));
    if(new_line == NULL){
        allocation_fault("new_line",__FILE__,__LINE__);

        return false;
    }

    new_line->next_line = NULL;

    new_line->label = (char *) malloc(sizeof(char) * MAX_WORD_LENGTH);
    if(new_line->label == NULL){
        allocation_fault("new_line->label",__FILE__,__LINE__);

        return false;
    }

    strcpy(new_line->label, label);
    new_line->attr = attr;
    new_line->value = value;
    new_line->line_index = code_line;

    if(is_entry){
        new_line->isEntry = true;
    }

    if(is_extern){
        new_line->isExtern = true;
    }

    if(t->line == NULL){
        t->line = new_line;

        return true;
    }

    while (last->next_line != NULL){
        last = last -> next_line;
    }

    last->next_line = new_line;

    return true;
}

/* entry found, this might be an existing label - update, also might no be -create a new one */
void set_entry(table **t, char *label){
    table_line *line;

    line = search((*t), label);
    if(line && !(line->isEntry)){
        line->isEntry = true;
    } else if(line == NULL){
        push_to_table((*t),label,0,DATA_TABLE_ATTR_CODE, EMPTY_ENTRY, true, false);
    }
}

/* external label defined, need to make sure this name wasn't use already */
void set_extern(table **t, char *label, int line_number){
    table_line *line;
    char *buffer;

    line = search((*t), label);
    if(line){
        buffer = (char *) malloc(sizeof(char) * MAX_WORD_LENGTH);
        if(buffer == NULL){
            allocation_fault("buffer",__FILE__,__LINE__);

            return;
        }

        sprintf(buffer, "label %s is already used in line %d, it can't be an extern",label, line->line_index);

        log_error(buffer, line_number);

        free(buffer);
    } else{
        push_to_table((*t),label,0,DATA_TABLE_ATTR_CODE, line_number, false, true);
    }
}

/* used to update the address as data instruction get there final address only on second run */
void update_value(table *t, char *label, int value){
    table_line *p = search(t, label);

    if(p != NULL){
        p->value = value;
    }
}

/* free table from memory */
void free_table(table *t){
    table_line *line ;

    while(t->line != NULL){
        line = t->line->next_line;
        free(t->line);
        t->line = line;
    }

    free(t);
}