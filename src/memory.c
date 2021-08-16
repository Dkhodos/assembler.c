#include "../include/memory.h"

/*
    this file keeps track of the IC and DC of the program
*/

int IC = DEFAULT_IC_VALUE;
int DC = DEFAULT_DC_VALUE;

int get_IC(){
    return IC;
}

int get_DC(){
    return DC;
}

int advance_IC(){
    IC += DEFAULT_IC_ADVANCE;

    return IC;
}
int advance_IC_by(int n){
    IC += n;

    return IC;
}

int set_DC(int n){
    DC = n;

    return DC;
}

int reset_IC(){
    IC = DEFAULT_IC_VALUE;

    return IC;
}

int reset_DC(){
    DC = DEFAULT_DC_VALUE;

    return DC;
}