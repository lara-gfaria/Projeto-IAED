/* iaed24 - ist1106059 - project */
/*
 * Ficheiro:  cars.c
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
#include "dates.h"
#include "free.h"
#include "sort.h"

/* Definicao Constantes */
#define TRUE 1
#define FALSE 0
#define CAPACITY 20

/*---Funções da hashtable---*/
/* Retorna o valor que define o lugar na HashTable */
int hash(char *licence){
    int hash_value = 0;
    for(int i = 0; i < 8; i++){
        hash_value += 31 * licence[i];
    }
    return hash_value % TABLE_SIZE;
}

/* Inicia a HashTable a NULL */
void init_hash_table(System *sys){
    for(int i = 0; i < TABLE_SIZE; i++){
        sys->hash_table[i] = NULL;
    }
}

/* Insere um novo carro na HashTable */
void insert_car_hash_table(Car *car, System *sys){
    int index = hash(car->licence);
    if(sys->hash_table[index] != NULL)
        car->next = sys->hash_table[index];

    sys->hash_table[index] = car;
}

/* Procura um carro na HashTable e devolve-o*/
Car *find_car(char *licence, System *sys){
    int index = hash(licence);
    if(sys->hash_table[index] != NULL){
        for(Car *c = sys->hash_table[index]; c != NULL; c = c->next){
            if(strcmp(licence, c->licence) == 0)
                return c;
        }
    }
    return NULL;
}


/*  --- Funções comando e ---  */
/**
 * Lê os dados do input e atribui os respetivos parâmetros às estruturas
 * do carro e do parque recebidos.
 * @param input --> ponteiro para o input recebido
 * @param sys   --> ponteiro para estrutura do sistema
*/
void command_e(char *input, System *sys){
    Park *p = read_park_name(input, sys);
    if(p == NULL) return;

    //verifica se existem lugares disponíveis no parque
    if(p->available_slots == 0){
        printf("%s: parking is full.\n", p->name);
        return;
    }
    //lê a matrícula do carro, verifica se é válida e procura o carro
    char *licence_str = read_string(input);
    if(licence_valid(licence_str) == FALSE){
        printf("%s: invalid licence plate.\n", licence_str);
        free(licence_str);
        return;
    }
    Car *car = find_car(licence_str, sys);
    if(car == NULL){ //se o carro não existe, cria-o
        create_car(input, p, licence_str, sys);
    }
    else{ //caso contrário, regista a nova entrada do carro
        if(car->num_movs > -1){
            if(car->movs[car->last_mov_idx]->exit_date == NULL){
                printf("%s: invalid vehicle entry.\n", licence_str);
                free(licence_str);
                return;
            }
        }
        if(registe_entry(car, p, input, sys) == FALSE){
            free_car(car, sys);
            return;
        }
        free(licence_str);
    }
}

/**
 * Cria um novo carro, lendo os dados do input e atribuindo-os
 * aos respetivos parâmetros na estrutura carro.
 * @param input --> ponteiro para o input recebido
 * @param p   --> ponteiro para a estrutura do parque
 * @param licence_str --> matrícula do carro
 * @param sys   --> ponteiro para estrutura do sistema
*/
void create_car(char *input, Park *p, char *licence_str, System *sys){
    //cria o novo carro e verifica a alocação de memória
    Car *car = (Car *)malloc(sizeof(Car));
    if(car == NULL) {
        printf("Erro allocating memory.\n");
        free(licence_str);
        return;
    }

    //inicializa os parametros na estrutura do carro
    init_car(car, licence_str);

    //regista a entrada do carro no parque
    if(registe_entry(car, p, input, sys) == FALSE){
        free_car(car, sys);
        return;
    }
    //insere o carro na hashtable
    insert_car_hash_table(car, sys);
}

/**
 * Inicializa os parâmetros da estrutura do carro criado.
 * @param car --> ponteiro para a estrutura do carro criado
 * @param licence --> string da matrícula do carro criado
*/
void init_car(Car *car, char *licence){
    //inicializa os parametros da estrutura
    car->licence = licence;
    car->last_mov_idx = 0;
    car->movs_capacity = CAPACITY;
    car->num_movs = -1;
    car->next = NULL;

    //inicializa a lista de CarMovs e verifica a alocação de memória
    car->movs = (CarMovs **)calloc(1, car->movs_capacity * sizeof(CarMovs *));
    if(car->movs == NULL) {
        printf("Erro allocating memory.\n");
        free(licence);
        free(car);
        return;
    }
}

/**
 * Regista a entrada do carro no parque.
 * Retorna TRUE caso o registo tenha sido bem sucedido e FALSE caso contrário.
 * @param car   --> ponteiro para a estrutura do carro que entra no parque
 * @param park  --> ponteiro para a estrutura do parque em que o carro entra
 * @param input --> ponteiro para o input recebido
 * @param sys   --> ponteiro para estrutura do sistema
 * @return TRUE ou FALSE
*/
int registe_entry(Car *car, Park *park, char *input, System *sys){
    //cria uma data e verifica que foi bem sucedido
    Date *date = create_date(input, sys);
    if(date == NULL){
        return FALSE;
    }

    if(registe_carMov(car, park, date) == FALSE){
        free_date(date);
        return FALSE;
    }

    //altera a última data registada no sistema para a data de entrada
    sys->last_date_registe = date;
    sys->num_cars++;

    //adiciona o carro à lista de carros dentro do parque
    park->cars_in_park[park->capacity - park->available_slots] = car->licence;

    //diminui o número de lugares disponíveis no parque
    park->available_slots--;

    printf("%s %d\n", park->name, park->available_slots);
    return TRUE;
}

/**
 * Regista um novo movimento do carro criando outra estrutura CarMovs.
 * Retorna FALSE caso o registo não seja bem sucedido.
 * @param car --> ponteiro para a estrutura do carro que entra no parque
 * @param park --> ponteiro para a estrutura do parque onde o carro entra
 * @param date --> ponteiro para a estrutura da data de entrada
 * @return TRUE, FALSE
*/
int registe_carMov(Car *car, Park *park, Date *date){
    car->num_movs++;
    //se num movimentos igual à capacidade, aloca mais memória
    if(car->num_movs == car->movs_capacity){
        car->movs_capacity *= 2;
        car->movs = (CarMovs**)realloc(car->movs, 
        car->movs_capacity*sizeof(CarMovs*));
    }

    //aloca memória para o novo dado de entrada
    car->movs[car->num_movs] = (CarMovs*)malloc(sizeof(CarMovs));
    if(car->movs[car->num_movs] == NULL){
        printf("Erro aloccating memory.\n");
        free(car->movs);
        return FALSE;
    }

    //inicializa os parâmetros da nova entrada
    car->movs[car->num_movs]->park = park;
    car->movs[car->num_movs]->entry_date = date;
    car->movs[car->num_movs]->exit_date = NULL;
    car->movs[car->num_movs]->price = 0;

    //altera o ponteiro de último endereço para o número de movimentos
    car->last_mov_idx = car->num_movs;
    return TRUE;
}

/**
 * Verifica que a matrícula recebida é válida.
 * Retorna TRUE se for e FALSE caso contrário.
 * @param licence   --> matrícula a ser validada
 * @return TRUE ou FALSE
*/
int licence_valid(char *licence){
    int digit = 0;
    int letter = 0;
    //verifica se o formato da matrícula é válido
    if(strlen(licence) != 8 || licence[2] != '-' || licence[5] != '-')
        return FALSE;

    //para cada caracter da matrícula verifica se é digito ou letra maíuscula
    for(int i = 0; i < 8; i++){
        if(isupper(licence[i]) && isupper(licence[i+1])){
            letter++;
            i++;
        }
        else if(isdigit(licence[i]) && isdigit(licence[i+1])){
            digit++;
            i++;
        }
        else if(licence[i] == '-')
            continue;
        else
            return FALSE;
    }
    //verifica que tem dois pares de letras ou dois pares de dígitos
    if(digit > 2 || letter > 2)
        return FALSE;
    return TRUE;
}


/*  --- Funções comando s ---  */
/**
 * Lê os dados do input e regista a saída do carro de um parque
 * @param input --> ponteiro para o input recebido
 * @param sys   --> ponteiro para estrutura do sistema
*/
void command_s(char *input, System *sys){
    Park *p = read_park_name(input, sys);
    if(p == NULL)
        return;
   
   //verifica se a matricula é valida
    char *licence_str = read_string(input);
    if(licence_valid(licence_str) == FALSE){
        printf("%s: invalid licence plate.\n", licence_str);
        free(licence_str);
        return;
    }
    //procura o carro 
    Car *car = find_car(licence_str, sys);
    //se o carro nao existir ou não estiver dentro do parque
    if(car == NULL || car->movs[car->last_mov_idx]->park != p || 
    car->movs[car->last_mov_idx]->exit_date != NULL){
        printf("%s: invalid vehicle exit.\n", licence_str);
        free(licence_str);
        return;
    }
    //regista a saída do carro do parque
    if(registe_exit(car, p, input, sys) == FALSE){
        free(licence_str);
        return;
    }
}

/**
 * Lê os restantes dados do input e regista a saída do carro.
 * Retorna FALSE caso algum registo não seja bem sucedido.
 * @param car --> ponteiro para o carro que sai do parque
 * @param park   --> ponteiro para o parque de onde sai do carro
 * @param input --> ponteiro para o input recebido
 * @param sys   --> ponteiro para estrutura do sistema
 * @return TRUE, FALSE
*/
int registe_exit(Car *car, Park *park, char *input, System *sys){
    //cria a data de saída e verifica se é válida
    Date *date = create_date(input, sys);
    if(date == NULL)
        return FALSE;
    if(date_valid(date, sys) == FALSE){
        printf("invalid date.\n");
        free_date(date);
        return FALSE;
    }
    //procura a estrutura de registo das saídas do parque na data recebida
    DayPark *dayPark = find_dayPark(date, park);
    if(dayPark == NULL) 
        dayPark = create_dayPark(date, park);
    
    if( realloc_list_dayPark(dayPark) == FALSE)
        return FALSE;
    
    car->movs[car->last_mov_idx]->exit_date = date;
    //calcula o preço do intervalo de tempo do carro no parque
    float price = calcule_price(car, park);
    //atualiza os parametros da estrutura dayPark
    update_dayPark(dayPark, price, car, date);
    //altera o valor do preço e dá print dos parametros do movimento de saida
    update_and_print_carMovs(car, price);
    //retira carro da lista de carros do parque
    update_cars_park(park, car);
    return TRUE;
}

/**
 * Retira o carro da lista de carros dentro do parque e atualiza o número de 
 * lugares disponíveis no parque.
 * @param park --> ponteiro para a estrutura do parque
 * @param car --> ponteiro para a estrutura do carro a retirar do parque
*/
void update_cars_park(Park *park, Car *car){
    //retira o carro da lista de carros dentro do parque
    for(int i = 0; i < park->capacity - park->available_slots; i++){
        if(strcmp(park->cars_in_park[i], car->licence) == 0){
            switch_place(&park->cars_in_park[i], 
            &park->cars_in_park[park->capacity - park->available_slots - 1]);
            park->cars_in_park[park->capacity - park->available_slots - 1] = NULL;
            break;
        }
    }
    park->available_slots++;
}

/**
 * Atualiza o valor do preço e apresenta as informações do movimento do carro.
 * @param car --> ponteiro para a estrutura do carro
 * @param price --> valor do preço pago pelo último movimento do carro
*/
void update_and_print_carMovs(Car *car, float price){
    //atualiza os dados dos movimentos dos carros
    car->movs[car->last_mov_idx]->price  = price; 

    printf("%s %s %s %s %s %.2f\n", car->licence, 
    car->movs[car->last_mov_idx]->entry_date->date, 
    car->movs[car->last_mov_idx]->entry_date->hours, 
    car->movs[car->last_mov_idx]->exit_date->date, 
    car->movs[car->last_mov_idx]->exit_date->hours, price);
}

/**
 * Verifica se é necessário alocar mais memórias nas listas do dayPark e faz essa alocação de
 * memória caso seja necessário. Retorna FALSE caso a realocação de memória não tenha sido bem
 * sucedida.
 * @param dayPark --> ponteiro para a estrutura do dayPark
 * @return TRUE ou FALSE
*/
int realloc_list_dayPark(DayPark *dayPark){
    //verifica se é necessário alocar mais memória nas listas do dayPark
    if (dayPark->num_cars == dayPark->list_capacity) {
        dayPark->list_capacity *= 2;
        dayPark->licence_list = (char **)realloc(dayPark->licence_list,
         dayPark->list_capacity * sizeof(char *));
        if (dayPark->licence_list == NULL) {
            printf("Error reallocating memory.\n");
            return FALSE;
        }

        dayPark->exithours_list = (char **)realloc(dayPark->exithours_list, 
        dayPark->list_capacity * sizeof(char *));
        if (dayPark->exithours_list == NULL) {
            printf("Error reallocating memory.\n");
            return FALSE;
        }
    
        dayPark->price_list = (float *)realloc(dayPark->price_list, 
        dayPark->list_capacity * sizeof(float));
        if (dayPark->price_list == NULL) {
            printf("Error reallocating memory.\n");
            return FALSE;
        }
    }
    return TRUE;
}

/**
 * Calcula o preço de acordo com o intervalo de tempo que o carro permanece
 * no parque.
 * @param car --> ponteiro para o carro que sai do parque
 * @param park --> ponteiro para o parque em questão
 * @return price --> valor a pagar pelo carro
*/
float calcule_price(Car *car, Park *park){
    float price = 0;
    int minutes = 0;
    int num_days = days(car);
    
    minutes += num_days * 24 * 60;

    // Se o carro entrou e saiu no mesmo dia
    if (strcmp(car->movs[car->last_mov_idx]->entry_date->date,
               car->movs[car->last_mov_idx]->exit_date->date) == 0) {
        minutes += calcule_minutes_same_day(car);
    }
    else { // Se o carro permaneceu pelo menos uma noite no parque
        minutes += calcule_minutes_entryday(car->movs[car->last_mov_idx]->entry_date);
        minutes += calcule_minutes_exitday(car->movs[car->last_mov_idx]->exit_date);
    }
    
    price = calcule_price_periods(minutes, park);
    return price;
}

/**
 * Calcula o preço de acordo com o número de períodos de 24horas e de
 * 15 minutos que o carro permaneceu no parque. Associa cada intervalo ao
 * preço correspondente.
 * @param minutes --> número de minutos totais que o carro permaneneu no parque
 * @param park --> ponteiro para o parque em que o carro esteve
 * @return price --> preço total de acordo com os intervalos de tempo
*/
float calcule_price_periods(int minutes, Park *park){
    float price = 0;
    int periods_15 = 0;
    int periods_24 = 0;

    //calculo do numero de periodos de 24horas
    periods_24 = minutes / (24 * 60);
    //calculo do numero de intervalos de 15 restantes
    periods_15 = (minutes - periods_24*24*60) / 15;
    if(minutes % 15 != 0)
        periods_15++;

    //se os periodos de 15min forem menores que 4, esteve menos de 1 hora
    if (periods_15 <= 4) {
        price += periods_15 * park->value_15;
        price += periods_24 * park->value_max_day;
    }
    else {
        price += 4 * park->value_15;
        price += (periods_15 - 4) * park->value_15_after_1hour;

        if (price > park->value_max_day)
            price = park->value_max_day;

        price += periods_24 * park->value_max_day;
    }
    return price;
}

/**
 * Calcula o número de minutos do intervalo de entrada e saida quando o carro
 * entrou e saiu no mesmo dia.
 * @param car --> ponteiro para o carro
 * @return minutes --> diferença de minutos entre a hora de saida e hora de entrada
*/
int calcule_minutes_same_day(Car *car){
    int minutes = 0;
    //se a hora de entrada for inferior a hora de saida
    if(car->movs[car->last_mov_idx]->entry_date->hour < 
    car->movs[car->last_mov_idx]->exit_date->hour){
        //Se os minutos de saida forem maiores ou iguais
        if(car->movs[car->last_mov_idx]->exit_date->mins >= 
        car->movs[car->last_mov_idx]->entry_date->mins){

            minutes += (car->movs[car->last_mov_idx]->exit_date->hour - 
            car->movs[car->last_mov_idx]->entry_date->hour) * 60;

            minutes += (car->movs[car->last_mov_idx]->exit_date->mins -
            car->movs[car->last_mov_idx]->entry_date->mins);
        }
        //se os minutos de entrada forem menores que os de saida
        else{ 
            //subtraimos -1 porque a hora não foi completada
            minutes += (car->movs[car->last_mov_idx]->exit_date->hour - 
            car->movs[car->last_mov_idx]->entry_date->hour - 1) * 60;

            minutes += (60 - car->movs[car->last_mov_idx]->entry_date->mins + 
            car->movs[car->last_mov_idx]->exit_date->mins);
        }
    }
    //se a hora de entrada for igual a hora de saida
    else{
        minutes += car->movs[car->last_mov_idx]->exit_date->mins - 
        car->movs[car->last_mov_idx]->entry_date->mins;
    }
    return minutes;
}

/**
 * Calcula o número de minutos do dia de entrada.
 * @param date --> ponteiro para a estrutura da data de entrada do carro
 * @return minutes --> minutos totais do dia de entrada do carro no parque
*/
int calcule_minutes_entryday(Date *date){
    int minutes = 0;
    minutes += (23 - date->hour) * 60 + (60 - date->mins);
    return minutes;
}

/**
 * Calcula o número de minutos do dia de saída.
 * @param date --> ponteiro para a estrutura da data de saída do carro
 * @return minutes --> minutos totais do dia de saída do carro do parque
*/
int calcule_minutes_exitday(Date *date){
    int minutes = 0;
    minutes += date->hour * 60 + date->mins;
    return minutes;
}

/**
 * Calcula o número de dias inteiros entre a data de entrada e a data de saída
 * do carro de dentro de um parque.
 * @param car --> ponteiro para a estrutura do carro
 * @return days --> número de dias inteiros entre as duas datas
*/
int days(Car *car) {
    int entry_day = car->movs[car->last_mov_idx]->entry_date->day;
    int entry_month = car->movs[car->last_mov_idx]->entry_date->month;
    int entry_year = car->movs[car->last_mov_idx]->entry_date->year;

    int exit_day = car->movs[car->last_mov_idx]->exit_date->day;
    int exit_month = car->movs[car->last_mov_idx]->exit_date->month;
    int exit_year = car->movs[car->last_mov_idx]->exit_date->year;

    int num_days = 0;

    // Se as datas de entrada e saída ocorrem no mesmo ano
    if (entry_year == exit_year) {
        num_days += same_year(entry_month, exit_month, entry_day, exit_day);
    } 
    // Se as datas de entrada e saída ocorrem em anos diferentes
    else {
        num_days += diff_year(entry_year, exit_year, entry_month, exit_month,
        entry_day, exit_day);
    }
    return num_days;
}

/**
 * Retorna o número de dias inteiros entre duas datas do mesmo ano.
 * @param entry_month --> valor do mês de entrada
 * @param exit_month --> valor do mês de saída
 * @param entry_day --> valor do dia de entrada
 * @param exit_day --> valor do dia de saída
*/
int same_year(int entry_month, int exit_month, int entry_day, int exit_day){
    int num_days = 0;
    // Se as datas de entrada e saída ocorrem no mesmo mês
    if (entry_month == exit_month) {
        if(entry_day == exit_day) return 0;
        num_days = exit_day - entry_day - 1;
    }
    else {
        // Adiciona os dias restantes do mês de entrada
        num_days += num_days_of_month(entry_month) - entry_day;
        
        // Adiciona os dias completos dos meses intermediários
        for (int month = entry_month + 1; month < exit_month; month++) {
            num_days += num_days_of_month(month);
        }

        // Adiciona os dias completos do mês de saída
        num_days += exit_day - 1;
    }
    return num_days;
}

/**
 * Retorna o número de dias inteiros entre duas datas de anos diferentes.
 * @param entry_year --> valor do ano de entrada
 * @param exit_year --> valor do ano de saída
 * @param entry_month --> valor do mês de entrada
 * @param exit_month --> valor do mês de saída
 * @param entry_day --> valor do dia de entrada
 * @param exit_day --> valor do dia de saída
*/
int diff_year(int entry_year, int exit_year, int entry_month, 
int exit_month, int entry_day, int exit_day){

    int num_days = 0;
    // Adiciona os dias restantes do mês de entrada
    num_days += num_days_of_month(entry_month) - entry_day;
    
    // Adiciona os dias completos dos meses intermediários do ano de entrada
    for (int month = entry_month + 1; month <= 12; month++) {
        num_days += num_days_of_month(month);
    }

    //Adiciona os dias mpletos do anos intermediários
    num_days += (exit_year - entry_year - 1) * 365;

    // Adiciona os dias completos do mês de saída do ano de saída
    for (int month = 1; month < exit_month; month++) {
        num_days += num_days_of_month(month);
    }

    // Adiciona os dias da data de saída
    num_days += exit_day - 1;

    return num_days;
}

/* --- Funções comando v --- */
/**
 * Lê os dados do input e printa os movimentos do carro
 * @param input --> ponteiro para o input recebido
 * @param sys   --> ponteiro para estrutura do sistema
*/
void command_v(char *input, System *sys){
    //lê a matrícula do carro e verifica se é válida
    char *licence_str = read_string(input);
    if(licence_valid(licence_str) == FALSE){
        printf("%s: invalid licence plate.\n", licence_str);
        free(licence_str);
        return;
    }
    //procura o carro e verifica que tem movimentos
    Car *car = find_car(licence_str, sys);
    if(car == NULL || car->num_movs < 0){
        printf("%s: no entries found in any parking.\n", licence_str);
        free(licence_str);
        return;
    }
    free(licence_str);

    //ordena lista de carmovs
    if(car->num_movs >= 0)
        quicksort_carMovs(car->movs, 0, car->num_movs, &car->last_mov_idx);

    for(int i = 0; i <= car->num_movs; i++){
        if(car->movs[i]->exit_date == NULL)
            printf("%s %s %s\n", car->movs[i]->park->name, car->movs[i]->entry_date->date,
            car->movs[i]->entry_date->hours);
        else
            printf("%s %s %s %s %s\n", car->movs[i]->park->name, car->movs[i]->entry_date->date,
            car->movs[i]->entry_date->hours, car->movs[i]->exit_date->date, car->movs[i]->exit_date->hours);
    }
}


/* --- Funções comando f --- */
/**
 * Lê os dados do input e verifica se recebe apenas o nome do parque, ou
 * também recebe uma data. Chama a respetiva função para cada caso.
 * @param input --> ponteiro para o input recebido
 * @param sys   --> ponteiro para estrutura do sistema
*/
void command_f(char *input, System *sys){
    Park *park = read_park_name(input, sys);
    if(park == NULL)
        return;

    if(input[0] == '\0')
        list_date_profit(park);
    else{
        char *date = read_string(input);
        list_car_profit(park, date, sys);
        free(date);
    }
}

/**
 * Lista as datas e a faturação desse dia de um determinado parque.
 * @param park --> ponteiro para o parques
*/
void list_date_profit(Park *park){
    for(DayPark *d = park->head_day; d != NULL; d = d->next){
        printf("%s %.2f\n", d->date, d->daily_price);
    }
}

/**
 * Lista a matrícula, hora de saída e preço pago por cada carro que saí do 
 * parque numa determinada data.
 * @param park --> ponteiro para o parque
 * @param date --> ponteiro para a data de saída que queremos procurar
 * @param sys --> ponteiro para a estrutura do sistema
*/
void list_car_profit(Park *park, char *date, System *sys){
    //verifica que a data recebida é válida e antes do ultimo registo no sistema
    if(date_before_sys(date, sys) == FALSE){
        printf("invalid date.\n");
        return;
    }
    
    //percorre os dayParks do parque em questão até encontrar a respetiva data
    for(DayPark *d = park->head_day; d != NULL; d = d->next){
        if(strcmp(d->date, date) == 0){
            for(int i = 0; i < d->num_cars; i++) {
                printf("%s %s %.2f\n", d->licence_list[i], 
                d->exithours_list[i], d->price_list[i]);
            }
            return;
        }
    }
}

/**
 * Verifica que a data recebida é antes da última data registada no sistema
 * @param date --> ponteiro para a estrutura da data a analisar
 * @param sys --> ponteiro para a estrutura do sistema
*/
int date_before_sys(char *date, System *sys){
    int day = atoi(strndup(date, 2));
    int month = atoi(strndup(date + 3, 2));
    int year = atoi(strndup(date + 6, 4));

    //verifica o formato da data
    if(strlen(date) != 10 || date[2] != '-' || date[5] != '-')
        return FALSE;

    //verifica que o dia do mes é valido
    if(valid_day_of_month(day, month) == FALSE 
    || (day == 29 && month == 2))
        return FALSE;

    //verifica que a data é antes do ultimo registo no sistema
    if(sys->last_date_registe != NULL){
        if(year > sys->last_date_registe->year ||
        (year == sys->last_date_registe->year &&
         month > sys->last_date_registe->month) ||
        (year == sys->last_date_registe->year && 
        month == sys->last_date_registe->month && 
        day > sys->last_date_registe->day)) {
            return FALSE;
        }
    }
    return TRUE;
}

