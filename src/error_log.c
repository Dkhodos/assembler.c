#include "../include/error_log.h"
#include "../include/boolean.h"
#include "../include/words.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct ErrorLog {
    char *msg;
    int line;
    error_log *next_err;
};

struct ErrorLogs{
    error_log *errs;
    int length;
};

/* global error logger */
static error_logs *ERROR_LOGS;

/* create a new logger - only needed if there are actual errors
    return true if allocation worked and false otherwise
*/

bool new_error_logs(){
    ERROR_LOGS = (error_logs *) malloc(sizeof(error_logs));
    if(ERROR_LOGS == NULL){
        allocation_fault("ERROR_LOGS", __FILE__, __LINE__);

        return false;
    }

    ERROR_LOGS->length = 0;
    ERROR_LOGS->errs = NULL;

    return true;
}

/* log a new message to the global error log 
    return true if allocation worked and message added and false otherwise
*/
bool log_error(char *message, int line_number){
    if(ERROR_LOGS == NULL && !new_error_logs()){
        return false;
    }

    if(push_error_log(message, line_number)){
        ERROR_LOGS->length++;

        return true;
    }

    return false;
}

/* push a new error to the global logger 
return true if allocation worked and error added and false otherwise
*/

bool push_error_log(char *msg, int line_number){
    error_log *new_error;
    error_log *last = ERROR_LOGS->errs; 

    new_error = (error_log *) malloc(sizeof(error_log));
    if(new_error == NULL){
        return false;
    }
    new_error->next_err = NULL;

    new_error->msg = (char *) malloc(sizeof(char) * MAX_WORD_LENGTH);
    if(new_error->msg == NULL){
        return false;
    }

    strcpy(new_error->msg, msg);
    new_error->line = line_number;

    if(ERROR_LOGS->errs == NULL){
        ERROR_LOGS->errs = new_error;
        return true;
    }

    while (last->next_err != NULL){
        last = last -> next_err;
    }

    last->next_err = new_error;
    return true;
}

/* clean logs from memory */
void free_logs(){
    error_log *tmp;

    if(ERROR_LOGS == NULL || ERROR_LOGS->errs == NULL){
        return;
    }

    tmp = ERROR_LOGS->errs;
    while (ERROR_LOGS->errs != NULL){
        tmp = ERROR_LOGS->errs->next_err;
        free(ERROR_LOGS->errs);
        ERROR_LOGS->errs = tmp;
    }

    free(ERROR_LOGS);
}

/* return if global object has any errors 
    true there are messages and false if it's 0 or NULL
*/
bool has_errors(){
    if(ERROR_LOGS == NULL){
        return false;
    }

    return ERROR_LOGS->length > 0 ? true : false;
}

/* error count getter, NULL will return 0 */
int get_error_count(){
    if(ERROR_LOGS == NULL){
        return false;
    }

    return ERROR_LOGS->length;
}

/* print errors for the global log */
void print_errors(){
    error_log *ptr;

    if(ERROR_LOGS == NULL){
        return;
    }

    ptr = ERROR_LOGS->errs;
    while(ptr != NULL){
        printf("in line %d, %s\n",ptr->line,ptr->msg);
        ptr = ptr->next_err;
    }
}

/* a function that handle allocation fault, as we don't have any retires and every allocation is needed exit the progarm if this is called*/
void allocation_fault(char *variable_name, char *file_name, int line){
    printf("memory allocation fault, info: \n");
    printf("# variable name: %s\n# line: %d\n# file_name: %s\n\n",variable_name, line, file_name);

    exit(0);
}