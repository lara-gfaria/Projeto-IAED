/* iaed24 - ist1106059 - project */

#ifndef PARKS_H_
#define PARKS_H_

#include "project.h"
#include "parser.h"
#include "cars.h"
#include "free.h"

/* Funções Comando p */
void command_p(char *input, System *sys);
void create_parks(char *input, System *sys);
int read_param_park(char *input, Park *park, System *sys);
int read_park_prices(char *input, Park *park);
void change_park_list_sys(Park *park, System *sys);
void list_parks(System *sys);
Park *find_park(char *park_name, System *sys);

/* Funções estrutura DayPark */
DayPark *find_dayPark(Date *date, Park *park);
int init_dayPark(DayPark *dayPark, Park *park);
DayPark *create_dayPark(Date *date, Park *park);
void update_dayPark(DayPark *dayPark, float price, Car *c, Date *d);

/* Funções Comando r */
void command_r(char *input, System *sys);

#endif