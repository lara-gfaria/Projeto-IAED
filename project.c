/* iaed24 - ist1106059 - project */
/*
 * Ficheiro:  project.c
 * Autor:  Lara Faria ist1106059
 * Descrição: Um programa que lê um comando dado no input e define parques e o 
 * registo de entradas e saídas de veículos, assim como a sua consulta e 
 * facturação.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "project.h"
#include "cars.h"
#include "parks.h"
#include "free.h"

/* Definicao Constantes */
#define TRUE 1
#define FALSE 0


/*  --- Função main ---  */
int main(){
    char command;
    char input[BUFSIZ];
    //cria o sistema e verifica a alocação de memória
    System *sys = (System *)malloc(sizeof(System));
    if(sys == NULL){
        printf("Erro alocating memory.\n");
        return 1;
    }                      

    sys->num_parks = 0;
    sys->num_cars = 0;
    sys->last_date_registe = NULL;
    sys->head_p = NULL;
    sys->tail_p = NULL;
    init_hash_table(sys);
    
    fgets(input, sizeof(input), stdin);
    command = input[0];

    while(command != 'q'){
        read_command(command, input, sys);
        fgets(input, sizeof(input), stdin);
        command = input[0];
    }

    free_system(sys);

    return 0;
}

/* Atribui respetivo comando */
void read_command(char command, char *input, System *sys){
    input += 2;
    switch (command) {
        case 'q':
            break;
        case 'p':
            command_p(input, sys);
            break;
        case 'e':
            command_e(input, sys);
            break;
        case 's':
            command_s(input, sys);
            break;
        case 'v':
            command_v(input, sys);
            break;
        case 'f':
            command_f(input, sys);
            break;
        case 'r':
            command_r(input, sys);
            break;
        default:
            break;
    }
}

/* Dá free ao sistema */
void free_system(System *sys){
    for(Park *p = sys->head_p; p != NULL; p = sys->head_p){
        free_park(p, sys);
    }
    sys->head_p = NULL;
    sys->tail_p = NULL;
    

    if(sys->num_cars != 0){
        for(int i = 0; i < TABLE_SIZE; i++){
            if(sys->hash_table[i] != NULL){
                Car *c = sys->hash_table[i];
                while(c != NULL){
                    Car *aux = c;
                    c = aux->next;
                    free_car(aux, sys);
                }
            }
        }
    }
    
    if(sys->last_date_registe != NULL)
        free_date(sys->last_date_registe);
    free(sys);
}
