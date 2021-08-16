#ifndef ERROR_H
#define ERROR_H

typedef struct ErrorLog error_log;
typedef struct ErrorLogs error_logs;

#include "boolean.h"

bool new_error_logs();
bool log_error(char *message, int line_number);
bool push_error_log(char *msg, int line_number);
error_log *get_last_error_log();
void free_logs();
bool has_errors();
int get_error_count();
void print_errors();
void allocation_fault(char *variable_name, char *file_name, int line);

#endif