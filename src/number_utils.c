#include "../include/boolean.h"
#include "../include/number_utils.h"
#include <ctype.h>

/* check if a given string is a number as atoi can't distingwish beetween 0 and a none number */
bool is_number(char *word){
    int i = 0;
    bool symbol_flag = false;

    while(word[i] != '\0'){
        if((word[i] == '-' || word[i] == '+') && !symbol_flag){
            symbol_flag = true;
        } else if(!isdigit(word[i])){
            return false;
        }
        i++;
    }

    return true;
}