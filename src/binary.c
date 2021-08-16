#include "../include/binary.h"
#include "../include/assembler.h"
#include "../include/reserved.h"
#include "../include/error_log.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

/*
    this file in in charge of translation from decimal values to hex values
    1. from decimal to binary
    2. split into section of 8 and translate back to decimal
    3. translate to hex

    asciz:
    we can use sprintf, so we translate directly to hex
*/

const char *HEX = "0123456789ABCDEF"; /*hex values for translation*/

/* translate r type code instruction to xx xx xx xx hex values */
segments *get_binary_r_type(int opcode, int rs, int rt, int rd, int funct){
    segments *sgm;
    int i = (SEGMENT_SIZE * 4) - 1;
    int *binary;

    sgm = (segments *) malloc(sizeof(segments));
    if(sgm == NULL){
        allocation_fault("sgm",__FILE__,__LINE__);

        return NULL;
    }

    binary = (int *) malloc(sizeof(int) * SEGMENT_SIZE * 4);
    if(binary == NULL){
        allocation_fault("binary",__FILE__,__LINE__);

        return NULL;
    }

    /* translate the r type instruction to binary  */
    i = write_to_r_type_to_array(&binary, 0, R_TYPE_UNUSED_SIZE, i);
    i = write_to_r_type_to_array(&binary, funct, FUNCT_SIZE, i);
    i = write_to_r_type_to_array(&binary, rd, REGISTER_SIZE, i);
    i = write_to_r_type_to_array(&binary, rt, REGISTER_SIZE, i);
    i = write_to_r_type_to_array(&binary, rs, REGISTER_SIZE, i);
    i = write_to_r_type_to_array(&binary, opcode, OPCODE_SIZE, i);

    sgm->first = (char *) malloc(sizeof(char) * MAX_HEX_SIZE);
    sgm->second = (char *) malloc(sizeof(char) * MAX_HEX_SIZE);
    sgm->third = (char *) malloc(sizeof(char) * MAX_HEX_SIZE);
    sgm->fourth = (char *) malloc(sizeof(char) * MAX_HEX_SIZE);
    if(sgm->first == NULL || sgm->second == NULL || sgm->third == NULL || sgm->fourth == NULL){
        allocation_fault("sgm",__FILE__,__LINE__);

        return NULL;
    }

    /* now use sgm and save hex values for easier formating*/
    strcpy(sgm->first, binary_to_hex(binary + 24, 4));
    strcpy(sgm->second, binary_to_hex(binary + 16, 4));
    strcpy(sgm->third, binary_to_hex(binary + 8, 4));
    strcpy(sgm->fourth, binary_to_hex(binary, 4));

    return sgm;
}

/* translate i type code instruction to xx xx xx xx hex values */
segments *get_binary_i_type(int opcode, int rs, int rt, int immed){
    segments *sgm;
    int i = (SEGMENT_SIZE * 4) - 1;
    int *binary;

    sgm = (segments *) malloc(sizeof(segments));
    if(sgm == NULL){
        allocation_fault("sgm",__FILE__,__LINE__);

        return NULL;
    }


    binary = (int *) malloc(sizeof(int) * SEGMENT_SIZE * 4);
    if(binary == NULL){
        allocation_fault("binary",__FILE__,__LINE__);

        return NULL;
    }

    /* translate the i type instruction to binary  */
    i = write_to_r_type_to_array(&binary, immed, IMMEDIATE_SIZE, i);
    i = write_to_r_type_to_array(&binary, rt, REGISTER_SIZE, i);
    i = write_to_r_type_to_array(&binary, rs, REGISTER_SIZE, i);
    i = write_to_r_type_to_array(&binary, opcode, OPCODE_SIZE, i);

    sgm->first = (char *) malloc(sizeof(char) * MAX_HEX_SIZE);
    sgm->second = (char *) malloc(sizeof(char) * MAX_HEX_SIZE);
    sgm->third = (char *) malloc(sizeof(char) * MAX_HEX_SIZE);
    sgm->fourth = (char *) malloc(sizeof(char) * MAX_HEX_SIZE);
    if(sgm->first == NULL || sgm->second == NULL || sgm->third == NULL || sgm->fourth == NULL){
        allocation_fault("sgm",__FILE__,__LINE__);

        return NULL;
    }

    /* now use sgm and save hex values for easier formating*/
    strcpy(sgm->first, binary_to_hex(binary + 24, 4));
    strcpy(sgm->second, binary_to_hex(binary + 16, 4));
    strcpy(sgm->third, binary_to_hex(binary + 8, 4));
    strcpy(sgm->fourth, binary_to_hex(binary, 4));

    return sgm;
}

/* translate j type code instruction to xx xx xx xx hex values */
segments *get_binary_j_type(int opcode, int reg, int address){
    segments *sgm;
    int i = (SEGMENT_SIZE * 4) - 1;
    int *binary;

    sgm = (segments *) malloc(sizeof(segments));
    if(sgm == NULL){
        allocation_fault("sgm",__FILE__,__LINE__);

        return NULL;
    }

    binary = (int *) malloc(sizeof(int) * SEGMENT_SIZE * 4);
    if(binary == NULL){
        allocation_fault("binary",__FILE__,__LINE__);

        return NULL;
    }

    /* translate the j type instruction to binary */
    i = write_to_r_type_to_array(&binary, address, ADDRESS_SIZE, i);
    i = write_to_r_type_to_array(&binary, reg, J_REGISTER_SIZE, i);
    i = write_to_r_type_to_array(&binary, opcode, OPCODE_SIZE, i);

    sgm->first = (char *) malloc(sizeof(char) * MAX_HEX_SIZE);
    sgm->second = (char *) malloc(sizeof(char) * MAX_HEX_SIZE);
    sgm->third = (char *) malloc(sizeof(char) * MAX_HEX_SIZE);
    sgm->fourth = (char *) malloc(sizeof(char) * MAX_HEX_SIZE);
    if(sgm->first == NULL || sgm->second == NULL || sgm->third == NULL || sgm->fourth == NULL){
        allocation_fault("sgm",__FILE__,__LINE__);

        return NULL;
    }

    /* now use sgm and save hex values for easier formating*/
    strcpy(sgm->first, binary_to_hex(binary + 24, 4));
    strcpy(sgm->second, binary_to_hex(binary + 16, 4));
    strcpy(sgm->third, binary_to_hex(binary + 8, 4));
    strcpy(sgm->fourth, binary_to_hex(binary, 4));

    return sgm;
}

/* translate r type code instruction to binary and and to the full instruction array
    return location as the update index of where the commnd will be written
 */
int write_to_r_type_to_array(int **binary, int number, int size, int location){
    int *buffer;
    int j;

    buffer = (int *) malloc(sizeof(int) * MAX_NUMBER_SIZE);
    if(buffer == NULL){
        allocation_fault("buffer",__FILE__,__LINE__);

        return 0;
    }

    buffer = dec_to_binary(number);
    for(j = MAX_NUMBER_SIZE - 1; j > MAX_NUMBER_SIZE - 1 - size; j-- ){
        (*binary)[location--] = buffer[j];
    }

    free(buffer);
    return location;
}

/* take a decimal number and convert in to binary, size of number will be n
    return the new int array
*/
int *dec_to_binary(int n){
    int *binary_num;
    int i;
    unsigned int value  = (unsigned int) n; /* to handle negative numbers */

    binary_num = (int *) malloc(sizeof(int) * MAX_NUMBER_SIZE);
    if(binary_num == NULL){
        allocation_fault("binary_num",__FILE__,__LINE__);

        return NULL;
    }

    for (i = 0; value > 0; i++) {
        binary_num[i] = value % 2;
        value = value / 2;
    }

    /* reverse the array for easier handeling */
    reverse_array(&binary_num,0,MAX_NUMBER_SIZE - 1);
 
    return binary_num;
}

/* take a binary array of ints and translate it to hex char* */
char *binary_to_hex(int *binary, int n){
    int decimal1 = 0, decimal2 = 0;
    char *hex;

    hex = (char *) malloc(sizeof(char) * MAX_HEX_SIZE);
    if(hex == NULL){
        allocation_fault("hex",__FILE__,__LINE__);

        return NULL;
    }

    decimal1 = binary_to_decimal(binary, n); /* translate the first n number of binary digit to decimal*/
    decimal2 = binary_to_decimal(binary + n, n); /* translate the second n number of binary digit to decimal*/

    /* new use global HEX for a straight translations */
    hex[0] = HEX[decimal1];
    hex[1] = HEX[decimal2];

    return hex;
}

/* take a binary array of ints and translate it to a decimal number */
int binary_to_decimal(int *binary, int n){
    int count = 0, i, j = 0;

    for (i = n - 1; i > - 1; i--, j++){
        count += binary[j] * pow(2, i);
    }

    return count;
}

/* a testing function - prints an array of 1s and 0s */
void print_binary(int *binary, int n){
    int i = 0;

    while(i < n){
        printf("%d", binary[i++]);
        if(i % 8 == 0 && i != 0){
            printf(" ");
        }
    }
    printf("\n");
}

/* basic array reverse function - helps with the handeling of the code when the binary number is reversed */
void reverse_array(int **arr, int start, int end){
    int temp;
    while (start < end){
        temp = (*arr)[start];  
        (*arr)[start] = (*arr)[end];
        (*arr)[end] = temp;
        start++;
        end--;
    }  
}  

/* receive a data instruction of asciz and translate it to hex */
void set_asciz_data_segments(word_list *data_values, word_list *wl){
    char *text = wl->text;
    char *hex;

    while(text[0] != '\0'){
        if(text[0] != '"'){ /* ignore the "" they are not saved */
            hex = char_to_hex(text[0]);
            push_word(&data_values, hex);
        }
        text += 1;
    }

    hex = (char *) malloc(sizeof(char) * MAX_HEX_SIZE);
    if(hex == NULL){
        allocation_fault("hex",__FILE__,__LINE__);

        return;
    }

    strcpy(hex, "00"); /* the \0 is also saved */
    push_word(&data_values, hex);
}

/* receive a data instruction of bytes and translate it to hex */
void set_byte_data_segments(word_list *data_values, word_list *wl){
    int number;
    char *hex;

    while (wl != NULL){
        number = atoi(wl->text);

        hex = byte_to_hex(number);
        
        push_word(&data_values, hex);

        wl = wl->next_word;
    }
    
}
/* receive a data instruction of half words and translate it to hex */
void set_half_word_data_segments(word_list *data_values, word_list *wl){
    char *hex;
    int number1, number2;
    int *binary;

    binary = (int *) malloc(sizeof(int) * MAX_NUMBER_SIZE);
    if(binary == NULL){
        allocation_fault("binary",__FILE__,__LINE__);

        return;
    }

    while (wl != NULL){
        binary = dec_to_binary(atoi(wl->text));

        number1 = binary_to_decimal(binary + 16, 8);
        number2 = binary_to_decimal(binary + 24, 8);

        hex = byte_to_hex(number2);
        push_word(&data_values, hex);

        hex = byte_to_hex(number1);
        push_word(&data_values, hex);

        wl = wl->next_word;
    }
}

/* receive a data instruction of words and translate it to hex */
void set_word_data_segments(word_list *data_values, word_list *wl){
    char *hex;
    int *binary;
    int number1, number2, number3, number4;

    binary = (int *) malloc(sizeof(int) * MAX_NUMBER_SIZE);
    if(binary== NULL){
        allocation_fault("binary",__FILE__,__LINE__);

        return;
    }

    while (wl != NULL){
        binary = dec_to_binary(atoi(wl->text));

        number1 = binary_to_decimal(binary, 8);
        number2 = binary_to_decimal(binary + 8, 8);
        number3 = binary_to_decimal(binary, 16);
        number4 = binary_to_decimal(binary + 24, 8);

        hex = byte_to_hex(number4);
        push_word(&data_values, hex);

        hex = byte_to_hex(number3);
        push_word(&data_values, hex);
        
        hex = byte_to_hex(number2);
        push_word(&data_values, hex);

        hex = byte_to_hex(number1);
        push_word(&data_values, hex);

        wl = wl->next_word;
    }
}
/* the a char and use sprintf to translate to a hex value */
char *char_to_hex(char a){
    char *hex;

    hex = (char *) malloc(sizeof(char) * MAX_HEX_SIZE);
    if(hex == NULL){
        allocation_fault("hex",__FILE__,__LINE__);

        return NULL;
    }

    sprintf(hex,"%02X", a);
    
    return hex;
}

/*translate a byte to hex*/
char *byte_to_hex(int n){
    char *hex;
    unsigned long int value = (unsigned int) n;
    long int ceil16 = 1;
    int temp;

    hex = (char *) malloc(sizeof(char) * MAX_NUMBER_SIZE);
    if(hex == NULL){
        allocation_fault("hex",__FILE__,__LINE__);

        return NULL;
    }

    /* saving time as there is an easy way out*/
    if(value < 17){
        hex[0] = '0';
        hex[1] = HEX[value];

        return hex;
    }

    /* find the highest pow of 16 method to translate to hex 
        we only need the last two so this is adjusted to our needs
    */
    while (value > 0){
        while (ceil16 * 16 < value){
            ceil16 = ceil16 * 16;
        }

        temp = value/ceil16;

        if(ceil16 == 1){
            hex[1] = HEX[temp];
        } else{
            hex[0] = HEX[temp];
        }
        value = value - (temp * ceil16);
        ceil16 = 1;
    }

    if(strlen(hex) == 1){
        hex[1] = '0';
    }

    return hex;
}
