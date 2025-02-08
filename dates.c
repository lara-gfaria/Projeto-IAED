/* iaed24 - ist1106059 - project */
/*
 * Ficheiro:  dates.c
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

/* Definicao Constantes */
#define TRUE 1
#define FALSE 0


/*  --- Funções datas ---  */
/**
 * Cria uma nova data, lendo o resto do input para atribuir 
 * os parâmetros da mesma, retornando-a.
 * @param input --> ponteiro para o input recebido
 * @param sys   --> ponteiro para estrutura do sistema
 * @return date --> ponteiro para a data criada
*/
Date *create_date(char *input, System *sys){
    //cria uma nova data e verifica a alocação de memória
    Date *date = (Date *)malloc(sizeof(Date));
    if(date == NULL){
        printf("Erro allocating memory.\n");
        return NULL;
    }

    //lê data de entrada do carro
    date->date = read_string(input);
    //le hora de entrada do carro
    char *hours = read_string(input);

    param_date(date, hours);
    
    //verifica que a data e hora são válidas
    if(date_valid(date, sys) == FALSE){
        printf("invalid date.\n");
        free(date->date);
        free(date->hours);
        free(date);
        return NULL;
    }

    return date;
}

/**
 * Atribui os parâmetros na estrutura data, da data e horas correspondentes
 * @param date --> ponteiro da data recebida
 * @param hours --> string das horas recebidas no input
*/
void param_date(Date *date, char *hours){
    //caso o tamanho da hora seja menor que 4 é acrescentado o caracter '0'
    if(strlen(hours) == 4 && hours[1] == ':'){
        char *hours_0 = (char *)malloc((strlen(hours) + 2)* sizeof(char));
        hours_0[0] = '0';
        memmove(hours_0 + 1, hours, 5*sizeof(char));
        hours_0[5] = '\0';
        free(hours);
        date->hours = hours_0;
    }
    else{
        date->hours = hours;
    }

    //atribui a cada paramêtro da data o seu valor em inteiro
    date->day = atoi(strndup(date->date, 2));
    date->month = atoi(strndup(date->date + 3, 2));
    date->year = atoi(strndup(date->date + 6, 4));

    date->hour = atoi(strndup(date->hours, 2));
    date->mins = atoi(strndup(date->hours + 3, 2));
}

/**
 * Verifica a validade da date, se o formato é válido e é após a última data 
 * registada no sistema. Retorna TRUE ou FALSE caso a data seja válida ou 
 * inválida, respetivamente.
 * @param date --> ponteiro para a data a verificar
 * @param sys  --> ponteiro para a estrutura do sistema
 * @return TRUE or FALSE
*/
int date_valid(Date *date, System *sys){
    //verifica que o formato da data está certo
    if(strlen(date->date) != 10 || date->date[2] != '-' 
    || date->date[5] != '-')
        return FALSE;

    //Verifica se o dia do mês é válido
    if(valid_day_of_month(date->day, date->month) == FALSE 
    || (date->day == 29 && date->month == 2))
        return FALSE;

    //Verifica que a data é após a última data registada no sistema
    if(sys->last_date_registe != NULL){
        if(date_cmp(date, sys->last_date_registe) == 0 
        && hour_valid(date, sys, TRUE) == FALSE)
            return FALSE;

        if(date_cmp(date, sys->last_date_registe) == -1 
        || hour_valid(date, sys, FALSE) == FALSE)
            return FALSE;
    }
    //altera a última data registada no sistema para a recebida
    sys->last_date_registe = date;
    return TRUE;
}

/**
 * Verifica se o dia é válido em relação ao respetivo ao mês.
 * Retorna TRUE caso o dia seja válido para o mês e FALSE caso contrário.
 * @param day --> valor do dia do mês a avaliar
 * @param month --> valor do mês a avaliar
 * @return TRUE or FALSE
*/
int valid_day_of_month(int day, int month){
    if(day <= 0 || month <= 0)
        return FALSE;
    
    if(day > num_days_of_month(month))
        return FALSE;

    return TRUE;
}

/**
 * Verifica que a hora é válida. Retorna TRUE se a hora é válida 
 * e FALSE caso contrário
 * @param date  --> ponteiro para a data das horas a avaliar
 * @param sys   --> ponteiro para a estrutura do sistema
 * @param same_day --> data recebida e do sistema são ou não no mesmo dia
 * @return TRUE or FALSE
*/
int hour_valid(Date *date, System *sys, int same_day){
    //verifica que o formato da hora é válido
    if((strlen(date->hours) != 5) || date->hours[2] != ':')
        return FALSE;

    //verifica que as horas são válidas, dentro dos intervalos certos
    if(date->hour < 0 || date->hour > 23 || date->mins < 0 || date->mins > 59)
        return FALSE;

    //caso as datas sejam o mesmo dia, verifica que a hora é após a do sistema
    if(same_day == TRUE){
        if(date->hour == sys->last_date_registe->hour 
        && date->mins < sys->last_date_registe->mins)
            return FALSE;
        
        if(date->hour < sys->last_date_registe->hour)
            return FALSE;
    }
    return TRUE;
}

/**
 * Retorna o número de dias do mês.
 * @param month
 * @return 30, 31 ou 28 --> número de dias do mês recebido
*/
int num_days_of_month(int month){
    //se o mês for menor que 8 e o mês for par diferente de 2
    if(month < 8 && month % 2 == 0 && month != 2){ 
        return 30;
    }
    //se o mês for menor que 8 e o mês for ímpar
    if(month < 8 && month % 2 == 1){
        return 31;
    }
    //se for mês 2
    if(month == 2){
        return 28;
    }
    //se o mês for maior que 8 e par
    if(month >= 8 && month % 2 == 0){
        return 31;
    }
    //se o mês for maior que 8 e ímpar
    else{
        return 30;
    }
}

/**
 * Compara duas datas, retornando -1 se a primeira data for
 * mais recente que a primeira, 0 se forem iguais.
 * @param date1 --> ponteiro para a primeira data
 * @param date2 --> ponteiro para a segunda data
 * @return -1, 0, 1 --> resultado da comparação das datas
*/
int date_cmp(Date *date1, Date *date2){
    //se date1 é mais recente que date2
    if(date1->year > date2->year || (date1->year == date2->year 
    && date1->month > date2->month) || (date1->year == date2->year 
    && date1->month == date2->month && date1->day > date2->day))
        return 1;
    //se date1 é no mesmo dia que date2
    else if(date1->year == date2->year && date1->month == date2->month 
    && date1->day == date2->day){

        if(date1->hour > date2->hour || (date1->hour == date2->hour 
        && date1->mins > date2->mins))
            return 1;

        else if(date1->hour == date2->hour && date1->mins == date2->mins)
            return 0;

        else
            return -1;
    }
    //se date1 é mais antiga que date2
    else
        return -1;
}