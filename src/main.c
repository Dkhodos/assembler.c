#include <stdio.h>
#include <stdlib.h>
#include "../include/words.h"
#include "../include/parser.h"
#include "../include/error_log.h"
#include "../include/assembler.h"
#include "../include/memory.h"

/*
    Author: Danny Khodos
*/

/*
    This program grabs an asm file and translates it to hex "machine code" file
*/

int main (int argc, char *argv[] ){
    FILE *asm_input;
    asm_file *asm_f;

    int file_counter = 1;

    /* check if files have been added */
    if(argc < 2){
        printf("no files entered, existing... \n");

        exit(0);
    }

    /* loop trough given file locations */
    for(; file_counter < argc; file_counter++){
        asm_input = fopen(argv[file_counter], "r");
        if(asm_input == NULL){
            printf("file %s does not exist, skiping assembler",argv[file_counter]);
            continue;
        }

        printf("Begin parsing file No.%d \n",file_counter);

        /* first run - collect all instruction and save them to memory */
        asm_f = new_asm(asm_input);

        if(has_errors()){
            printf("%d errors detected, files will not be created:\n", get_error_count());
            print_errors();
        } else {
            /* second run - print all machine code to files */
            printf("Begin assembling file No.%d \n",file_counter);
            if(assemble_code_file(asm_f, argv[file_counter])){
                printf("assembling file No.%d completed :)\n\n",file_counter);
            } else{
                printf("%d errors detected, files will not be created:\n", get_error_count());
                print_errors();
            }
        }

        free_logs();
        fclose(asm_input);
        free_asm_file(asm_f);
        reset_DC();
        reset_IC();
    }

    printf("program completed :) \n\n");

    return 0;
}