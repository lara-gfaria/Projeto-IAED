/* iaed24 - ist1106059 - project */

#ifndef DATES_H_
#define DATES_H_

#include "project.h"
#include "parser.h"
#include "cars.h"
#include "parks.h"

/* Funções Datas */
Date *create_date(char *input, System *sys);
void param_date(Date *date, char *hours);
int date_valid(Date *date, System *sys);
int valid_day_of_month(int day, int month);
int hour_valid(Date *date, System *sys, int same_day);
int num_days_of_month(int month);
int date_cmp(Date *date1, Date *date2);

#endif