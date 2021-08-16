#ifndef MEMORY_H
#define MEMORY_H

#define DEFAULT_IC_VALUE 100
#define DEFAULT_DC_VALUE 0
#define DEFAULT_IC_ADVANCE 4

int get_IC();
int get_DC();
int advance_IC();
int set_DC(int n);
int reset_IC();
int reset_DC();
int advance_IC_by(int n);

#endif