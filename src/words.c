#include "../include/words.h"
#include "../include/boolean.h"
#include "../include/error_log.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
    this file is in charge of string linked list
    - creating a new list
    - writing new word
    - connecting words
    - copy words
    - get word count
*/

/* create a new string linked list 
    this works a 'join' connet strings using the split flag
*/
word_list *new_words(char *line, char *split){
    word_list *wl = NULL;
    char *word_buffer;

    word_buffer = (char *) malloc(sizeof(char) * MAX_WORD_LENGTH);
    if(word_buffer == NULL){
        allocation_fault("word_buffer",__FILE__,__LINE__);

        return NULL;
    }

    word_buffer = strtok(line, split);
    while (word_buffer != NULL){
        push_word(&wl, word_buffer);

        word_buffer = strtok(NULL,split);
    }

    return wl;
}

/* free a string linked list from memory*/
void free_words(word_list *wl){
    word_list *tmp;

    if(wl == NULL){
        return;
    }

    while (wl != NULL){
        tmp = wl->next_word;
        free(wl);
        wl = tmp;
    } 
}

/* add a new word to an existing list*/
bool push_word(word_list **wl, char *text){
    word_list *new_word;
    word_list *last = *wl;

    new_word = (word_list *) malloc(sizeof(word_list));
    if(new_word == NULL){
        allocation_fault("new_word",__FILE__,__LINE__);

        return false;
    }

    new_word->next_word = NULL;

    new_word->text = (char *) malloc(sizeof(char) * MAX_WORD_LENGTH);
    if(new_word->text == NULL){
        allocation_fault("new_word->text",__FILE__,__LINE__);

        return false;
    }

    text[strcspn(text, "\n")] = 0;
    
    strcpy(new_word->text, text);

    if(*wl == NULL){
        *wl = new_word;

        return true;
    }

    while (last->next_word != NULL){
        last = last -> next_word;
    }

    last->next_word = new_word;

    return true;
}

/* get a linked list of strings and connect it to a single string */
char *connet_word(word_list *wl){
    char *buffer;
    word_list *ptr;

    buffer = (char *) malloc(sizeof(char) * MAX_WORD_LENGTH);
    if(buffer == NULL){
        allocation_fault("buffer",__FILE__,__LINE__);

        return false;
    }

    ptr = wl;
    while(ptr){
        strcat(buffer, ptr->text);
        ptr = ptr->next_word;
    }

    return buffer;
}

/* create a copy  of a string linked list to prevent overwrites*/
word_list *copy_words(word_list *word){
    word_list *copy = NULL, *ptr;

    if(word == NULL){
        return NULL;
    }

    ptr = word;
    while(ptr != NULL){
        push_word(&copy, ptr->text);
        ptr = ptr->next_word;
    }
    return copy;
}

/* get the length of a linked list of strings*/
int words_length(word_list *word){
    int count = 0;

    while(word != NULL){
        count++;
        word = word->next_word;
    }

    return count;
}