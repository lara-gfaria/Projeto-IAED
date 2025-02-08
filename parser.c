/* iaed24 - ist1106059 - project */
/*
 * Ficheiro:  parser.c
 * Autor:  Lara Faria ist1106059
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "parser.h"
#include "project.h"
#include "cars.h"
#include "parks.h"

/* Definicao Constantes */
#define TRUE 1
#define FALSE 0

/*  --- Funções parser ---  */
/* Lê o input devolvendo uma string lida e retirando-a do input*/
char* read_string(char *input) {
    int str_len = 0;
    //caso a string esteja delimitada por aspas
    if(input[str_len] != '"'){
        while(input[str_len] != ' ' && input[str_len] != '\0')
            str_len++;
    }
    else{
        memmove(input, input + 1, strlen(input));
        while(input[str_len] != '"')
            str_len++;
    }
    
    char *str = (char *)malloc((str_len + 1) * sizeof(char));
    if(str == NULL){
        printf("Error allocating memory.\n");
        return NULL;
    }
    
    strncpy(str, input, str_len);

    if(input[str_len] == '\0')
        str[str_len - 1] = '\0';
    else 
        str[str_len] = '\0';
    

    if(input[str_len] == ' ')
        memmove(input, input + str_len + 1, strlen(input) - str_len);
    else if(input[str_len] == '"')
        memmove(input, input + str_len + 2, strlen(input) - str_len);
    else
        input[0] = '\0';
    
    return str;
}

/* Lê uma string do input até ao espaço e passa para inteiro */
int read_int(char *input) {
    char *str_int = read_string(input);
    int num = atoi(str_int);
    free(str_int);
    return num;
}

/* Lê uma string do input até ao espaço e passa para float */
float read_float(char *input){
    char *str_float = read_string(input);
    float num = atof(str_float);
    free(str_float);
    return num;
}

/* Lê o nome do parque, devolve-o se existir ou NULL caso contrário */
Park *read_park_name(char *input, System *sys){
    //lê o nome do parque, procura-o e verifica que existe
    char *park_name = read_string(input);
    Park *p = find_park(park_name, sys);
    if(p == NULL){
        printf("%s: no such parking.\n", park_name);
        free(park_name);
        return NULL;
    }
    free(park_name);
    return p;
}