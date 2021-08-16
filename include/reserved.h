#ifndef RESERVED_H
#define RESERVED_H

#include "boolean.h"

/* defaults */
#define NUM_OF_INSTRUCTIONS 26
#define NUM_OF_DATA_INSTRUCTIONS 6
#define NO_FUNCT -1

typedef struct Instruction instruction;
typedef struct DataInstruction data_instruction;

/* all types and enums  */
typedef enum {INS_TYPE_NONE,TYPE_R,TYPE_R_COPY,TYPE_I, TYPE_I_BRANCH,TYPE_I_STORE,TYPE_J, TYPE_J_STOP} instruction_type;
typedef enum {DATA_TYPE_NONE,TYPE_ASCIZ, TYPE_BYTE, TYPE_WORD, TYPE_HALF_WORD, TYPE_ENTRY, TYPE_EXTERN} data_instruction_type;
typedef enum {OPCODE_TYPE, OPCODE_FUNC, OPCODE_VALUE, OPCODE_OPERNAD_COUNT} opcode_key;
typedef enum {DATA_KEY_NAME, DATA_KEY_TYPE} data_key;

/* prototypes */
int search_instruction(char *word);
int get_instruction_key(int index, opcode_key key);
data_instruction_type get_data_type(int index);
int search_data_instruction(char *word);

#endif