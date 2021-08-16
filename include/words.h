#ifndef WORDS_H
#define WORDS_H

#include "boolean.h"

/* defaults*/ 
#define MAX_WORD_LENGTH 256

/* a struct of string linked list */
struct WordList{
    char *text; 
    struct WordList *next_word;
};
typedef struct WordList word_list;

/* protypes */
word_list *new_words(char *line, char *split);
void free_words(word_list *word_list);
bool push_word(word_list **wl, char *text);
char *connet_word(word_list *wl);
word_list *copy_words(word_list *word);
int words_length(word_list *word);

#endif