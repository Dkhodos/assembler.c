#ifndef ASSEMBLER_H
#define ASSEMBLER_H
#include <stdio.h>
#include "parser.h"
#include "boolean.h"

/* the amount of memory in bits every part of a instruction needs */
#define R_TYPE_UNUSED_SIZE 6
#define FUNCT_SIZE 5
#define REGISTER_SIZE 5
#define J_REGISTER_SIZE 1
#define OPCODE_SIZE 6
#define IMMEDIATE_SIZE 16
#define ADDRESS_SIZE 25
#define ASCIZ_SIZE 8

/* defaults */
#define OB_FILE_POSTFIX ".ob"
#define ENTRY_FILE_POSTFIX ".ent"
#define EXTERN_FILE_POSTFIX ".ext"
#define OUTPUT_PATH "../output/"

/* prototypes */
bool assemble_code_file(asm_file *asm_f, char *asm_output_name);
char *get_file_name(char *prefix,char *name, char *postfix);
void set_hex_table_title(FILE *asm_output, int ic);
void handle_instruction_by_type(FILE *asm_output,code_line *code, table *symbols,  word_list *externs);
void decode_r_type(FILE *asm_output, code_line *code, bool is_rt);
void decode_i_type(FILE *asm_output, code_line *code);
int word_to_register(char *reg);
bool decode_j_type(FILE *asm_output, code_line *code, table *symbols, word_list *externs);
bool decode_i_branch_type(FILE *asm_output, code_line *code, table *symbols, word_list *externs);
void adjust_data_addresses(code_line **code, table **t);
bool decode_j_stop_type(FILE *asm_output, code_line *code);
void handle_data_instruction_by_type(code_line *code, word_list **data_values);
void print_data_instructions(int ic, FILE *asm_output, word_list *words);
void print_externs(word_list *externs, FILE *extern_file);
bool print_entry_table(table_line *line, FILE *entry_file);
void free_table(table *t);
#endif
