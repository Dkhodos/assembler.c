#include <stdio.h>
#include "../include/reserved.h"
#include "../include/boolean.h"
#include "../include/words.h"
#include "../include/parser.h"
#include <stdlib.h>
#include <ctype.h>
#include "../include/symbol_table.h"
#include "../include/error_log.h"
#include <string.h>
#include "../include/memory.h"
#include "../include/number_utils.h"

/* 
    this file is in charge of the first run - parse all asm line to actual data
    - save address
    - save operands
    - check for syntax or logical errors
    - save symbols
    - save data from reserved.c

    Algorithem:
    1. read file line by line
    2. check if it a label
        2.1 if it's a label check if it's valid
        2.2 save it if it's valid print error other wise
        2.3 make sure ut's the only label
    3. check if this is a code instruction
        3.1 if it's a code instruction check if it's valid
        3.2 if it's not valid print the right error, other wise save it
    4. check if it's a data instruction
        4.1 if it's a data instruction check if it's valid
        4.2 if it's not valid print the right error, other wise save it
    5. collect extern and entry - in the same loop
 */

asm_file *new_asm(FILE *fp){
    asm_file *asm_f;
    code_line *code;
    char *buffer;
    int count = 1;
    attribute table_attr;

    buffer = (char *) malloc(sizeof(char) * BUFFER_MAX_SIZE);
    if(buffer == NULL){
        allocation_fault("buffer",__FILE__,__LINE__);

        return NULL;
    }

    asm_f = new_asm_file();
    if(asm_f == NULL){
        allocation_fault("buffer",__FILE__,__LINE__);

        return NULL;
    }

    /* loop trough the file line by line*/
    code = asm_f->code_line;
    while( fgets(buffer, MAX_WORD_LENGTH, fp) != NULL ){        
        /* skip spaces */
        while (isspace(buffer[0])){
            buffer++;
        }

        /* it's a comment or and empty line, skip */
        if(buffer[0] == ';' || buffer[0] == '\0'){
            continue;
        }

        code->line_number = count;

        code->line = (char *) malloc(sizeof(char) * MAX_WORD_LENGTH);
        if(code->line == NULL){
            allocation_fault("code->line",__FILE__,__LINE__);

            return NULL;
        }
        strcpy(code->line, buffer);

        /* parse this line to memory */
        if(parse_line(asm_f,buffer, &code)){
            if(code->label != NULL){
                if(code->is_extern){
                    set_extern(&asm_f->symbol_table, code->label, count);
                }else if(code->is_entry){
                    set_entry(&asm_f->symbol_table, code->label);
                } else {
                    /* add new symbol */
                    table_attr = code->label_type == INSTRUCTION_LABEL ? CODE_TABLE_ATTR_CODE : (code->is_extern ? ETERNAL_TABLE_ATTR_CODE : DATA_TABLE_ATTR_CODE);
                    
                    write_line(&asm_f->symbol_table, code->label,code->label_value, table_attr, count);
                }
            }
        }

        code->next_code_line = (code_line *) malloc(sizeof(code_line));
        if(code->next_code_line == NULL){
            allocation_fault("code->next_code_line",__FILE__,__LINE__);

            return NULL;
        }

        code->next_code_line->operands = NULL;
        code->next_code_line->next_code_line = NULL;

        code = code->next_code_line;
        asm_f->line_count++;
        count++;
    }

    return asm_f;
}

/* create a new file struct - this be used to save the instruction to memory before we print them */
asm_file *new_asm_file(){
    asm_file *asm_f;

    asm_f = (asm_file*) malloc(sizeof(asm_file));
    if(asm_f == NULL){
        allocation_fault("asm_f",__FILE__,__LINE__);

        return NULL;
    }

    asm_f->symbol_table = new_table();
    if(asm_f->symbol_table == NULL){
        allocation_fault("asm_f->symbol_table",__FILE__,__LINE__);

        return NULL;
    }

    asm_f->code_line = (code_line *) malloc(sizeof(code_line));
    if(asm_f->code_line == NULL){
        allocation_fault("asm_f->code_line",__FILE__,__LINE__);

        return NULL;
    }

    asm_f->line_count = 1;
    asm_f->code_line->next_code_line = NULL;
    
    return asm_f;
}

/* parse an asm line to memory
    a failure in any step will send false otherwise true
*/
bool parse_line(asm_file *asm_f, char *line, code_line **code){
    word_list *wl, *ptr;
    char *buffer;
    char *label;
    int instruction_index;
    bool label_found = false;
    int expected_operands, actual_operands;
    
    wl = new_words(line, " ");
    if(wl == NULL){
        return false;
    }

    buffer = (char *) malloc(sizeof(char) * MAX_WORD_LENGTH);
    if(buffer == NULL){
        allocation_fault("buffer",__FILE__,__LINE__);

        return false;
    }

    label = (char *) malloc(sizeof(char) * MAX_WORD_LENGTH);
    if(label == NULL){
        allocation_fault("label",__FILE__,__LINE__);

        return false;
    }

    for(ptr = wl; ptr != NULL; ptr = ptr->next_word){
        /* check if this is a label */
        buffer = strchr(ptr->text, ':');
        if(buffer != NULL){
            if(label_found){
                log_error("label already used, invlid syntax",(*code)->line_number);

                return false;
            }

            (*code)->label = (char *) malloc(sizeof(char) * MAX_WORD_LENGTH);

            strcpy(label, ptr->text);
            sscanf(strtok(label, ":"), "%s", buffer);

            if(is_label(buffer)){
                strcpy((*code)->label, buffer);
                label_found = true;
            } else{
                strcpy(label, buffer);
                sprintf(buffer, "illegal label %s",label);

                log_error(buffer,(*code)->line_number);

                return false;
            }


            continue;
        }

        /* could this be an instruction */
        instruction_index = search_instruction(ptr->text);
        if(instruction_index != -1){
            (*code)->label_type = INSTRUCTION_LABEL;
            (*code)->opcode = get_instruction_key(instruction_index, OPCODE_VALUE);
            (*code)->funct = get_instruction_key(instruction_index, OPCODE_FUNC);
            (*code)->instruction_type = get_instruction_key(instruction_index, OPCODE_TYPE);
            (*code)->label_value = get_IC();
            (*code)->address = get_IC();

            advance_IC();

            expected_operands = get_instruction_key(instruction_index, OPCODE_OPERNAD_COUNT);
            actual_operands = read_operands(ptr->next_word, expected_operands,(*code));

            if(expected_operands != actual_operands){
                if((*code) -> instruction_type == TYPE_J_STOP){
                    log_error("stop excepts 0 operands", (*code)->line_number);
                } else {
                    if(buffer == NULL){
                        buffer = (char *) malloc(sizeof(char) * MAX_WORD_LENGTH);
                    }

                    sprintf(buffer,"for instruction '%s' invlid opernad fromat detected in opernad number %d",ptr->text,actual_operands + 1);

                    log_error(buffer, (*code)->line_number);
                }

                return false;
            }

            break; /*we are done with this like, get out of while*/
        }

        /* could this be an data instruction */
        instruction_index = search_data_instruction(ptr->text);
        if(instruction_index != -1){
            (*code)->label_type = DATA_LABEL;
            (*code)->data_instruction_type = get_data_type(instruction_index);

            if((*code)->data_instruction_type == TYPE_EXTERN){
                (*code)->label_value = 0;
                (*code)->is_extern = true;
            } else{
                (*code)->label_value = get_DC();
            }

            if((*code)->data_instruction_type == TYPE_ENTRY){
                (*code)->is_entry= true;
            }

            if((*code)->is_entry || (*code)->is_extern){
                if(!is_valid_extern(ptr->next_word)){
                    log_error("invalid extern/ entry arguments, this must contain a valid label", (*code)->line_number);

                    return false;
                } else{
                    if((*code)->label == NULL){
                        (*code)->label = (char *) malloc(sizeof(char) * MAX_WORD_LENGTH);
                    }

                    strcpy((*code)->label, ptr->next_word->text);

                    return true;
                }
            } else{
                handle_data_by_type(ptr->next_word,(*code),(*code)->data_instruction_type);
            }


            break;
        }

        /*by this point we didn't identify the line - print error*/
        if(buffer == NULL){
            buffer = (char *) malloc(sizeof(char) * MAX_WORD_LENGTH);
            if(buffer == NULL){
                allocation_fault("buffer",__FILE__,__LINE__);
            }
        }

        sprintf(buffer, "invlid syntax unfamiliar with %s in %s",ptr->text, (*code)->line);
        log_error(buffer, (*code)->line_number);

        return false;
    }

    free(buffer);
    free(label);

    return true;
}

/* check ig a given string is a valid label*/
bool is_label(char *word) {
    int i;
    int word_length;
    word_length = strlen(word);

    if(word[word_length-1] == ':') {
        word[word_length-1] = '\0'; /*length-1 in order to deprecate ":"*/
        word_length--;              /*Update word_length accordingly*/
    }
    

    if(word_length == 0) { /*If ':' is the only char*/
        return false;
    }
    
    if(isalpha(word[0]) == 0) {/*If first letter is not alphabetical*/
        return false;
    }

    for(i=0; i<word_length; i++) {
        if(!isalpha(word[i]) && !isdigit(word[i])) {
            return false;
        }
    }
    
    /* cant be a code intruction it's a saved word */
    if(search_instruction(word) > -1) {
        return false;
    }

    /* cant be a data intruction it's a saved word */
    if(search_data_instruction(word) > -1){
        return false;
    }
    
    return true;
}

/* read operands for all instructions
    using a split logic to linked list al tha values regarding the sapces, any small error is a syntax error line:
    - not a valid operand
    - use of illegal symbols
    - ',' is missing

    return the list of well written operands, if there is a mismatch with the expected count there is a symtax error
 */
int read_operands(word_list *word, int expected_count, code_line *code){
    word_list *wl = word;
    char *buffer;
    int count = 0;

    /* if its a stop*/
    if(code->instruction_type == TYPE_J_STOP){
        return word == NULL ? 0 : 1;
    }

    buffer = connet_word(word);
    wl = new_words(buffer,",");
    
    count = check_operands_by_type(wl, code);
    if(count == expected_count){
        code->operands = copy_words(wl);
    }

    free(buffer);
    return count;
}

/* a router to move every case to it;s own handler - 
the handler will parse the operands and send the count of the well written operands */
int check_operands_by_type(word_list *word,code_line *code){
    switch(code->instruction_type){
        case INS_TYPE_NONE:{
            return -1;
        }
        case TYPE_R_COPY:
        case TYPE_R:{
            return check_type_r_operands(word);
        }
        case TYPE_I_STORE:
        case TYPE_I:{
            return check_type_i_operands(word);
        }
        case TYPE_I_BRANCH: {
            return check_type_i_branch_operands(word);
        }
        case TYPE_J:{
            return check_type_j_operands(word);
        }
        case TYPE_J_STOP:{
            return 0;
        }
    } 
    return -1;   
}

/* check r type operands expected pattern is [$reg], [$reg], [$reg] (sapces dont matter)*/
/* return number of well written operands */
int check_type_r_operands(word_list *word){
    word_list *wl = word;
    int good_words =0;

    if(wl == NULL){
        return good_words;
    }

    while (wl != NULL){
        if(is_register(wl->text)){
            good_words++;
        } else{
            return good_words;
        }
        wl = wl->next_word;
    }
    
    return  good_words;
}

/* check i type operands expected pattern is [$reg], [immed], [$reg] (sapces dont matter) */
/* return number of well written operands */
int check_type_i_operands(word_list *word){
    word_list *wl = word;
    int good_words = 0;
    int i = 0;

    if(wl == NULL){
        return good_words;
    }

    while (wl != NULL){
        if( i % 2 == 0){
            if(is_register(wl->text)){
                good_words++;
            } else{
                return good_words;
            }
        } else {
            if(is_number(wl->text)){
                good_words++;
            } else{
                return good_words;
            }
        }
        i++;
        wl = wl->next_word;
    }

    return good_words;
}

/* check i (branch) type operands expected pattern is [$reg], [label or $reg], [$reg] (sapces dont matter) */
/* return number of well written operands */
int check_type_i_branch_operands(word_list *word){
    word_list *wl = word;
    int good_words = 0;
    int i = 0;

    if(wl == NULL){
        return good_words;
    }

    while (wl != NULL){
        if( i != 2){
            if(is_register(wl->text)){
                good_words++;
            } else{
                return good_words;
            }
        } else {
            if(is_label(wl->text)){
                good_words++;
            } else{
                return good_words;
            }
        }
        i++;
        wl = wl->next_word;
    }

    return good_words;
}

/* check j (branch) type operands expected pattern is [label or $reg] (sapces dont matter) */
/* return number of well written operands */
int check_type_j_operands(word_list *word){
    word_list *wl = word;

    /* jump except a single operand no need to loop */
    if(wl == NULL || wl->next_word != NULL){
        return 0;
    }

    if(is_label(wl->text) || is_register(wl->text)){
        return 1;
    }

    return 0;
}

/* check if a given string is a register true / false as a response*/
bool is_register(char *word){
    int number;

    number = atoi(word + 1);

    return word[0] == '$' && is_number(word + 1) && number > -1 && number < 32 ? true: false;
}

/* an data instruction pre memory saveing function, this help collect all the values before ther are sent to be handeled 
    split tha values by ','
*/
bool handle_data_by_type(word_list *word, code_line *code,data_instruction_type type){
    word_list *wl = word;
    char *buffer;
    int status= false;

    buffer = (char *) malloc(sizeof(char) * MAX_WORD_LENGTH);
    if(buffer == NULL){
        allocation_fault("buffer",__FILE__,__LINE__);

        return false;
    }

    if(wl == NULL){
        sprintf(buffer, "for instruction %s no values received", word->text);
        log_error(buffer, code->line_number);
        return false;
    }

    buffer = connet_word(word);
    wl = new_words(buffer,",");

    /* status will tell us if parsing to memory wnt well*/
    status = save_data(wl, code, type);
    if(!status){
        log_error("invalid arguments", code->line_number);
    }

    free(buffer);
    return status;
}

/* save the data instruction to memory, a pre-router using is_valid_by_type to check for errors and save data */
bool save_data(word_list *word, code_line *code,data_instruction_type type){
    word_list *ptr = word;

    while (ptr){
        if(!is_valid_by_type(ptr, type)){
            return false;
        }

        ptr = ptr->next_word;
    }

    code->operands = copy_words(word);

    return true;
}

/* data instruction check and save router - every type has it;s own handler */
bool is_valid_by_type(word_list *word, data_instruction_type type){
    switch(type){
        case TYPE_ASCIZ:{
            return is_asci(word->text);
        }
        case TYPE_BYTE:{
            return is_byte(word->text);
        }
        case TYPE_WORD:{
            return is_word(word->text);
        }
        case TYPE_HALF_WORD:{
            return is_half_word(word->text);
        }
        case DATA_TYPE_NONE:
        case TYPE_EXTERN:
        case TYPE_ENTRY:{
            return false;
        }
    }

    return false;
}

/* check if a given string is a valid asciz line, true /false as a response*/
bool is_asci(char* string){
    int len;

    len = strlen(string);

    return string[0] == '"' && string[len - 1] == '"' ? true : false;
}

/* check if a given string is a valid byte line, true /false as a response */
bool is_byte(char* byte){
    int number;

    number = atoi(byte);

    return is_number(byte) && number > -257 && number < 256;
}

/* check if a given string is a word byte line, true /false as a response */
bool is_word(char* word){
    long number;

    number = atol(word);

    return is_number(word) && number > -4294967297 && number < 4294967296;
}

/* check if a given string is a half word byte line, true /false as a response */
bool is_half_word(char *half){
    long number;

    number = atol(half);

    return is_number(half) && number > -65538 && number < 65537;
}

/* check if a given string is a valid extern, true /false as a response */
bool is_valid_extern(word_list* word){
    int len;

    len = strlen(word->text);

    if(word->text[len - 1] == ':' || word->next_word){
        return false;
    }

    return is_label(word->text);
}

/* free asm struc from memory */
void free_asm_file(asm_file *asm_f){
    free_code_lines(asm_f->code_line);
    free_table(asm_f->symbol_table);
    free(asm_f);
}

/* free asm struc from memory helper - free code lines*/
void free_code_lines(code_line *line){
    code_line *l;
    while (line != NULL){
        l = line->next_code_line;
        free_words(line->operands);
        free(line);
        line = l;
    }
}