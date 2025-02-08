/* iaed24 - ist1106059 - project */

#ifndef SORT_H_
#define SORT_H_

#include "project.h"
#include "cars.h"

/* Funções Ordenação */
void quicksort_carMovs(CarMovs **a, int left, int right, int *idx);
int partition_carMovs(CarMovs **a, int left, int right, int *idx);
void switch_carMovs(CarMovs **s1, CarMovs **s2);

void quicksort(char **a, int left, int right);
int partition(char **a, int left, int right);
void switch_place(char **s1, char **s2);


#endif