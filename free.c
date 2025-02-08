/* iaed24 - ist1106059 - project */
/*
 * Ficheiro:  free.c
 * Autor:  Lara Faria ist1106059
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "project.h"
#include "cars.h"
#include "parks.h"

/* Definicao Constantes */
#define TRUE 1
#define FALSE 0


/* Funções free */
/**
 * Liberta todas as informações relativas ao parque do sistema.
 * @param p --> ponteiro para a estrutura do parque a retirar
 * @param sys --> ponteiro para a estrutura do sistema
*/
void free_park(Park *p, System *sys){ 
    for(int i = 0; i < p->capacity - p->available_slots; i++){
        Car *car = find_car(p->cars_in_park[i], sys);
        if(car->movs[car->last_mov_idx]->park == p){
            if(car->movs[car->last_mov_idx]->entry_date != sys->last_date_registe)
                free_date(car->movs[car->last_mov_idx]->entry_date);
            
            switch_carMovs(&car->movs[car->last_mov_idx], &car->movs[car->num_movs]);
            car->movs[car->last_mov_idx]->park = NULL;
            free(car->movs[car->num_movs]);
            car->num_movs--;
            if(car->num_movs >= 0) car->last_mov_idx = car->num_movs;
        }
    }
    free_dayPark(p, sys);
    remove_park_sys(p, sys);    

    free(p->cars_in_park);
    free(p->name);
    free(p);
    sys->num_parks--;
}

/**
 * Remove o parque da linked list de parques do sistema
 * @param p --> ponteiro para a estrutura do parque a retirar
 * @param sys --> ponteiro para a estrutura do sistema
*/
void remove_park_sys(Park *p, System *sys){
    //parque a ser excluido é o primeiro da lista
    if (p->prev == NULL){
        //parque a ser excluido era o unico criado
        if(p->next == NULL){    
            sys->head_p = NULL;
            sys->tail_p = NULL;
        }
        else{       
            sys->head_p = p->next;
            p->next->prev = NULL;
        }
    }
    //parque a ser excluido é o ultimo da lista
    else if(p->next == NULL){   
        p->prev->next = NULL;
        sys->tail_p = p->prev;
    }
    //parque a ser excluido estava no meio da lista
    else{                       
        p->prev->next = p->next;
        p->next->prev = p->prev;
    }
}

/**
 * Liberta a memória das informações de cada estrutura dayPark do 
 * parque a libertar.
 * @param p --> ponteiro para a estrutura do parque a remover
 * @param sys --> ponteiro para a estrutura do sistema
*/
void free_dayPark(Park *p, System *sys){
    for(DayPark *d = p->head_day, *current = d; current!= NULL; d = current){
        for (int i = 0; i < d->num_cars; i++) {
            Car *car = find_car(d->licence_list[i], sys);
            free_park_carMov(car, p, sys);
        } 
        free(d->licence_list);
        free(d->exithours_list);
        free(d->price_list);
        free(d->date);
        current = current->next;
        free(d);
    }
}

/**
 * Remove os movimentos do carro relativos ao parque a remover.
 * @param car --> ponteiro para a estrutura do carro dentro do parque
 * @param par --> ponteiro para a estrutura do parque que queremos remover
 * @param sys --> ponteiro para a estrutura do sistema
*/
void free_park_carMov(Car *car, Park *park, System *sys){
    for(int i = 0; i <= car->num_movs; i++){
        if(car->movs[i]->park == park){
            free_date(car->movs[i]->entry_date);
            if(car->movs[i]->exit_date != sys->last_date_registe)
                free_date(car->movs[i]->exit_date);
            if(i == car->last_mov_idx)
                car->last_mov_idx = car->num_movs;
            else if(car->last_mov_idx == car->num_movs)
                car->last_mov_idx = i;
            switch_carMovs(&car->movs[i], &car->movs[car->num_movs]);
            car->movs[car->num_movs]->park = NULL;
            free(car->movs[car->num_movs]);
            car->num_movs--;
            i--;
        }
    } 
}

/**
 * Liberta a memória dos dados da data.
 * @param date --> ponteiro para a estrutura da data a libertar
*/
void free_date(Date *date){
    free(date->date);
    free(date->hours);
    free(date);
    return;
}

/**
 * Liberta a memória alocada restante na estrutura carro.
 * @param car --> ponteiro para a estrutura carro.
 * @param sys --> ponteiro para a estrutura do sistema
*/
void free_car(Car *car, System *sys){
    if(car->num_movs > -1){
        for(int i = 0; i < car->num_movs; i++){
            free_date(car->movs[i]->entry_date);
            if(car->movs[i]->exit_date != sys->last_date_registe)
                free_date(car->movs[i]->exit_date);
            free_park(car->movs[i]->park, sys);
            free(car->movs[i]);
        }
    }
    free(car->movs);
    free(car->licence);
    free(car);
}

