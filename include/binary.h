#ifndef BINARY_H
#define BINARY_H

#include "boolean.h"
#include "words.h"

/* defaults binary*/
#define MAX_NUMBER_SIZE 32
#define SEGMENT_SIZE 8
#define MAX_HEX_SIZE 5

/* struct to handle the file formating for code instructions*/
typedef struct Segments{
    char *first;
    char *second;
    char *third;
    char *fourth;
} segments;

/* prototypes*/
segments *get_binary_r_type(int opcode, int rs, int rt, int rd, int funct);
int write_to_r_type_to_array(int **binary, int number, int size, int location);
char *binary_to_hex(int *binary, int n);
int binary_to_decimal(int *binary, int n);
int *dec_to_binary(int n);
void reverse_array(int **arr, int start, int end);
void print_binary(int *binary, int n);
segments *get_binary_i_type(int opcode, int rs, int rt, int immed);
segments *get_binary_j_type(int opcode, int reg, int address);
void set_asciz_data_segments(word_list *data_values, word_list *wl);
char *char_to_hex(char a);
void set_byte_data_segments(word_list *data_values, word_list *wl);
char *byte_to_hex(int n);
void set_half_word_data_segments(word_list *data_values, word_list *wl);
void set_word_data_segments(word_list *data_values, word_list *wl);

#endif 