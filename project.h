/* iaed24 - ist1106059 - project */

#ifndef PROJECT_H_
#define PROJECT_H_

/* Definicao Constantes */
#define TABLE_SIZE 997

/* ---- Estruturas ---- */
typedef struct date{
    int day;
    int month;
    int year;
    int hour;
    int mins;
    char *date;
    char *hours;
} Date;

typedef struct dayPark{
    char *date;
    char **licence_list;
    char **exithours_list;
    float *price_list;
    int num_cars;
    int list_capacity;
    float daily_price;
    struct dayPark *next;
} DayPark;

typedef struct park{
    char *name;
    int capacity;
    int available_slots;
    float value_15;
    float value_15_after_1hour;
    float value_max_day;
    char **cars_in_park;
    DayPark *head_day, *tail_day;
    struct park *prev, *next;    
} Park;

typedef struct carMovs{
    Park *park;
    Date *entry_date;
    Date *exit_date;
    float price;
} CarMovs;

typedef struct car{
    char *licence;
    CarMovs **movs;
    int last_mov_idx;
    int movs_capacity;
    int num_movs;
    struct car *next;                   /*Carro depois do atual na linha da hash table*/
} Car;

typedef struct{
    Park *head_p, *tail_p;              /*Cabeça e cauda da lista de parques*/
    int num_parks;
    Date *last_date_registe;            /*último dia de que há registos*/
    Car *hash_table[TABLE_SIZE];
    int num_cars;
} System;


/* Funções Sistema */
void read_command(char command, char input[], System *sys);
void free_system(System *sys);


#endif
