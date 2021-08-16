#include "../include/assembler.h"
#include "../include/parser.h"
#include "../include/memory.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../include/binary.h"
#include "../include/symbol_table.h"
#include "../include/boolean.h"
#include "../include/error_log.h"

/*
    this file is in charse of phase 2 - second read
    1. translate all data to hex
    2. print to files
    3. delete unended files

    algorithm:
    1. create new files based on the origin file name
    2. loop trough all instruction and update the data instruction address, also update symbol table
    3. print headers
    4. translate all code instruction to hex and print to file, while that heppeds collect all data instruction as hex
    5. print data instruction to file
    6. print entry table - if nothing to print delete file
    7. print extern table - if nothing to print delete file
    8. remove all file if error are collected
*/


/* main function follows the algorithm above - return true if all is good and false if errors occurred */
bool assemble_code_file(asm_file *asm_f, char *asm_output_name){
    FILE *asm_output, *extern_output, *entry_output;
    code_line *code;
    word_list *data_values, *externs;
    bool entry_printed = false, extern_printed = false;
    char *asm_file_name, *extern_file_name, *entry_file_name;
    int ic_before_data = get_IC() - 4; /* save IC before it advances for the data instructions */

    asm_file_name = get_file_name(OUTPUT_PATH,asm_output_name, OB_FILE_POSTFIX);
    asm_output = fopen (asm_file_name, "w");
    if(asm_output == NULL){
        printf("could not open new file %s, exit program\n", asm_file_name);

        return false;
    }

    extern_file_name = get_file_name(OUTPUT_PATH,asm_output_name, EXTERN_FILE_POSTFIX);
    extern_output = fopen (extern_file_name, "w");
    if(extern_output == NULL){
        printf("could not open new file %s, exit program\n", extern_file_name);

        return false;
    }

    entry_file_name = get_file_name(OUTPUT_PATH,asm_output_name, ENTRY_FILE_POSTFIX);
    entry_output = fopen (entry_file_name, "w");
    if(entry_output == NULL){
        printf("could not open new file %s, exit program\n", entry_file_name);

        return false;
    }

    data_values = (word_list *) malloc(sizeof(word_list));
    if(data_values == NULL){
        allocation_fault("data_values",__FILE__,__LINE__);

        return false;
    }

    externs = (word_list *) malloc(sizeof(word_list));
    if(externs == NULL){
        allocation_fault("externs",__FILE__,__LINE__);

        return false;
    }

    code = asm_f->code_line;

    /* fix data instruction addresses */
    adjust_data_addresses(&code, &(asm_f->symbol_table));

    /* set file top data */
    set_hex_table_title(asm_output, ic_before_data + 4);

    /* print instructions to file, save data instruction for later*/
    while(code != NULL){
        if(code->instruction_type != INS_TYPE_NONE){
            handle_instruction_by_type(asm_output, code, asm_f->symbol_table, externs);
        } else if(code->data_instruction_type != DATA_TYPE_NONE){
            handle_data_instruction_by_type(code, &data_values);
        }

        code = code->next_code_line;
    }

    /* print data instructions */
    if(words_length(data_values) > 0){
        print_data_instructions(ic_before_data + 4, asm_output, data_values);
    }

    /* print extern table */
    if(words_length(externs) > 0){
        print_externs(externs, extern_output);
        extern_printed = true;
    }

    /* print entry table */
    if(asm_f->symbol_table != NULL && print_entry_table(asm_f->symbol_table->line, entry_output)){
        entry_printed = true;
    }

    /* close all files */
    fclose(asm_output);
    fclose(extern_output);
    fclose(entry_output);

    if(!entry_printed){
        printf("entry table not needed\n");
        remove(entry_file_name);
    }

    if(!extern_printed){
        printf("extern table not needed\n");
        remove(extern_file_name);
    }

    /* check for errors */
    if(has_errors()){
        remove(asm_file_name);
        remove(entry_file_name);
        remove(extern_file_name);
        return false;
    }

    return true;
}

/* pasrse out the file name and add desires predix and postfix
    return the new file name
*/
char *get_file_name(char *prefix,char *name, char *postfix){
    char *file_name;
    char *name_copy;
    word_list *wl;

    name_copy = (char *) malloc(sizeof(char) * MAX_WORD_LENGTH);
    if(name_copy == NULL){
        allocation_fault("name_copy",__FILE__,__LINE__);

        return NULL;
    }
    strcpy(name_copy,name);

    wl = new_words(name_copy, "/");
    while(wl->next_word != NULL){
        wl = wl->next_word;
    }

    wl = new_words(wl->text, ".");

    file_name = (char *) malloc(sizeof(char) * MAX_WORD_LENGTH);
    if(file_name == NULL){
        allocation_fault("file_name",__FILE__,__LINE__);

        return NULL;
    }

    strcpy(file_name, prefix);
    strcat(file_name, wl->text);
    strcat(file_name, postfix);

    free(name_copy);
    free_words(wl);

    return file_name;
}

/* second run that means we need to fix the addresses of all data instruction to the end of the file 
   every data structure advances the IC differently
*/
void adjust_data_addresses(code_line **code, table **t){
    code_line *ptr = (*code);
    int advance = 0;

    while(ptr != NULL){
        if(ptr->data_instruction_type != DATA_TYPE_NONE){
            switch(ptr->data_instruction_type){
                case DATA_TYPE_NONE:
                case TYPE_EXTERN:
                case TYPE_ENTRY: {
                    break;
                }
                case TYPE_ASCIZ:{
                    advance = (strlen(ptr->operands->text) - 1);
                    break;
                }
                case TYPE_WORD:{
                    advance = words_length(ptr->operands) * 4;
                    break;
                }
                case TYPE_HALF_WORD:{
                    advance = words_length(ptr->operands) * 2;
                    break;
                }
                case TYPE_BYTE:{
                    advance = words_length(ptr->operands);
                    break;
                }
           }
           if(advance > 0){
               ptr->address = get_IC();
               if(ptr->label){
                   update_value((*t),ptr->label, get_IC());
                }
            }

           advance_IC_by(advance);
           advance = 0;
       }
       
       ptr = ptr->next_code_line;
    }

    set_DC(get_IC());
}

/* set .ob file  top titles */
void set_hex_table_title(FILE *asm_output, int ic){
    fprintf(asm_output,"     %02d %02d     \n",ic - 100, get_DC() - ic);
}

/* print to file the code instructions, every code instruction has a bit of a diffrent handlings, route by type to the right handler */
void handle_instruction_by_type(FILE *asm_output,code_line *code, table *symbols, word_list *externs){   
    switch(code->instruction_type){
        case INS_TYPE_NONE: {
            break;
        }
        case TYPE_R:{
            decode_r_type(asm_output, code, true);
            break;
        }
        case TYPE_R_COPY:{
            decode_r_type(asm_output, code, false);
            break;
        }
        case TYPE_I_STORE:
        case TYPE_I: {
            decode_i_type(asm_output, code);
            break;
        }
        case TYPE_J:{
            decode_j_type(asm_output, code, symbols, externs);
            break;
        }
        case TYPE_I_BRANCH:{
            decode_i_branch_type(asm_output, code, symbols, externs);
            break;
        }
        case TYPE_J_STOP: {
            decode_j_stop_type(asm_output,code);
        }
    }
}

/* save data instruction as an hex linked list to print afterwords, every data instruction has a bit of a diffrent handlings,
 route by type to the right handler */
void handle_data_instruction_by_type(code_line *code, word_list **data_values){
    switch(code->data_instruction_type){
        case TYPE_ENTRY:
        case TYPE_EXTERN:
        case DATA_TYPE_NONE:{
            break;
        }
        case TYPE_ASCIZ:{
            set_asciz_data_segments((*data_values), code->operands);
            break;
        }
        case TYPE_BYTE:{
            set_byte_data_segments((*data_values), code->operands);
            break;
        }
        case TYPE_HALF_WORD:{
            set_half_word_data_segments((*data_values), code->operands);
            break;
        }

        case TYPE_WORD: {
            set_word_data_segments((*data_values), code->operands);
        }
    }
}

/* decode r type instructions and r_move instruction */
void decode_r_type(FILE *asm_output, code_line *code, bool is_rt){
    segments *sgm;
    int rs,rd,rt;

    if(is_rt){
        rs = word_to_register(code->operands->text);
        rt = word_to_register(code->operands->next_word->text);
        rd = word_to_register(code->operands->next_word->next_word->text);
    } else{
        rs = word_to_register(code->operands->text);
        rd = word_to_register(code->operands->next_word->text);
        rt = 0;
    }

    sgm = get_binary_r_type(code->opcode,rs, rt, rd, code->funct);

    fprintf(asm_output,"%04d %s %s %s %s\n",code->address, sgm->first,sgm->second,sgm->third,sgm->fourth);
}

/* decode i type instructions*/
void decode_i_type(FILE *asm_output, code_line *code){
    segments *sgm;
    int rs,rt,immed;

    rs = word_to_register(code->operands->text);
    immed = atoi(code->operands->next_word->text);
    rt = word_to_register(code->operands->next_word->next_word->text);

    sgm = get_binary_i_type(code->opcode, rs, rt, immed);

    fprintf(asm_output,"%04d %s %s %s %s\n",code->address, sgm->first,sgm->second,sgm->third,sgm->fourth);
}

/* decode j type instructions, might be a label that doesn't exist so log it if needed */
bool decode_j_type(FILE *asm_output, code_line *code, table *symbols, word_list *externs){
    segments *sgm;
    table_line *line;
    int reg, address;
    char *buffer;

    if(is_register(code->operands->text)){
        reg = 1;
        address = word_to_register(code->operands->text);
    } else {
        line = search(symbols, code->operands->text);
        if(line == NULL){
            buffer = (char *) malloc (sizeof(char) * MAX_WORD_LENGTH);
            if(buffer == NULL){
                allocation_fault("buffer",__FILE__,__LINE__);
            }
            sprintf(buffer,"label %s is reference but it is undefined",code->operands->text);
            log_error(buffer, code->line_number);

            return false;
        }

        reg = 0;
        address = line->value;

        if(line->isExtern){
            buffer = (char *) malloc (sizeof(char) * MAX_WORD_LENGTH);
            if(buffer == NULL){
                allocation_fault("buffer",__FILE__,__LINE__);
            }
            sprintf(buffer,"%s %04d", line->label, code->address);
            push_word(&externs, buffer);
        }
    }

    sgm = get_binary_j_type(code->opcode, reg, address);

    fprintf(asm_output,"%04d %s %s %s %s\n",code->address, sgm->first,sgm->second,sgm->third,sgm->fourth);

    return true;
}

/* decode j stop type instructions, doesn't need ant operand hendeling */
bool decode_j_stop_type(FILE *asm_output, code_line *code){
    segments *sgm;

    sgm = get_binary_j_type(code->opcode, 0, 0);

    fprintf(asm_output,"%04d %s %s %s %s\n",code->address, sgm->first,sgm->second,sgm->third,sgm->fourth);

    return true;
}

/* decode i type branch instructions, might be a label that doesn't exist so log it if needed */
bool decode_i_branch_type(FILE *asm_output, code_line *code, table *symbols, word_list *externs){
    segments *sgm;
    table_line *line;
    int rs, rt, address;
    char *buffer;

    rs = word_to_register(code->operands->text);
    rt = word_to_register(code->operands->next_word->text);

    line = search(symbols, code->operands->next_word->next_word->text);
    if(line == NULL){
        buffer = (char *) malloc (sizeof(char) * MAX_WORD_LENGTH);
        if(buffer == NULL){
            allocation_fault("buffer",__FILE__,__LINE__);
        }

        sprintf(buffer,"label %s is reference but it is undefined",code->operands->text);
        log_error(buffer, code->line_number);

        return false;
    }

    address = (line->value) - code->address;
    address += address > 0 ? 4 : 0;

    sgm = get_binary_i_type(code->opcode, rs, rt ,address);

    fprintf(asm_output,"%04d %s %s %s %s\n",code->address, sgm->first,sgm->second,sgm->third,sgm->fourth);

    return true;
}

/* translate the register to it's value */
int word_to_register(char *reg){
    return atoi(reg + 1);
}

/* data instruction were collected while we printed code instruction, now print data instructions */
void print_data_instructions(int ic, FILE *asm_output, word_list *words){
    int i = 0;
    while (words != NULL){
        if(words->text && strcmp(words->text,"") != 0){
            if (i % 4 == 0){
                fprintf(asm_output,"%04d",ic);
            }

            fprintf(asm_output, " %s",words->text);

            if(i % 4 == 3){
                fprintf(asm_output, "\n");
                ic += 4;
            }
            i++;
        }
        words = words->next_word;
    }
}

/* print externs - this will only be called if the exist */
void print_externs(word_list *externs, FILE *extern_file){
    while(externs != NULL){
        if(externs->text){
            fprintf(extern_file, "%s\n",externs->text);
        }
        externs = externs->next_word;
    }
}

/* print entries, if nothing was printer return false so the file will be deleted */
bool print_entry_table(table_line *line, FILE *entry_file){
    int count = 0;
    char *buffer;

    while (line != NULL){
        /* might be an entry to a label that does not exist*/
        if(line->isEntry && line->line_index == EMPTY_ENTRY){
            buffer = (char *) malloc(sizeof(char) * MAX_WORD_LENGTH);
            if(buffer == NULL){
                allocation_fault("buffer",__FILE__,__LINE__);
            }

            sprintf(buffer,"label %s set as entry but it's not in the file",line->label);
            log_error(buffer, 0);

            return false;
        }

        if(line->isEntry){
            fprintf(entry_file, "%s %04d\n",line->label,line->value);
            count++;
        }
        line = line->next_line;
    }
    
    return count > 0 ? true : false;
}