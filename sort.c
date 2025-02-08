/* iaed24 - ist1106059 - project */
/*
 * Ficheiro:  sort.c
 * Autor:  Lara Faria ist1106059
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* Definicao Constantes */
#define TRUE 1
#define FALSE 0

#include "project.h"
#include "sort.h"
#include "cars.h"

//* Funções Ordenação CarMovs*/
/**
 * Ordena um vetor de carMovs por ordem alfabetica do nome do parque.
 * @param a --> inteiro para um vetor de carMovs
 * @param left --> indice do elemento mais a esquerda
 * @param right --> indice do elemento mais a direita
 * @param idx -->  ponteiro para o last mov index para o podermos alterar
*/
void quicksort_carMovs(CarMovs **a, int left, int right, int *idx){
    int i;

    if(right <= left)
        return;

    i = partition_carMovs(a, left, right, idx);

    quicksort_carMovs(a, left, i-1, idx);
    quicksort_carMovs(a, i+1, right, idx);
}

/**
 * Ordena um vetor de carMovs em torno de um elemento colocando os elementos 
 * menores que ele à sua esquerda e os maiores à sua direira
 * @param a --> ponteiro para um vetor de carMovs
 * @param left --> indice do elemento mais à esquerda
 * @param right --> indice do elemento mais à direita
 * @param idx --> ponteiro para o last mov index para o podermos alterar
*/
int partition_carMovs(CarMovs **a, int left, int right, int *idx){
    int i = left-1;
    int j = right;

    CarMovs *pivot = a[right];

    while(i < j){
        while(strcmp(a[++i]->park->name, pivot->park->name) < 0 || 
              (strcmp(a[i]->park->name, pivot->park->name) == 0 && date_cmp(a[i]->entry_date, pivot->entry_date) < 0));
        while(strcmp(pivot->park->name, a[--j]->park->name) < 0 || 
              (strcmp(pivot->park->name, a[j]->park->name) == 0 && date_cmp(pivot->entry_date, a[j]->entry_date) < 0))
            if(j == left)
                break;
        if(i < j){
            if(i == *idx) *idx = j;
            else if(j == *idx) *idx = i;
            switch_carMovs(&a[i], &a[j]);
        }
    }
    // Trocar o pivot com o elemento onde os contadores se encontraram
    if(i == *idx) *idx = right;
    else if(right == *idx) *idx = i;
    switch_carMovs(&a[i], &a[right]);
    
    return i;
}

/*
 * Troca o conteudo de dois carMovs um com o outro
 * @param s1 --> ponteiro para um dos carMovs que queremos trocar
 * @param s2 --> ponteiro para o outro carMov
 */
void switch_carMovs(CarMovs **s1, CarMovs **s2){
    CarMovs *temp = *s1;
    *s1 = *s2;
    *s2 = temp;
}


/* Funções Ordenação */
/**
 * Ordena um vetor de strings por ordem alfabetica
 * @param a --> ponteiro para um vetor de strings
 * @param left --> indice do elemento mais a esquerda
 * @param right --> indice do elemento mais a direita
*/
void quicksort(char **a, int left, int right){
    int i;

    if(right <= left)
        return;

    i = partition(a, left, right);

    quicksort(a, left, i-1);
    quicksort(a, i+1, right);
}

/**
 * Ordena um vetor de strings em torno de um elemento
 * colocando os elementos menores que ele à sua esquerda e os maiores
 * à sua direira
 * @param a --> ponteiro para um vetor de strings
 * @param left --> indice do elemento mais à esquerda
 * @param right --> índice do elemento mais à direita
*/
int partition(char **a, int left, int right){
    int i = left-1;
    int j = right;

    //Declara o pivot como a string mais a direita
    char *pivot = a[right];

    while(i < j){
        while(strcmp(a[++i], pivot) < 0);
        while(strcmp(pivot, a[--j]) < 0)
            if(j == left)
                break;
        if(i < j)
            switch_place(&a[i], &a[j]);
    }
    // Troca o pivot com o elemento onde os contadores se encontraram
    switch_place(&a[i], &a[right]);
    
    return i;
}

/*
 * Troca o conteudo de duas strings uma com a outra
 * @param s1 --> ponteiro para ponteiro para uma das strings que queremos trocar
 * @param s2 --> ponteiro para ponteiro para a outra string
 */
void switch_place(char **s1, char **s2){
    char *temp = *s1;
    *s1 = *s2;
    *s2 = temp;
}