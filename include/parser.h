#ifndef PARSER_H
#define PARSER_H

#include "boolean.h"
#include "reserved.h"
#include "symbol_table.h"
#include <stdio.h>
#include "words.h"
/* default */
#define BUFFER_MAX_SIZE 256

/* label type */
typedef enum {NONE_LABEL, DATA_LABEL, INSTRUCTION_LABEL} label_type;

/* how is a code line save in memory - linked list of code lines*/
typedef struct CodeLine{
    char *line; /* original code line - mostly for debug */
    int address; /* line address*/
    int line_number; /*line count*/

    char *label; /* label name - if there is one */
    label_type label_type; /* label type - if there is one  */
    int label_value; /* label value - if there is one */

    int opcode; /* line opcode value */
    int funct; /* line funct value*/

    instruction_type instruction_type; /* id this is a code instruction it will have a type */
    data_instruction_type data_instruction_type; /* id this is a data instruction it will have a type */

    word_list *operands; /* a linked list of operands ad string*/

    struct CodeLine *next_code_line; /* the next code line*/

    bool is_entry; /* true if this is an entry*/
    bool is_extern; /* true if this is an extrenal value  */
} code_line;

typedef struct ASM{
    code_line *code_line; /* linked list of code lines */
    int line_count; /* line count */
    table *symbol_table; /* table struct tha has a linked list of symbols */
} asm_file;

/* prototpes*/
asm_file *new_asm(FILE *fp);
asm_file *new_asm_file();
bool parse_line(asm_file *asm_f, char *line, code_line **code);
bool is_label(char *word);
char *read_word(char* line);
int check_type_r_operands(word_list *word);
int check_type_i_branch_operands(word_list *word);
int check_operands_by_type(word_list *word,code_line *code);
bool is_register(char *word);
int check_type_j_operands(word_list *word);
int check_type_i_operands(word_list *word);
int check_type_j_operands(word_list *word);
bool is_register(char *word);
bool handle_data_by_type(word_list *word, code_line *code,data_instruction_type type);
bool save_data(word_list *word, code_line *code,data_instruction_type type);
bool is_valid_by_type(word_list *word, data_instruction_type type);
bool is_asci(char* string);
bool is_byte(char* byte);
bool is_word(char* word);
bool is_half_word(char *half);
bool is_valid_extern(word_list* word);
int read_operands(word_list *word, int expected_count, code_line *code);
void free_asm_file(asm_file *asm_f);
void free_code_lines(code_line *line);

# endif