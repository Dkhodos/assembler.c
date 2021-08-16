#include "../include/reserved.h"
#include "../include/boolean.h"
#include <string.h>

/*
    this file os for system default
    - code instructions
    - data instructions
    - search functions
    - getters
*/

struct Instruction {
    char *opcode_name;
    instruction_type type;    
    int func;
    int code;
    int expect_operands;
};

/* represents all code instructions and their defaults */
static const instruction INSTRUCTIONS[NUM_OF_INSTRUCTIONS] = {
    {"add",  TYPE_R,     1, 0, 3},
    {"sub",  TYPE_R,     2, 0, 3},
    {"and",  TYPE_R,     3, 0, 3},
    {"or",   TYPE_R,      4, 0, 3},
    {"nor",  TYPE_R,      5, 0, 3},
    {"move", TYPE_R_COPY, 1, 1, 2},
    {"mvhi", TYPE_R_COPY, 2, 1, 2},
    {"mvlo", TYPE_R_COPY, 3, 1, 2},
    {"addi", TYPE_I, NO_FUNCT, 10, 3},
    {"subi", TYPE_I, NO_FUNCT, 11, 3},
    {"andi", TYPE_I, NO_FUNCT, 12, 3},
    {"ori",  TYPE_I, NO_FUNCT, 13, 3},
    {"nori", TYPE_I, NO_FUNCT, 14, 3},
    {"bne",  TYPE_I_BRANCH, NO_FUNCT, 15, 3},
    {"beq",  TYPE_I_BRANCH, NO_FUNCT, 16, 3},
    {"blt",  TYPE_I_BRANCH, NO_FUNCT, 17, 3},
    {"bgt",  TYPE_I_BRANCH, NO_FUNCT, 18, 3},
    {"lb",   TYPE_I_STORE, NO_FUNCT, 19, 3},
    {"sb",   TYPE_I_STORE, NO_FUNCT, 20, 3},
    {"lw",   TYPE_I_STORE, NO_FUNCT, 21, 3},
    {"sw",   TYPE_I_STORE, NO_FUNCT, 22, 3},
    {"lh",   TYPE_I_STORE, NO_FUNCT, 23, 3},
    {"jmp",  TYPE_J, NO_FUNCT, 30, 1},
    {"la",   TYPE_J, NO_FUNCT, 31, 1},
    {"call", TYPE_J, NO_FUNCT, 32, 1},
    {"stop", TYPE_J_STOP, NO_FUNCT, 63, 0}
};

struct DataInstruction {
    char *name;
    instruction_type type;    
};

/* represents all data instructions and their defaults */
const data_instruction DATA_INSTRUCTIONS[NUM_OF_DATA_INSTRUCTIONS] = {
    {".db", TYPE_BYTE}, 
    {".dw", TYPE_WORD},
    {".dh", TYPE_HALF_WORD},
    {".asciz", TYPE_ASCIZ},

    {".entry", TYPE_ENTRY},
    {".extern", TYPE_EXTERN}
};

/* search and code instruction by name rerun index if found and -1 otherwise */
int search_instruction(char *word){
    int i =0;

    for(; i < NUM_OF_INSTRUCTIONS ; i++){
        if(strcmp(INSTRUCTIONS[i].opcode_name, word) == 0){
            return i;
        }
    }
    
    return -1;
}

/* getter for a code instruction info*/
int get_instruction_key(int index, opcode_key key){
    if(index >= 0 && index < NUM_OF_INSTRUCTIONS){
        switch(key){
            case OPCODE_TYPE:{
                return INSTRUCTIONS[index].type;
            }
            case OPCODE_VALUE:{
                return INSTRUCTIONS[index].code;
            }
            case OPCODE_OPERNAD_COUNT:{
                return INSTRUCTIONS[index].expect_operands;
            }
            case OPCODE_FUNC:{
                return INSTRUCTIONS[index].func;
            }
        }
    }

    return -1;
}

/* search and data instruction by name rerun index if found and -1 otherwise */
int search_data_instruction(char *word){
    int i = 0;

    for(; i< NUM_OF_DATA_INSTRUCTIONS; i++){
        if(strcmp(word, DATA_INSTRUCTIONS[i].name) == 0){
            return i;
        }
    }

    return - 1;
}

/* getter for a data instruction type*/
data_instruction_type get_data_type(int index){
    return DATA_INSTRUCTIONS[index].type;
}