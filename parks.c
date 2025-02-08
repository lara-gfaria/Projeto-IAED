/* iaed24 - ist1106059 - project */
/*
 * Ficheiro:  parks.c
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
#include "free.h"

/* Definicao Constantes */
#define TRUE 1
#define FALSE 0


/*  --- Funções comando p ---  */
/**
 * Verifica se o input é vazio ou não, direcionando para a função
 * correspondente.
 * @param input --> ponteiro para o input recebido
 * @param sys   --> ponteiro para estrutura do sistema
 *  
*/
void command_p(char *input, System *sys){
    //se o input for só o comando p
    if(strlen(input) > 1)              
        create_parks(input, sys);
    else
        list_parks(sys);
}

/**
 * Cria um parque, lendo o resto do input para definir os parâmetros do mesmo     
 * @param input --> ponteiro para o input recebido
 * @param sys   --> ponteiro para estrutura do sistema
*/
void create_parks(char *input, System *sys){
    //cria o novo parque e verifica a alocação de memória
    Park *park = (Park *)malloc(sizeof(Park));
    if(park == NULL) {
        printf("Erro allocating memory.\n");
        return;
    }

    //verifica se o número de parques não excede o máximo
    if(sys->num_parks == 20){
        printf("too many parks.\n");
        free(park);
        return;
    }

    if(read_param_park(input, park, sys) == FALSE){
        free(park->cars_in_park);
        free(park->name);
        free(park);
        return;
    }

    if(read_park_prices(input, park) == FALSE){
        printf("invalid cost.\n");
        free(park->cars_in_park);
        free(park->name);
        free(park);
        return;
    }
    
    change_park_list_sys(park, sys);
}

/**
 * Lê do input o nome e capacidade do parque, atualizando esses parâmetros
 * na estrutura do parque recebido. Retorna FALSE caso algum parâmetro não seja
 * válido.
 * @param input --> ponteiro para o input recebido
 * @param park  --> ponteiro para o parque criado
 * @param sys   --> ponteiro para estrutura do sistema
 * @return TRUE ou FALSE
*/
int read_param_park(char *input, Park *park, System *sys){
    //lê o nome do parque e verifica se já existe
    park->name = read_string(input);
    for(Park *p = sys->head_p; p != NULL; p = p->next){
        if(strcmp(p->name, park->name) == 0){
            printf("%s: parking already exists.\n", park->name);
            return FALSE;
        }
    }

    //lê a capacidade do parque
    park->capacity = read_int(input);
    if(park->capacity <= 0){
        printf("%d: invalid capacity.\n", park->capacity);
        return FALSE;
    }
    //coloca os lugares disponíveis igual à capacidade
    park->available_slots = park->capacity;

    //inicializa lista de carros no parque e verifica alocação de memória
    park->cars_in_park = (char**)calloc(1, park->capacity * sizeof(char*));
    if(park->cars_in_park == NULL){
        printf("Erro alocating memory.\n");
        return FALSE;
    }
    return TRUE;
}

/**
 * Lê do input os valores dos preços dos vários intervalos de tempo de
 * permanência no parque. Retorna FALSE caso algum preço não seja válido
 * @param input --> ponteiro para o input recebido
 * @param park --> ponteiro para o parque criado
 * @return TRUE ou FALSE
*/
int read_park_prices(char *input, Park *park){
    //lê o valor para 15min durante a primeira hora
    park->value_15 = read_float(input);    

    //lê o valor de 15min após a primeira hora, verificando a ordem crescente
    park->value_15_after_1hour = read_float(input);  

    //lê o valor máximo diário, verificando a ordem crescente
    park->value_max_day = read_float(input); 

    if(park->value_15 <= 0 || park->value_15_after_1hour <= 0 
    || park->value_15_after_1hour < park->value_15 || park->value_max_day <= 0 
    || park->value_max_day < park->value_15_after_1hour
    || park->value_max_day < park->value_15 ){
        return FALSE;
    }
    return TRUE;
}

/**
 * Atualiza a linked list dos parques na estrutura sistema, adicionado 
 * o novo parque.
 * @param park --> ponteiro para o parque criado
 * @param sys --> ponteiro para a estrutura do sistema
*/
void change_park_list_sys(Park *park, System *sys){
    //coloca o proximo parque a NULL, o atual é o ultimo da lista
    park->next = NULL;     

    //se não existe nenhum parque no sistema, coloca este como primeiro
    if(sys->head_p == NULL){                    
        sys->head_p = park;
        sys->tail_p = park;
        park->prev = NULL;
    }
    //caso contrario atualiza este parque para ultimo lugar da lista
    else{                                      
        park->prev = sys->tail_p;
        sys->tail_p->next = park;
        sys->tail_p = park;
    }

    park->head_day = NULL;
    park->tail_day = NULL;

    sys->num_parks++;
}

/**
 * Lista os parques existentes no sistema, apresentando o seu nome,
 * capacidade e lugares disponíveis.
 * @param sys   --> ponteiro para estrutura do sistema
*/
void list_parks(System *sys){
    for(Park *p = sys->head_p; p != NULL; p = p->next){
        printf("%s %d %d\n", p->name, p->capacity, p->available_slots);
    }
}

/**
 * Procura um determinado parque no sistema através do seu nome, 
 * retornando-o se encontrar ou NULL caso contrário.
 * @param park_name --> nome do parque a procurar
 * @param sys --> ponteiro para estrutura do sistema
 * @return p --> ponteiro para o parque com o nome recebido
*/
Park *find_park(char *park_name, System *sys){
    for(Park *p = sys->head_p; p != NULL; p = p->next){
        if(strcmp(p->name, park_name) == 0){
            return p;
        }
    }
    return NULL;
}



/* Funções DayPark */
/**
 * Cria uma estrutura dayPark para guardar as informações das saídas do parque 
 * num determinado dia. Retorna NULL caso não tenha sido uma criação bem sucedida.
 * @param date --> ponteiro para estrutura da data de saída
 * @param park --> ponteiro para estrutura do parque em questão
 * @return dayPark --> retorna o dayPark criado
*/
DayPark *create_dayPark(Date *date, Park *park){
    DayPark *dayPark = (DayPark *)malloc(sizeof(DayPark));
    if(dayPark == NULL) {
        printf("Error allocating memory.\n");
        return NULL;
    }

    dayPark->date =(char *)malloc((strlen(date->date) + 1)*sizeof(char));
    strcpy(dayPark->date, date->date);

    if(init_dayPark(dayPark, park) == FALSE)
        return NULL;

    dayPark->next = NULL;

    if(park->head_day == NULL){
        park->head_day = dayPark;
    }
    else{
        park->tail_day->next = dayPark;
    }

    park->tail_day = dayPark;

    return dayPark;
}

/**
 * Inicializa os parâmetros da estrutura dayPark. Retorna FALSE caso
 * não tenha sido bem sucedida.
 * @param dayPark --> ponteiro para a estrutura dayPark a inicializar
 * @param park --> ponteiro para a estrutura do parque
 * @return TRUE, FALSE
*/
int init_dayPark(DayPark *dayPark, Park *park){
    dayPark->list_capacity = park->capacity;
    dayPark->num_cars = 0;
    dayPark->daily_price = 0;

    dayPark->licence_list = (char **)malloc(dayPark->list_capacity * 
    sizeof(char *));
    if(dayPark->licence_list == NULL){
        printf("Error allocating memory.\n");
        free(dayPark);
        return FALSE;
    }
    dayPark->exithours_list = (char **)malloc(dayPark->list_capacity * 
    sizeof(char *));
    if(dayPark->exithours_list == NULL){
        printf("Error allocating memory.\n");
        free(dayPark->licence_list);
        free(dayPark);
        return FALSE;
    }
    dayPark->price_list = (float *)malloc(dayPark->list_capacity * 
    sizeof(float));
    if(dayPark->price_list == NULL){
        printf("Error allocating memory.\n");
        free(dayPark->exithours_list);
        free(dayPark->licence_list);
        free(dayPark);
        return FALSE;
    }
    return TRUE;
}

/**
 * Procura um dayPark de um determinado dia na linked list de um determinado
 * parque. Retorna-o se encontrar, ou NULL caso contrário.
 * @param date --> ponteiro para a estrutura da data a procurar
 * @param park --> ponteiro para o parque onde procurar
*/
DayPark *find_dayPark(Date *date, Park *park){
    for(DayPark *d = park->head_day; d != NULL; d = d->next){
        if(strcmp(d->date, date->date) == 0)
            return d;
    }
    return NULL;
}

/**
 * Atualiza os dados da estrutura dayPark com os parâmetros recebidos como
 * argumento.
 * @param dayPark --> ponteiro para estrutura do dayPark a atualizar
 * @param price --> preço a adicionar ao dayPark
 * @param c --> ponteiro para estrutura do carro que sai do parque
 * @param d -->ponteiro para estrutura da data de saída
*/
void update_dayPark(DayPark *dayPark, float price, Car *c, Date *d){
    dayPark->daily_price += price; 
    dayPark->price_list[dayPark->num_cars] =  price;
    dayPark->exithours_list[dayPark->num_cars] = d->hours;
    dayPark->licence_list[dayPark->num_cars] = c->licence;
    dayPark->num_cars++;
}


/* --- Funções comando r --- */
/**
 * Lê os dados do input e remove o parque recebido do sistema
 * @param input --> ponteiro para o input recebido
 * @param sys   --> ponteiro para estrutura do sistema
*/
void command_r(char *input, System *sys){
    Park *p = read_park_name(input, sys);
    if(p == NULL){
        return;
    }
    //liberta as informações do parque do sistema
    free_park(p, sys);
    //cria uma lista com os nomes dos parques restantes
    char **parks_list = (char**)malloc(sys->num_parks*sizeof(char*));
    int i=0;
    for(Park *p = sys->head_p; p != NULL; p = p->next){
        parks_list[i] = p->name;
        i++;   
    }
    //organiza a lista por ordem alfabetica
    quicksort(parks_list, 0, i-1);
    //retorna o nome dos parques
    for(int j = 0; j < i; j++){
        printf("%s\n", parks_list[j]);
    }

    free(parks_list);
}
